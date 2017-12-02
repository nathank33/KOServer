#include "stdafx.h"

#include "Map.h"
#include "GameEvent.h"

#include "KnightsManager.h"
#include "DBAgent.h"
#include "KingSystem.h"

#include "../shared/database/OdbcRecordset.h"
#include "../shared/database/ItemTableSet.h"
#include "../shared/database/BotTableSet.h"
#include "../shared/database/SetItemTableSet.h"
#include "../shared/database/ItemMiningSet.h"
#include "../shared/database/ItemExchangeSet.h"
#include "../shared/database/ItemUpgradeSet.h"
#include "../shared/database/ItemOpSet.h"
#include "../shared/database/MagicTableSet.h"
#include "../shared/database/MagicType1Set.h"
#include "../shared/database/MagicType2Set.h"
#include "../shared/database/MagicType3Set.h"
#include "../shared/database/MagicType4Set.h"
#include "../shared/database/MagicType5Set.h"
#include "../shared/database/MagicType6Set.h"
#include "../shared/database/MagicType7Set.h"
#include "../shared/database/MagicType8Set.h"
#include "../shared/database/MagicType9Set.h"
#include "../shared/database/ObjectPosSet.h"
#include "../shared/database/ZoneInfoSet.h"
#include "../shared/database/EventSet.h"
#include "../shared/database/CoefficientSet.h"
#include "../shared/database/LevelUpTableSet.h"
#include "../shared/database/PetLevelUpTableSet.h"
#include "../shared/database/ServerResourceSet.h"
#include "../shared/database/QuestHelperSet.h"
#include "../shared/database/QuestMonsterSet.h"
#include "../shared/database/KnightsSet.h"
#include "../shared/database/ReKnightsSet.h"
#include "../shared/database/KnightsUserSet.h"
#include "../shared/database/KnightsAllianceSet.h"
#include "../shared/database/KnightsRankSet.h"
#include "../shared/database/KnightsCapeSet.h"
#include "../shared/database/UserPersonalRankSet.h"
#include "../shared/database/UserKnightsRankSet.h"
#include "../shared/database/StartPositionSet.h"
#include "../shared/database/StartPositionRandomSet.h"
#include "../shared/database/BattleSet.h"
#include "../shared/database/RentalItemSet.h"
#include "../shared/database/KingSystemSet.h"
#include "../shared/database/KingCandidacyNoticeBoardSet.h"
#include "../shared/database/KingElectionListSet.h"
#include "../shared/database/EventTriggerSet.h"
#include "../shared/database/MonsterChallenge.h"
#include "../shared/database/EventTimes.h"
#include "../shared/database/MonsterChallengeSummonList.h"
#include "../shared/database/MonsterSummonListSet.h"
#include "../shared/database/MonsterSummonListZoneSet.h"
#include "../shared/database/MonsterRespawnListSet.h"
#include "../shared/database/MonsterRespawnListInformationSet.h"
#include "../shared/database/MonsterRespawnListRandomSet.h"
#include "../shared/database/PremiumItemSet.h"
#include "../shared/database/PremiumItemExpSet.h"
#include "../shared/database/PetTableSet.h"
#include "../shared/database/CypherTableSet.h"
#include "../shared/database/UserDailyOpSet.h"
#include "../shared/database/UserItemSet.h"
#include "../shared/database/KnightsSiegeWar.h"
#include "../shared/database/AchieveComSet.h"
#include "../shared/database/AchieveWarSet.h"
#include "../shared/database/AchieveTitleSet.h"
#include "../shared/database/AchieveMainSet.h"
#include "../shared/database/AchieveNormalSet.h"
#include "../shared/database/AchieveMonsterSet.h"
#include "../shared/database/ItemCrashSet.h"
#include "../shared/database/ItemMixSet.h"
#include "../shared/database/ItemDuper.h"
#include "../shared/database/IlegalItems.h"
#include "../shared/database/ItemExchangeExpSet.h"

bool CGameServerDlg::LoadItemCrashTable()
{
	LOAD_TABLE(CItemCrashSet,g_DBAgent.m_GameDB,&m_ItemCrashArray,true,false);
}

