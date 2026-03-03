// ======================================================================
//
// CreatureObject_Ships.cpp
//
// Copyright 2003 Sony Online Entertainment
//
// ======================================================================

#include "serverGame/FirstServerGame.h"
#include "serverGame/CreatureObject.h"

#include "serverGame/CellObject.h"
#include "serverGame/ServerWorld.h"
#include "serverGame/ConfigServerGame.h"
#include "serverGame/ContainerInterface.h"
#include "serverGame/GameServer.h"
#include "serverGame/GroupObject.h"
#include "serverGame/PlayerCreatureController.h"
#include "serverGame/ShipClientUpdateTracker.h"
#include "serverGame/ShipObject.h"
#include "sharedFoundation/ConstCharCrcLowerString.h"
#include "sharedGame/ShipSlotIdManager.h"
#include "sharedObject/CellProperty.h"
#include "sharedObject/ContainedByProperty.h"
#include "sharedObject/SlottedContainer.h"
#include "sharedObject/SlottedContainmentProperty.h"

// ======================================================================

/**
 * Instruct this pilot to ride on the specified ship.
 *
 * @param mountObject  the ShipObject that will be ridden by this
 *                     instance.
 *
 * @return  true if this pilot was able to mount the ship; false otherwise.
 */

bool CreatureObject::pilotShip(ServerObject &pilotSlotObject)
{
	//-- Ensure ships are enabled.
	if (!ConfigServerGame::getShipsEnabled())
	{
		return false;
	}

	//-- Ensure we (the pilot) are not already piloting a ship.
	if (getPilotedShip())
	{
		DEBUG_FATAL(true, ("pilotShip(): pilot id=[%s] template=[%s] is already piloting a ship.", getNetworkId().getValueString().c_str(), getObjectTemplateName()));
		return false;
	}

	//-- Ensure both the ship and the pilot are authoritative on this server.
	//   (They should be by design of how this function gets called.)
	if (!isAuthoritative() || !pilotSlotObject.isAuthoritative())
	{
		DEBUG_WARNING(true, ("pilotShip(): pilot id=[%s] or ship id=[%s] was not authoritative on this server, mounting aborted.", getNetworkId().getValueString().c_str(), pilotSlotObject.getNetworkId().getValueString().c_str()));
		return false;
	}

	ShipObject *shipObject = ShipObject::getContainingShipObject(&pilotSlotObject);
	NOT_NULL(shipObject);

	//-- Reject if the ship is not in the world cell
	if (!shipObject->isInWorldCell())
	{
		DEBUG_WARNING(true, ("pilotShip(): ship id=[%s] is not in the world cell, mounting ship aborted.", shipObject->getNetworkId().getValueString().c_str()));
		return false;
	}

	// If directly contained by a ship, we use the pilot slot, else we use the pob pilot slot
	SlotId const pilotSlotId = pilotSlotObject.asShipObject() ? ShipSlotIdManager::getShipPilotSlotId() : ShipSlotIdManager::getPobShipPilotSlotId();

	Container::ContainerErrorCode errorCode = Container::CEC_Success;
	bool const transferSuccess = ContainerInterface::transferItemToSlottedContainer(pilotSlotObject, *this, pilotSlotId, nullptr, errorCode);
	DEBUG_FATAL(transferSuccess && (errorCode != Container::CEC_Success), ("pilotShip(): transferItemToSlottedContainer() returned success but container error code returned error %d.", static_cast<int>(errorCode)));

	if (transferSuccess)
	{
		Client * const client = getClient();
		if (client)
			client->addControlledObject(*shipObject);
	}

	//-- Indicate transfer success.
	return transferSuccess;
}

// ----------------------------------------------------------------------

