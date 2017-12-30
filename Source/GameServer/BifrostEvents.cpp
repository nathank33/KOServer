#include "stdafx.h"
#include "BifrostEvents.h"
#include "WorldEvent.h"

bool CBifrostEventEasy::Start() {
	if (!CWorldEvent::Start()) {
		return false;
	}
	m_gameServer->SpawnEventNpc(MON_GREED, true, ZONE_RONARK_LAND, 1769, 0, 1160, SPAWN_COUNT, 50, SPAWN_DURATION, SPAWN_REGEN, SPAWN_NATION);
	return true;
}

bool CBifrostEventEasy::Stop() {
	if (!CWorldEvent::Stop()) {
		return false;
	}
	m_gameServer->KillNpcType(MON_GREED);
	return true;
}