bool CGameServerDlg::LoadItemTable()
{
	LOAD_TABLE(CItemTableSet, g_DBAgent.m_GameDB, &m_ItemtableArray, false, false);
}

bool CGameServerDlg::LoadItemDuper()
{
	LOAD_TABLE(CItemDuper, g_DBAgent.m_GameDB, &m_ItemDupersArray,true, false);
}

bool CGameServerDlg::LoadIlegalItems()
{
	LOAD_TABLE(CIlegalItems, g_DBAgent.m_GameDB, &m_IlegalItemsArray,true, false);
}

bool CGameServerDlg::LoadSetItemTable()
{
	LOAD_TABLE(CSetItemTableSet, g_DBAgent.m_GameDB, &m_SetItemArray,true, false);
}

bool CGameServerDlg::LoadAchieveMonsterTable()
{
	LOAD_TABLE(CAchieveMonsterSet, g_DBAgent.m_GameDB, &ACHIEVE_MONSTER,true, false);
}

bool CGameServerDlg::LoadAchieveNormalTable()
{
	LOAD_TABLE(CAchieveNormalSet, g_DBAgent.m_GameDB, &ACHIEVE_NORMAL,true, false);
}

bool CGameServerDlg::LoadAchieveTitleTable()
{
	LOAD_TABLE(CAchieveTitleSet, g_DBAgent.m_GameDB, &ACHIEVE_TITLE,true, false);
}

bool CGameServerDlg::LoadAchieveWarTable()
{
	LOAD_TABLE(CAchieveWarSet, g_DBAgent.m_GameDB, &ACHIEVE_WAR,true, false);
}

bool CGameServerDlg::LoadAchieveMainTable()
{
	LOAD_TABLE(CAchieveMainSet, g_DBAgent.m_GameDB, &ACHIEVE_MAIN,true, false);
}

bool CGameServerDlg::LoadAchieveComTable()
{
	LOAD_TABLE(CAchieveComSet, g_DBAgent.m_GameDB, &ACHIEVE_COM,true, false);
}

bool CGameServerDlg::LoadKnightsSiegeWarsTable()
{
	LOAD_TABLE(CKnightsSiegeWarfare, g_DBAgent.m_GameDB, &m_KnightsSiegeWarfareArray,true, false);
}
bool CGameServerDlg::LoadItemExchangeTable()
{
	LOAD_TABLE(CItemExchangeSet, g_DBAgent.m_GameDB, &m_ItemExchangeArray,true, false);
}

bool CGameServerDlg::LoadItemMixTable()
{
	LOAD_TABLE(CItemMixSet, g_DBAgent.m_GameDB, &m_ItemMixArray,true, false);
}

bool CGameServerDlg::LoadItemExchangeExpTable()
{
	LOAD_TABLE(CItemExchangeExpSet, g_DBAgent.m_GameDB, &m_ItemExchangeExpArray,true, false);
}

uint8 CGameServerDlg::GetTariffByZone(int zoneid)
{

	_KNIGHTS_SIEGE_WARFARE *pSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(1);
	CKingSystem *pKingSystemK = g_pMain->m_KingSystemArray.GetData(1);
	CKingSystem *pKingSystemH = g_pMain->m_KingSystemArray.GetData(2);
	
		switch (zoneid)
		{
		case ZONE_KARUS:
			if (pKingSystemK != nullptr)
				return (10 + pKingSystemK->m_nTerritoryTariff);
			else 
				return 10;
			break;
		case ZONE_KARUS_ESLANT:
			if (pKingSystemK != nullptr)
				return (10 + pKingSystemK->m_nTerritoryTariff);
			else 
				return 10;
			break;
		case ZONE_ELMORAD:
			if (pKingSystemH != nullptr)
				return (10 + pKingSystemH->m_nTerritoryTariff);
			else 
				return 10;
			break;
		case ZONE_ELMORAD_ESLANT:
			if (pKingSystemH != nullptr){
				return (10 + pKingSystemH->m_nTerritoryTariff);
			}else 
				return 10;
			break;
		case ZONE_MORADON:
			if (pSiegeWar != nullptr){
				return (uint8)pSiegeWar->sMoradonTariff;
			}else 
				return 0;
			break;
		case ZONE_DELOS:
			if (pSiegeWar != nullptr){
				return (uint8)pSiegeWar->sDellosTariff;
			}else 
				return 0;
			break;
		default:
			break;
		}
	
	return 0;
}
bool CGameServerDlg::LoadItemMiningTable()
{
	
	memset(&bRandArrayNormalMattock, 0, sizeof(bRandArrayNormalMattock)); 
	memset(&bRandArrayGoldenMattock, 0, sizeof(bRandArrayGoldenMattock)); 

	g_pMain->TotalMiningExchangeRate = 0;
	g_pMain->TotalGoldenMiningExchangeRate = 0;

	LOAD_TABLE(CItemMiningSet, g_DBAgent.m_GameDB, &m_ItemMiningArray,true, false);

	
	if (g_pMain->TotalMiningExchangeRate != 10000)
		printf("WARNING : Total Mining Exchange Rate(ITEM_MINING) have to be [10000]!\n");

	if (g_pMain->TotalGoldenMiningExchangeRate != 10000)
		printf("WARNING : Total Golden Mining Exchange Rate(ITEM_MINING) have to be [10000]!\n");

}

