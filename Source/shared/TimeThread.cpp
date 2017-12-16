#include "stdafx.h"
#include <time.h>
#include "TimeThread.h"

time_t UNIXTIME;
tm g_localTime;

static Thread s_timeThread;
static bool g_bRunningThread;

void StartTimeThread() {
	UNIXTIME = time(nullptr);
	g_localTime = *localtime(&UNIXTIME);
	s_timeThread.start(TimeThread);
}

void CleanupTimeThread() {
	printf("Waiting for time thread to shutdown...");
	g_bRunningThread = false;
	s_timeThread.waitForExit();
	printf(" done.\n");
}

uint32 THREADCALL TimeThread(void * lpParam) {
	g_bRunningThread = true;
	while (g_bRunningThread) {
		auto t = time(nullptr);
		if (UNIXTIME != t) {
			UNIXTIME = t;
			g_localTime = *localtime(&t);
		}

		sleep(1000);
	}

	return 0;
}