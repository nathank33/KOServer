#pragma once

class UserThread
{
public:
	// Startup the database threads
	static void Startup(uint32 dwThreads);

	// Add to the queue and notify threads of activity.
	static void AddRequest(Packet pkt, uint16 SockId);

	// Main thread procedure
	
	// Shutdown threads.
	static void Shutdown();
};