bool CGameServerDlg::LoadItemUpgradeTable()
{
	LOAD_TABLE(CItemUpgradeSet, g_DBAgent.m_GameDB, &m_ItemUpgradeArray, false, false);
}

bool CGameServerDlg::LoadItemOpTable()
{
	LOAD_TABLE(CItemOpSet, g_DBAgent.m_GameDB, &m_ItemOpArray,true, false);
}

bool CGameServerDlg::LoadServerResourceTable()
{
	LOAD_TABLE(CServerResourceSet, g_DBAgent.m_GameDB, &m_ServerResourceArray, false, false);
}

bool CGameServerDlg::LoadQuestHelperTable()
{
	Guard lock(m_questNpcLock);
	m_QuestNpcList.clear();
	LOAD_TABLE(CQuestHelperSet, g_DBAgent.m_GameDB, &m_QuestHelperArray,true, false);
}

bool CGameServerDlg::LoadQuestMonsterTable()
{
	LOAD_TABLE(CQuestMonsterSet, g_DBAgent.m_GameDB, &m_QuestMonsterArray,true, false);
}

bool CGameServerDlg::LoadMagicTable()
{
	LOAD_TABLE(CMagicTableSet, g_DBAgent.m_GameDB, &m_MagictableArray, false, false);
}

bool CGameServerDlg::LoadMagicType1()
{
	LOAD_TABLE(CMagicType1Set, g_DBAgent.m_GameDB, &m_Magictype1Array, false, false);
}

bool CGameServerDlg::LoadMagicType2()
{
	LOAD_TABLE(CMagicType2Set, g_DBAgent.m_GameDB, &m_Magictype2Array, false, false);
}

bool CGameServerDlg::LoadMagicType3()
{
	LOAD_TABLE(CMagicType3Set, g_DBAgent.m_GameDB, &m_Magictype3Array, false, false);
}

bool CGameServerDlg::LoadMagicType4()
{
	LOAD_TABLE(CMagicType4Set, g_DBAgent.m_GameDB, &m_Magictype4Array, false, false);
}

bool CGameServerDlg::LoadMagicType5()
{
	LOAD_TABLE(CMagicType5Set, g_DBAgent.m_GameDB, &m_Magictype5Array, false, false);
}

bool CGameServerDlg::LoadMagicType6()
{
	LOAD_TABLE(CMagicType6Set, g_DBAgent.m_GameDB, &m_Magictype6Array, false, false);
}

bool CGameServerDlg::LoadMagicType7()
{
	LOAD_TABLE(CMagicType7Set, g_DBAgent.m_GameDB, &m_Magictype7Array, false, false);
}

bool CGameServerDlg::LoadMagicType8()
{
	LOAD_TABLE(CMagicType8Set, g_DBAgent.m_GameDB, &m_Magictype8Array, false, false);
}

bool CGameServerDlg::LoadMagicType9()
{
	LOAD_TABLE(CMagicType9Set, g_DBAgent.m_GameDB, &m_Magictype9Array, false, false);
}

