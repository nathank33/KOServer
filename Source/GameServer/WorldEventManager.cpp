#include "stdafx.h"
#include "WorldEventManager.h"

CWorldEventManager::CWorldEventManager(CGameServerDlg* gameServer) {
	m_gameServer = gameServer;
}

void CWorldEventManager::Tick() {
	for (auto eventPair : m_events) {
		eventPair.second->Tick();
	}
}

bool CWorldEventManager::AddEvent(uint8 eventId, std::shared_ptr<CWorldEvent> event) {
	if (m_events.find(eventId) != m_events.end()) {
		// One of these types of events has already been started.
		return false;
	}
	m_events.insert(std::make_pair(eventId, event));
	event->Start();
	event->SendStartNotification();
	return true;
}

bool CWorldEventManager::RemoveEvent(uint8 eventId) {
	auto iter = m_events.find(eventId);
	if (iter == m_events.end()) {
		// Could not find the event.
		return false;
	}
	auto event = iter->second;
	event->Stop();
	event->SendStopNotification();
	m_events.erase(iter);
	return true;
}

bool CWorldEventManager::AddBifrostEvent() {
	return AddEvent(WORLD_EVENT_BIFROST, std::make_shared<CWorldEvent>(m_gameServer));
}

bool CWorldEventManager::RemoveBifrostEvent() {
	return RemoveEvent(WORLD_EVENT_BIFROST);
}