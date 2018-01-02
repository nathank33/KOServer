#include "stdafx.h"
#include "BifrostEvents.h"
#include "WorldEvent.h"

bool CBifrostEventAshiton::Start() {
	if (!CWorldEvent::Start()) {
		return false;
	}
	std::uniform_int_distribution<int> distrib(1, 8);
	for (int i = 0; i < 3; i++) {
		for (auto spawnMon : m_spawnMonsters) {
			auto orcCount = distrib(m_rand);
			auto humanCount = distrib(m_rand);
			auto orcPos = GetRandomOrcPosition();
			auto humanPos = GetRandomHumanPosition();

			m_gameServer->SpawnEventNpc(spawnMon.sSid, true, ZONE_RONARK_LAND, orcPos.fX, 0, orcPos.fZ, orcCount, orcPos.sRadius, spawnMon.sDuration, spawnMon.sRegenTime, spawnMon.bNation);
			m_gameServer->SpawnEventNpc(spawnMon.sSid, true, ZONE_RONARK_LAND, humanPos.fX, 0, humanPos.fZ, humanCount, humanPos.sRadius, spawnMon.sDuration, spawnMon.sRegenTime, spawnMon.bNation);
		}
	}
	return true;
}

bool CBifrostEventAshiton::Stop() {
	if (!CWorldEvent::Stop()) {
		return false;
	}
	for (auto spawnMon : m_spawnMonsters) {
		m_gameServer->KillNpcType(spawnMon.sSid);
	}
	return true;
}