bool CGameServerDlg::LoadRentalList()
{
	LOAD_TABLE(CRentalItemSet, g_DBAgent.m_GameDB, &m_RentalItemArray,true, false);
}

bool CGameServerDlg::LoadCoefficientTable()
{
	LOAD_TABLE(CCoefficientSet, g_DBAgent.m_GameDB, &m_CoefficientArray, false, false);
}

bool CGameServerDlg::LoadLevelUpTable()
{
	LOAD_TABLE(CLevelUpTableSet, g_DBAgent.m_GameDB, &m_LevelUpArray, false, false);
}

bool CGameServerDlg::LoadPetLevelUpTable()
{
	LOAD_TABLE(CPetLevelUpTableSet, g_DBAgent.m_GameDB, &m_PetLevelUpArray, false, false);
}

bool CGameServerDlg::LoadAllKnights(bool bIsSlient)
{
	Guard lock(m_KnightsArray.m_lock);
	LOAD_TABLE(CKnightsSet, g_DBAgent.m_GameDB, &m_KnightsArray,true, bIsSlient);
}

bool CGameServerDlg::ReloadAllKnights(bool bIsSlient)
{
	Guard lock(m_KnightsArray.m_lock);
	LOAD_TABLE(CReKnightsSet, g_DBAgent.m_GameDB, &m_KnightsArray,true, bIsSlient);
}

bool CGameServerDlg::LoadAllKnightsUserData(bool bIsSlient)
{
	LOAD_TABLE(CKnightsUserSet, g_DBAgent.m_GameDB, nullptr,true, bIsSlient);
}

bool CGameServerDlg::LoadKnightsAllianceTable(bool bIsSlient)
{
	LOAD_TABLE(CKnightsAllianceSet, g_DBAgent.m_GameDB, &m_KnightsAllianceArray,true, bIsSlient);
}

bool CGameServerDlg::LoadUserRankings()
{
	CUserPersonalRankSet UserPersonalRankSet(g_DBAgent.m_GameDB, &m_UserPersonalRankMap);
	CUserKnightsRankSet  UserKnightsRankSet(g_DBAgent.m_GameDB, &m_UserKnightsRankMap);
	TCHAR * szError = nullptr;

	// Cleanup first, in the event it's already loaded (we'll have this automatically reload in-game)
	CleanupUserRankings();

	// Acquire the lock for thread safety, and load both tables.
	//Guard lock(m_userRankingsLock);

	szError = UserPersonalRankSet.Read(true);
	if (szError != nullptr)
	{
		printf("ERROR: Failed to load personal rankings, error:\n%s\n", szError);
		return false;
	}

	szError = UserKnightsRankSet.Read(true);
	if (szError != nullptr)
	{
		printf("ERROR: Failed to load user knights rankings, error:\n%s\n", szError);
		return false;
	}

	foreach (itr, m_UserKnightsRankMap)
	{
		CUser *pUser = GetUserPtr(itr->first, TYPE_CHARACTER);
		if (pUser == nullptr)
			continue;

		GetUserRank(pUser);

	}

	foreach (itr, m_UserPersonalRankMap)
	{
		CUser *pUser = GetUserPtr(itr->first, TYPE_CHARACTER);
		if (pUser == nullptr)
			continue;

		GetUserRank(pUser);
	}

	return true;
}

