#include "stdafx.h"
#include "WorldEvent.h"

const uint16 MON_ASHITON = 6651;
const uint16 MON_EGO = 5851;
const uint16 MON_GLUTTON = 5951;
const uint16 MON_WRATCH = 6051;
const uint16 MON_SLOTH = 6151;
const uint16 MON_LUST = 6251;
const uint16 MON_ENVY = 6351;
const uint16 MON_GREED = 6451;

const uint16 SPAWN_COUNT = 10;
const uint16 SPAWN_DURATION = 0;
const uint16 SPAWN_RADIUS = 50;
const uint16 SPAWN_NATION = 0;
const uint16 SPAWN_REGEN = 30 * SECOND;

const uint16 QUEST_START = 20000;
const uint16 QUEST_END = 20010;

const uint8 STATE_SAVE_MOBS = 1;
const uint8 STATE_CLEAR_MOBS = 2;

CWorldEvent::CWorldEvent(CGameServerDlg* gameServer) {
	m_gameServer = gameServer;
	m_zones = {ZONE_RONARK_LAND};
}

bool CWorldEvent::Start() {
	m_gameServer->SpawnEventNpc(MON_GREED, true, ZONE_RONARK_LAND, 1769, 0, 1160, SPAWN_COUNT, 50, SPAWN_DURATION, SPAWN_REGEN, SPAWN_NATION);
	return true;
}

bool CWorldEvent::Stop() {
	m_gameServer->KillNpcType(MON_GREED);
	return true;
}

bool CWorldEvent::SendStartNotification() {
	SessionMap sessMap = m_gameServer->m_socketMgr.GetActiveSessionMap();
	for (auto sessNum : sessMap) {
		CUser *pUser = TO_USER(sessNum.second);
		if (!pUser->isInGame()) {
			continue;
		}
		pUser->V3_QuestEvent(QUEST_START, STATE_SAVE_MOBS);
	}
	return true;
}

bool CWorldEvent::SendStopNotification() {
	SessionMap sessMap = m_gameServer->m_socketMgr.GetActiveSessionMap();
	for (auto sessNum : sessMap) {
		CUser *pUser = TO_USER(sessNum.second);
		if (!pUser->isInGame()) {
			continue;
		}
		pUser->V3_QuestEvent(QUEST_END, STATE_CLEAR_MOBS);
	}
	return true;
}