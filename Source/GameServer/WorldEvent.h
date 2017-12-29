#pragma once

#include <vector>
#include <chrono>

class CWorldEvent {
public:
	CWorldEvent(CGameServerDlg* gameServer);

	bool Start();
	bool Stop();
	bool SendStartNotification();
	bool SendStopNotification();
	INLINE std::vector<uint8> GetZones() {
		return m_zones;
	}

	void Tick();

	bool m_started;
	std::chrono::system_clock::time_point m_lastQuestSubmissionTime;
	std::vector<uint8> m_zones;
	CGameServerDlg* m_gameServer;
};