#pragma once
#include <vector>
#include "WorldEvent.h"
#include "GameServerDlg.h"

#define WORLD_EVENT_BIFROST 1

class CWorldEventManager {
public:
	CWorldEventManager(CGameServerDlg* gameServer);

	void Tick();

	bool AddEvent(uint8 eventId, std::shared_ptr<CWorldEvent> event);
	bool RemoveEvent(uint8 eventId);

	bool AddBifrostEvent();
	bool RemoveBifrostEvent();

	std::map<uint8, std::shared_ptr<CWorldEvent>> INLINE GetEvents() {
		return m_events;
	}

private:
	CGameServerDlg* m_gameServer;
	std::map<uint8, std::shared_ptr<CWorldEvent>> m_events;
};