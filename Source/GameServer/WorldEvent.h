#pragma once

#include <vector>
#include <chrono>

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

const uint8 STATE_SAVE_MOBS = 1;
const uint8 STATE_EXCHANGE = 3;
const uint8 STATE_CLEAR_MOBS = 2;

const uint8 QUEST_START_OPCODE = 7;
const uint8 QUEST_EXCHANGE_OPCODE = 7;
const uint8 QUEST_END_OPCODE = 5;

const std::chrono::seconds QUEST_SUBMISSION_DELAY(5);

class CWorldEvent {
public:
	CWorldEvent(CGameServerDlg* gameServer);

	virtual bool Start();
	virtual bool Stop();
	virtual bool StartUser(CUser* pUser);
	virtual bool StopUser(CUser* pUser);
	virtual void Tick();

	virtual uint16 GetQuestStartId() = 0;
	virtual uint16 GetQuestExchangeId() = 0;
	virtual uint16 GetQuestEndId() = 0;

	INLINE std::chrono::system_clock::time_point GetStartTime() {
		return m_startTime;
	}

	CGameServerDlg* m_gameServer;

private:
	bool m_started;
	std::chrono::system_clock::time_point m_startTime;
	std::chrono::system_clock::time_point m_lastQuestSubmissionTime;
	std::vector<uint8> m_zones;
};