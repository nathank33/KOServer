#pragma once
#include "WorldEvent.h"

class CBifrostEventEasy : public CWorldEvent {
public:
	CBifrostEventEasy(CGameServerDlg* gameServer) : CWorldEvent(gameServer) {};

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
};