void CGameServerDlg::CleanupUserRankings()
{
	std::set<_USER_RANK *> deleteSet, DELETESET2;
	Guard lock(m_userRankingsLock);

	// Go through the personal rank map, reset the character's rank and insert
	// the _USER_RANK struct instance into the deletion set for later.
	foreach (itr, m_UserPersonalRankMap)
	{
		CUser *pUser = GetUserPtr(itr->first, TYPE_CHARACTER);
		if (pUser != nullptr)
			pUser->m_bPersonalRank = -1;

		deleteSet.insert(itr->second);
	}

	// Go through the knights rank map, reset the character's rank and insert
	// the _USER_RANK struct instance into the deletion set for later.
	foreach (itr, m_UserKnightsRankMap)
	{
		CUser *pUser = GetUserPtr(itr->first, TYPE_CHARACTER);
		if (pUser != nullptr)
			pUser->m_bKnightsRank = -1;

		DELETESET2.insert(itr->second);
	}

	// Clear out the maps
	m_UserPersonalRankMap.clear();
	m_UserKnightsRankMap.clear();

	// Free the memory used by the _USER_RANK structs
	foreach (itr, deleteSet)
		delete *itr;

	foreach (itr, DELETESET2)
		delete *itr;
	
	// These only store pointers to memory that was already freed by the primary rankings maps.
	m_playerRankings[KARUS_ARRAY].clear();
	m_playerRankings[ELMORAD_ARRAY].clear();
	m_playerPersonalRankings[KARUS_ARRAY].clear();
	m_playerPersonalRankings[ELMORAD_ARRAY].clear();
}

bool CGameServerDlg::LoadKnightsCapeTable()
{
	LOAD_TABLE(CKnightsCapeSet, g_DBAgent.m_GameDB, &m_KnightsCapeArray, false, false);
}

bool CGameServerDlg::LoadKnightsRankTable(bool bWarTime /*= false*/, bool bIsSlient /*= false*/)
{
	std::string strKarusCaptainNames, strElmoCaptainNames;
	LOAD_TABLE_ERROR_ONLY(CKnightsRankSet, g_DBAgent.m_GameDB, nullptr, true, bIsSlient);

	BoardCache[0].clear();
	BoardCache[1].clear();


	if (!bWarTime)
		return true;

	CKnightsRankSet & pSet = _CKnightsRankSet; // kind ugly generic naming

	if (pSet.nKarusCount > 0)
	{
		Packet result;
		GetServerResource(IDS_KARUS_CAPTAIN, &strKarusCaptainNames, 
			pSet.strKarusCaptain[0], pSet.strKarusCaptain[1], pSet.strKarusCaptain[2], 
			pSet.strKarusCaptain[3], pSet.strKarusCaptain[4]);
		ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &strKarusCaptainNames);
		Send_All(&result, nullptr, KARUS);
	}

	if (pSet.nElmoCount > 0)
	{
		Packet result;
		GetServerResource(IDS_ELMO_CAPTAIN, &strElmoCaptainNames,
			pSet.strElmoCaptain[0], pSet.strElmoCaptain[1], pSet.strElmoCaptain[2], 
			pSet.strElmoCaptain[3], pSet.strElmoCaptain[4]);
		ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &strElmoCaptainNames);
		Send_All(&result, nullptr, ELMORAD);
	}

	return true;
}

bool CGameServerDlg::LoadStartPositionTable()
{
	LOAD_TABLE(CStartPositionSet, g_DBAgent.m_GameDB, &m_StartPositionArray, false, false);
}

bool CGameServerDlg::LoadBattleTable()
{
	LOAD_TABLE(CBattleSet, g_DBAgent.m_GameDB, &m_byOldVictory,true, false);
}

bool CGameServerDlg::LoadKingSystem()
{
	LOAD_TABLE_ERROR_ONLY(CKingSystemSet, g_DBAgent.m_GameDB, &m_KingSystemArray,true, false);
	LOAD_TABLE_ERROR_ONLY(CKingCandidacyNoticeBoardSet, g_DBAgent.m_GameDB, &m_KingSystemArray,true, false);
	LOAD_TABLE(CKingElectionListSet, g_DBAgent.m_GameDB, &m_KingSystemArray,true, false);
}

bool CGameServerDlg::LoadMonsterChallengeTable()
{
	LOAD_TABLE(CMonsterChallenge, g_DBAgent.m_GameDB, &m_MonsterChallengeArray,true, false);
}

bool CGameServerDlg::LoadEventTimesTable()
{
	LOAD_TABLE(CEventTimes, g_DBAgent.m_GameDB, &m_EventTimesArray,true, false);
}

bool CGameServerDlg::LoadMonsterChallengeSummonListTable()
{
	LOAD_TABLE(CMonsterChallengeSummonList, g_DBAgent.m_GameDB, &m_MonsterChallengeSummonListArray,true, false);
}

