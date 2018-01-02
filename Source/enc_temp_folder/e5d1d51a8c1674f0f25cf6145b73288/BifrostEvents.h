#pragma once
#include "WorldEvent.h"

class CBifrostEventAshiton : public CWorldEvent {
public:
	CBifrostEventAshiton(CGameServerDlg* gameServer) : CWorldEvent(gameServer) {};

	uint16 GetQuestStartId() {
		return 20000;
	}
	uint16 GetQuestExchangeId() {
		return 20005;
	}
	uint16 GetQuestEndId() {
		return 20000;
	}

	bool Start();
	bool Stop();

	std::vector<SpawnMonster> m_spawnMonsters{
		SpawnMonster{MON_ASHITON},
		SpawnMonster{MON_EGO},
		SpawnMonster{MON_GLUTTON}
	};
};