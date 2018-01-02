#pragma once
#include "WorldEvent.h"
#include "GameServerDlg.h"

#define WORLD_EVENT_BIFROST_ASHITON 1
#define WORLD_EVENT_BIFROST_WRATH 2
#define WORLD_EVENT_BIFROST_ENVY 3

const std::chrono::minutes WORLD_EVENT_DELAY(45);
const std::chrono::minutes WORLD_EVENT_DURATION(30);

class CWorldEventManager {
public:
	CWorldEventManager(CGameServerDlg* gameServer);

	void Tick();
	bool StartEvent(uint8 eventId);
	bool StopEvent(uint8 eventId);
	bool StartBifrostEventAshiton();
	bool StopBifrostEventAshiton();
	bool StartBifrostEventWrath();
	bool StopBifrostEventWrath();
	bool StartBifrostEventEnvy();
	bool StopBifrostEventEnvy();
	uint8 GetRandomEvent();
	/* Closes quest windows that might still be on the users screen, if they logged out
	while participating in an event. */
	void RefreshEvents(CUser* pUser);

private:
	CGameServerDlg* m_gameServer;
	std::map<uint8, std::shared_ptr<CWorldEvent>> m_events;
	std::map<uint8, std::shared_ptr<CWorldEvent>> m_startedEvents;
	std::chrono::system_clock::time_point m_lastEventStartTime;
};