bool CGameServerDlg::LoadMonsterSummonListTable()
{
	LOAD_TABLE(CMonsterSummonListSet, g_DBAgent.m_GameDB, &m_MonsterSummonList,true, false);
}

bool CGameServerDlg::LoadMonsterSummonListZoneTable()
{
	LOAD_TABLE(CMonsterSummonListZoneSet, g_DBAgent.m_GameDB, &m_MonsterSummonListZoneArray,true, false);
}

bool CGameServerDlg::LoadMonsterRespawnListTable()
{
	LOAD_TABLE(CMonsterRespawnListSet, g_DBAgent.m_GameDB, &m_MonsterRespawnListArray,true, false);
}

bool CGameServerDlg::LoadMonsterRespawnListInformationTable()
{
	LOAD_TABLE(CMonsterRespawnListInformationSet, g_DBAgent.m_GameDB, &m_MonsterRespawnListInformationArray,true, false);
}

bool CGameServerDlg::LoadMonsterRespawnListRandomTable()
{
	LOAD_TABLE(CMonsterRespawnListRandomSet, g_DBAgent.m_GameDB, &m_MonsterRespawnListRandomArray,true, false);
}

bool CGameServerDlg::LoadPremiumItemTable()
{
	LOAD_TABLE(CPremiumItemSet, g_DBAgent.m_GameDB, &m_PremiumItemArray,true, false);
}

bool CGameServerDlg::LoadPremiumItemExpTable()
{
	LOAD_TABLE(CPremiumItemExpSet, g_DBAgent.m_GameDB, &m_PremiumItemExpArray,true, false);
}

bool CGameServerDlg::LoadPetTable()
{
	LastPetID = 2;
	LOAD_TABLE(CPetTableSet, g_DBAgent.m_GameDB, &m_PetArray,true, false);
}

bool CGameServerDlg::LoadBotTable()
{
	LOAD_TABLE(CBotTableSet, g_DBAgent.m_GameDB, &m_arBotArray,true, false);
}

bool CGameServerDlg::LoadCypherRingTable()
{
	LOAD_TABLE(CCypherTableSet, g_DBAgent.m_GameDB, &m_CypherRingArray,true, false);
}

bool CGameServerDlg::LoadUserDailyOpTable()
{
	LOAD_TABLE(CUserDailyOpSet, g_DBAgent.m_GameDB, &m_UserDailyOpMap,true, false);
}

bool CGameServerDlg::LoadEventTriggerTable()
{
	LOAD_TABLE(CEventTriggerSet, g_DBAgent.m_GameDB, &m_EventTriggerArray,true, false);
}

bool CGameServerDlg::LoadStartPositionRandomTable()
{
	LOAD_TABLE(CStartPositionRandomSet, g_DBAgent.m_GameDB, &m_StartPositionRandomArray,true, false);
}

bool CGameServerDlg::LoadUserItemTable()
{
	LOAD_TABLE(CUserItemSet, g_DBAgent.m_GameDB, &m_UserItemArray,true, false);
}

bool CGameServerDlg::LoadObjectPosTable()
{
	LOAD_TABLE(CObjectPosSet, g_DBAgent.m_GameDB, &m_ObjectEventArray,true, false);
}

bool CGameServerDlg::MapFileLoad()
{
	ZoneInfoMap zoneMap;
	LOAD_TABLE_ERROR_ONLY(CZoneInfoSet, g_DBAgent.m_GameDB, &zoneMap, false, false); 

	foreach (itr, zoneMap)
	{
		C3DMap *pMap = new C3DMap();
		_ZONE_INFO *pZone = itr->second;
		if (!pMap->Initialize(pZone))
		{
			printf("ERROR: Unable to load SMD - %s\n", pZone->m_MapName.c_str());
			delete pZone;
			delete pMap;
			m_ZoneArray.DeleteAllData();
			return false;
		}

		delete pZone;
		m_ZoneArray.PutData(pMap->m_nZoneNumber, pMap);
	}

	LOAD_TABLE_ERROR_ONLY(CEventSet, g_DBAgent.m_GameDB, &m_ZoneArray, true, false);
	return true;
}