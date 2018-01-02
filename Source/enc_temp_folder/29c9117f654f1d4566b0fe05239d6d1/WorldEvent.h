#pragma once

const uint16 MON_ASHITON = 6651;
const uint16 MON_EGO = 5851;
const uint16 MON_GLUTTON = 5951;
const uint16 MON_WRATH = 6051;
const uint16 MON_SLOTH = 6151;
const uint16 MON_LUST = 6251;
const uint16 MON_ENVY = 6351;
const uint16 MON_GREED = 6451;

const uint16 DEFAULT_COUNT = 5;
const uint16 DEFAULT_DURATION = 0;
const uint8 DEFAULT_NATION = 0;
const uint16 DEFAULT_REGEN_TIME = 30 * SECOND;

const uint8 STATE_SAVE_MOBS = 1;
const uint8 STATE_EXCHANGE = 3;
const uint8 STATE_CLEAR_MOBS = 2;

const uint8 QUEST_START_OPCODE = 7;
const uint8 QUEST_EXCHANGE_OPCODE = 7;
const uint8 QUEST_END_OPCODE = 5;

const std::chrono::seconds QUEST_SUBMISSION_DELAY(5);

struct SpawnPosition {
	float fX;
	float fZ;
	uint16 sRadius;
};

struct SpawnMonster {
	uint16 sSid;
	uint16 sCount = DEFAULT_COUNT;
	uint16 sRegenTime = DEFAULT_REGEN_TIME;
	uint16 sDuration = DEFAULT_DURATION;
	uint8 bNation = DEFAULT_NATION;
};

const std::vector<SpawnPosition> ORC_SPAWN_POSITIONS{
	SpawnPosition {1769, 1160, 50},
	SpawnPosition {1100, 1200, 50},
};

const std::vector<SpawnPosition> HUMAN_SPAWN_POSITIONS{
	SpawnPosition{100, 1160, 50},
	SpawnPosition{200, 1200, 50},
};

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

	std::chrono::system_clock::time_point GetStartTime() {
		return m_startTime;
	}

	SpawnPosition GetRandomOrcPosition();
	SpawnPosition GetRandomHumanPosition();

	CGameServerDlg* m_gameServer;
	std::default_random_engine m_rand;

private:
	bool m_started;
	std::chrono::system_clock::time_point m_startTime;
	std::chrono::system_clock::time_point m_lastQuestSubmissionTime;
};