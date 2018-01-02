#include "stdafx.h"
#include "WorldEventManager.h"
#include "BifrostEvents.h"

std::default_random_engine s_randEngine;

CWorldEventManager::CWorldEventManager(CGameServerDlg* gameServer) {
	m_gameServer = gameServer;
	m_events.insert(std::make_pair(WORLD_EVENT_BIFROST_ASHITON, std::make_shared<CBifrostEventAshiton>(m_gameServer)));
	m_events.insert(std::make_pair(WORLD_EVENT_BIFROST_WRATH, std::make_shared<CBifrostEventAshiton>(m_gameServer)));
	m_events.insert(std::make_pair(WORLD_EVENT_BIFROST_ENVY, std::make_shared<CBifrostEventAshiton>(m_gameServer)));
	m_lastEventStartTime = std::chrono::system_clock::time_point::min();
}

void CWorldEventManager::Tick() {
	// Try to start a new event.
	if (m_startedEvents.size() == 0
		&& std::chrono::system_clock::now() >= m_lastEventStartTime + WORLD_EVENT_DELAY
		&& m_gameServer->m_aiServerConnected) {
		// Some events spawn event monsters. We need the AIServer to be ready to do so.
		StartEvent(GetRandomEvent());
	}
	// Try to stop any started events. This needs to be a range-based for loop
	// because the stop method may delete elements.
	for (auto it = m_startedEvents.begin(); it != m_startedEvents.end();) {
		auto eventId = it->first;
		auto event = it->second;
		if (std::chrono::system_clock::now() >= event->GetStartTime() + WORLD_EVENT_DURATION) {
			StopEvent(eventId);
		} else {
			it++;
		}
	}

	// Tick any started events.
	for (auto eventPair : m_startedEvents) {
		eventPair.second->Tick();
	}
}

bool CWorldEventManager::StartEvent(uint8 eventId) {
	auto eventsIter = m_events.find(eventId);
	if (eventsIter == m_events.end()) {
		// The event was never created in the constructor.
		printf("CWorldEventManager::StartEvent could not find eventId %d\n", eventId);
		return false;
	} else if (m_startedEvents.find(eventId) != m_startedEvents.end()) {
		// One of these types of events has already been started.
		return false;
	}

	auto event = eventsIter->second;
	m_startedEvents.insert(std::make_pair(eventId, event));
	event->Start();
	m_lastEventStartTime = std::chrono::system_clock::now();
	printf("CWorldManager Starting Event %d\n", eventId);
	return true;
}

bool CWorldEventManager::StopEvent(uint8 eventId) {
	auto iter = m_startedEvents.find(eventId);
	if (iter == m_startedEvents.end()) {
		// Could not find the event.
		return false;
	}
	auto event = iter->second;
	event->Stop();
	m_startedEvents.erase(iter);
	return true;
}

uint8 CWorldEventManager::GetRandomEvent() {
	std::uniform_int_distribution<int> randDistribution(0, m_events.size() - 1);
	auto randomInt = randDistribution(s_randEngine);
	auto randomIter = std::next(std::begin(m_events), randomInt);
	return randomIter->first;
}

void CWorldEventManager::RefreshEvents(CUser* pUser) {
	for (auto eventPair : m_events) {
		if (m_startedEvents.find(eventPair.first) == m_startedEvents.end()) {
			// The event has not been started, so we should make sure this event is
			// cleared up for this user.
			eventPair.second->StopUser(pUser);
		} else {
			// The event was started, make sure they can do it as well.
			eventPair.second->StartUser(pUser);
		}
	}
}

bool CWorldEventManager::StartBifrostEventAshiton() {
	return StartEvent(WORLD_EVENT_BIFROST_ASHITON);
}

bool CWorldEventManager::StopBifrostEventAshiton() {
	return StopEvent(WORLD_EVENT_BIFROST_ASHITON);
}

bool CWorldEventManager::StartBifrostEventWrath() {
	return StartEvent(WORLD_EVENT_BIFROST_WRATH);
}

bool CWorldEventManager::StopBifrostEventWrath() {
	return StopEvent(WORLD_EVENT_BIFROST_WRATH);
}

bool CWorldEventManager::StartBifrostEventEnvy() {
	return StartEvent(WORLD_EVENT_BIFROST_ENVY);
}

bool CWorldEventManager::StopBifrostEventEnvy() {
	return StopEvent(WORLD_EVENT_BIFROST_ENVY);
}