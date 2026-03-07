// ======================================================================
//
// CalendarEventMessage.h
// Copyright 2026 Titan Project
//
// Message to save/delete calendar events to/from the database.
//
// ======================================================================

#ifndef INCLUDED_CalendarEventMessage_H
#define INCLUDED_CalendarEventMessage_H

// ======================================================================

#include "sharedNetworkMessages/GameNetworkMessage.h"
#include "sharedFoundation/NetworkId.h"
#include "Unicode.h"

#include <string>

// ======================================================================

/**
 * Message sent to the database server to save a calendar event.
 */
class SaveCalendarEventMessage : public GameNetworkMessage
{
public:
	SaveCalendarEventMessage(
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
	);
	explicit SaveCalendarEventMessage(Archive::ReadIterator & source);
	~SaveCalendarEventMessage();

	std::string const & getEventId() const { return m_eventId.get(); }
	std::string const & getTitle() const { return m_title.get(); }
	std::string const & getDescription() const { return m_description.get(); }
	int32 getEventType() const { return m_eventType.get(); }
	int32 getYear() const { return m_year.get(); }
	int32 getMonth() const { return m_month.get(); }
	int32 getDay() const { return m_day.get(); }
	int32 getHour() const { return m_hour.get(); }
	int32 getMinute() const { return m_minute.get(); }
	int32 getDuration() const { return m_duration.get(); }
	int32 getGuildId() const { return m_guildId.get(); }
	int32 getCityId() const { return m_cityId.get(); }
	std::string const & getServerEventKey() const { return m_serverEventKey.get(); }
	bool getRecurring() const { return m_recurring.get(); }
	int32 getRecurrenceType() const { return m_recurrenceType.get(); }
	bool getBroadcastStart() const { return m_broadcastStart.get(); }
	bool getActive() const { return m_active.get(); }
	NetworkId const & getCreatorId() const { return m_creatorId.get(); }

private:
	SaveCalendarEventMessage();
	SaveCalendarEventMessage(SaveCalendarEventMessage const &);
	SaveCalendarEventMessage & operator=(SaveCalendarEventMessage const &);

	Archive::AutoVariable<std::string> m_eventId;
	Archive::AutoVariable<std::string> m_title;
	Archive::AutoVariable<std::string> m_description;
	Archive::AutoVariable<int32> m_eventType;
	Archive::AutoVariable<int32> m_year;
	Archive::AutoVariable<int32> m_month;
	Archive::AutoVariable<int32> m_day;
	Archive::AutoVariable<int32> m_hour;
	Archive::AutoVariable<int32> m_minute;
	Archive::AutoVariable<int32> m_duration;
	Archive::AutoVariable<int32> m_guildId;
	Archive::AutoVariable<int32> m_cityId;
	Archive::AutoVariable<std::string> m_serverEventKey;
	Archive::AutoVariable<bool> m_recurring;
	Archive::AutoVariable<int32> m_recurrenceType;
	Archive::AutoVariable<bool> m_broadcastStart;
	Archive::AutoVariable<bool> m_active;
	Archive::AutoVariable<NetworkId> m_creatorId;
};

// ======================================================================

/**
 * Message sent to the database server to delete a calendar event.
 */
class DeleteCalendarEventMessage : public GameNetworkMessage
{
public:
	explicit DeleteCalendarEventMessage(std::string const & eventId);
	explicit DeleteCalendarEventMessage(Archive::ReadIterator & source);
	~DeleteCalendarEventMessage();

	std::string const & getEventId() const { return m_eventId.get(); }

private:
	DeleteCalendarEventMessage();
	DeleteCalendarEventMessage(DeleteCalendarEventMessage const &);
	DeleteCalendarEventMessage & operator=(DeleteCalendarEventMessage const &);

	Archive::AutoVariable<std::string> m_eventId;
};

// ======================================================================

/**
 * Message sent to the database server to save calendar settings.
 */
class SaveCalendarSettingsMessage : public GameNetworkMessage
{
public:
	SaveCalendarSettingsMessage(
		std::string const & bgTexture,
		int32 srcX,
		int32 srcY,
		int32 srcW,
		int32 srcH,
		NetworkId const & modifiedBy
	);
	explicit SaveCalendarSettingsMessage(Archive::ReadIterator & source);
	~SaveCalendarSettingsMessage();

	std::string const & getBgTexture() const { return m_bgTexture.get(); }
	int32 getSrcX() const { return m_srcX.get(); }
	int32 getSrcY() const { return m_srcY.get(); }
	int32 getSrcW() const { return m_srcW.get(); }
	int32 getSrcH() const { return m_srcH.get(); }
	NetworkId const & getModifiedBy() const { return m_modifiedBy.get(); }

private:
	SaveCalendarSettingsMessage();
	SaveCalendarSettingsMessage(SaveCalendarSettingsMessage const &);
	SaveCalendarSettingsMessage & operator=(SaveCalendarSettingsMessage const &);

	Archive::AutoVariable<std::string> m_bgTexture;
	Archive::AutoVariable<int32> m_srcX;
	Archive::AutoVariable<int32> m_srcY;
	Archive::AutoVariable<int32> m_srcW;
	Archive::AutoVariable<int32> m_srcH;
	Archive::AutoVariable<NetworkId> m_modifiedBy;
};

// ======================================================================

#endif // INCLUDED_CalendarEventMessage_H

