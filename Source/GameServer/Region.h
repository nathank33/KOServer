#pragma once

#include <set>
#include "Define.h"
#include "GameDefine.h"
#include "../shared/STLMap.h"

typedef CSTLMap <_LOOT_BUNDLE>	ZoneItemArray;
typedef std::set<uint16>    ZoneUserArray;
typedef std::set<uint16>    ZoneBotArray;
typedef std::set<uint16>    ZoneNpcArray;

class CNpc;
class CUser;
class CBot;

class CRegion  
{
public:
	std::recursive_mutex		m_lock;
	ZoneUserArray	m_RegionUserArray;
	ZoneBotArray	m_RegionBotArray;
	ZoneNpcArray	m_RegionNpcArray;
	
	void Add(CBot * pBot);
	void Remove(CBot * pBot);
	void Add(CUser * pUser);
	void Remove(CUser * pUser);
	void Add(CNpc * pNpc);
	void Remove(CNpc * pNpc);
};
