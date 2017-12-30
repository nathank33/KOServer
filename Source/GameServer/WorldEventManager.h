#pragma once
#include <chrono>
#include <vector>
#include "WorldEvent.h"
#include "GameServerDlg.h"

#define WORLD_EVENT_BIFROST 1

const std::chrono::minutes WORLD_EVENT_DELAY(45);
const std::chrono::minutes WORLD_EVENT_DURATION(30);

class CWorldEventManager {
public:
	CWorldEventManager(CGameServerDlg* gameServer);

	void Tick();
	bool StartEvent(uint8 eventId);
	bool StopEvent(uint8 eventId);
	bool StartBifrostEvent();
	bool StopBifrostEvent();
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