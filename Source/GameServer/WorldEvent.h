#pragma once

#include <vector>

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

	bool m_inProgress;
	std::vector<uint8> m_zones;
	CGameServerDlg* m_gameServer;
};