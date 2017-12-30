#pragma once

#include <vector>
#include <chrono>

class CWorldEvent {
public:
	CWorldEvent(CGameServerDlg* gameServer);

	bool Start();
	bool Stop();
	bool StartUser(CUser* pUser);
	bool StopUser(CUser* pUser);
	void Tick();
	INLINE std::chrono::system_clock::time_point GetStartTime() {
		return m_startTime;
	}

private:
	bool m_started;
	std::chrono::system_clock::time_point m_startTime;
	std::chrono::system_clock::time_point m_lastQuestSubmissionTime;
	std::vector<uint8> m_zones;
	CGameServerDlg* m_gameServer;
};