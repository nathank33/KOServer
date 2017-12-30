#include "stdafx.h"
#include "WorldEvent.h"

CWorldEvent::CWorldEvent(CGameServerDlg* gameServer) {
	m_gameServer = gameServer;
}

bool CWorldEvent::Start() {
	m_startTime = std::chrono::system_clock::now();
	m_lastQuestSubmissionTime = std::chrono::system_clock::now();
	m_started = true;
	SessionMap sessMap = m_gameServer->m_socketMgr.GetActiveSessionMap();
	for (auto sessNum : sessMap) {
		CUser *pUser = TO_USER(sessNum.second);
		StartUser(pUser);
	}
	return true;
}

bool CWorldEvent::Stop() {
	m_started = false;
	SessionMap sessMap = m_gameServer->m_socketMgr.GetActiveSessionMap();
	for (auto sessNum : sessMap) {
		CUser *pUser = TO_USER(sessNum.second);
		StopUser(pUser);
	}
	return true;
}

bool CWorldEvent::StartUser(CUser* pUser) {
	if (!pUser->isInGame()) {
		return false;
	}
	pUser->V3_QuestProcessHelper(QUEST_END_OPCODE, GetQuestEndId());
	pUser->V3_QuestProcessHelper(QUEST_START_OPCODE, GetQuestStartId());
	return true;
}

bool CWorldEvent::StopUser(CUser* pUser) {
	if (!pUser->isInGame()) {
		return false;
	}
	pUser->V3_QuestProcessHelper(QUEST_END_OPCODE, GetQuestEndId());
	return true;
}

void CWorldEvent::Tick() {
	if (m_started) {
		if (m_lastQuestSubmissionTime + QUEST_SUBMISSION_DELAY
			<= std::chrono::system_clock::now()) {
			m_lastQuestSubmissionTime = std::chrono::system_clock::now();

			SessionMap sessMap = m_gameServer->m_socketMgr.GetActiveSessionMap();
			for (auto sessNum : sessMap) {
				CUser *pUser = TO_USER(sessNum.second);
				if (!pUser->isInGame()) {
					continue;
				}
				pUser->V3_QuestProcessHelper(QUEST_EXCHANGE_OPCODE, GetQuestExchangeId());
			}
		}
	}
}