bool CreatureObject::unpilotShip()
{
	//-- Only do this on the authoritative pilot.
	if (!isAuthoritative())
	{
		DEBUG_REPORT_LOG(true, ("CreatureObject::unpilotShip(): server id=[%d], ship id=[%s], skipping call because ship is not authoritative.",
			static_cast<int>(GameServer::getInstance().getProcessId()),
			getNetworkId().getValueString().c_str()));
		return false;
	}

	//-- Ensure ships are enabled.
	if (!ConfigServerGame::getShipsEnabled())
		return false;

	bool unpilotedShip = false;

	ShipObject* ship = getPilotedShip();
	if (!ship)
		return false;

	ServerObject * const containingObject = NON_NULL(safe_cast<ServerObject *>(ContainerInterface::getContainedByObject(*this)));

	// If directly contained by a ship, we use the pilot slot, else we use the pob pilot slot
	SlotId const pilotSlotId = containingObject->asShipObject() ? ShipSlotIdManager::getShipPilotSlotId() : ShipSlotIdManager::getPobShipPilotSlotId();

	// If this creature is piloting the ship, he is in the pilot slot of either the ship or an object contained by a cell of the ship.
	SlottedContainer * const slottedContainer = ContainerInterface::getSlottedContainer(*containingObject);
	if (slottedContainer)
	{
		//-- Check for a pilot.
		Container::ContainerErrorCode errorCode = Container::CEC_Success;
		CachedNetworkId pilotId = slottedContainer->getObjectInSlot(pilotSlotId, errorCode);
		if (errorCode == Container::CEC_Success)
		{
			ServerObject * const pilot = safe_cast<ServerObject*>(pilotId.getObject());
			FATAL(pilot != this, ("Somehow told to unpilot a ship when we are not the pilot of the ship!"));

			// Remove ship control first so client is not in ship-control mode when receiving the transfer.
			// May prevent client crash when exiting POB (access violation during control handoff).
			Client * const client = getClient();
			if (client)
			{
				client->removeControlledObject(*ship);
				ShipClientUpdateTracker::queueForUpdate(*client, *ship);
			}

			// Transfer pilot to location of the object it was contained by, which may or may not have been the ship, but it something with a pilot slot.
			// Resolve dest cell: getAttachedTo may be the cell, or we may need to walk up (e.g. pilot seat in ship in cell).
			CellObject * destCell = dynamic_cast<CellObject *>(containingObject->getAttachedTo());
			if (!destCell)
			{
				CellProperty * const parentCell = containingObject->getParentCell();
				if (parentCell && !parentCell->isWorldCell())
					destCell = safe_cast<CellObject *>(&parentCell->getOwner());
			}
			if (destCell)
			{
				// Use player's world position converted to cell space for correct placement (avoids interior cell transform issues).
				Transform worldToCell;
				worldToCell.invert(destCell->getTransform_o2w());
				Transform tr(worldToCell.rotateTranslate_p2l(getTransform_o2w()));
				// push them back a meter as well if they are unpiloting a pob ship
				if (!containingObject->asShipObject())
					tr.move_l(Vector(0.f, 0.f, -1.f));
				// Use cell-aligned upright orientation so the player stands on the floor and the overhead map aligns correctly.
				// Ship orientation can be tilted (pitch/roll); we want yaw only, standing on the cell's floor (Y up in cell space).
				{
					Transform const & shipTransform = ship->getTransform_o2w();
					Vector kHorizontal(shipTransform.getLocalFrameK_p().x, 0.f, shipTransform.getLocalFrameK_p().z);
					if (!kHorizontal.normalize())
						kHorizontal = Vector(0.f, 0.f, 1.f);
					Transform const & cellToWorld = destCell->getTransform_o2w();
					Vector forward_cell = cellToWorld.rotate_p2l(kHorizontal);
					// Flatten to cell XZ plane so we only keep yaw (player stands on floor, Y up in cell space).
					forward_cell.y = 0.f;
					if (forward_cell.normalize())
					{
						// Cell floor is XZ with +Y up; use cell's natural up so map and geometry align.
						Vector const up_cell(Vector::unitY);
						tr.setLocalFrameKJ_p(forward_cell, up_cell);
					}
				}
				if (ContainerInterface::transferItemToCell(*destCell, *this, tr, 0, errorCode))
				{
					// Sync transform to client immediately (avoids interior cell transform desync).
					PlayerCreatureController * const pcc = dynamic_cast<PlayerCreatureController *>(getController());
					if (pcc)
						pcc->teleport(tr, safe_cast<ServerObject *>(destCell));
				}
			}
			else
			{
				Transform const & shipTransform = containingObject->getTransform_o2w();
				Transform tr = shipTransform;
				// In atmospheric flight, use upright orientation (yaw only) so the player walks forward correctly.
				if (ServerWorld::isAtmosphericFlightScene())
				{
					Vector kHorizontal(shipTransform.getLocalFrameK_p().x, 0.f, shipTransform.getLocalFrameK_p().z);
					if (!kHorizontal.normalize())
						kHorizontal = Vector(0.f, 0.f, 1.f);  // Fallback if ship pointed straight up/down
					tr.setPosition_p(shipTransform.getPosition_p());
					tr.setLocalFrameKJ_p(kHorizontal, Vector::unitY);
				}
				// World transfer: do not call teleport() immediately - can cause access violation on landing
				// when client processes control handoff. Rely on baseline/delta sync.
				IGNORE_RETURN(ContainerInterface::transferItemToWorld(*this, tr, 0, errorCode));
			}
			if (errorCode == Container::CEC_Success)
				unpilotedShip = true;
		}
	}

	return unpilotedShip;
}


