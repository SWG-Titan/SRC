// ======================================================================
//
// CalendarEventMessage.cpp
// Copyright 2026 Titan Project
//
// Message to save/delete calendar events to/from the database.
//
// ======================================================================

#include "serverNetworkMessages/FirstServerNetworkMessages.h"
#include "serverNetworkMessages/CalendarEventMessage.h"

// ======================================================================
// SaveCalendarEventMessage
// ======================================================================

SaveCalendarEventMessage::SaveCalendarEventMessage(
	std::string const & eventId,
	std::string const & title,
	std::string const & description,
	int32 eventType,
	int32 year,
	int32 month,
	int32 day,
	int32 hour,
	int32 minute,
	int32 duration,
	int32 guildId,
	int32 cityId,
	std::string const & serverEventKey,
	bool recurring,
	int32 recurrenceType,
	bool broadcastStart,
	bool active,
	NetworkId const & creatorId
) :
	GameNetworkMessage("SaveCalendarEventMessage"),
	m_eventId(eventId),
	m_title(title),
	m_description(description),
	m_eventType(eventType),
	m_year(year),
	m_month(month),
	m_day(day),
	m_hour(hour),
	m_minute(minute),
	m_duration(duration),
	m_guildId(guildId),
	m_cityId(cityId),
	m_serverEventKey(serverEventKey),
	m_recurring(recurring),
	m_recurrenceType(recurrenceType),
	m_broadcastStart(broadcastStart),
	m_active(active),
	m_creatorId(creatorId)
{
	addVariable(m_eventId);
	addVariable(m_title);
	addVariable(m_description);
	addVariable(m_eventType);
	addVariable(m_year);
	addVariable(m_month);
	addVariable(m_day);
	addVariable(m_hour);
	addVariable(m_minute);
	addVariable(m_duration);
	addVariable(m_guildId);
	addVariable(m_cityId);
	addVariable(m_serverEventKey);
	addVariable(m_recurring);
	addVariable(m_recurrenceType);
	addVariable(m_broadcastStart);
	addVariable(m_active);
	addVariable(m_creatorId);
}

// ----------------------------------------------------------------------

SaveCalendarEventMessage::SaveCalendarEventMessage(Archive::ReadIterator & source) :
	GameNetworkMessage("SaveCalendarEventMessage")
{
	addVariable(m_eventId);
	addVariable(m_title);
	addVariable(m_description);
	addVariable(m_eventType);
	addVariable(m_year);
	addVariable(m_month);
	addVariable(m_day);
	addVariable(m_hour);
	addVariable(m_minute);
	addVariable(m_duration);
	addVariable(m_guildId);
	addVariable(m_cityId);
	addVariable(m_serverEventKey);
	addVariable(m_recurring);
	addVariable(m_recurrenceType);
	addVariable(m_broadcastStart);
	addVariable(m_active);
	addVariable(m_creatorId);
	unpack(source);
}

// ----------------------------------------------------------------------

SaveCalendarEventMessage::~SaveCalendarEventMessage()
{
}

// ======================================================================
// DeleteCalendarEventMessage
// ======================================================================

DeleteCalendarEventMessage::DeleteCalendarEventMessage(std::string const & eventId) :
	GameNetworkMessage("DeleteCalendarEventMessage"),
	m_eventId(eventId)
{
	addVariable(m_eventId);
}

// ----------------------------------------------------------------------

DeleteCalendarEventMessage::DeleteCalendarEventMessage(Archive::ReadIterator & source) :
	GameNetworkMessage("DeleteCalendarEventMessage")
{
	addVariable(m_eventId);
	unpack(source);
}

// ----------------------------------------------------------------------

DeleteCalendarEventMessage::~DeleteCalendarEventMessage()
{
}

// ======================================================================
// SaveCalendarSettingsMessage
// ======================================================================

SaveCalendarSettingsMessage::SaveCalendarSettingsMessage(
	std::string const & bgTexture,
	int32 srcX,
	int32 srcY,
	int32 srcW,
	int32 srcH,
	NetworkId const & modifiedBy
) :
	GameNetworkMessage("SaveCalendarSettingsMessage"),
	m_bgTexture(bgTexture),
	m_srcX(srcX),
	m_srcY(srcY),
	m_srcW(srcW),
	m_srcH(srcH),
	m_modifiedBy(modifiedBy)
{
	addVariable(m_bgTexture);
	addVariable(m_srcX);
	addVariable(m_srcY);
	addVariable(m_srcW);
	addVariable(m_srcH);
	addVariable(m_modifiedBy);
}

// ----------------------------------------------------------------------

SaveCalendarSettingsMessage::SaveCalendarSettingsMessage(Archive::ReadIterator & source) :
	GameNetworkMessage("SaveCalendarSettingsMessage")
{
	addVariable(m_bgTexture);
	addVariable(m_srcX);
	addVariable(m_srcY);
	addVariable(m_srcW);
	addVariable(m_srcH);
	addVariable(m_modifiedBy);
	unpack(source);
}

// ----------------------------------------------------------------------

SaveCalendarSettingsMessage::~SaveCalendarSettingsMessage()
{
}

// ======================================================================

