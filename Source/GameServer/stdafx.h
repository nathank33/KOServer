#pragma once

#include "../shared/stdafx.h"
#include <math.h>
#include "DatabaseThread.h"
#include <boost\foreach.hpp>
#include <boost\algorithm\string.hpp>

#if defined(DEBUG)
#	define DISABLE_PLAYER_BLINKING
#endif

#include "GameServerDlg.h"

extern bool g_bRunning;