//----------------------------------------------------------------------

ShipObject const *CreatureObject::getPilotedShip() const
{
	return const_cast<CreatureObject*>(this)->getPilotedShip();
}

//----------------------------------------------------------------------

ShipObject *CreatureObject::getPilotedShip()
{
	//-- Ensure ships are enabled.
	if (!ConfigServerGame::getShipsEnabled())
		return 0;

	// If we are occupying a "pilot" slot of our immediate container, then we are indirectly contained by a ship which we are piloting
	SlottedContainmentProperty const *slottedContainmentProperty = ContainerInterface::getSlottedContainmentProperty(*this);
	if (slottedContainmentProperty)
	{
		int currentArrangement = slottedContainmentProperty->getCurrentArrangement();
		if (currentArrangement != -1)
		{
			SlottedContainmentProperty::SlotArrangement const &slotArrangement = slottedContainmentProperty->getSlotArrangement(currentArrangement);
			for (SlottedContainmentProperty::SlotArrangement::const_iterator i = slotArrangement.begin(); i != slotArrangement.end(); ++i)
				if ((*i) == ShipSlotIdManager::getShipPilotSlotId() || (*i) == ShipSlotIdManager::getPobShipPilotSlotId())
					return ShipObject::getContainingShipObject(this);
		}
	}
	return 0;
}

//----------------------------------------------------------------------

void CreatureObject::getAllShipsInDatapad(std::vector<NetworkId> & result) const
{
	ServerObject const * const datapad = getDatapad();
	if(datapad)
	{
		Container const * const datapadContainer = ContainerInterface::getContainer(*datapad);
		if(datapadContainer)
		{
			//look at each item in the datapad for the ship control devices
			for(ContainerConstIterator iter = datapadContainer->begin(); iter != datapadContainer->end(); ++iter)
			{
				Object const * const itemO = (*iter).getObject();
				ServerObject const * const itemSO = itemO ? itemO->asServerObject() : nullptr;
				if(itemSO && (itemSO->getGameObjectType() == SharedObjectTemplate::GOT_data_ship_control_device))
				{
					Container const * const itemContainer = ContainerInterface::getContainer(*itemSO);
					{
						if(!itemContainer)
							continue;

						//for each ship control device, see if the item contains a shipobject
						for(ContainerConstIterator iter2 = itemContainer->begin(); iter2 != itemContainer->end(); ++iter2)
						{
							Object const * const o = (*iter2).getObject();
							ServerObject const * const so = o ? o->asServerObject() : nullptr;
							ShipObject const * const ship = so ? so->asShipObject() : nullptr;
							if(ship)
							{
								result.push_back(ship->getNetworkId());
							}
						}
					}
				}
			}
		}
	}
}

// ======================================================================

