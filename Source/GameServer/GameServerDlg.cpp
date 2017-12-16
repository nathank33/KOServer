#include "stdafx.h"
#include "KingSystem.h"
#include "KnightsManager.h"

#include "../shared/ClientSocketMgr.h"
#include "../shared/Ini.h"
#include "../shared/DateTime.h"

#include <time.h>
#include <iostream>
#include <fstream>
#include "Map.h"

#include "AISocket.h"

#include "DBAgent.h"

#include <boost\foreach.hpp>

using namespace std;
using namespace std::chrono;

#define NUM_FLAG_VICTORY  4
#define AWARD_GOLD	100000
#define AWARD_EXP	5000

std::vector<Thread *> g_timerThreads;

WORD	g_increase_serial = 1;

CGameServerDlg::CGameServerDlg() {
	DateTime now;
	m_sYear = now.GetYear();
	m_sMonth = now.GetMonth();
	m_sDate = now.GetDay();
	m_sHour = now.GetHour();
	m_sMin = now.GetMinute();
	m_sSec = now.GetSecond();

	m_ReloadKnightAndUserRanksMinute = 0;
	m_DevakillSecond = 0;

	m_byWeather = 0;
	m_sWeatherAmount = 0;
	m_byKingWeatherEvent = 0;
	m_byKingWeatherEvent_Day = 0;
	m_byKingWeatherEvent_Hour = 0;
	m_byKingWeatherEvent_Minute = 0;

	m_byExpEventAmount = 0;
	m_byCoinEventAmount = 0;
	m_byNPEventAmount = 0;

	m_sPartyIndex = 0;

	m_nCastleCapture = 0;

	m_bFirstServerFlag = false;
	m_bPointCheckFlag = true;

	OpenArdream = false;
	OpenCZ = false;

	m_nServerNo = 0;
	m_nServerGroupNo = 0;
	m_nServerGroup = 0;
	m_sDiscount = 0;

	m_bPermanentChatMode = false;
	m_bSantaOrAngel = FLYING_NONE;

	m_nextEvent = -1;

	m_pWorldEventManager = std::make_unique<CWorldEventManager>(this);
}

/**
* @brief	Loads config, table data, initialises sockets and generally
* 			starts up the server.
*
* @return	true if it succeeds, false if it fails.
*/
bool CGameServerDlg::Startup() {
	DateTime time;

	m_sZoneCount = 0;
	m_sErrorSocketCount = 0;

	m_bFirstServerFlag = false;
	m_bServerCheckFlag = false;

	GetTimeFromIni();

	if (!g_pMain->m_socketMgr.Listen(m_GameServerPort, MAX_USER)) {
		printf(_T("ERROR : Failed to listen on server port (%d).\n"), m_GameServerPort);
		return false;
	}

	// Bit tacky, but there's no reason we can't reuse the existing completion port for our AI socket
	m_aiSocketMgr.SetCompletionPort(g_pMain->m_socketMgr.GetCompletionPort());
	m_aiSocketMgr.InitSessions(1);

	if (!g_DBAgent.Startup(m_bMarsEnabled,
		m_strAccountDSN, m_strAccountUID, m_strAccountPWD,
		m_strGameDSN, m_strGameUID, m_strGamePWD)
		|| !LoadItemTable()
		|| !LoadItemDuper()
		|| !LoadIlegalItems()
		|| !LoadSetItemTable()
		|| !LoadAchieveMonsterTable()
		|| !LoadAchieveComTable()
		|| !LoadAchieveMainTable()
		|| !LoadAchieveNormalTable()
		|| !LoadAchieveTitleTable()
		|| !LoadAchieveWarTable()
		|| !LoadItemMiningTable()
		|| !LoadItemExchangeTable()
		|| !LoadItemExchangeExpTable()
		|| !LoadItemUpgradeTable()
		|| !LoadItemOpTable()
		|| !LoadServerResourceTable()
		|| !LoadQuestHelperTable()
		|| !LoadQuestMonsterTable()
		|| !LoadMagicTable()
		|| !LoadMagicType1()
		|| !LoadMagicType2()
		|| !LoadMagicType3()
		|| !LoadMagicType4()
		|| !LoadMagicType5()
		|| !LoadMagicType6()
		|| !LoadMagicType7()
		|| !LoadMagicType8()
		|| !LoadMagicType9()
		|| !LoadObjectPosTable()
		|| !LoadRentalList()
		|| !LoadCoefficientTable()
		|| !LoadLevelUpTable()
		|| !LoadPetLevelUpTable()
		|| !LoadAllKnights()
		|| !LoadAllKnightsUserData()
		|| !LoadKnightsAllianceTable()
		|| !LoadKnightsSiegeWarsTable()
		|| !LoadUserRankings()
		|| !LoadKnightsCapeTable()
		|| !LoadKnightsRankTable()
		|| !LoadStartPositionTable()
		|| !LoadStartPositionRandomTable()
		|| !LoadBattleTable()
		|| !LoadKingSystem()
		|| !MapFileLoad()
		|| !LoadEventTriggerTable()
		|| !LoadMonsterChallengeTable()
		|| !LoadMonsterChallengeSummonListTable()
		|| !LoadEventTimesTable()
		|| !LoadMonsterSummonListTable()
		|| !LoadMonsterSummonListZoneTable()
		|| !LoadMonsterRespawnListTable()
		|| !LoadMonsterRespawnListInformationTable()
		|| !LoadMonsterRespawnListRandomTable()
		|| !LoadPremiumItemTable()
		|| !LoadPremiumItemExpTable()
		|| !LoadPetTable()
		|| !LoadBotTable()
		|| !LoadCypherRingTable()
		|| !LoadUserDailyOpTable()
		|| !LoadUserItemTable()
		|| !LoadItemCrashTable()
		|| !LoadItemMixTable())
		return false;

	// Clear any remaining users in the currently logged in list
	// that may be left as a result of an improper shutdown.
	g_DBAgent.ClearRemainUsers();
	ReloadKnightAndUserRanks();
	// Logs Start
	CreateDirectory("Logs", NULL);

	m_fpDeathUser = fopen(string_format("./Logs/DeathUser_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
	if (m_fpDeathUser == nullptr) {
		printf("ERROR: Unable to open death user log file.\n");
		return false;
	}

	m_fpMerchant = fopen(string_format("./Logs/Merchant_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
	if (m_fpMerchant == nullptr) {
		printf("ERROR: Unable to open merchant log file.\n");
		return false;
	}

	m_fpTrade = fopen(string_format("./Logs/Trade_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
	if (m_fpTrade == nullptr) {
		printf("ERROR: Unable to open trade log file.\n");
		return false;
	}

	m_fpDeathNpc = fopen(string_format("./Logs/DeathNpc_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
	if (m_fpDeathNpc == nullptr) {
		printf("ERROR: Unable to open death npc log file.\n");
		return false;
	}

	m_fpChat = fopen(string_format("./Logs/Chat_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
	if (m_fpChat == nullptr) {
		printf("ERROR: Unable to open chat log file.\n");
		return false;
	}

	m_fpGM = fopen(string_format("./Logs/GM_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
	if (m_fpGM == nullptr) {
		printf("ERROR: Unable to open GM log file.\n");
		return false;
	}

	m_upgradelog = fopen(string_format("./Logs/ItemUpgrade_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
	if (m_upgradelog == nullptr) {
		printf("ERROR: Unable to open GM log file.\n");
		return false;
	}

	m_fpCheat = fopen(string_format("./Logs/Cheat_%d_%d_%d.log", time.GetDay(), time.GetMonth(), time.GetYear()).c_str(), "a");
	if (m_fpCheat == nullptr) {
		printf("ERROR: Unable to open cheat log file.\n");
		return false;
	}
	// CheatRoomArray Delete
	m_ChatRoomArray.DeleteAllData();

	// Logs End

	LoadNoticeData();
	LoadNoticeUpData();

	printf("\n");
	if (!m_luaEngine.Initialise())
		return false;

	AIServerConnect();

	// Initialise the command tables
	InitServerCommands();
	CUser::InitChatCommands();

	g_pMain->m_socketMgr.RunServer();

	if (MaxLevel62Control == 0) {
		OpenArdream = false;
		OpenCZ = true;
	} else {
		OpenArdream = true;
		OpenCZ = false;
	}

	return true;
}

uint16 CGameServerDlg::CalcEventRemainingTime() {
	DateTime now(&g_localTime);
	uint16 Saat, Dakika, SaatFarki, DakikaFarki, xSaatFarki, BaslSaati;
	bool BugunVarMi, isFirst;
	xSaatFarki = -1;
	BaslSaati = -1;
	SaatFarki = -1;
	DakikaFarki = -1;
	isFirst = true;
	BugunVarMi = false;
	Saat = now.GetHour();
	Dakika = now.GetMinute();

	for (int i = 0; i < BIFROST_EVENT_COUNT; i++) {
		if (m_nBifrostTime[i] > 23)
			continue;
		if (m_nBifrostTime[i] > Saat)
			BugunVarMi = true;
	}
	// Gün bitmeden olup olmadığı bilgisini çektik

	for (int i = 0; i < BIFROST_EVENT_COUNT; i++) {
		if (m_nBifrostTime[i] < 24)
			xSaatFarki = m_nBifrostTime[i] - Saat;
		else
			continue;

		if (!BugunVarMi)
			xSaatFarki += 24;

		if (xSaatFarki > 0 && BaslSaati == -1) {
			SaatFarki = xSaatFarki - 1;
			DakikaFarki = (60 - Dakika);
			BaslSaati = m_nBifrostTime[i];
		}
		if (xSaatFarki < (SaatFarki + 1) && BaslSaati > -1) {
			SaatFarki = xSaatFarki - 1;
			DakikaFarki = (60 - Dakika);
			BaslSaati = m_nBifrostTime[i];
		}
	}

	if (DakikaFarki == -1 && SaatFarki == -1)
		SaatFarki = 12;

	if (SaatFarki > 15)
		SaatFarki = 15;

	if (DakikaFarki > 59)
		DakikaFarki = 0;

	return ((DakikaFarki + (SaatFarki * 60)) * MINUTE);
}

/**
* @brief	Loads the server's config from the INI file.
*/
void CGameServerDlg::GetTimeFromIni() {
	CIni ini(CONF_GAME_SERVER);
	int year = 0, month = 0, date = 0, hour = 0, server_count = 0, sgroup_count = 0, i = 0;
	char ipkey[20];

	BoardCache[0].clear();
	BoardCache[1].clear();

	// This is so horrible.
	ini.GetString("ODBC", "GAME_DSN", "KO_GAME", m_strGameDSN, false);
	ini.GetString("ODBC", "GAME_UID", "username", m_strGameUID, false);
	ini.GetString("ODBC", "GAME_PWD", "password", m_strGamePWD, false);

	m_bMarsEnabled = ini.GetBool("ODBC", "GAME_MARS", false);

	ini.GetString("ODBC", "ACCOUNT_DSN", "KO_MAIN", m_strAccountDSN, false);
	ini.GetString("ODBC", "ACCOUNT_UID", "username", m_strAccountUID, false);
	ini.GetString("ODBC", "ACCOUNT_PWD", "password", m_strAccountPWD, false);

	bool bMarsEnabled = ini.GetBool("ODBC", "ACCOUNT_MARS", false);

	// Both need to be enabled to use MARS.
	if (!m_bMarsEnabled || !bMarsEnabled)
		m_bMarsEnabled = false;

	m_byWeather = ini.GetInt("TIMER", "WEATHER", 1);
	RoyalG1 = ini.GetBool("GAME", "AutoRoyalG1", 0);
	ResetLoyaltyMonthlyAt15 = ini.GetBool("GAME", "ResetLoyaltyMonthlyAt15", 0);
	NpToKcSystem = ini.GetBool("GAME", "NPtoKCSystem", 0);
	KcToThousandNp = ini.GetInt("GAME", "KCto1000NP", 0);
	GoldToKcSystem = ini.GetBool("GAME", "GoldtoKCSystem", 0);
	KcToGoldBar = ini.GetInt("GAME", "KCto1GB", 0);

	AutoNovice = ini.GetBool("AUTOS", "AUTO NOVICE", 0);
	AutoMaster = ini.GetBool("AUTOS", "AUTO MASTER", 0);
	AutoSkills = ini.GetBool("AUTOS", "AUTO SKILLS OPEN", 0);

	OfflineMerchanting = ini.GetBool("GAME", "OFFLINE MERCHANT", 0);

	MinutePk = ini.GetInt("ONLINE KAL", "MINUTE PK", 0);
	MinuteMoradon = ini.GetInt("ONLINE KAL", "MINUTE MORADON", 0);
	GiftKcMoradon = ini.GetInt("ONLINE KAL", "GIFT KC MORADON", 0);
	GiftKcPk = ini.GetInt("ONLINE KAL", "GIFT KC PK", 0);

	ini.GetString("BATTLE", "DAYS", "1,6", m_sBattleZoneOpenDays, false);

	for (int i = 0; i < WAR_TIME_COUNT; i++)
		m_nBattleZoneOpenHourStart[i] = ini.GetInt("BATTLE", string_format("START_TIME%d", i).c_str(), (i + 1) * 7);

	for (int i = 0; i < WAR_ZONE_COUNT; i++) {
		m_nBattlezoneOpenWarZone[i] = ini.GetInt("BATTLE", string_format("WAR_ZONE%d", i).c_str(), ZONE_BATTLE + i);
		if (m_nBattlezoneOpenWarZone[i] > ZONE_BATTLE_BASE)
			m_nBattlezoneOpenWarZone[i] = m_nBattlezoneOpenWarZone[i] - ZONE_BATTLE_BASE;
	}
	memset(Zone1Family, 0, sizeof(Zone1Family));
	memset(Zone2Family, 0, sizeof(Zone2Family));
	memset(Zone3Family, 0, sizeof(Zone3Family));
	memset(Zone1Boss, 0, sizeof(Zone1Boss));
	memset(Zone2Boss, 0, sizeof(Zone2Boss));
	memset(Zone3Boss, 0, sizeof(Zone3Boss));
	memset(Zone1, 0, sizeof(Zone1));
	memset(Zone2, 0, sizeof(Zone2));
	memset(Zone3, 0, sizeof(Zone3));

	for (int i = 0; i < MAX_MONSTER_STONE_EVENT_ROOM; i++) {
		Zone1[i] = 0;
		Zone2[i] = 0;
		Zone3[i] = 0;
		Zone1Family[i] = 0;
		Zone2Family[i] = 0;
		Zone3Family[i] = 0;
		Zone1Boss[i] = 0;
		Zone2Boss[i] = 0;
		Zone3Boss[i] = 0;
		IsBusy1[i] = false;
		IsBusy2[i] = false;
		IsBusy3[i] = false;
	}

	ini.GetString("PVPEVENT", "EDAYS1", "1,6", m_sEventZoneOpenDays1, false);
	ini.GetString("DEVILSQU", "EDAYS2", "1,6", m_sEventZoneOpenDays2, false);
	ini.GetString("LOSTCASTLE", "EDAYS3", "1,6", m_sEventZoneOpenDays3, false);

	m_nCastleCapture = ini.GetInt("CASTLE", "NATION", 1);
	m_nServerNo = ini.GetInt("ZONE_INFO", "MY_INFO", 1);
	m_nServerGroup = ini.GetInt("ZONE_INFO", "SERVER_NUM", 0);
	server_count = ini.GetInt("ZONE_INFO", "SERVER_COUNT", 1);
	if (server_count < 1) {
		printf("ERROR: Invalid SERVER_COUNT property in INI.\n");
		return;
	}

	for (i = 0; i < server_count; i++) {
		_ZONE_SERVERINFO *pInfo = new _ZONE_SERVERINFO;
		sprintf(ipkey, "SERVER_%02d", i);
		pInfo->sServerNo = ini.GetInt("ZONE_INFO", ipkey, 1);
		sprintf(ipkey, "SERVER_IP_%02d", i);
		ini.GetString("ZONE_INFO", ipkey, "127.0.0.1", pInfo->strServerIP);
		m_ServerArray.PutData(pInfo->sServerNo, pInfo);
	}

	if (m_nServerGroup != 0) {
		m_nServerGroupNo = ini.GetInt("SG_INFO", "GMY_INFO", 1);
		sgroup_count = ini.GetInt("SG_INFO", "GSERVER_COUNT", 1);
		if (server_count < 1) {
			printf("ERROR: Invalid GSERVER_COUNT property in INI.\n");
			return;
		}

		for (i = 0; i < sgroup_count; i++) {
			_ZONE_SERVERINFO *pInfo = new _ZONE_SERVERINFO;
			sprintf(ipkey, "GSERVER_%02d", i);
			pInfo->sServerNo = ini.GetInt("SG_INFO", ipkey, 1);
			sprintf(ipkey, "GSERVER_IP_%02d", i);
			ini.GetString("SG_INFO", ipkey, "127.0.0.1", pInfo->strServerIP);

			m_ServerGroupArray.PutData(pInfo->sServerNo, pInfo);
		}
	}

	ini.GetString("AI_SERVER", "IP", "127.0.0.1", m_AIServerIP);
	m_AIServerPort = ini.GetInt("AI_SERVER", "PORT", 10020);

	m_GameServerPort = ini.GetInt("SETTINGS", "PORT", 15001);
	m_nGameMasterRHitDamage = ini.GetInt("SETTINGS", "GAME_MASTER_R_HIT_DAMAGE", 30000);
	/*m_nBonusTimeInterval = ini.GetInt("BONUS","SITDOWN_TIME", 15);
	m_nBonusTimeGold = ini.GetInt("BONUS","SITDOWN_GOLD", 1);
	m_nBonusPVPWarItem = ini.GetInt("BONUS","PVP_WAR_ITEM", 0);
	m_CountofTickets = ini.GetInt("BONUS","COUNT_EVENT_ITEM", 1);*/
	m_nPlayerRankingResetTime = ini.GetInt("SETTINGS", "PLAYER_RANKINGS_RESET_TIME", 12);
	ini.GetString("SETTINGS", "PLAYER_RANKINGS_REWARD_ZONES", "71,72,73", m_sPlayerRankingsRewardZones, false);
	m_nPlayerRankingKnightCashReward = ini.GetInt("SETTINGS", "PLAYER_RANKINGS_KNIGHT_CASH_REWARD", 0);
	m_nPlayerRankingLoyaltyReward = ini.GetInt("SETTINGS", "PLAYER_RANKINGS_LOYALTY_REWARD", 0);

	m_Grade1 = ini.GetInt("SETTINGS", "GRADE1", 720000);
	m_Grade2 = ini.GetInt("SETTINGS", "GRADE2", 360000);
	m_Grade3 = ini.GetInt("SETTINGS", "GRADE3", 154000);
	m_Grade4 = ini.GetInt("SETTINGS", "GRADE4", 77000);

	f_Drop1 = ini.GetInt("FISHING", "FISHING", 399127000);
	f_Drop2 = ini.GetInt("FISHING", "FISHING_1", 399128000);
	f_Drop3 = ini.GetInt("FISHING", "FISHING_2", 399129000);
	gf_Drop1 = ini.GetInt("FISHING", "GOLDEN_FISHING", 399127000);
	gf_Drop2 = ini.GetInt("FISHING", "GOLDEN_FISHING_1", 399128000);
	gf_Drop3 = ini.GetInt("FISHING", "GOLDEN_FISHING_2", 399129000);

	Reskillmoney = ini.GetInt("BONUS", "RESKILL_NOMONEY_ACTIVE", 1);
	Restatmoney = ini.GetInt("BONUS", "RESTAT_NOMONEY_ACTIVE", 1);
	m_byPKZoneExpEventAmount = ini.GetInt("SETTINGS", "PK_EXP", 0);
	// gold 2, Gold 10 , Gold 50, Gold 100, Gold 500, Gold 1000, Gold TOTAL

	m_byGoldx2 = ini.GetInt("JACKPOINTS", "GOLD_2", 1100); // 2500
	m_byGoldx10 = ini.GetInt("JACKPOINTS", "GOLD_10", 200); // 1000
	m_byGoldx10 += m_byGoldx2;
	m_byGoldx50 = ini.GetInt("JACKPOINTS", "GOLD_50", 50); // 100
	m_byGoldx50 += m_byGoldx10;
	m_byGoldx100 = ini.GetInt("JACKPOINTS", "GOLD_100", 25);
	m_byGoldx100 += m_byGoldx50;
	m_byGoldx500 = ini.GetInt("JACKPOINTS", "GOLD_500", 10);
	m_byGoldx500 += m_byGoldx100;
	m_byGoldx1000 = ini.GetInt("JACKPOINTS", "GOLD_1000", 5);
	m_byGoldx1000 += m_byGoldx500;
	m_byGoldTotal = m_byGoldx1000;

	// EXP 2, EXP 10 , EXP 50, EXP 100, EXP 500, EXP 1000, EXP TOTAL
	m_byExpx2 = ini.GetInt("JACKPOINTS", "EXP_2", 1100);
	m_byExpx10 = ini.GetInt("JACKPOINTS", "EXP_10", 200);
	m_byExpx10 += m_byExpx2;
	m_byExpx50 = ini.GetInt("JACKPOINTS", "EXP_50", 50);
	m_byExpx50 += m_byExpx10;
	m_byExpx100 = ini.GetInt("JACKPOINTS", "EXP_100", 25);
	m_byExpx100 += m_byExpx50;
	m_byExpx500 = ini.GetInt("JACKPOINTS", "EXP_500", 10);
	m_byExpx500 += m_byExpx100;
	m_byExpx1000 = ini.GetInt("JACKPOINTS", "EXP_1000", 5);
	m_byExpx1000 += m_byExpx500;
	m_byExpTotal = m_byExpx1000;

	m_byExpEventAmount = ini.GetInt("BONUS", "EXP", 0);
	MAXLVLINI = ini.GetInt("GAME", "MAX_LEVEL", 83);
	MaxLevel62Control = ini.GetInt("BONUS", "MAXLV62CONTROL", 0);
	m_byCoinEventAmount = ini.GetInt("BONUS", "MONEY", 0);
	m_byNPEventAmount = ini.GetInt("BONUS", "NP", 0);

	KarusBaseMilitaryCampCount = ini.GetInt("MILITARY_CAMP", "ZONE_KARUS_BASE", 0);
	if (KarusBaseMilitaryCampCount > 3)
		KarusBaseMilitaryCampCount = 3;

	ElmoradBaseMilitaryCampCount = ini.GetInt("MILITARY_CAMP", "ZONE_ELMORAD_BASE", 0);
	if (ElmoradBaseMilitaryCampCount > 3)
		ElmoradBaseMilitaryCampCount = 3;

	KarusEslantMilitaryCampCount = ini.GetInt("MILITARY_CAMP", "ZONE_KARUS_ESLANT", 0);
	if (KarusEslantMilitaryCampCount > 3)
		KarusEslantMilitaryCampCount = 3;

	ElmoradEslantMilitaryCampCount = ini.GetInt("MILITARY_CAMP", "ZONE_ELMORAD_ESLANT", 0);
	if (ElmoradEslantMilitaryCampCount > 3)
		ElmoradEslantMilitaryCampCount = 3;

	MoradonMilitaryCampCount = ini.GetInt("MILITARY_CAMP", "ZONE_MORADON", 0);
	if (MoradonMilitaryCampCount > 5)
		MoradonMilitaryCampCount = 5;

	ini.GetString("BONUS", "NOTICE", "Edit auto notice string.", m_AutoNoticeText);
	m_AutoNoticeIsActive = ini.GetInt("BONUS", "NOTICE_ISACTIVE", 1);

	m_Loyalty_Ardream_Source = ini.GetInt("NATIONAL_POINTS", "ARDREAM_SOURCE", 32);
	m_Loyalty_Ardream_Target = ini.GetInt("NATIONAL_POINTS", "ARDREAM_TARGET", -25);
	m_Loyalty_Ronark_Land_Base_Source = ini.GetInt("NATIONAL_POINTS", "RONARK_LAND_BASE_SOURCE", 64);
	m_Loyalty_Ronark_Land_Base_Target = ini.GetInt("NATIONAL_POINTS", "RONARK_LAND_BASE_TARGET", -50);
	m_Loyalty_Ronark_Land_Source = ini.GetInt("NATIONAL_POINTS", "RONARK_LAND_SOURCE", 64);
	m_Loyalty_Ronark_Land_Target = ini.GetInt("NATIONAL_POINTS", "RONARK_LAND_TARGET", -50);
	m_Loyalty_Other_Zone_Source = ini.GetInt("NATIONAL_POINTS", "OTHER_ZONE_SOURCE", 64);
	m_Loyalty_Other_Zone_Target = ini.GetInt("NATIONAL_POINTS", "OTHER_ZONE_TARGET", -50);
	m_Loyalty_Mini_Pvp_Source = ini.GetInt("NATIONAL_POINTS", "MINI_PVP_SOURCE", 0);
	m_Loyalty_Mini_Pvp_Target = ini.GetInt("NATIONAL_POINTS", "MINI_PVP_TARGET", 0);

	printf("m_Loyalty_Ronark_Land_Source %d, m_Loyalty_Ronark_Land_Target %d\n", m_Loyalty_Ronark_Land_Source, m_Loyalty_Ronark_Land_Target);

	for (int i = 0; i < BIFROST_EVENT_COUNT; i++)
		m_nBifrostTime[i] = ini.GetInt("BIFROST", string_format("START_TIME%d", i + 1).c_str(), 0);

	for (int i = 0; i < BORDER_DEFENSE_WAR_EVENT_COUNT; i++) {
		m_nBorderDefenseWarTime[i] = ini.GetInt("BDW", string_format("START_TIME%d", i + 1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonItemNo1[i] = ini.GetInt("BDW", string_format("ITEM_WON_PRIZE1_%d", i + 1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonItemNo2[i] = ini.GetInt("BDW", string_format("ITEM_WON_PRIZE2_%d", i + 1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonItemNo3[i] = ini.GetInt("BDW", string_format("ITEM_WON_PRIZE3_%d", i + 1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonItemNo4_K[i] = ini.GetInt("BDW", string_format("ITEM_WON_PRIZE4KARUS_%d", i + 1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonItemNo4_H[i] = ini.GetInt("BDW", string_format("ITEM_WON_PRIZE4HUMAN_%d", i + 1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonLoyalty[i] = ini.GetInt("BDW", string_format("ITEM_WON_PRIZE_LOYALTY_%d", i + 1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonKnightCash[i] = ini.GetInt("BDW", string_format("ITEM_WON_PRIZE_KC_%d", i + 1).c_str(), 0);
		m_nBorderDefenseWarPrizeLoserKnightCash[i] = ini.GetInt("BDW", string_format("ITEM_LOSE_PRIZE_KC_%d", i + 1).c_str(), 0);
		m_nBorderDefenseWarPrizeLoserLoyalty[i] = ini.GetInt("BDW", string_format("ITEM_LOSE_PRIZE_LOYALTY_%d", i + 1).c_str(), 0);
		m_nBorderDefenseWarPrizeLoserItem[i] = ini.GetInt("BDW", string_format("ITEM_LOSE_PRIZE_ITEM_%d", i + 1).c_str(), 0);
		m_nBorderDefenseWarMAXLEVEL[i] = ini.GetInt("BDW", string_format("MAX_LEVEL_%d", i + 1).c_str(), 80);
		m_nBorderDefenseWarMINLEVEL[i] = ini.GetInt("BDW", string_format("MIN_LEVEL_%d", i + 1).c_str(), 1);
	}

	for (int i = 0; i < JURAD_MOUNTAIN_EVENT_COUNT; i++) {
		m_nJuraidMountainOdulTipi[i] = ini.GetBool("JR", string_format("ODULTIPI%d", i + 1).c_str(), 0);
		m_nJuraidTime[i] = ini.GetInt("JR", string_format("START_TIME%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeWonItemNo1[i] = ini.GetInt("JR", string_format("ITEM_WON_PRIZE1_%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeWonItemNo2[i] = ini.GetInt("JR", string_format("ITEM_WON_PRIZE2_%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeWonItemNo3[i] = ini.GetInt("JR", string_format("ITEM_WON_PRIZE3_%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeWonItemNo4_K[i] = ini.GetInt("JR", string_format("ITEM_WON_PRIZE4KARUS_%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeWonItemNo4_H[i] = ini.GetInt("JR", string_format("ITEM_WON_PRIZE4HUMAN_%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeWonLoyalty[i] = ini.GetInt("JR", string_format("ITEM_WON_PRIZE_LOYALTY_%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeWonExp[i] = ini.GetInt("JR", string_format("ITEM_WON_PRIZE_EXP_%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeWonKnightCash[i] = ini.GetInt("JR", string_format("ITEM_WON_PRIZE_KC_%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeLoserKnightCash[i] = ini.GetInt("JR", string_format("ITEM_LOSE_PRIZE_KC_%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeLoserLoyalty[i] = ini.GetInt("JR", string_format("ITEM_LOSE_PRIZE_LOYALTY_%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeLoserItem[i] = ini.GetInt("JR", string_format("ITEM_LOSE_PRIZE_ITEM_%d", i + 1).c_str(), 0);
		m_nJuraidMountainPrizeLoserExp[i] = ini.GetInt("JR", string_format("ITEM_LOSE_PRIZE_EXP_%d", i + 1).c_str(), 0);
		m_nJuraidMountainMAXLEVEL[i] = ini.GetInt("JR", string_format("MAX_LEVEL_%d", i + 1).c_str(), 80);
		m_nJuraidMountainMINLEVEL[i] = ini.GetInt("JR", string_format("MIN_LEVEL_%d", i + 1).c_str(), 1);
	}

	for (int i = 0; i < CHAOS_EVENT_COUNT; i++) {
		m_nChaosTime[i] = ini.GetInt("CHAOS", string_format("START_TIME%d", i + 1).c_str(), 0);
		m_nChaosPrizeWonItemNo1[i] = ini.GetInt("CHAOS", string_format("ITEM_WON_PRIZE1_%d", i + 1).c_str(), 0);
		m_nChaosPrizeWonItemNo2[i] = ini.GetInt("CHAOS", string_format("ITEM_WON_PRIZE2_%d", i + 1).c_str(), 0);
		m_nChaosPrizeWonItemNo3[i] = ini.GetInt("CHAOS", string_format("ITEM_WON_PRIZE3_%d", i + 1).c_str(), 0);
		m_nChaosPrizeWonItemNo4_K[i] = ini.GetInt("CHAOS", string_format("ITEM_WON_PRIZE4KARUS_%d", i + 1).c_str(), 0);
		m_nChaosPrizeWonItemNo4_H[i] = ini.GetInt("CHAOS", string_format("ITEM_WON_PRIZE4HUMAN_%d", i + 1).c_str(), 0);
		m_nChaosPrizeWonLoyalty[i] = ini.GetInt("CHAOS", string_format("ITEM_WON_PRIZE_LOYALTY_%d", i + 1).c_str(), 0);
		m_nChaosPrizeWonExp[i] = ini.GetInt("CHAOS", string_format("ITEM_WON_PRIZE_EXP_%d", i + 1).c_str(), 0);
		m_nChaosPrizeWonKnightCash[i] = ini.GetInt("CHAOS", string_format("ITEM_WON_PRIZE_KC_%d", i + 1).c_str(), 0);
		m_nChaosPrizeLoserKnightCash[i] = ini.GetInt("CHAOS", string_format("ITEM_LOSE_PRIZE_KC_%d", i + 1).c_str(), 0);
		m_nChaosPrizeLoserLoyalty[i] = ini.GetInt("CHAOS", string_format("ITEM_LOSE_PRIZE_LOYALTY_%d", i + 1).c_str(), 0);
		m_nChaosPrizeLoserItem[i] = ini.GetInt("CHAOS", string_format("ITEM_LOSE_PRIZE_ITEM_%d", i + 1).c_str(), 0);
		m_nChaosPrizeLoserExp[i] = ini.GetInt("CHAOS", string_format("ITEM_LOSE_PRIZE_EXP_%d", i + 1).c_str(), 0);
		printf("[CHAOS#%d] Event start hour: %d\n", (i + 1), m_nChaosTime[i]);
	}

	m_nTempleEventRemainSeconds = 0;
	m_sRankResetHour = 0;

	m_UserRankingArray[KARUS_ARRAY].DeleteAllData();
	m_UserRankingArray[ELMORAD_ARRAY].DeleteAllData();

	m_byBattleOpenedTime = 0;
	m_byBattleTime = (2 * 60) * 60; // 2 Hours

	m_byEventTime = (1 * 60) * 60; // 1 Hour

	m_xBifrostRemainingTime = CalcEventRemainingTime();  // Bifrost remaining time ( 2 hour ).
	printf("Bifrost remaining time: %d\n", CalcEventRemainingTime());
	m_xBifrostMonumentAttackTime = (30 * MINUTE); // Players is attack a monument last 30 minute.
	m_xBifrostTime = (120 * MINUTE); // Victory nation time ( 1 hour and 30 minute )
	m_xJoinOtherNationBifrostTime = (60 * MINUTE); // Other nation join time ( last 1 hour )

	m_bAttackBifrostMonument = false;
	m_BifrostVictory = 0;
	m_sBifrostRemainingTime = m_xBifrostRemainingTime + 60;
	m_sBifrostTime = 0;
	m_sBifrostWarStart = false;
	m_sBifrostVictoryAll = 3;
	m_sBifrostVictoryNoticeAll = false;

	m_IsMagicTableInUpdateProcess = false;
	m_IsPlayerRankingUpdateProcess = false;

	m_nEventMonumentNation[ZONE_ARDREAM] = 0;
	m_nEventMonumentNation[ZONE_PVP_EVENT] = 0;
	m_nEventMonumentNation[ZONE_RONARK_LAND_BASE] = 0;
	m_nPVPMonumentNation[ZONE_RONARK_LAND] = 0;
	m_nEventMonumentNation[ZONE_BORDER_DEFENSE_WAR] = 0;

	pTempleEvent.ActiveEvent = -1;
	pTempleEvent.ZoneID = 0;
	pTempleEvent.LastEventRoom = 1;
	pTempleEvent.StartTime = 0;
	pTempleEvent.AllUserCount = 0;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;
	pTempleEvent.isAttackable = false;
	pTempleEvent.isActive = false;
	memset(pTempleEvent.m_sBdwMiniTimer, -1, sizeof(pTempleEvent.m_sBdwMiniTimer));
	memset(pTempleEvent.m_sMiniTimerNation, -1, sizeof(pTempleEvent.m_sMiniTimerNation));
	memset(pTempleEvent.KarusDeathCount, 0, sizeof(pTempleEvent.KarusDeathCount));
	memset(pTempleEvent.ElmoDeathCount, 0, sizeof(pTempleEvent.ElmoDeathCount));
	memset(pTempleEvent.KarusDeathRoom1, 0, sizeof(pTempleEvent.KarusDeathRoom1));
	memset(pTempleEvent.KarusDeathRoom2, 0, sizeof(pTempleEvent.KarusDeathRoom2));
	memset(pTempleEvent.KarusDeathRoom3, 0, sizeof(pTempleEvent.KarusDeathRoom3));
	memset(pTempleEvent.ElmoDeathRoom1, 0, sizeof(pTempleEvent.ElmoDeathRoom1));
	memset(pTempleEvent.ElmoDeathRoom2, 0, sizeof(pTempleEvent.ElmoDeathRoom2));
	memset(pTempleEvent.ElmoDeathRoom3, 0, sizeof(pTempleEvent.ElmoDeathRoom3));
	m_bForgettenTempleIsActive = false;
	m_nForgettenTempleStartHour = 0;
	m_nForgettenTempleLevelMin = 0;
	m_nForgettenTempleLevelMax = 0;
	m_nForgettenTempleStartTime = 0;
	m_nForgettenTempleChallengeTime = 0;
	m_bForgettenTempleSummonMonsters = false;
	m_nForgettenTempleCurrentStage = 0;
	m_nForgettenTempleLastStage = 0;
	m_nForgettenTempleLastSummonTime = 0;
	m_nForgettenTempleBanishFlag = false;
	m_nForgettenTempleBanishTime = 0;

	m_bEventZoneIsActive = false;
	m_nEventZoneStartHour = 0;
	m_nEventZoneLevelMin = 0;
	m_nEventZoneLevelMax = 0;
	m_nEventZoneStartTime = 0;
	m_nEventZoneTime = 0;
	m_nEventZoneBanishFlag = false;
	m_nEventZoneBanishTime = 0;
	g_timerThreads.push_back(new Thread(Timer_CheckGameEvents));
	g_timerThreads.push_back(new Thread(Timer_BifrostTime));
	g_timerThreads.push_back(new Thread(Timer_UpdateGameTime));
	g_timerThreads.push_back(new Thread(Timer_UpdateSessions));
	g_timerThreads.push_back(new Thread(Timer_UpdateConcurrent));
}

bool CGameServerDlg::WordGuardSystem(std::string Word, uint8 WordStr) {
	char *pword = &Word[0];
	bool bGuard[32] = {false};
	std::string WordGuard = "qwertyuopadfhsgcijklzxvbnmQWERTYUOPSGICADFHJKLZXVBNM1234567890";
	char *pWordGuard = &WordGuard[0];
	for (uint8 i = 0; i < WordStr; i++) {
		for (uint8 j = 0; j < strlen(pWordGuard); j++)
			if (pword[i] == pWordGuard[j])
				bGuard[i] = true;

		if (bGuard[i] == false)
			return false;
	}
	return true;
}

/**
* @brief	Gets & formats a cached server resource (_SERVER_RESOURCE entry).
*
* @param	nResourceID	Identifier for the resource.
* @param	result	   	The string to store the formatted result in.
*/
void CGameServerDlg::GetServerResource(int nResourceID, string * result, ...) {
	_SERVER_RESOURCE *pResource = m_ServerResourceArray.GetData(nResourceID);
	if (pResource == nullptr) {
		*result = nResourceID;
		return;
	}

	va_list args;
	va_start(args, result);
	_string_format(pResource->strResource, result, args);
	va_end(args);
}

/**
* @brief	Gets the starting positions (for both nations)
* 			for the specified zone.
*
* @param	nZoneID	Identifier for the zone.
*/
_START_POSITION *CGameServerDlg::GetStartPosition(int nZoneID) {
	return m_StartPositionArray.GetData(nZoneID);
}

/**
* @brief	Gets the experience points required for the
* 			specified level.
*
* @param	nLevel	The level.
*
* @return	The experience points required to level up from
* 			the specified level.
*/
int64 CGameServerDlg::GetExpByLevel(int nLevel) {
	LevelUpArray::iterator itr = m_LevelUpArray.find(nLevel);
	if (itr != m_LevelUpArray.end())
		return itr->second;

	return 0;
}

int64 CGameServerDlg::GetPetExpByLevel(int nLevel) {
	LevelUpArray::iterator itr = m_PetLevelUpArray.find(nLevel);
	if (itr != m_PetLevelUpArray.end())
		return itr->second;

	return 0;
}

/**
* @brief	Gets zone by its identifier.
*
* @param	zoneID	Identifier for the zone.
*
* @return	null if it fails, otherwise the zone.
*/
C3DMap * CGameServerDlg::GetZoneByID(int zoneID) {
	return m_ZoneArray.GetData(zoneID);
}

/**
* @brief	Looks up a user by name.
*
* @param	findName	The name to find.
* @param	type		The type of name (account, character).
*
* @return	null if it fails, else the user pointer.
*/
CUser* CGameServerDlg::GetUserPtr(string findName, NameType type) {
	// As findName is a copy of the string passed in, we can change it
	// without worry of affecting anything.
	STRTOUPPER(findName);

	NameMap::iterator itr;
	if (type == TYPE_ACCOUNT) {
		Guard lock(m_accountNameLock);
		BOOST_FOREACH(auto itr, m_accountNameMap)
			if (itr.first == findName)
				return itr.second;
	} else if (type == TYPE_CHARACTER) {
		Guard lock(m_characterNameLock);
		BOOST_FOREACH(auto itr, m_characterNameMap)
			if (itr.first == findName
				&& itr.first.length() == (findName.length()))
				return itr.second;
	}

	return nullptr;
}

CBot* CGameServerDlg::GetBotPtr(string findName, NameType type) {
	// As findName is a copy of the string passed in, we can change it
	// without worry of affecting anything.
	STRTOUPPER(findName);

	BotNameMap::iterator itr;
	if (type == TYPE_CHARACTER) {
		Guard lock(m_BotcharacterNameLock);
		BOOST_FOREACH(auto itr, m_BotcharacterNameMap)
			if (itr.first == findName
				&& (itr.first.length()) == (findName.length()))
				return itr.second;
	}

	return nullptr;
}

/**
* @brief	Adds the account name & session to a hashmap (on login)
*
* @param	pSession	The session.
*/
void CGameServerDlg::AddAccountName(CUser *pSession) {
	Guard lock(m_accountNameLock);
	string upperName = pSession->m_strAccountID;
	STRTOUPPER(upperName);
	m_accountNameMap[upperName] = pSession;
}

/**
* @brief	Adds the character name & session to a hashmap (when in-game)
*
* @param	pSession	The session.
*/
void CGameServerDlg::AddCharacterName(CUser *pSession) {
	Guard lock(m_characterNameLock);
	string upperName = pSession->GetName();
	STRTOUPPER(upperName);
	m_characterNameMap[upperName] = pSession;
}

/**
* @brief	Removes an existing character name/session from the hashmap,
* 			replaces the character's name and reinserts the session with
* 			the new name into the hashmap.
*
* @param	pSession		The session.
* @param	strNewUserID	Character's new name.
*/
void CGameServerDlg::ReplaceCharacterName(CUser *pSession, std::string & strNewUserID) {
	Guard lock(m_characterNameLock);

	// Remove the old name from the map
	string upperName = pSession->GetName();
	STRTOUPPER(upperName);
	m_characterNameMap.erase(upperName);

	// Update the character's name & re-add them to the map.
	pSession->m_strUserID = strNewUserID;
	AddCharacterName(pSession);
}

/**
* @brief	Removes the account name & character names from the hashmaps (on logout)
*
* @param	pSession	The session.
*/
void CGameServerDlg::RemoveSessionNames(CUser *pSession) {
	string upperName = pSession->m_strAccountID;
	STRTOUPPER(upperName);

	{ // remove account name from map (limit scope)
		Guard lock(m_accountNameLock);
		m_accountNameMap.erase(upperName);
	}

	if (pSession->isInGame()) {
		upperName = pSession->GetName();
		STRTOUPPER(upperName);

		Guard lock(m_characterNameLock);
		m_characterNameMap.erase(upperName);
	}
}

CUser				* CGameServerDlg::GetUserPtr(uint16 sUserId) { return m_socketMgr[sUserId]; }
CNpc				* CGameServerDlg::GetNpcPtr(uint16 sNpcId) { return m_arNpcArray.GetData(sNpcId); }
_PARTY_GROUP		* CGameServerDlg::GetPartyPtr(uint16 sPartyID) { return m_PartyArray.GetData(sPartyID); }
CKnights			* CGameServerDlg::GetClanPtr(uint16 sClanID) { return m_KnightsArray.GetData(sClanID); }
_KNIGHTS_ALLIANCE	* CGameServerDlg::GetAlliancePtr(uint16 sAllianceID) { return m_KnightsAllianceArray.GetData(sAllianceID); }
_ITEM_TABLE			* CGameServerDlg::GetItemPtr(uint32 nItemID) { return m_ItemtableArray.GetData(nItemID); }
_ACHIEVE_MONSTER	* CGameServerDlg::GetAchieveMonsterPointer(uint32 ID) { return ACHIEVE_MONSTER.GetData(ID); }
_ACHIEVE_COM		* CGameServerDlg::GetAchieveComPointer(uint32 ID) { return ACHIEVE_COM.GetData(ID); }
_ACHIEVE_MAIN		* CGameServerDlg::GetAchieveMainPointer(uint32 ID) { return ACHIEVE_MAIN.GetData(ID); }
_ACHIEVE_TITLE		* CGameServerDlg::GetAchieveTitlePointer(uint32 ID) { return ACHIEVE_TITLE.GetData(ID); }
_ACHIEVE_WAR		* CGameServerDlg::GetAchieveWarPointer(uint32 ID) { return ACHIEVE_WAR.GetData(ID); }
_ACHIEVE_NORMAL		* CGameServerDlg::GetAchieveNormalPointer(uint32 ID) { return ACHIEVE_NORMAL.GetData(ID); }
CPet				* CGameServerDlg::GetPetPtr(uint64 nItemSerial) {
	Guard lock(g_pMain->m_PetArraylock);
	PetArray::iterator itr = m_PetArray.find(nItemSerial);
	return (itr != m_PetArray.end() ? itr->second : nullptr);
}
_CYPHERRING_DATA	* CGameServerDlg::GetCypherRingPtr(uint64 nItemSerial) {
	CypherRingArray::iterator itr = m_CypherRingArray.find(nItemSerial);
	return (itr != m_CypherRingArray.end() ? itr->second : nullptr);
}
_KNIGHTS_SIEGE_WARFARE	* CGameServerDlg::GetSiegeMasterKnightsPtr(uint16 sCastleIndex) { return m_KnightsSiegeWarfareArray.GetData(sCastleIndex); }

Unit * CGameServerDlg::GetUnitPtr(uint16 id) {
	if (id < NPC_BAND)
		return GetUserPtr(id);

	return GetNpcPtr(id);
}

/**
* @brief	Spawn one or more event NPC/monsters.
*
* @param	sSid	  	The database ID of the NPC/monster to spawn.
* @param	bIsMonster	true if we are spawning a monster, false for an NPC.
* @param	byZone	  	The zone ID to spawn the monster to.
* @param	fX		  	The x coordinate.
* @param	fY		  	The y coordinate.
* @param	fZ		  	The z coordinate.
* @param	sCount	  	Number of spawns to create.
* @param	sRadius	  	Spawn radius.
*/
void CGameServerDlg::SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ, uint16 sCount /*= 1*/, uint16 sRadius /*= 0*/, uint16 sDuration /*= 0*/, uint16 sRegenTime /*= 0*/, uint8 nation /*= 0*/, int16 socketID /*= -1*/, uint16 nEventRoom, bool nIsPet, std::string strPetName, std::string strUserName, uint64 nSerial, uint16 UserId /* = -1*/) {
	Packet result(AG_NPC_SPAWN_REQ);
	result << sSid << bIsMonster
		<< byZone
		<< uint16(fX * 10) << uint16(fY * 10) << uint16(fZ * 10)
		<< sCount
		<< sRadius
		<< sDuration
		<< sRegenTime
		<< nation
		<< socketID
		<< nEventRoom
		<< nIsPet
		<< strPetName
		<< strUserName
		<< nSerial
		<< UserId;
	Send_AIServer(&result);
}

uint16 CGameServerDlg::SpawnBot(int Minute, uint8 byZone, float fX, float fY, float fZ, uint8 Restipi, uint8 minlevel /* = 1*/) {
	foreach_stlmap(itr, m_arBotArray) {
		CBot *Bot = itr->second;
		// ResType(1 Mining 2 Fishing 3 Standing 4 Sitting)
		if (Bot->m_state == GAME_STATE_INGAME)
			continue;

		if (byZone <= ZONE_ELMORAD && byZone != Bot->m_bNation
			|| (byZone >= ZONE_KARUS_ESLANT && byZone <= ZONE_ELMORAD_ESLANT && byZone != (Bot->m_bNation + 10)))
			continue;

		if (Bot->m_bLevel > minlevel)
			continue;

		memset(&Bot->m_arSellMerchantItems, 0, sizeof(Bot->m_arSellMerchantItems));
		Bot->m_bMerchantState = MERCHANT_STATE_NONE;
		Bot->LastWarpTime = UNIXTIME + (Minute * 60);
		Bot->m_pMap = GetZoneByID(byZone);
		Bot->m_bZone = byZone;

		_ITEM_DATA * pItem = &Bot->m_sItemArray[RIGHTHAND];
		_ITEM_TABLE * pTable = GetItemPtr(pItem->nNum);

		if (pItem != nullptr && pTable != nullptr && Restipi == 1) {
			if (pTable->isPickaxe())
				Bot->m_bResHpType = USER_MINING;
			else
				continue;
		} else if (pItem != nullptr && pTable != nullptr && Restipi == 2) {
			if (pTable->isFishing())
				Bot->m_bResHpType = USER_FLASHING;
			else
				continue;
		} else if (Restipi == 3
			|| Restipi == 4)
			Bot->m_bResHpType = Restipi == 3 ? USER_STANDING : USER_SITDOWN;
		else if (Restipi == 5) {
			int Random = myrand(0, 10000);

			Bot->m_bResHpType = Random > 5000 ? USER_STANDING : USER_SITDOWN;
		} else
			continue;

		Bot->SetPosition(fX, fY, fZ);
		Bot->SetRegion(Bot->GetNewRegionX(), Bot->GetNewRegionZ());
		Bot->UserInOut(INOUT_IN);

		return Bot->GetID();
	}

	return false;
}

void CGameServerDlg::KillNpc(uint16 sNid) {
	Packet result(AG_NPC_KILL_REQ);
	result << sNid;
	Send_AIServer(&result);
}

void CGameServerDlg::NpcUpdate(uint16 sSid, bool bIsMonster, uint8 byGroup, uint16 sPid) {
	Packet result(AG_NPC_UPDATE);
	result << sSid << bIsMonster << byGroup << sPid;
	Send_AIServer(&result);
}

_PARTY_GROUP * CGameServerDlg::CreateParty(CUser *pLeader) {
	_PARTY_GROUP * pParty = new _PARTY_GROUP;

	pLeader->m_bInParty = true;
	pLeader->m_sPartyIndex = m_sPartyIndex.increment();

	pParty->wIndex = pLeader->GetPartyID();
	pParty->uid[0] = pLeader->GetSocketID();
	if (!m_PartyArray.PutData(pParty->wIndex, pParty)) {
		delete pParty;
		pLeader->m_bInParty = false;
		pLeader->m_sPartyIndex = -1;
		pParty = nullptr;
	}

	return pParty;
}

void CGameServerDlg::DeleteParty(uint16 sIndex) {
	m_PartyArray.DeleteData(sIndex);
}
void CGameServerDlg::SnowBattleZoneOpenTimer() {
	int nWeekDay = g_localTime.tm_wday;
	int nHour = g_localTime.tm_hour;
	int nMin = g_localTime.tm_min;

	if (!isWarOpen() && nMin == 0) {
		std::list<std::string> vargs = StrSplit(m_sBattleZoneOpenDays, ",");
		uint8 nDaySize = (uint8) vargs.size();
		if (nDaySize > 0) {
			uint8 nDay = 0;
			for (int i = 0; i < nDaySize; i++) {
				nDay = atoi(vargs.front().c_str());
				if (nDay == nWeekDay) {
					for (int x = 0; x < WAR_TIME_COUNT; x++) {
						if (m_nBattleZoneOpenHourStart[x] == nHour && nMin == 0)
							BattleZoneOpen(SNOW_BATTLEZONE_OPEN, m_nBattlezoneOpenWarZone[x]);
					}
				}
				vargs.pop_front();
			}
		}
	} else if (m_byBattleOpen == SNOW_BATTLE) {
		int32 WarElapsedTime = int32(UNIXTIME) - m_byBattleOpenedTime;
		m_byBattleRemainingTime = m_byBattleTime - WarElapsedTime;
		uint8 nBattleZone = ZONE_SNOW_BATTLE;

		if (m_bVictory == 0) {
			if (WarElapsedTime == (m_byBattleTime / 24)) // Select captain
			{
				m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
				m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();
				LoadKnightsRankTable(true, true);
			} else if (WarElapsedTime == (m_byBattleTime / 2)) // War half time.
			{
				if (nBattleZone == ZONE_SNOW_BATTLE)
					BattleWinnerResult(BATTLE_WINNER_KILL);
			}

			m_sBattleTimeDelay++;

			if (WarElapsedTime >= m_byBattleTime) // War is over.
			{
				m_sBattleTimeDelay++;

				if (m_sBattleTimeDelay >= (m_byBattleTime / 24)) {
					m_sBattleTimeDelay = 0;
					SnowBattleZoneClose();
				}
			}
		}
	}
	if (m_bySnowBanishFlag) {
		m_sBanishDelay++;

		if (m_sBanishDelay == (m_byBattleTime / 360))
			Announcement(SNOW_BAN);
		else if (m_sBanishDelay == (m_byBattleTime / 120)) {
			m_bySnowBanishFlag = false;
			m_sBanishDelay = 0;
			SnowBanishLosers();
		}
	}
}
uint32 CGameServerDlg::Timer_CheckGameEvents(void * lpParam) {
	while (g_bRunning) {
		g_pMain->BattleZoneOpenTimer();  // Check if it's time for the BattleZone to open or end.
		g_pMain->SnowBattleZoneOpenTimer(); // SnowWar
		g_pMain->TempleEventTimer();
		g_pMain->ForgettenTempleEventTimer();
		g_pMain->AutoNoticeEventTimer();
		//g_pMain->EventZoneTimer();
		if (g_pMain->isWarOpen() && ((UNIXTIME - g_pMain->m_lastBlessTime) >= (5 * NATION_MONUMENT_REWARD_SECOND)))
			g_pMain->CheckNationMonumentRewards();
		g_pMain->CheckEventTime();
		g_pMain->m_pWorldEventManager->Tick();

		sleep(1 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::Timer_BifrostTime(void * lpParam) {
	while (g_bRunning) {
		g_pMain->SendEventRemainingTime(true, nullptr, ZONE_RONARK_LAND);
		if (g_pMain->m_BifrostVictory == 0) {
			g_pMain->m_sBifrostRemainingTime -= 60;

			if (g_pMain->m_sBifrostRemainingTime == 0 && !g_pMain->m_sBifrostWarStart) {
				g_pMain->m_BifrostVictory = 0;
				//g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostRemainingTime;
				g_pMain->m_sBifrostRemainingTime = g_pMain->CalcEventRemainingTime();
				g_pMain->m_sBifrostTime = 0;
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_START, Nation::ALL, false);

				if (!g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = true;

				g_pMain->m_sBifrostWarStart = true;
				g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostMonumentAttackTime;
				g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);
			} else if (g_pMain->m_sBifrostRemainingTime == 0 && g_pMain->m_sBifrostWarStart) {
				g_pMain->m_sBifrostWarStart = false;
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_FINISH, Nation::ALL, false);

				//g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostRemainingTime;
				g_pMain->m_sBifrostRemainingTime = g_pMain->CalcEventRemainingTime();
				g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);

				if (g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = false;
			}
		} else if (g_pMain->m_BifrostVictory != 0) {
			g_pMain->m_sBifrostTime -= 60;
			g_pMain->m_sBifrostRemainingTime = g_pMain->m_sBifrostTime;

			if (g_pMain->m_sBifrostTime <= 90 * MINUTE && !g_pMain->m_sBifrostVictoryNoticeAll) {
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_DRAW, Nation::ALL, false);
				g_pMain->m_sBifrostVictoryNoticeAll = true;
			}

			if (g_pMain->m_sBifrostTime == 0) {
				g_pMain->m_BifrostVictory = 0;
				g_pMain->m_sBifrostVictoryNoticeAll = false;
				//g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostRemainingTime;
				g_pMain->m_sBifrostRemainingTime = g_pMain->CalcEventRemainingTime();
				g_pMain->m_sBifrostTime = 0;
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_FINISH, Nation::ALL, false);

				if (g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = false;

				g_pMain->m_sBifrostWarStart = false;
				g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);
				g_pMain->KickOutZoneUsers(ZONE_BIFROST, ZONE_RONARK_LAND);
			} else if (g_pMain->m_sBifrostTime == g_pMain->m_xJoinOtherNationBifrostTime) {
				if (!g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = true;
			}
		}
		sleep(60 * SECOND);
	}
	return 0;
}
CNpc*  CGameServerDlg::UpdateNpc() {
	foreach_stlmap(itr, m_arNpcArray) {
		CNpc * pNpc = TO_NPC(itr->second);
		if (pNpc != nullptr) {
			if (pNpc->m_bType3Flag)
				pNpc->HPTimeChangeType3();
			// Check for expired type 4 buffs
			pNpc->InitType4(true);
		}
	}
	return 0;
}

uint32 CGameServerDlg::Timer_UpdateGameTime(void * lpParam) {
	while (g_bRunning) {
		g_pMain->UpdateGameTime();

		if (++g_pMain->m_sErrorSocketCount > 3)
			g_pMain->AIServerConnect();

		g_pMain->UpdateNpc();
		sleep(10 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::Timer_UpdateSessions(void * lpParam) {
	while (g_bRunning) {
		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		BOOST_FOREACH(auto itr, sessMap) {
			CUser * pUser = TO_USER(itr.second);
			uint32 timeout = KOSOCKET_TIMEOUT;

			if (!pUser->isCryptoEnabled())
				timeout = 5;

			// User has authed, but isn't in-game yet (creating a character, or is waiting for the game to load).
			if (!pUser->m_strAccountID.empty() && !pUser->isInGame())
				timeout = KOSOCKET_LOADING_TIMEOUT;

			// Disconnect timed out sessions
			if ((uint32(UNIXTIME) - pUser->GetLastResponseTime()) >= timeout && !g_pMain->OfflineMerchanting && !pUser->OfflineMerchant) {
				pUser->Disconnect();
				continue;
			}

			if (pUser->isInGame())
				pUser->Update();
		}
		sleep(30 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::Timer_UpdateConcurrent(void * lpParam) {
	while (g_bRunning) {
		g_pMain->ReqUpdateConcurrent();
		sleep(60 * SECOND);
	}
	return 0;
}

void CGameServerDlg::ReqUpdateConcurrent() {
	SessionMap sessMap = m_socketMgr.GetActiveSessionMap();
	Packet result(WIZ_ZONE_CONCURRENT);
	result << uint32(m_nServerNo)
		<< uint32(sessMap.size());
	AddDatabaseRequest(result);
}

void CGameServerDlg::AIServerConnect() {
	// Are there any (note: we only use 1 now) idle/disconnected sessions?
	SessionMap sessMap = m_aiSocketMgr.GetIdleSessionMap();

	// Copy the map (should only be 1 socket anyway) to avoid breaking the iterator
	SessionMap idleSessions = sessMap;

	// No idle sessions? Excellent.
	if (idleSessions.empty())
		return;

	// Attempt reconnecting to the server
	BOOST_FOREACH(auto itr, idleSessions) {
		CAISocket *pSock = static_cast<CAISocket *>(itr.second);
		bool bReconnecting = pSock->IsReconnecting();
		if (!pSock->Connect(m_AIServerIP.c_str(), m_AIServerPort)) // couldn't connect... let's leave you alone for now
			continue;

		// Connected! Now send the connection packet.
		Packet result(AI_SERVER_CONNECT);
		result << bReconnecting;
		pSock->Send(&result);

		TRACE("**** AISocket Connect Success!! , server = %s:%d ****\n", pSock->GetRemoteIP().c_str(), pSock->GetRemotePort());
	}
}

/**
* @brief	Sends a packet to all users in the zone matching the specified class types.
*
* @param	pkt				   	The packet.
* @param	bZoneID			   	Identifier for the zone.
* @param	pExceptUser		   	The except user.
* @param	nation			   	The nation.
* @param	seekingPartyOptions	Bitmask of classes to send to.
*/
void CGameServerDlg::Send_Zone_Matched_Class(Packet *pkt, uint8 bZoneID, CUser* pExceptUser,
	uint8 nation, uint8 seekingPartyOptions, uint16 nEventRoom) {
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser * pUser = TO_USER(itr.second);
		if (pUser == pExceptUser
			|| !pUser->isInGame()
			|| pUser->GetZoneID() != bZoneID
			|| pUser->isInParty()) // looking for users to join the party
			continue;

		if (nEventRoom != pUser->GetEventRoom() && nEventRoom > 0)
			continue;

		// If we're in the neutral zone (Moradon), it doesn't matter which nation we party with.
		// For all other zones, we must party with a player of the same nation.
		if (pUser->GetZoneID() == ZONE_MORADON
			|| pUser->GetNation() == nation) {
			if (((seekingPartyOptions & 1) && pUser->JobGroupCheck(ClassWarrior))
				|| ((seekingPartyOptions & 2) && pUser->JobGroupCheck(ClassRogue))
				|| ((seekingPartyOptions & 4) && pUser->JobGroupCheck(ClassMage))
				|| ((seekingPartyOptions & 8) && pUser->JobGroupCheck(ClassPriest))
				|| ((seekingPartyOptions & 10) && pUser->JobGroupCheck(ClassPorutu)))
				pUser->Send(pkt);
		}
	}
}

/**
* @brief	Sends a packet to all players in the specified zone
* 			matching the specified criteria.
*
* @param	pkt		   	The packet.
* @param	bZoneID	   	Zone's identifier.
* @param	pExceptUser	User to except. If specified, will ignore this user.
* @param	nation	   	Nation to allow. If unspecified, will default to Nation::ALL
* 						which will send to all/both nations.
*/
void CGameServerDlg::Send_Zone(Packet *pkt, uint8 bZoneID, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 0*/, uint16 nEventRoom, float fRange) {
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser * pUser = TO_USER(itr.second);

		if (pUser == nullptr)
			continue;

		if (!pUser->isInGame()
			|| pUser->GetZoneID() != bZoneID
			|| (nation != Nation::ALL && nation != pUser->GetNation())) {
			if (pExceptUser != nullptr) {
				if (pUser == pExceptUser
					|| (fRange > 0.0f && pUser->isInRangeSlow(pExceptUser, fRange)))
					continue;
			}

			continue;
		}

		if (nEventRoom != pUser->GetEventRoom() && nEventRoom > 0 && pUser->GetEventRoom() > 0)
			continue;

		pUser->Send(pkt);
	}
}

/**
* @brief	Sends a packet to all users connected and logged into the server.
*
* @param	pkt		   	The packet.
* @param	pExceptUser	User to except. If specified, will ignore this user.
* @param	nation	   	Nation to allow. If unspecified, will default to Nation::ALL
* 						which will send to all/both nations.
*/
void CGameServerDlg::Send_All(Packet *pkt, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 3*/,
	uint8 ZoneID /*= 0*/, bool isSendEventUsers /* false */, uint16 nEventRoom /*= 0*/) {
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser * pUser = TO_USER(itr.second);
		if (pUser == pExceptUser
			|| !pUser->isInGame()
			|| (nation != Nation::ALL && nation != pUser->GetNation()))
			continue;

		if (ZoneID != 0)
			if (pUser->GetZoneID() != ZoneID)
				continue;

		if (nEventRoom != pUser->GetEventRoom() && nEventRoom > 0)
			continue;

		if (isSendEventUsers && !pUser->isEventUser())
			continue;

		pUser->Send(pkt);
	}
}

void CGameServerDlg::Send_Region(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom) {
	foreach_region(rx, rz)
		Send_UnitRegion(pkt, pMap, rx + x, rz + z, pExceptUser, nEventRoom);
}

void CGameServerDlg::Send_UnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, CUser *pExceptUser, uint16 nEventRoom) {
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return;

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;

	// Protect array changes...
	ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
	BOOST_FOREACH(auto itr, cm_RegionUserArray) {
		CUser *pUser = GetUserPtr(itr);
		if (pUser == nullptr
			|| pUser == pExceptUser
			|| !pUser->isInGame())
			continue;

		if (nEventRoom != pUser->GetEventRoom() && pUser->GetEventRoom() > 0)
			continue;

		pUser->Send(pkt);
	}
}

// TODO: Move the following two methods into a base CUser/CNpc class
void CGameServerDlg::Send_OldRegions(Packet *pkt, int old_x, int old_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom) {
	if (old_x != 0) {
		Send_UnitRegion(pkt, pMap, x + old_x * 2, z + old_z - 1, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x + old_x * 2, z + old_z, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x + old_x * 2, z + old_z + 1, nullptr, nEventRoom);
	}

	if (old_z != 0) {
		Send_UnitRegion(pkt, pMap, x + old_x, z + old_z * 2, nullptr, nEventRoom);
		if (old_x < 0)
			Send_UnitRegion(pkt, pMap, x + old_x + 1, z + old_z * 2, nullptr, nEventRoom);
		else if (old_x > 0)
			Send_UnitRegion(pkt, pMap, x + old_x - 1, z + old_z * 2, nullptr, nEventRoom);
		else {
			Send_UnitRegion(pkt, pMap, x + old_x - 1, z + old_z * 2, nullptr, nEventRoom);
			Send_UnitRegion(pkt, pMap, x + old_x + 1, z + old_z * 2, nullptr, nEventRoom);
		}
	}
}

void CGameServerDlg::Send_NewRegions(Packet *pkt, int new_x, int new_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom) {
	if (new_x != 0) {
		Send_UnitRegion(pkt, pMap, x + new_x, z - 1, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x + new_x, z, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x + new_x, z + 1, nullptr, nEventRoom);
	}

	if (new_z != 0) {
		Send_UnitRegion(pkt, pMap, x, z + new_z, nullptr, nEventRoom);

		if (new_x < 0)
			Send_UnitRegion(pkt, pMap, x + 1, z + new_z, nullptr, nEventRoom);
		else if (new_x > 0)
			Send_UnitRegion(pkt, pMap, x - 1, z + new_z, nullptr, nEventRoom);
		else {
			Send_UnitRegion(pkt, pMap, x - 1, z + new_z, nullptr, nEventRoom);
			Send_UnitRegion(pkt, pMap, x + 1, z + new_z, nullptr, nEventRoom);
		}
	}
}

void CGameServerDlg::Send_NearRegion(Packet *pkt, C3DMap *pMap, int region_x, int region_z, float curx, float curz, CUser* pExceptUser, uint16 nEventRoom) {
	int left_border = region_x * VIEW_DISTANCE, top_border = region_z * VIEW_DISTANCE;
	Send_FilterUnitRegion(pkt, pMap, region_x, region_z, curx, curz, pExceptUser, nEventRoom);
	if (((curx - left_border) > (VIEW_DISTANCE / 2.0f))) {			// RIGHT
		if (((curz - top_border) > (VIEW_DISTANCE / 2.0f))) {	// BOTTOM
			Send_FilterUnitRegion(pkt, pMap, region_x + 1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z + 1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x + 1, region_z + 1, curx, curz, pExceptUser, nEventRoom);
		} else {													// TOP
			Send_FilterUnitRegion(pkt, pMap, region_x + 1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z - 1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x + 1, region_z - 1, curx, curz, pExceptUser, nEventRoom);
		}
	} else {														// LEFT
		if (((curz - top_border) > (VIEW_DISTANCE / 2.0f))) {	// BOTTOM
			Send_FilterUnitRegion(pkt, pMap, region_x - 1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z + 1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x - 1, region_z + 1, curx, curz, pExceptUser, nEventRoom);
		} else {													// TOP
			Send_FilterUnitRegion(pkt, pMap, region_x - 1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z - 1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x - 1, region_z - 1, curx, curz, pExceptUser, nEventRoom);
		}
	}
}

void CGameServerDlg::Send_FilterUnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, float ref_x, float ref_z, CUser *pExceptUser, uint16 nEventRoom) {
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return;

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;
	// Protect array changes...
	ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
	BOOST_FOREACH(auto itr, cm_RegionUserArray) {
		CUser *pUser = GetUserPtr(itr);
		if (pUser == nullptr
			|| pUser == pExceptUser
			|| !pUser->isInGame())
			continue;

		if (nEventRoom != pUser->GetEventRoom() && nEventRoom > 0)
			continue;

		if (sqrt(pow((pUser->m_curx - ref_x), 2) + pow((pUser->m_curz - ref_z), 2)) < 32)
			pUser->Send(pkt);
	}
}

void CGameServerDlg::Send_PartyMember(int party, Packet *result) {
	_PARTY_GROUP* pParty = GetPartyPtr(party);
	if (pParty == nullptr)
		return;

	for (int i = 0; i < MAX_PARTY_USERS; i++) {
		CUser *pUser = GetUserPtr(pParty->uid[i]);
		if (pUser == nullptr)
			continue;

		pUser->Send(result);
	}
}

void CGameServerDlg::Send_KnightsMember(int index, Packet *pkt) {
	CKnights* pKnights = GetClanPtr(index);
	if (pKnights == nullptr)
		return;

	pKnights->Send(pkt);
}

void CGameServerDlg::Send_KnightsAlliance(uint16 sAllianceID, Packet *pkt) {
	_KNIGHTS_ALLIANCE* pAlliance = GetAlliancePtr(sAllianceID);
	if (pAlliance == nullptr)
		return;

	Send_KnightsMember(pAlliance->sMainAllianceKnights, pkt);
	Send_KnightsMember(pAlliance->sSubAllianceKnights, pkt);
	Send_KnightsMember(pAlliance->sMercenaryClan_1, pkt);
	Send_KnightsMember(pAlliance->sMercenaryClan_2, pkt);
}

void CGameServerDlg::Send_AIServer(Packet *pkt) {
	m_aiSocketMgr.SendAll(pkt);
}

void CGameServerDlg::UpdateGameTime() {
	DateTime now(&g_localTime);

	// Check timed King events.
	foreach_stlmap_nolock(itr, m_KingSystemArray)
		itr->second->CheckKingTimer();
	// Juraid Deval Kill Code By YNG
	if (m_sSec != now.GetSecond()) {
		if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_JURAD_MOUNTAIN) {
			for (int i = 1; i < MAX_TEMPLE_EVENT_ROOM; i++) {
				if (pTempleEvent.isDevaControl[i]) {
					m_DevakillSecond++;

					if (pTempleEvent.isDevaFlag[i] && pTempleEvent.isAttackable && pTempleEvent.isActive) {
						pTempleEvent.isDevaFlag[i] = false;

						Packet cresult(WIZ_EVENT);
						cresult << uint8(TEMPLE_EVENT_FINISH)
							<< uint8(2) << uint8(0)
							<< uint8(1) << uint8(20) << uint32(0);

						Packet dresult(WIZ_EVENT);
						dresult << uint8(TEMPLE_EVENT_FINISH)
							<< uint8(1) << uint8(0)
							<< uint8(2) << uint8(20) << uint32(0);

						foreach_stlmap(itr, m_TempleEventUserArray) {
							CUser * pUser = GetUserPtr(itr->second->m_socketID);

							if (pUser == nullptr)
								continue;

							if (!pUser->isInGame())
								continue;

							if (pUser->GetEventRoom() != i)
								continue;

							if (pTempleEvent.DevaNation[i] == KARUS)
								pUser->Send(&cresult);
							else if (pTempleEvent.DevaNation[i] == ELMORAD)
								pUser->Send(&dresult);
						}
					}
					if (m_DevakillSecond == DEVA_KILL_TIME && pTempleEvent.isActive) {
						TempleEventFinish(i, pTempleEvent.DevaNation[i]);
						m_DevakillSecond = 0;
						pTempleEvent.isDevaControl[i] = false;
					}
				}
			}
		}
	}
	// Every minute
	if (m_sMin != now.GetMinute()) {
		m_ReloadKnightAndUserRanksMinute++;

		if (m_ReloadKnightAndUserRanksMinute == RELOAD_KNIGHTS_AND_USER_RATING) {
			m_ReloadKnightAndUserRanksMinute = 0;
			ReloadKnightAndUserRanks();
		}

		// Player Ranking Rewards
		std::list<std::string> vargs = StrSplit(m_sPlayerRankingsRewardZones, ",");
		uint8 nZones = (uint8) vargs.size();
		if (nZones > 0) {
			uint8 nZoneID = 0;
			/*for (int i = 0; i < nZones; i++)
			{
				nZoneID = atoi(vargs.front().c_str());
				SetPlayerRankingRewards(nZoneID);
				vargs.pop_front();
			}*/
		}
	}

	// Every hour
	if (m_sHour != now.GetHour()) {
		ResetPlayerRankings();
		UpdateWeather();
		SetGameTime();

		if (m_bSantaOrAngel)
			SendFlyingSantaOrAngel();
	}

	// Every day
	if (m_sDate != now.GetDay()) {
	}

	// Every month
	if (m_sMonth != now.GetMonth()) {
		// Reset monthly NP.
		ResetLoyaltyMonthly();
	}

	// Update the server time
	m_sYear = now.GetYear();
	m_sMonth = now.GetMonth();
	m_sDate = now.GetDay();
	m_sHour = now.GetHour();
	m_sMin = now.GetMinute();
	m_sSec = now.GetSecond();
	Packet result(AG_TIME_WEATHER);
	result << m_sYear << m_sMonth << m_sDate << m_sHour << m_sMin << m_byWeather << m_sWeatherAmount;
	Send_AIServer(&result);
}

void CGameServerDlg::ResetLoyaltyMonthly() {
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser * pUser = TO_USER(itr.second);
		pUser->m_iLoyaltyMonthly = 0;
		pUser->SendLoyaltyChange(); // update the client (note: official doesn't bother)
	}

	// Attempt to update the database in this thread directly, while the session map's locked.
	// This prevents new users from logging in before they've been reset (hence keeping last month's total).
	g_DBAgent.ResetLoyaltyMonthly();
}

void CGameServerDlg::UpdateWeather() {
	if (m_byKingWeatherEvent) {
		int16 sEventExpiry;
		if (g_localTime.tm_mday == m_byKingWeatherEvent_Day)
			sEventExpiry = g_localTime.tm_min + 60 * (g_localTime.tm_hour - m_byKingWeatherEvent_Hour) - m_byKingWeatherEvent_Minute;
		else
			sEventExpiry = g_localTime.tm_min + 60 * (g_localTime.tm_hour - m_byKingWeatherEvent_Hour + 24) - m_byKingWeatherEvent_Minute;

		// Weather events last for 5 minutes
		if (sEventExpiry > 5) {
			m_byKingWeatherEvent = 0;
			m_byKingWeatherEvent_Day = 0;
			m_byKingWeatherEvent_Hour = 0;
			m_byKingWeatherEvent_Minute = 0;
		}
	} else {
		int weather = 0, rnd = myrand(0, 100);
		if (rnd < 2)		weather = WEATHER_SNOW;
		else if (rnd < 7)	weather = WEATHER_RAIN;
		else				weather = WEATHER_FINE;

		m_sWeatherAmount = myrand(0, 100);
		if (weather == WEATHER_FINE) {
			if (m_sWeatherAmount > 70)
				m_sWeatherAmount /= 2;
			else
				m_sWeatherAmount = 0;
		}
		m_byWeather = weather;
	}

	// Real weather data for most users.
	Packet realWeather(WIZ_WEATHER, m_byWeather);
	realWeather << m_sWeatherAmount;

	// Fake, clear weather for users in certain zones (e.g. Desp & Hell Abysses, Arena)
	Packet fakeWeather(WIZ_WEATHER, uint8(WEATHER_FINE));
	fakeWeather << m_sWeatherAmount;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser * pUser = TO_USER(itr.second);
		if (!pUser->isInGame())
			continue;

		if (pUser->GetZoneID() == 32
			|| pUser->GetZoneID() == 33
			|| pUser->GetZoneID() == 48)
			pUser->Send(&fakeWeather);
		else
			pUser->Send(&realWeather);
	}
}

void CGameServerDlg::SetGameTime() {
	CIni ini(CONF_GAME_SERVER);
	ini.SetInt("TIMER", "WEATHER", m_byWeather);
}

void CGameServerDlg::ResetPlayerRankings(uint8 ZoneID) {
	if (m_sRankResetHour == m_nPlayerRankingResetTime || ZoneID > 0) {
		m_IsPlayerRankingUpdateProcess = true;
		m_sRankResetHour = 0;

		for (int nation = KARUS_ARRAY; nation <= ELMORAD_ARRAY; nation++) {
			foreach_stlmap_nolock(itr, g_pMain->m_UserRankingArray[nation]) {
				_USER_RANKING * pRankInfo = g_pMain->m_UserRankingArray[nation].GetData(itr->first);

				if (pRankInfo == nullptr)
					continue;

				CUser * pUser = g_pMain->GetUserPtr(pRankInfo->m_socketID);

				if (pUser == nullptr
					|| pUser->isInTempleEventZone()
					|| (ZoneID > 0 && pUser->GetZoneID() != ZoneID))
					continue;

				pRankInfo->m_iLoyaltyDaily = 0;
				pRankInfo->m_iLoyaltyPremiumBonus = 0;
				pUser->m_iLoyaltyDaily = 0;
				pUser->m_iLoyaltyPremiumBonus = 0;
			}
		}

		m_IsPlayerRankingUpdateProcess = false;
		return;
	}

	m_sRankResetHour++;
}

void CGameServerDlg::AddDatabaseRequest(Packet & pkt, CUser *pUser /*= nullptr*/) {
	Packet *newPacket = new Packet(pkt.GetOpcode(), pkt.size() + 2);
	*newPacket << uint16(pUser == nullptr ? -1 : pUser->GetSocketID());
	if (pkt.size())
		newPacket->append(pkt.contents(), pkt.size());
	DatabaseThread::AddRequest(newPacket);
}

void CGameServerDlg::UserInOutForMe(CUser *pSendUser) {
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_REQ_USERIN);
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 user_count = 0, max_count = 150, total_count = 0;

	result << uint16(0); // placeholder for the user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z) {
		uint16 region_x = rx + x, region_z = rz + z;

		Guard lock(pMap->m_lock);
		CRegion *pRegion = pMap->GetRegion(region_x, region_z);
		if (pRegion == nullptr)
			continue;

		if (pRegion->m_RegionUserArray.size() <= 0 && pRegion->m_RegionBotArray.size() <= 0)
			continue;

		if (pRegion->m_RegionUserArray.size() > 0) {
			BOOST_FOREACH(auto itr, pRegion->m_RegionUserArray) {
				CUser *pUser = GetUserPtr(itr);
				if (pUser == nullptr
					|| !pUser->isInGame())
					continue;

				if (pSendUser->GetEventRoom() != pUser->GetEventRoom() || total_count > max_count)
					continue;

				result << uint8(0) << pUser->GetSocketID();
				pUser->GetUserInfo(result);
				user_count++;
				total_count++;

				if (user_count >= 10) {
					result.put(0, uint16(user_count));
					pSendUser->SendCompressed(&result);
					result.clear();
					result << uint16(0);
					user_count = 0;
				}
			}
		}

		std::vector<CBot *> deleted;
		if (pRegion->m_RegionBotArray.size() > 0) {
			BOOST_FOREACH(auto itr, pRegion->m_RegionBotArray) {
				CBot *pBot = m_arBotArray.GetData(itr);
				if (pBot == nullptr
					|| !pBot->isInGame() || total_count > max_count)
					continue;

				if (pBot->LastWarpTime < UNIXTIME)
					deleted.push_back(pBot);

				result << uint8(0) << pBot->GetID();
				pBot->GetUserInfo(result);
				user_count++;
				total_count++;

				if (user_count >= 10) {
					result.put(0, uint16(user_count));
					pSendUser->SendCompressed(&result);
					result.clear();
					result << uint16(0);
					user_count = 0;
				}
			}
		}

		BOOST_FOREACH(auto itr, deleted)
			(itr)->UserInOut(INOUT_OUT);
	}

	if (user_count > 0) {
		result.put(0, uint16(user_count));
		pSendUser->SendCompressed(&result);
	}
}

void CGameServerDlg::RegionUserInOutForMe(CUser *pSendUser) {
	if (pSendUser == nullptr)
		return;

	Packet reflesh(WIZ_REGIONCHANGE, uint8(0));
	pSendUser->SendCompressed(&reflesh);

	Packet result(WIZ_REGIONCHANGE, uint8(1));
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 user_count = 0;

	result << uint16(0); // placeholder for the user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionUserList(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());

	result.put(1, user_count);

	pSendUser->SendCompressed(&result);

	Packet test(WIZ_REGIONCHANGE, uint8(2));
	pSendUser->SendCompressed(&test);
}

void CGameServerDlg::GetRegionUserList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom) {
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	if (pRegion->m_RegionUserArray.size() <= 0 && pRegion->m_RegionBotArray.size() <= 0)
		return;

	if (pRegion->m_RegionUserArray.size() > 0) {
		BOOST_FOREACH(auto itr, pRegion->m_RegionUserArray) {
			CUser *pUser = GetUserPtr(itr);
			if (pUser == nullptr
				|| !pUser->isInGame())
				continue;

			if (nEventRoom != pUser->GetEventRoom() && nEventRoom > 0)
				continue;

			pkt << pUser->GetSocketID();
			t_count++;
		}
	}

	if (pRegion->m_RegionBotArray.size() <= 0)
		return;
	std::vector<CBot *> deleted;
	if (pRegion->m_RegionBotArray.size() > 0) {
		BOOST_FOREACH(auto itr, pRegion->m_RegionBotArray) {
			CBot *pBot = m_arBotArray.GetData(itr);
			if (pBot == nullptr
				|| !pBot->isInGame())
				continue;

			if (pBot->LastWarpTime < UNIXTIME)
				deleted.push_back(pBot);

			pkt << pBot->GetID();
			t_count++;
		}
	}

	BOOST_FOREACH(auto itr, deleted)
		(itr)->UserInOut(INOUT_OUT);
}

void CGameServerDlg::MerchantUserInOutForMe(CUser *pSendUser) {
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_MERCHANT_INOUT, uint8(1));
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 user_count = 0;

	result << uint16(0); // placeholder for user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionMerchantUserIn(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());

	result.put(1, user_count);
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::GetRegionMerchantUserIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom) {
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;

	BOOST_FOREACH(auto itr, pRegion->m_RegionUserArray) {
		CUser *pUser = GetUserPtr(itr);
		if (pUser == nullptr
			|| !pUser->isInGame()
			|| !pUser->isMerchanting())
			continue;

		if (nEventRoom != pUser->GetEventRoom() && nEventRoom > 0)
			continue;

		pkt << pUser->GetSocketID()
			<< pUser->GetMerchantState() // 0 is selling, 1 is buying
			<< (pUser->GetMerchantState() == 1 ? false : pUser->m_bPremiumMerchant); // Type of merchant [normal - gold] // bool

		for (int i = 0, listCount = (pUser->GetMerchantState() == 1 ? 4 : (pUser->m_bPremiumMerchant ? 8 : 4)); i < listCount; i++) {
			if (pUser->isSellingMerchant()) {
				if (pUser->m_arSellMerchantItems[i > 3 ? i + 2 : i].IsSoldOut)
					pkt << uint32(0);
				else
					pkt << pUser->m_arSellMerchantItems[i > 3 ? i + 2 : i].nNum;
			} else
				pkt << pUser->m_arBuyMerchantItems[i > 3 ? i + 2 : i].nNum;
		}
		t_count++;
	}

	if (pRegion->m_RegionBotArray.size() <= 0)
		return;

	std::vector<CBot *> deleted;
	if (pRegion->m_RegionBotArray.size() > 0) {
		BOOST_FOREACH(auto itr, pRegion->m_RegionBotArray) {
			CBot *pBot = m_arBotArray.GetData(itr);

			if (pBot == nullptr
				|| !pBot->isInGame()
				|| !pBot->isMerchanting())
				continue;

			if (pBot->LastWarpTime < UNIXTIME)
				deleted.push_back(pBot);

			pkt << pBot->GetID()
				<< pBot->GetMerchantState() // 0 is selling, 1 is buying
				<< (pBot->GetMerchantState() == 1 ? false : pBot->m_bPremiumMerchant); // Type of merchant [normal - gold] // bool

			for (int i = 0, listCount = (pBot->GetMerchantState() == 1 ? 4 : (pBot->m_bPremiumMerchant ? 8 : 4)); i < listCount; i++) {
				if (pBot->isSellingMerchant()) {
					if (pBot->m_arSellMerchantItems[i > 3 ? i + 2 : i].IsSoldOut)
						pkt << uint32(0);
					else
						pkt << pBot->m_arSellMerchantItems[i > 3 ? i + 2 : i].nNum;
				} else
					pkt << pBot->m_arBuyMerchantItems[i > 3 ? i + 2 : i].nNum;
			}

			t_count++;
		}
	}

	BOOST_FOREACH(auto itr, deleted)
		(itr)->UserInOut(INOUT_OUT);
}

void CGameServerDlg::NpcInOutForMe(CUser* pSendUser) {
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_REQ_NPCIN);
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 npc_count = 0;
	result << uint16(0); // placeholder for NPC count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionNpcIn(pMap, rx + x, rz + z, result, npc_count, pSendUser->GetEventRoom(), pSendUser);

	result.put(0, npc_count);
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::GetRegionNpcIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom, CUser* pSendUser) {
	if (!m_bPointCheckFlag || pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion * pRegion = pMap->GetRegion(region_x, region_z);

	if (pRegion == nullptr)
		return;

	if (pRegion->m_RegionNpcArray.size() <= 0)
		return;

	CKnights *pKnights = g_pMain->GetClanPtr(pSendUser->m_bKnights);
	_KNIGHTS_SIEGE_WARFARE *pSiegeWars = g_pMain->GetSiegeMasterKnightsPtr(1);

	ZoneNpcArray RegionNpcArray = pRegion->m_RegionNpcArray;
	BOOST_FOREACH(auto itr, RegionNpcArray) {
		CNpc *pNpc = GetNpcPtr(itr);
		if (pNpc == nullptr
			|| pNpc->isDead())
			continue;

		if (nEventRoom != pNpc->GetEventRoom() && nEventRoom > 0)
			continue;

		pkt << pNpc->GetID();

		if (pNpc->m_sSid == 541 && pNpc->GetType() == NPC_DESTROYED_ARTIFACT && pNpc->m_bZone == ZONE_DELOS) {
			if (pKnights != nullptr && pSiegeWars != nullptr) {
				if ((pSiegeWars->sMasterKnights == pKnights->m_sAlliance && pKnights->m_sAlliance != 0) || pSiegeWars->sMasterKnights == pKnights->m_sIndex)
					pNpc->GetNpcInfo(pkt, 3);
				else
					pNpc->GetNpcInfo(pkt, 0);
			} else
				pNpc->GetNpcInfo(pkt, 0);
		} else
			pNpc->GetNpcInfo(pkt);

		t_count++;
	}
}

void CGameServerDlg::RegionNpcInfoForMe(CUser *pSendUser) {
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_NPC_REGION);
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 npc_count = 0;
	result << uint16(0); // placeholder for NPC count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionNpcList(pMap, rx + x, rz + z, result, npc_count, pSendUser->GetEventRoom());

	result.put(0, npc_count);
	if (npc_count < 500)
		pSendUser->Send(&result);
	else
		pSendUser->SendCompressed(&result);
}

void CGameServerDlg::GetUnitListFromSurroundingRegions(Unit * pOwner, std::vector<uint16> * pList) {
	if (pOwner == nullptr)
		return;

	C3DMap * pMap = pOwner->GetMap();
	ASSERT(pMap != nullptr);

	int16 rx = pOwner->GetRegionX(), rz = pOwner->GetRegionZ();

	Guard lock(pMap->m_lock);
	foreach_region(x, z) {
		uint16 region_x = rx + x, region_z = rz + z;
		CRegion * pRegion = pMap->GetRegion(region_x, region_z);

		if (pRegion == nullptr)
			continue;

		// Add all potential NPCs to list
		BOOST_FOREACH(auto itr, pRegion->m_RegionNpcArray)
			pList->push_back(itr);

		// Add all potential users to list
		BOOST_FOREACH(auto itr, pRegion->m_RegionUserArray)
			pList->push_back(itr);
	}
}

void CGameServerDlg::GetRegionNpcList(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom) {
	if (!m_bPointCheckFlag
		|| pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion * pRegion = pMap->GetRegion(region_x, region_z);

	if (pRegion == nullptr)
		return;

	if (pRegion->m_RegionNpcArray.size() <= 0)
		return;

	ZoneNpcArray RegionNpcArray = pRegion->m_RegionNpcArray;
	BOOST_FOREACH(auto itr, RegionNpcArray) {
		CNpc *pNpc = GetNpcPtr(itr);
		if (pNpc == nullptr || pNpc->isDead())
			continue;

		if (nEventRoom != pNpc->GetEventRoom() && nEventRoom > 0)
			continue;

		pkt << pNpc->GetID();
		t_count++;
	}
}

void CGameServerDlg::HandleConsoleCommand(const char * msg) {
	string message = msg;
	if (message.empty())
		return;

	if (ProcessServerCommand(message)) {
		printf("Command accepted.\n");
		return;
	}

	printf("Invalid command. If you're trying to send a notice, please use /notice\n");
}

bool CGameServerDlg::LoadNoticeData() {
	ifstream file("./Notice.txt");
	string line;
	int count = 0;

	// Clear out the notices first
	memset(&m_ppNotice, 0, sizeof(m_ppNotice));

	if (!file) {
		TRACE("Notice.txt could not be opened.\n");
		return false;
	}

	while (!file.eof()) {
		if (count > 19) {
			TRACE("Too many lines in Notice.txt\n");
			break;
		}

		getline(file, line);
		if (line.length() > 128) {
			TRACE("Notice.txt contains line that exceeds the limit of 128 characters.\n");
			break;
		}

		strcpy(m_ppNotice[count++], line.c_str());
	}

	file.close();
	return true;
}

bool CGameServerDlg::LoadNoticeUpData() {
	ifstream file("./Notice_up.txt");
	string line;
	int count = 0;

	// Clear out the notices first
	memset(&m_peNotice, 0, sizeof(m_peNotice));

	if (!file) {
		TRACE("Notice_up.txt could not be opened.\n");
		return false;
	}

	while (!file.eof()) {
		if (count > 19) {
			TRACE("Too many lines in Notice_up.txt\n");
			break;
		}

		getline(file, line);
		if (line.length() > 128) {
			TRACE("Notice_up.txt contains line that exceeds the limit of 128 characters.\n");
			break;
		}

		strcpy(m_peNotice[count++], line.c_str());
	}

	file.close();
	return true;
}

void CGameServerDlg::SendAllUserInfo() {
	Packet result(AG_USER_INFO_ALL);
	uint8 count = 0;
	result << count; // placeholder for user count
	const int tot = 20;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		TO_USER(itr.second)->GetUserInfoForAI(result);
		if (++count == tot) {
			result.put(0, count);
			Send_AIServer(&result);
			count = 0;
			result.clear();
		}
	}

	if (count != 0 && count < (tot - 1)) {
		result.put(0, count);
		Send_AIServer(&result);
		count = 0;
		result.clear();
	}

	foreach_stlmap(itr, m_PartyArray) {
		_PARTY_GROUP *pParty = itr->second;
		if (pParty == nullptr)
			continue;

		result.Initialize(AG_PARTY_INFO_ALL);
		result << uint16(itr->first);
		for (int i = 0; i < MAX_PARTY_USERS; i++)
			result << pParty->uid[i];

		Send_AIServer(&result);
	}
}

void CGameServerDlg::DeleteAllNpcList(int flag) {
	if (!m_bServerCheckFlag
		|| !m_bPointCheckFlag)
		return;

	TRACE("[Monster Point Delete]\n");
	TRACE("*** DeleteAllNpcList - Start *** \n");

	// Remove spawns from users to prevent them from getting bugged...
	m_arNpcArray.m_lock.lock();
	foreach_stlmap_nolock(itr, m_arNpcArray) {
		if (itr->second->isAlive())
			itr->second->SendInOut(INOUT_OUT, 0.0f, 0.0f, 0.0f);

		// decrease the reference count (freeing it if nothing else is using it)
		itr->second->DecRef();
	}

	// all the data should now be freed (if not, it will be by whatever's using it)
	m_arNpcArray.m_UserTypeMap.clear();
	m_arNpcArray.m_lock.unlock();

	// Now remove all spawns from all regions
	foreach_stlmap_nolock(itr, m_ZoneArray) {
		C3DMap *pMap = itr->second;
		if (pMap == nullptr)
			continue;

		Guard lock(pMap->m_lock);
		for (int i = 0; i < pMap->GetXRegionMax(); i++) {
			for (int j = 0; j < pMap->GetZRegionMax(); j++)
				pMap->m_ppRegion[i][j].m_RegionNpcArray.clear();
		}
	}
	m_bServerCheckFlag = false;

	TRACE("*** DeleteAllNpcList - End *** \n");
}

/**
* @brief	Searches for the first NPC in the specified zone
* 			with the specified picture/model ID.
*
* @param	sPid	Picture/model ID of the NPC.
* @param	byZone	Zone to search in.
*
* @return	null if it fails, else the NPC instance we found.
*/
CNpc*  CGameServerDlg::FindNpcInZone(uint16 sPid, uint8 byZone) {
	if (!m_bPointCheckFlag)
		return nullptr;

	Guard lock(m_arNpcArray.m_lock);
	foreach_stlmap(itr, m_arNpcArray) {
		CNpc * pNpc = itr->second;
		if (pNpc == nullptr || pNpc->GetZoneID() != byZone
			// This isn't a typo, it's actually just a hack.
				// The picture/model ID of most spawns is the same as their prototype ID.
					// When there's more than one spawn prototype (i.e. different sSid), we keep using
						// the same picture/model ID. So we check this instead of the sSid...
			|| pNpc->m_sPid != sPid)
			continue;

		return pNpc;
	}

	return nullptr;
}

void CGameServerDlg::BattleZoneOpenTimer() {
	int nWeekDay = g_localTime.tm_wday;
	int nHour = g_localTime.tm_hour;
	int nMin = g_localTime.tm_min;

	if (!isWarOpen() && nMin == 0) {
		std::list<std::string> vargs = StrSplit(m_sBattleZoneOpenDays, ",");
		uint8 nDaySize = (uint8) vargs.size();
		if (nDaySize > 0) {
			uint8 nDay = 0;
			for (int i = 0; i < nDaySize; i++) {
				nDay = atoi(vargs.front().c_str());
				if (nDay == nWeekDay) {
					for (int x = 0; x < WAR_TIME_COUNT; x++) {
						if (m_nBattleZoneOpenHourStart[x] == nHour && nMin == 0)
							BattleZoneOpen(BATTLEZONE_OPEN, m_nBattlezoneOpenWarZone[x]);
					}
				}
				vargs.pop_front();
			}
		}
	} else if (m_byBattleOpen == NATION_BATTLE) {
		int32 WarElapsedTime = int32(UNIXTIME) - m_byBattleOpenedTime;
		m_byBattleRemainingTime = m_byBattleTime - WarElapsedTime;
		uint8 nBattleZone = g_pMain->m_byBattleZone + ZONE_BATTLE_BASE;

		if (m_bVictory == 0) {
			if (WarElapsedTime == (m_byBattleTime / 24)) // Select captain
			{
				m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
				m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();
				LoadKnightsRankTable(true, true);
			} else if (WarElapsedTime == (m_byBattleTime / 8) && nBattleZone == ZONE_BATTLE4) // War half time + 15 minutes for Nereid's Island.
			{
				if (nBattleZone == ZONE_BATTLE4) // Nereid's Island
				{
					if (m_sKarusMonuments >= 7 && m_sElmoMonuments == 0)
						BattleZoneResult(KARUS);
					else if (m_sKarusMonuments == 0 && m_sElmoMonuments >= 7)
						BattleZoneResult(ELMORAD);
				}
			} else if (WarElapsedTime == (m_byBattleTime / 2)) // War half time.
			{
				if (nBattleZone == ZONE_BATTLE
					|| nBattleZone == ZONE_BATTLE2
					|| nBattleZone == ZONE_BATTLE6
					|| nBattleZone == ZONE_BATTLE3)
					BattleWinnerResult(BATTLE_WINNER_NPC);
				else if (nBattleZone == ZONE_BATTLE4) // Nereid's Island
					BattleWinnerResult(BATTLE_WINNER_MONUMENT);
				else if (nBattleZone == ZONE_BATTLE6) // Oreads
					BattleWinnerResult(BATTLE_WINNER_KILL);
			}

			m_sBattleTimeDelay++;

			if (m_sBattleTimeDelay >= (nBattleZone == ZONE_BATTLE4 ? (m_byBattleTime / 48) : (m_byBattleTime / 24))) {
				m_sBattleTimeDelay = 0;
				Announcement(DECLARE_BATTLE_ZONE_STATUS);
			}
		} else {
			if (WarElapsedTime < m_byBattleTime) // Won the war.
			{
				m_sBattleTimeDelay++;
				if (m_sBattleTimeDelay >= (m_byBattleTime / 24)) {
					m_sBattleTimeDelay = 0;
					Announcement(UNDER_ATTACK_NOTIFY);
				}
			}
		}

		if (m_bResultDelay) {
			m_sBattleResultDelay++;
			if (m_sBattleResultDelay == (m_byBattleTime / (m_byBattleTime / 10))) {
				m_bResultDelay = false;
				BattleZoneResult(m_bResultDelayVictory);
			}
		}

		if (WarElapsedTime >= m_byBattleTime) // War is over.
			BattleZoneClose();
	} else if (m_byBattleOpen == CLAN_BATTLE) {
		if (m_byBattleSiegeWarStartTime == 0) {
			if (m_sBattleTimeDelay == 20 * MINUTE) {
				m_SiegeWarNoticeTime = 30;
				Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
			} else if (m_sBattleTimeDelay == 30 * MINUTE) {
				m_SiegeWarNoticeTime = 20;
				Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
			} else if (m_sBattleTimeDelay == 40 * MINUTE) {
				m_SiegeWarNoticeTime = 10;
				Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
			} else if (m_sBattleTimeDelay == 45 * MINUTE) {
				m_SiegeWarNoticeTime = 5;
				Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
			} else if (m_sBattleTimeDelay == 47 * MINUTE) {
				m_SiegeWarNoticeTime = 3;
				Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
			} else if (m_sBattleTimeDelay == 49 * MINUTE) {
				m_SiegeWarNoticeTime = 1;
				Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
			}

			m_sBattleTimeDelay++;
			m_byBattleSiegeWarOccupy--;
		} else if (m_byBattleSiegeWarStartTime > 0) {
			if (m_byBattleSiegeWarStartTime == (3 * MINUTE)) {
				m_SiegeWarNoticeTime = 3;
				Announcement(IDS_SIEGE_WAR_READY_TIME_NOTICE);
			} else if (m_byBattleSiegeWarStartTime == (2 * MINUTE)) {
				m_SiegeWarNoticeTime = 2;
				Announcement(IDS_SIEGE_WAR_READY_TIME_NOTICE);
			} else if (m_byBattleSiegeWarStartTime == (1 * MINUTE)) {
				m_SiegeWarNoticeTime = 1;
				Announcement(IDS_SIEGE_WAR_READY_TIME_NOTICE);
			} else if (m_byBattleSiegeWarStartTime == 1) {
				Announcement(IDS_SIEGE_WAR_START);
				KickOutZoneUsers(ZONE_DELOS, ZONE_DELOS);
				m_byBattleSiegeWarOpen = true;
				m_byBattleSiegeWarOccupy = 3000;
				bool Status = 1;
				Packet result2(AG_CSW_OP_CL);
				result2 << Status;
				Send_AIServer(&result2);

				_KNIGHTS_SIEGE_WARFARE *pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(1);
				CKnights *pKnights = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);

				Packet result16(WIZ_SIEGE, uint8(2));
				result16.SByte();
				result16 << uint8((pKnights != nullptr && g_pMain->m_byBattleSiegeWarOpen ? 1 : 0) + 1) << (pKnights != nullptr ? pKnights->GetID() : uint16(0)) << (pKnights != nullptr ? pKnights->m_sMarkVersion : uint16(0)) << uint16(0)
					<< uint32(g_pMain->m_byBattleSiegeWarOpen ? g_pMain->m_byBattleSiegeWarOccupy : 0) << uint16(g_pMain->m_byBattleSiegeWarOpen ? ((50 * MINUTE) - g_pMain->m_sBattleTimeDelay) : 0)
					<< (pKnights != nullptr ? pKnights->GetName() : std::string(""));
				g_pMain->Send_Zone(&result16, ZONE_DELOS);
			}
			m_byBattleSiegeWarStartTime--;
		}

		if (m_sBattleTimeDelay == (50 * MINUTE)
			|| m_byBattleSiegeWarOccupy == 0) {
			Announcement(IDS_SIEGE_WAR_END);
			m_sBattleTimeDelay = 0;
			m_byBattleOpen = NO_BATTLE;
			BattleZoneClose();
			m_byBattleSiegeWarOpen = false;
			Announcement(IDS_SIEGE_WAR_VICTORY);

			_KNIGHTS_SIEGE_WARFARE *pKnightSiegeWarFare = g_pMain->GetSiegeMasterKnightsPtr(1);
			CKnights *pKnight = g_pMain->GetClanPtr(pKnightSiegeWarFare->sMasterKnights);
			if (pKnight == nullptr)
				return;

			foreach_array(i, pKnight->m_arKnightsUser) {
				_KNIGHTS_USER *p = &pKnight->m_arKnightsUser[i];
				if (!p->byUsed || p->pSession == nullptr)
					continue;

				p->pSession->GiveKnightCash(1000);
				p->pSession->SendLoyaltyChange(2500);
				p->pSession->AchieveType1(10);
			}
		}
	}
	if (m_byBanishFlag) {
		m_sBanishDelay++;

		if (m_sBanishDelay == (m_byBattleTime / 360))
			Announcement(DECLARE_BAN);
		else if (m_sBanishDelay == (m_byBattleTime / 120)) {
			m_byBanishFlag = false;
			m_sBanishDelay = 0;
			BanishLosers();
		}
	}
}

void CGameServerDlg::BattleZoneResult(uint8 nation) {
	if (m_bVictory)
		return;

	m_bVictory = nation;
	Announcement(DECLARE_WINNER, m_bVictory);
	Announcement(DECLARE_LOSER, nation == KARUS ? ELMORAD : KARUS);

	if (g_pMain->m_byBattleZoneType == ZONE_ARDREAM) {
		BattleZoneClose();
		return;
	}

	m_byKarusOpenFlag = nation == ELMORAD ? true : false;
	m_byElmoradOpenFlag = nation == KARUS ? true : false;
	m_byBanishFlag = true;
	m_sBanishDelay = 0;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser *pUser = TO_USER(itr.second);
		if (!pUser->isInGame())
			continue;

		if (isWarOpen()) {
			// Kick out losers
			if (pUser->GetMap()->isWarZone() && nation == pUser->GetNation())
				pUser->AchieveType1(4);
		}
	}
}

void CGameServerDlg::BattleWinnerResult(BattleWinnerTypes winnertype) {
	uint8 winner_nation = 0;
	uint8 nBattleZone = m_byBattleZone + ZONE_BATTLE_BASE;

	if (winnertype == BATTLE_WINNER_NPC) {
		if (m_sKilledKarusNpc > m_sKilledElmoNpc)
			winner_nation = KARUS;
		else if (m_sKilledElmoNpc > m_sKilledKarusNpc)
			winner_nation = ELMORAD;

		if (winner_nation == 0
			&& (nBattleZone == ZONE_BATTLE
				|| nBattleZone == ZONE_BATTLE2
				|| nBattleZone == ZONE_BATTLE6
				|| nBattleZone == ZONE_BATTLE3)) {
			BattleWinnerResult(BATTLE_WINNER_KILL);
			return;
		}
	} else if (winnertype == BATTLE_WINNER_MONUMENT) {
		if (m_sKarusMonumentPoint > m_sElmoMonumentPoint)
			winner_nation = KARUS;
		else if (m_sElmoMonumentPoint > m_sKarusMonumentPoint)
			winner_nation = ELMORAD;

		if (winner_nation == 0) {
			BattleWinnerResult(BATTLE_WINNER_KILL);
			return;
		}
	} else if (winnertype == BATTLE_WINNER_KILL) {
		if (m_sKarusDead > m_sElmoradDead)
			winner_nation = ELMORAD;
		else if (m_sElmoradDead > m_sKarusDead)
			winner_nation = KARUS;

		if (winner_nation == 0
			&& (nBattleZone == ZONE_BATTLE4
				|| nBattleZone == ZONE_BATTLE6)) {
			BattleWinnerResult(BATTLE_WINNER_NPC);
			return;
		}
	}

	if (winner_nation == 0) // Draw
	{
		TRACE("### WARNING : Battle is Draw Zone=%d, Karus Dead=%d, El Morad Dead=%d, Killed Karus NPC=%d, Killed Human NPC=%d, Karus Monument Point=%d, Karus Monument Point=%d ###\n", m_byBattleZone, m_sKarusDead, m_sElmoradDead, m_sKilledKarusNpc, m_sKilledElmoNpc, m_sKarusMonumentPoint, m_sKarusMonumentPoint);
		BattleZoneClose();
	} else
		BattleZoneResult(winner_nation);
}

void CGameServerDlg::BattleZoneOpen(int nType, uint8 bZone /*= 0*/) {
	if ((nType == BATTLEZONE_OPEN || nType == SNOW_BATTLEZONE_OPEN) && !g_pMain->isWarOpen()) {
		LunarGoldShells();
		m_byBattleOpen = nType == BATTLEZONE_OPEN ? NATION_BATTLE : SNOW_BATTLE;
		m_byOldBattleOpen = nType == BATTLEZONE_OPEN ? NATION_BATTLE : SNOW_BATTLE;
		m_byBattleZone = bZone;
		m_byBattleOpenedTime = int32(UNIXTIME);
		m_byBattleRemainingTime = m_byBattleTime;

		if (bZone + ZONE_BATTLE_BASE == ZONE_BATTLE4)
			SendEventRemainingTime(true, nullptr, ZONE_BATTLE4);

		KickOutZoneUsers(ZONE_ARDREAM);

		if (m_byBattleZoneType == 0) {
			KickOutZoneUsers(ZONE_RONARK_LAND_BASE);
			KickOutZoneUsers(ZONE_RONARK_LAND);
			KickOutZoneUsers(ZONE_BIFROST);
			KickOutZoneUsers(ZONE_KROWAZ_DOMINION);
		}
	} else if (nType == BATTLEZONE_CLOSE && isWarOpen()) {
		DeleteLunarGoldShells();
		Announcement(BATTLEZONE_CLOSE);
	} else if (nType == SNOW_BATTLEZONE_CLOSE && isWarOpen()) {
		DeleteLunarGoldShells();
		Announcement(SNOW_BATTLEZONE_CLOSE);
	} else
		return;

	Announcement(nType);	// Send an announcement out that the battlezone is open/closed.
	Packet result(AG_BATTLE_EVENT, uint8(BATTLE_EVENT_OPEN));
	result << uint8(nType);
	Send_AIServer(&result);
}

void CGameServerDlg::ArdreamEventOpen() {
	g_pMain->OpenArdream = true;
	g_pMain->OpenCZ = false;
	KickOutZoneUsers(ZONE_ARDREAM, ZONE_MORADON);
	KickOutZoneUsers(ZONE_RONARK_LAND, ZONE_MORADON);
	KickOutZoneUsers(ZONE_RONARK_LAND_BASE, ZONE_MORADON);

	std::string sEventMessage = string_format("The Event of Ardream is Opened..!");

	if (!sEventMessage.empty())
		g_pMain->SendAnnouncement(sEventMessage.c_str(), Nation::ALL);
}
void CGameServerDlg::CheckEventTime() {
	int32 EventElapsedTime = int32(UNIXTIME) - m_byEventOpenedTime;
	m_byEventRemainingTime = m_byEventTime - EventElapsedTime;

	if (EventElapsedTime >= m_byEventTime) // War is over.
	{
		if (g_pMain->OpenCZ)
			CZEventZoneClose();
		else
			ArdreamEventZoneClose();
	}
}

void CGameServerDlg::ArdreamEventZoneClose() {
	if (MaxLevel62Control == 0 && g_pMain->OpenArdream) {
		g_pMain->KickOutZoneUsers(ZONE_ARDREAM, ZONE_MORADON);
		g_pMain->OpenArdream = false;
		g_pMain->OpenCZ = true;
		std::string sEventMessage2 = string_format("The Event of Ardream is Closed. Thanks for your Participation.!");

		if (!sEventMessage2.empty())
			g_pMain->SendAnnouncement(sEventMessage2.c_str(), Nation::ALL);
		m_byEventOpenedTime = 0;
		m_byEventRemainingTime = 0;
	}
}
void CGameServerDlg::CZEventZoneClose() {
	if (g_pMain->MaxLevel62Control == 1 && g_pMain->OpenCZ) {
		g_pMain->KickOutZoneUsers(ZONE_RONARK_LAND, ZONE_MORADON);
		g_pMain->OpenCZ = false;
		g_pMain->OpenArdream = true;
		std::string sEventMessages2 = string_format("The Event of Ronark Land is Closed. Thanks for your Participation.!");

		if (!sEventMessages2.empty())
			g_pMain->SendAnnouncement(sEventMessages2.c_str(), Nation::ALL);
		m_byEventOpenedTime = 0;
		m_byEventRemainingTime = 0;
	}
}
void CGameServerDlg::CZEventOpen() {
	g_pMain->OpenCZ = true;
	g_pMain->OpenArdream = false;
	KickOutZoneUsers(ZONE_ARDREAM, ZONE_MORADON);
	KickOutZoneUsers(ZONE_RONARK_LAND, ZONE_MORADON);
	KickOutZoneUsers(ZONE_RONARK_LAND_BASE, ZONE_MORADON);

	std::string sEventMessage = string_format("The Event of Ronark Land is Opened..!");

	if (!sEventMessage.empty())
		g_pMain->SendAnnouncement(sEventMessage.c_str(), Nation::ALL);
}

void CGameServerDlg::SnowBattleZoneClose() {
	BattleZoneOpen(SNOW_BATTLEZONE_CLOSE);
	Packet result(AG_BATTLE_EVENT, uint8(BATTLE_EVENT_OPEN));
	result << uint8(SNOW_BATTLEZONE_CLOSE);
	Send_AIServer(&result);
	ResetBattleZone();
	m_bySnowBanishFlag = true;
}
void CGameServerDlg::SnowBanishLosers() {
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser *pUser = TO_USER(itr.second);
		if (!pUser->isInGame())
			continue;

		// Reset captains
		if (!isWarOpen()) {
			if (pUser->GetFame() == COMMAND_CAPTAIN)
				pUser->ChangeFame(CHIEF);
		}

		if (m_byBattleOpen == SNOW_BATTLE) {
			// Kick out losers
			if (pUser->GetMap()->isWarZone() && m_bVictory != pUser->GetNation())
				pUser->KickOutZoneUser(true);
		} else if (!g_pMain->isWarOpen()) {
			// Kick out invaders
			if ((pUser->GetZoneID() <= ELMORAD && pUser->GetZoneID() != pUser->GetNation())
				|| pUser->GetMap()->isWarZone())
				pUser->KickOutZoneUser(true);
		}
	}
}
void CGameServerDlg::CastleSiegeWarZoneOpen(int nType, uint8 bZone /*= 0*/) {
	m_byBattleOpen = nType;
	m_byOldBattleOpen = nType;
	m_byBattleZone = 0;
	m_sBattleTimeDelay = 0;
	m_byBattleOpenedTime = int32(UNIXTIME);
	m_byBattleSiegeWarOccupy = 6600;
	m_byBattleRemainingTime = m_byBattleTime;
	m_byBattleSiegeWarStartTime = (bZone > 0 ? bZone : 10) * MINUTE;
	m_SiegeWarNoticeTime = (bZone > 0 ? bZone : 10);
	m_byBattleSiegeWarTeleport = true;
	m_byBattleSiegeWarOpen = false;
	KickOutZoneUsers(ZONE_RONARK_LAND, ZONE_MORADON);
	KickOutZoneUsers(ZONE_DELOS, ZONE_MORADON);
	Announcement(IDS_SIEGE_WAR_READY_TIME_NOTICE);
	Packet result(AG_BATTLE_EVENT, uint8(BATTLE_EVENT_OPEN));
	result << uint8(nType);
	Send_AIServer(&result);
}
void CGameServerDlg::BattleZoneClose() {
	if (m_byBattleOpen == CLAN_BATTLE) {
		Announcement(IDS_SIEGE_WAR_END);
		Packet result(AG_BATTLE_EVENT, uint8(BATTLE_EVENT_OPEN));
		result << uint8(BATTLEZONE_CLOSE);
		Send_AIServer(&result);
		ResetBattleZone();
		bool Status = 0;
		Packet result2(AG_CSW_OP_CL);
		result2 << Status;
		Send_AIServer(&result2);
		m_byBanishFlag = true;
		KickOutZoneUsers(ZONE_DELOS, ZONE_MORADON);
	} else {
		BattleZoneOpen(BATTLEZONE_CLOSE);
		Packet result(AG_BATTLE_EVENT, uint8(BATTLE_EVENT_OPEN));
		result << uint8(BATTLEZONE_CLOSE);
		Send_AIServer(&result);
		ResetBattleZone();
		m_byBanishFlag = true;
	}
}

void CGameServerDlg::BattleZoneVictoryCheck() {
	if (m_bKarusFlag >= NUM_FLAG_VICTORY)
		m_bVictory = KARUS;
	else if (m_bElmoradFlag >= NUM_FLAG_VICTORY)
		m_bVictory = ELMORAD;
	else
		return;

	Announcement(DECLARE_WINNER);

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser* pTUser = TO_USER(itr.second);
		if (!pTUser->isInGame()
			|| pTUser->GetZoneID() != pTUser->GetNation()
			|| pTUser->GetNation() != m_bVictory)
			continue;

		pTUser->GoldGain(AWARD_GOLD);
		pTUser->ExpChange(AWARD_EXP);

		if (pTUser->GetFame() == COMMAND_CAPTAIN) {
			if (pTUser->isKing())
				pTUser->SendLoyaltyChange(500);
			else
				pTUser->SendLoyaltyChange(300);
		}

		// Make the winning nation use a victory emotion (yay!)
		pTUser->StateChangeServerDirect(4, 12);
	}
}

/**
* @brief	Kicks invaders out of the invaded nation after a war
*			and resets captains.
**/
void CGameServerDlg::BanishLosers() {
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser *pUser = TO_USER(itr.second);
		if (!pUser->isInGame())
			continue;

		// Reset captains
		if (!isWarOpen()) {
			if (pUser->GetFame() == COMMAND_CAPTAIN)
				pUser->ChangeFame(CHIEF);
		}

		if (m_byBattleOpen == NATION_BATTLE) {
			// Kick out losers
			if (pUser->GetMap()->isWarZone() && m_bVictory != pUser->GetNation())
				pUser->KickOutZoneUser(true);
		} else if (!g_pMain->isWarOpen()) {
			// Kick out invaders
			if ((pUser->GetZoneID() <= ELMORAD && pUser->GetZoneID() != pUser->GetNation())
				|| pUser->GetMap()->isWarZone())
				pUser->KickOutZoneUser(true);
		}
	}
}

void CGameServerDlg::ResetBattleZone() {
	m_bVictory = 0;

	m_sBanishDelay = 0;
	m_byBanishFlag = false;

	m_sBattleResultDelay = 0;
	m_bResultDelay = false;

	m_bKarusFlag = 0;
	m_bElmoradFlag = 0;

	m_byKarusOpenFlag = false;
	m_byElmoradOpenFlag = false;

	m_byBattleSave = false;

	m_byBattleZone = 0;
	m_byBattleZoneType = 0;

	m_byBattleOpen = NO_BATTLE;
	m_byOldBattleOpen = NO_BATTLE;

	m_byBattleOpenedTime = 0;
	m_byBattleRemainingTime = 0;
	m_sBattleTimeDelay = 0;

	m_byBattleSiegeWarStartTime = 0;
	m_SiegeWarNoticeTime = 0;

	m_byBattleSiegeWarOpen = false;
	m_byBattleSiegeWarMomument = false;
	m_byBattleSiegeWarTeleport = false;

	m_sKarusDead = 0;
	m_sElmoradDead = 0;

	m_sKilledKarusNpc = 0;
	m_sKilledElmoNpc = 0;

	m_sKarusMonumentPoint = 0;
	m_sElmoMonumentPoint = 0;
	m_sKarusMonuments = 0;
	m_sElmoMonuments = 0;

	for (int i = 0; i < 7; i++)
		m_sNereidsMonumentNation[i] = 0;

	foreach_stlmap(itr, m_arNpcArray) {
		if (itr->second == nullptr)
			continue;

		if (itr->second->GetZoneID() != ZONE_BATTLE4)
			continue;

		if (itr->second->m_tNpcType == NPC_BATTLE_MONUMENT) {
			itr->second->HpChange(-30000, nullptr, true);
			itr->second->HpChange(-30000, nullptr, true);
			itr->second->HpChange(-30000, nullptr, true);
			itr->second->HpChange(-30000, nullptr, true);
			itr->second->HpChange(-30000, nullptr, true);
			itr->second->HpChange(-30000, nullptr, true);
			g_pMain->NpcUpdate(itr->second->GetProtoID(), itr->second->m_bMonster, 4, 14005);
		}
	}

	m_NationMonumentInformationArray.DeleteAllData();
	m_bMiddleStatueNation = 0;
}

void CGameServerDlg::AutoNoticeEventTimer() {
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;
	uint32 nSecond = g_localTime.tm_sec;

	if ((nMinute % 7 == 0 || nMinute == 0) && nSecond == 0 && m_AutoNoticeIsActive == 1) {
		SendAnnouncement(m_AutoNoticeText.c_str());
	}
}

void CGameServerDlg::ForgettenTempleEventTimer() {
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;
	uint32 nSecond = g_localTime.tm_sec;

	if (!m_bForgettenTempleIsActive) {
		foreach_stlmap_nolock(itr, m_MonsterChallengeArray) {
			if ((nHour == itr->second->bStartTime1 - 1
				|| nHour == itr->second->bStartTime2 - 1
				|| nHour == itr->second->bStartTime3 - 1) && nMinute == 55 && nSecond == 0) {
				if (nHour + 1 == itr->second->bStartTime1)
					m_nForgettenTempleStartHour = itr->second->bStartTime1;
				else if (nHour + 1 == itr->second->bStartTime2)
					m_nForgettenTempleStartHour = itr->second->bStartTime2;
				else if (nHour + 1 == itr->second->bStartTime3)
					m_nForgettenTempleStartHour = itr->second->bStartTime3;

				m_nForgettenTempleLevelMin = itr->second->bLevelMin;
				m_nForgettenTempleLevelMax = itr->second->bLevelMax;

				Announcement(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT);
			} else if ((nHour == itr->second->bStartTime1 - 1
				|| nHour == itr->second->bStartTime2 - 1
				|| nHour == itr->second->bStartTime3 - 1) && nMinute == 59 && nSecond == 40) {
				m_nForgettenTempleLevelMin = itr->second->bLevelMin;
				m_nForgettenTempleLevelMax = itr->second->bLevelMax;
				Announcement(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT);
			} else if ((nHour == itr->second->bStartTime1
				|| nHour == itr->second->bStartTime2
				|| nHour == itr->second->bStartTime3) && nMinute == 0) {
				if (itr->second->bLevelMin == 30)
					m_nForgettenTempleChallengeTime = 1;
				else if (itr->second->bLevelMin == 46)
					m_nForgettenTempleChallengeTime = 2;
				else if (itr->second->bLevelMin == 60)
					m_nForgettenTempleChallengeTime = 3;
			}
		}

		if (m_nForgettenTempleChallengeTime > 0) {
			m_bForgettenTempleIsActive = true;
			m_nForgettenTempleStartTime = int32(UNIXTIME);

			foreach_stlmap_nolock(itr, m_MonsterChallengeSummonListArray)
				if (itr->second->bLevel == m_nForgettenTempleChallengeTime)
					m_nForgettenTempleLastStage++;

			if (m_nForgettenTempleLastStage > 56)
				m_nForgettenTempleLastStage = 56;

			Announcement(IDS_MONSTER_CHALLENGE_OPEN);
		}
	} else if (m_bForgettenTempleIsActive) {
		if (int32(UNIXTIME) - m_nForgettenTempleStartTime == 300 /* 5 minutes */
			&& !m_bForgettenTempleSummonMonsters) {
			m_bForgettenTempleSummonMonsters = true;
			m_nForgettenTempleCurrentStage = 1;
			m_nForgettenTempleLastSummonTime = int32(UNIXTIME) + 30;
			Announcement(IDS_MONSTER_CHALLENGE_START);
		} else if (m_bForgettenTempleSummonMonsters) {
			foreach_stlmap_nolock(itr, m_MonsterChallengeSummonListArray) {
				if (m_nForgettenTempleChallengeTime != itr->second->bLevel
					|| int32(UNIXTIME) - m_nForgettenTempleLastSummonTime < 45
					|| itr->second->bStage != m_nForgettenTempleCurrentStage
					&& m_nForgettenTempleCurrentStage != 57)
					continue;

				if (m_nForgettenTempleCurrentStage > m_nForgettenTempleLastStage)
					m_bForgettenTempleSummonMonsters = false;
				else {
					SpawnEventNpc(itr->second->sSid, true, ZONE_FORGOTTEN_TEMPLE, (float) itr->second->sPosX, 0, (float) itr->second->sPosZ, itr->second->sCount, itr->second->bRange);
					m_nForgettenTempleCurrentStage++;
					m_nForgettenTempleLastSummonTime = int32(UNIXTIME);
				}
			}
		} else {
			if (m_nForgettenTempleCurrentStage > m_nForgettenTempleLastStage && !m_nForgettenTempleBanishFlag) {
				if (m_ForgettenTempleMonsterList.size() == 0) {
					m_nForgettenTempleBanishFlag = true;
					m_nForgettenTempleBanishTime = 200;
					Announcement(IDS_MONSTER_CHALLENGE_VICTORY);
					switch (m_nForgettenTempleChallengeTime) {
					case 1:
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, RED_TREASURE_CHEST);
						break;
					case 2:
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, GREEN_TREASURE_CHEST);
						break;
					case 3:
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, KNIGHT_MEDAL);
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, BLUE_TREASURE_CHEST, 2);
						break;
					}
				} else if (int32(UNIXTIME) - m_nForgettenTempleStartTime >= (HOUR - (10 * MINUTE))) {
					m_nForgettenTempleBanishFlag = true;
					m_nForgettenTempleBanishTime = 0;
				}
			}
		}

		if (m_nForgettenTempleBanishFlag) {
			m_nForgettenTempleBanishTime++;

			if (m_nForgettenTempleBanishTime == 30 /* 30 seconds */)
				Announcement(IDS_MONSTER_CHALLENGE_CLOSE);
			else if (m_nForgettenTempleBanishTime == 230 /* 5 minutes */) {
				m_nForgettenTempleBanishFlag = false;
				m_nForgettenTempleBanishTime = 0;

				m_nForgettenTempleUsers.clear();
				m_bForgettenTempleIsActive = false;
				m_nForgettenTempleStartHour = 0;
				m_nForgettenTempleLevelMin = 0;
				m_nForgettenTempleLevelMax = 0;
				m_nForgettenTempleStartTime = 0;
				m_nForgettenTempleChallengeTime = 0;
				m_bForgettenTempleSummonMonsters = false;
				m_nForgettenTempleCurrentStage = 0;
				m_nForgettenTempleLastStage = 0;

				BOOST_FOREACH(auto itr, m_ForgettenTempleMonsterList)
					KillNpc(itr.first);

				m_ForgettenTempleMonsterList.clear();
				m_nForgettenTempleLastSummonTime = 0;

				SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, GOLD_EAGLE_WINGS, 1, 1);
				KickOutZoneUsers(ZONE_FORGOTTEN_TEMPLE);
			}
		}
	}
}

void CGameServerDlg::EventZoneTimer() {
	int nWeekDay = g_localTime.tm_wday;
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;
	uint32 nSecond = g_localTime.tm_sec;

	if (!m_bEventZoneIsActive) {
		foreach_stlmap_nolock(itr, m_EventTimesArray) {
			std::list<std::string> vargs1 = StrSplit(m_sEventZoneOpenDays1, ",");
			std::list<std::string> vargs2 = StrSplit(m_sEventZoneOpenDays2, ",");
			std::list<std::string> vargs3 = StrSplit(m_sEventZoneOpenDays3, ",");
			uint8 nDaySize1 = (uint8) vargs1.size();
			uint8 nDaySize2 = (uint8) vargs2.size();
			uint8 nDaySize3 = (uint8) vargs3.size();
			std::string sEventStart;
			std::string sPvpMiniStart;
			std::string sJuradEventStart;
			std::string sEventName;

			if (itr->second->sIndex == 1)
				sEventName = "Dark Land";
			if (itr->second->sIndex == 2)
				sEventName = "Mini PVP Zone";
			if (itr->second->sIndex == 3)
				sEventName = "Devils Square";
			if (itr->second->sIndex == 4)
				sEventName = "Lost Castle";
			if (itr->second->sIndex == 5)
				sEventName = "PVP Zone";
			if (itr->second->sIndex == 6)
				sEventName = "Juraid Mountain";

			sPvpMiniStart = string_format("%s will open at %d(GMT+2).Max level 45. To enter talk with npc [Exclusive Events] Nian.", sEventName.c_str(), m_nEventZoneStartHour);
			sEventStart = string_format("%s will open at %d(GMT+2).Min level 70. To enter talk with npc [Exclusive Events] Nian.", sEventName.c_str(), m_nEventZoneStartHour);
			sJuradEventStart = string_format("%s will open at %d(GMT+2).Min level 70. Only users in Eslant will be automatically teleported!", sEventName.c_str(), m_nEventZoneStartHour);

			if (nDaySize1 > 0 && itr->second->AllDays == 0 && itr->second->sIndex == 5) {
				uint8 nDay1 = 0;
				for (int i = 0; i < nDaySize1; i++) {
					nDay1 = atoi(vargs1.front().c_str());
					if (nWeekDay == nDay1) {
						if ((nHour == itr->second->bTime1 - 1
							|| nHour == itr->second->bTime2 - 1
							|| nHour == itr->second->bTime3 - 1) && nMinute == 55 && nSecond == 0) {
							if (nHour + 1 == itr->second->bTime1)
								m_nEventZoneStartHour = itr->second->bTime1;
							else if (nHour + 1 == itr->second->bTime2)
								m_nEventZoneStartHour = itr->second->bTime2;
							else if (nHour + 1 == itr->second->bTime3)
								m_nEventZoneStartHour = itr->second->bTime3;

							m_nEventZoneLevelMin = itr->second->bLvMin;
							m_nEventZoneLevelMax = itr->second->bLvMax;
							SendAnnouncement(sEventStart.c_str());
						} else if ((nHour == itr->second->bTime1 - 1
							|| nHour == itr->second->bTime2 - 1
							|| nHour == itr->second->bTime3 - 1) && nMinute == 59 && nSecond == 40) {
							m_nEventZoneLevelMin = itr->second->bLvMin;
							m_nEventZoneLevelMax = itr->second->bLvMax;
							SendAnnouncement(sEventStart.c_str());
						}

						else if ((nHour == itr->second->bTime1
							|| nHour == itr->second->bTime2
							|| nHour == itr->second->bTime3) && nMinute == 0) {
							if (itr->second->bLvMin == 50)
								m_nEventZoneTime = 5;
						}
					}
					vargs1.pop_front();
				}
			}

			else if (nDaySize2 > 0 && itr->second->sIndex == 3 && itr->second->AllDays == 0) {
				uint8 nDay2 = 0;
				for (int i = 0; i < nDaySize2; i++) {
					nDay2 = atoi(vargs2.front().c_str());
					if (nWeekDay == nDay2) {
						if ((nHour == itr->second->bTime1 - 1
							|| nHour == itr->second->bTime2 - 1
							|| nHour == itr->second->bTime3 - 1) && nMinute == 55 && nSecond == 0) {
							if (nHour + 1 == itr->second->bTime1)
								m_nEventZoneStartHour = itr->second->bTime1;
							else if (nHour + 1 == itr->second->bTime2)
								m_nEventZoneStartHour = itr->second->bTime2;
							else if (nHour + 1 == itr->second->bTime3)
								m_nEventZoneStartHour = itr->second->bTime3;

							m_nEventZoneLevelMin = itr->second->bLvMin;
							m_nEventZoneLevelMax = itr->second->bLvMax;
							SendAnnouncement(sEventStart.c_str());
						} else if ((nHour == itr->second->bTime1 - 1
							|| nHour == itr->second->bTime2 - 1
							|| nHour == itr->second->bTime3 - 1) && nMinute == 59 && nSecond == 40) {
							m_nEventZoneLevelMin = itr->second->bLvMin;
							m_nEventZoneLevelMax = itr->second->bLvMax;
							SendAnnouncement(sEventStart.c_str());
						}

						else if ((nHour == itr->second->bTime1
							|| nHour == itr->second->bTime2
							|| nHour == itr->second->bTime3) && nMinute == 0) {
							if (itr->second->bLvMin == 30)
								m_nEventZoneTime = 3;
						}
					}
					vargs2.pop_front();
				}
			}

			else if (nDaySize3 > 0 && itr->second->sIndex == 4 && itr->second->AllDays == 0) {
				uint8 nDay3 = 0;
				for (int i = 0; i < nDaySize3; i++) {
					nDay3 = atoi(vargs3.front().c_str());
					if (nWeekDay == nDay3) {
						if ((nHour == itr->second->bTime1 - 1
							|| nHour == itr->second->bTime2 - 1
							|| nHour == itr->second->bTime3 - 1) && nMinute == 55 && nSecond == 0) {
							if (nHour + 1 == itr->second->bTime1)
								m_nEventZoneStartHour = itr->second->bTime1;
							else if (nHour + 1 == itr->second->bTime2)
								m_nEventZoneStartHour = itr->second->bTime2;
							else if (nHour + 1 == itr->second->bTime3)
								m_nEventZoneStartHour = itr->second->bTime3;

							m_nEventZoneLevelMin = itr->second->bLvMin;
							m_nEventZoneLevelMax = itr->second->bLvMax;
							SendAnnouncement(sEventStart.c_str());
						} else if ((nHour == itr->second->bTime1 - 1
							|| nHour == itr->second->bTime2 - 1
							|| nHour == itr->second->bTime3 - 1) && nMinute == 59 && nSecond == 40) {
							m_nEventZoneLevelMin = itr->second->bLvMin;
							m_nEventZoneLevelMax = itr->second->bLvMax;
							SendAnnouncement(sEventStart.c_str());
						}

						else if ((nHour == itr->second->bTime1
							|| nHour == itr->second->bTime2
							|| nHour == itr->second->bTime3) && nMinute == 0) {
							if (itr->second->bLvMin == 40)
								m_nEventZoneTime = 4;
						}
					}
					vargs3.pop_front();
				}
			}

			else if (itr->second->AllDays == 1) {
				if ((nHour == itr->second->bTime1 - 1
					|| nHour == itr->second->bTime2 - 1
					|| nHour == itr->second->bTime3 - 1) && nMinute == 55 && nSecond == 0) {
					if (nHour + 1 == itr->second->bTime1)
						m_nEventZoneStartHour = itr->second->bTime1;
					else if (nHour + 1 == itr->second->bTime2)
						m_nEventZoneStartHour = itr->second->bTime2;
					else if (nHour + 1 == itr->second->bTime3)
						m_nEventZoneStartHour = itr->second->bTime3;

					m_nEventZoneLevelMin = itr->second->bLvMin;
					m_nEventZoneLevelMax = itr->second->bLvMax;
					if (itr->second->sIndex == 1)
						SendAnnouncement(sEventStart.c_str());
					else if (itr->second->sIndex == 2) {
						SendAnnouncement(sPvpMiniStart.c_str());
						SendNotice(sPvpMiniStart.c_str());
					} else {
						SendAnnouncement(sJuradEventStart.c_str());
						SendNotice(sJuradEventStart.c_str());
					}
				} else if ((nHour == itr->second->bTime1 - 1
					|| nHour == itr->second->bTime2 - 1
					|| nHour == itr->second->bTime3 - 1) && nMinute == 59 && nSecond == 40) {
					m_nEventZoneLevelMin = itr->second->bLvMin;
					m_nEventZoneLevelMax = itr->second->bLvMax;
					if (itr->second->sIndex == 1)
						SendAnnouncement(sEventStart.c_str());
					else if (itr->second->sIndex == 2) {
						SendAnnouncement(sPvpMiniStart.c_str());
						SendNotice(sPvpMiniStart.c_str());
					} else {
						SendAnnouncement(sJuradEventStart.c_str());
						SendNotice(sJuradEventStart.c_str());
					}
				}

				else if ((nHour == itr->second->bTime1
					|| nHour == itr->second->bTime2
					|| nHour == itr->second->bTime3) && nMinute == 0) {
					if (itr->second->bLvMin == 10)
						m_nEventZoneTime = 1;
					else if (itr->second->bLvMin == 20)
						m_nEventZoneTime = 2;
					else if (itr->second->bLvMin == 60)
						m_nEventZoneTime = 6;
				}
			}
		}

		if (m_nEventZoneTime > 0) {
			m_bEventZoneIsActive = true;
			m_nEventZoneStartTime = int32(UNIXTIME);

			Announcement(IDS_EVENT_ZONE_OPEN);

			if (m_nEventZoneTime == 5)
				KickOutZoneUsers(ZONE_RONARK_LAND);
			else if (m_nEventZoneTime == 6) {
				KickOutZoneUsers(ZONE_ELMORAD_ESLANT, ZONE_JURAD_MOUNTAIN);
				KickOutZoneUsers(ZONE_KARUS_ESLANT, ZONE_JURAD_MOUNTAIN);
			}
		}
	} else if (m_bEventZoneIsActive) {
		if (int32(UNIXTIME) - m_nEventZoneStartTime == 120) /* 2 minutes */
		{
			Announcement(IDS_EVENT_ZONE_START);
		} else if (!m_nEventZoneBanishFlag
			&& (((m_nEventZoneTime == 6 || m_nEventZoneTime == 4) && (int32(UNIXTIME) - m_nEventZoneStartTime >= (29 * MINUTE)))
				|| (m_nEventZoneTime != 6 && m_nEventZoneTime != 4 && (int32(UNIXTIME) - m_nEventZoneStartTime >= (44 * MINUTE))))) {
			m_nEventZoneBanishFlag = true;
			m_nEventZoneBanishTime = 180;
			Announcement(IDS_EVENT_ZONE_VICTO);

			switch (m_nEventZoneTime) {
			case 2:
			{
				std::string sEventRewardHuman;
				std::string sEventRewardOrc;
				SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
				BOOST_FOREACH(auto itr, sessMap) {
					CUser * pUser = TO_USER(itr.second);
					if (pUser == nullptr
						|| !pUser->isInGame()
						|| pUser->isGM()
						|| pUser->GetZoneID() != ZONE_PVP_EVENT)
						continue;

					if (pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) == 1) {
						if (pUser->GetNation() == KARUS)
							sEventRewardOrc = string_format("%s has been the Karus top 1 of the PvP Event!", pUser->GetName().c_str());
						else
							sEventRewardHuman = string_format("%s has been the Human top 1 of the PvP Event!", pUser->GetName().c_str());

						pUser->GiveItem(KNIGHT_MEDAL, 1);
						pUser->GiveItem(389196000, 1);
						pUser->GiveItem(GOLD_EAGLE_WINGS, 1, true, 2);
						pUser->SendLoyaltyChange(200);
					} else  if (pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) > 1 && pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) <= 3) {
						pUser->GiveItem(389197000, 1);
						pUser->GiveItem(900180669, 1, true, 2);
						pUser->SendLoyaltyChange(100);
					}
				}
				SendNotice(sEventRewardOrc.c_str());
				SendNotice(sEventRewardHuman.c_str());
				SendItemZoneUsers(ZONE_PVP_EVENT, GREEN_TREASURE_CHEST);
			}
			break;
			case 3:
			{
				std::string sEventRewardHuman;
				std::string sEventRewardOrc;
				SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
				BOOST_FOREACH(auto itr, sessMap) {
					CUser * pUser = TO_USER(itr.second);
					if (pUser == nullptr
						|| !pUser->isInGame()
						|| pUser->isGM()
						|| pUser->GetZoneID() != ZONE_RONARK_LAND_BASE)
						continue;

					if (pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) == 1) {
						if (pUser->GetNation() == KARUS)
							sEventRewardOrc = string_format("%s has been the Karus top 1 of the Devil Square Event!", pUser->GetName().c_str());
						else
							sEventRewardHuman = string_format("%s has been the Human top 1 of the Devil Square Event!", pUser->GetName().c_str());

						pUser->GiveItem(KNIGHT_MEDAL, 1);
						pUser->GiveItem(389196000, 1);
						pUser->GiveItem(900028683, 1, true, 2);
						pUser->SendLoyaltyChange(200);
					} else  if (pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) > 1 && pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) <= 3) {
						pUser->GiveItem(389197000, 1);
						pUser->GiveItem(910248763, 1, true, 2);
						pUser->SendLoyaltyChange(100);
					}
				}
				SendNotice(sEventRewardOrc.c_str());
				SendNotice(sEventRewardHuman.c_str());
				SendItemZoneUsers(ZONE_RONARK_LAND_BASE, GREEN_TREASURE_CHEST);
			}
			break;
			case 4:
				SendItemZoneUsers(ZONE_LOST_TEMPLE, BLUE_TREASURE_CHEST);
				break;
			case 5:
			{
				std::string sEventRewardHuman;
				std::string sEventRewardOrc;
				SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
				BOOST_FOREACH(auto itr, sessMap) {
					CUser * pUser = TO_USER(itr.second);
					if (pUser == nullptr
						|| !pUser->isInGame()
						|| pUser->isGM()
						|| pUser->GetZoneID() != ZONE_ARDREAM)
						continue;

					if (pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) == 1) {
						if (pUser->GetNation() == KARUS)
							sEventRewardOrc = string_format("%s has been the Karus top 1 of the PvP Event!", pUser->GetName().c_str());
						else
							sEventRewardHuman = string_format("%s has been the Human top 1 of the PvP Event!", pUser->GetName().c_str());

						pUser->GiveItem(KNIGHT_MEDAL, 1);
						pUser->GiveItem(389196000, 1);
						pUser->GiveItem(GOLD_EAGLE_WINGS, 1, true, 2);
						pUser->SendLoyaltyChange(200);
					} else  if (pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) > 1 && pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) <= 3) {
						pUser->GiveItem(389197000, 1);
						pUser->GiveItem(900180669, 1, true, 2);
						pUser->SendLoyaltyChange(100);
					}
				}
				SendNotice(sEventRewardOrc.c_str());
				SendNotice(sEventRewardHuman.c_str());
				SendItemZoneUsers(ZONE_ARDREAM, GREEN_TREASURE_CHEST);
			}
			break;
			case 6:
				SendItemZoneUsers(ZONE_JURAD_MOUNTAIN, BLUE_TREASURE_CHEST);
				break;
			}
		}

		if (m_nEventZoneBanishFlag) {
			m_nEventZoneBanishTime++;

			if (m_nEventZoneBanishTime == 30) {
				Announcement(IDS_EVENT_ZONE_CLOSE);
				m_nEventZoneBanishFlag = false;
				m_nEventZoneBanishTime = 0;

				m_nEventZoneUsers.clear();
				m_bEventZoneIsActive = false;
				m_nEventZoneStartHour = 0;
				m_nEventZoneLevelMin = 0;
				m_nEventZoneLevelMax = 0;
				m_nEventZoneStartTime = 0;
				m_nEventZoneTime = 0;

				KickOutZoneUsers(ZONE_DARK_LAND);
				KickOutZoneUsers(ZONE_LOST_TEMPLE);
				KickOutZoneUsers(ZONE_JURAD_MOUNTAIN);
				KickOutZoneUsers(ZONE_RONARK_LAND_BASE);
				KickOutZoneUsers(ZONE_ARDREAM);
				KickOutZoneUsers(ZONE_PVP_EVENT);
			}

			else if (!m_nEventZoneBanishFlag && !m_bEventZoneIsActive) {
				KickOutZoneUsers(ZONE_DARK_LAND);
				KickOutZoneUsers(ZONE_LOST_TEMPLE);
				KickOutZoneUsers(ZONE_JURAD_MOUNTAIN);
				KickOutZoneUsers(ZONE_RONARK_LAND_BASE);
				KickOutZoneUsers(ZONE_ARDREAM);
				KickOutZoneUsers(ZONE_PVP_EVENT);
			}
		}
	}
}

void CGameServerDlg::TempleEventTimer() {
	// Check if we are ready to start a new event.
	if (m_nextEvent == -1 && pTempleEvent.ActiveEvent == -1 && m_eventStartTime < system_clock::now()) {
		uint32 nHour = g_localTime.tm_hour;
		uint32 nMinute = g_localTime.tm_min;
		uint32 nSeconds = g_localTime.tm_sec;
		for (const auto eventTime : m_nBorderDefenseWarTime) {
			if (nHour == eventTime && nMinute == 0) {
				m_nextEvent = TEMPLE_EVENT_BORDER_DEFENCE_WAR;
				m_eventStartTime = system_clock::now() + minutes(10);
				break;
			}
		}
		for (const auto eventTime : m_nChaosTime) {
			if (nHour == eventTime && nMinute == 0) {
				m_nextEvent = TEMPLE_EVENT_CHAOS;
				m_eventStartTime = system_clock::now() + minutes(10);
				break;
			}
		}
		for (const auto eventTime : m_nJuraidTime) {
			if (nHour == eventTime && nMinute == 0) {
				m_nextEvent = TEMPLE_EVENT_JURAD_MOUNTAIN;
				m_eventStartTime = system_clock::now() + minutes(10);
				break;
			}
		}
	}

	// m_nTempleEventRemainSeconds keeps track of the seconds until
	// the event starts. This value is sent in a packet to the client.
	// This needs to go after setting the event otherwise we'll send 0 seconds
	// and the client will bug out.
	if (pTempleEvent.ActiveEvent != -1 ||
		m_eventStartTime > system_clock::now()) {
		auto timeDiff = m_eventStartTime - system_clock::now();
		auto timeDiffSec = duration_cast<seconds>(timeDiff).count();
		m_nTempleEventRemainSeconds = (uint16) max((int) timeDiffSec, 0);
	} else {
		m_nTempleEventRemainSeconds = 0;
	}

	// Initialize a new event
	if (pTempleEvent.ActiveEvent == -1 && m_nextEvent != -1) {
		if (m_nextEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR) {
			pTempleEvent.ActiveEvent = TEMPLE_EVENT_BORDER_DEFENCE_WAR;
			pTempleEvent.ZoneID = ZONE_BORDER_DEFENSE_WAR;

			pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1 = m_nBorderDefenseWarPrizeWonItemNo1[0];
			pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2 = m_nBorderDefenseWarPrizeWonItemNo2[0];
			pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3 = m_nBorderDefenseWarPrizeWonItemNo3[0];
			pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_K = m_nBorderDefenseWarPrizeWonItemNo4_K[0];
			pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_H = m_nBorderDefenseWarPrizeWonItemNo4_H[0];
			pTempleEvent.m_nBorderDefenseWarPrizeWonLoyalty = m_nBorderDefenseWarPrizeWonLoyalty[0];
			pTempleEvent.m_nBorderDefenseWarPrizeWonKnightCash = m_nBorderDefenseWarPrizeWonKnightCash[0];
			pTempleEvent.m_nBorderDefenseWarPrizeLoserKnightCash = m_nBorderDefenseWarPrizeLoserKnightCash[0];
			pTempleEvent.m_nBorderDefenseWarPrizeLoserLoyalty = m_nBorderDefenseWarPrizeLoserLoyalty[0];
			pTempleEvent.m_nBorderDefenseWarPrizeLoserItem = m_nBorderDefenseWarPrizeLoserItem[0];
			pTempleEvent.m_nBorderDefenseWarMAXLEVEL = m_nBorderDefenseWarMAXLEVEL[0];
			pTempleEvent.m_nBorderDefenseWarMINLEVEL = m_nBorderDefenseWarMINLEVEL[0];

			TempleEventStart();
		} else if (m_nextEvent == TEMPLE_EVENT_CHAOS) {
			pTempleEvent.ActiveEvent = TEMPLE_EVENT_CHAOS;
			pTempleEvent.ZoneID = ZONE_CHAOS_DUNGEON;

			pTempleEvent.m_nChaosPrizeWonItemNo1 = m_nChaosPrizeWonItemNo1[0];
			pTempleEvent.m_nChaosPrizeWonItemNo2 = m_nChaosPrizeWonItemNo2[0];
			pTempleEvent.m_nChaosPrizeWonItemNo3 = m_nChaosPrizeWonItemNo3[0];
			pTempleEvent.m_nChaosPrizeWonItemNo4_K = m_nChaosPrizeWonItemNo4_K[0];
			pTempleEvent.m_nChaosPrizeWonItemNo4_H = m_nChaosPrizeWonItemNo4_H[0];
			pTempleEvent.m_nChaosPrizeWonLoyalty = m_nChaosPrizeWonLoyalty[0];
			pTempleEvent.m_nChaosPrizeWonExp = m_nChaosPrizeWonExp[0];
			pTempleEvent.m_nChaosPrizeWonKnightCash = m_nChaosPrizeWonKnightCash[0];

			pTempleEvent.m_nChaosPrizeLoserKnightCash = m_nChaosPrizeLoserKnightCash[0];
			pTempleEvent.m_nChaosPrizeLoserLoyalty = m_nChaosPrizeLoserLoyalty[0];
			pTempleEvent.m_nChaosPrizeLoserItem = m_nChaosPrizeLoserItem[0];
			pTempleEvent.m_nChaosPrizeLoserExp = m_nChaosPrizeLoserExp[0];

			TempleEventStart();
		} else if (m_nextEvent == TEMPLE_EVENT_JURAD_MOUNTAIN) {
			pTempleEvent.ActiveEvent = TEMPLE_EVENT_JURAD_MOUNTAIN;
			pTempleEvent.ZoneID = ZONE_JURAD_MOUNTAIN;

			pTempleEvent.m_nJuraidMountainOdulTipi = m_nJuraidMountainOdulTipi[0];
			pTempleEvent.m_nJuraidMountainPrizeWonItemNo1 = m_nJuraidMountainPrizeWonItemNo1[0];
			pTempleEvent.m_nJuraidMountainPrizeWonItemNo2 = m_nJuraidMountainPrizeWonItemNo2[0];
			pTempleEvent.m_nJuraidMountainPrizeWonItemNo3 = m_nJuraidMountainPrizeWonItemNo3[0];
			pTempleEvent.m_nJuraidMountainPrizeWonItemNo4_K = m_nJuraidMountainPrizeWonItemNo4_K[0];
			pTempleEvent.m_nJuraidMountainPrizeWonItemNo4_H = m_nJuraidMountainPrizeWonItemNo4_H[0];
			pTempleEvent.m_nJuraidMountainPrizeWonLoyalty = m_nJuraidMountainPrizeWonLoyalty[0];
			pTempleEvent.m_nJuraidMountainPrizeWonExp = m_nJuraidMountainPrizeWonExp[0];
			pTempleEvent.m_nJuraidMountainPrizeWonKnightCash = m_nJuraidMountainPrizeWonKnightCash[0];

			pTempleEvent.m_nJuraidMountainPrizeLoserKnightCash = m_nJuraidMountainPrizeLoserKnightCash[0];
			pTempleEvent.m_nJuraidMountainPrizeLoserLoyalty = m_nJuraidMountainPrizeLoserLoyalty[0];
			pTempleEvent.m_nJuraidMountainPrizeLoserItem = m_nJuraidMountainPrizeLoserItem[0];
			pTempleEvent.m_nJuraidMountainPrizeLoserExp = m_nJuraidMountainPrizeLoserExp[0];
			pTempleEvent.m_nJuraidMountainMAXLEVEL = m_nJuraidMountainMAXLEVEL[0];
			pTempleEvent.m_nJuraidMountainMINLEVEL = m_nJuraidMountainMINLEVEL[0];
			JuraidTempleEventStart();
		}
		m_nextEvent = -1;
		return;
	}

	/* Update an event that was previously started. For some events an update can
	   consist of making the arena attackable, non-attackable, or shutting down the
	   event and teleporting everybody out. */
	if (pTempleEvent.ActiveEvent != -1) {
		if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR) {
			for (int i = 1; i < MAX_TEMPLE_EVENT_ROOM; i++) {
				if (pTempleEvent.m_sMiniTimerNation[i] == 0)
					continue;

				if (pTempleEvent.m_sBdwMiniTimer[i]-- != 0)
					continue;

				TempleEventFinish(i, pTempleEvent.m_sMiniTimerNation[i]);
			}

			if (system_clock::now() >= m_eventStartTime
				&& !pTempleEvent.isActive) {
				m_nTempleEventRemainSeconds = 0;
				pTempleEvent.LastEventRoom = 1;
				pTempleEvent.isActive = true;
				TempleEventStart();
				TempleEventTeleportUsers();
				TempleEventSummon();
			} else if (system_clock::now() >= m_eventStartTime + minutes(30)
				&& pTempleEvent.isAttackable) {
				TerminationFinish();
				pTempleEvent.isAttackable = false;
			} else if (system_clock::now() >= m_eventStartTime + minutes(30) + seconds(20)
				&& pTempleEvent.isActive) {
				TempleEventFinish(0, 0);
				pTempleEvent.ActiveEvent = -1;
			}
		} else if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_CHAOS) {
			if (system_clock::now() >= m_eventStartTime
				&& !pTempleEvent.isActive) {
				m_nTempleEventRemainSeconds = 0;
				pTempleEvent.LastEventRoom = 1;
				pTempleEvent.isActive = true;
				TempleEventStart(); // Set RemainSeconds to zero
				TempleEventTeleportUsers();
				TempleEventSummon();
				// Enable Attacking for 19 minutes
			} else if (system_clock::now() >= m_eventStartTime + minutes(1)
				&& system_clock::now() < m_eventStartTime + minutes(20)
				&& !pTempleEvent.isAttackable) {
				pTempleEvent.isAttackable = true;
				// Finish the event and allow 20 seconds until TP
			} else if (system_clock::now() >= m_eventStartTime + minutes(20)
				&& pTempleEvent.isAttackable) {
				TerminationFinish();
				pTempleEvent.isAttackable = false;
				// Tp and close the event
			} else if (system_clock::now() >= m_eventStartTime + minutes(20) + seconds(20)
				&& pTempleEvent.isActive) {
				TempleEventFinish(0, 0);
				pTempleEvent.ActiveEvent = -1;
			}
		} else if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_JURAD_MOUNTAIN) {
			if (system_clock::now() >= m_eventStartTime
				&& !pTempleEvent.isActive) {
				m_nTempleEventRemainSeconds = 0;
				pTempleEvent.LastEventRoom = 1;
				pTempleEvent.isActive = true;
				JuraidTempleEventStart(); // Set RemainSeconds to zero
				TempleEventTeleportUsers();
				TempleEventSummon();
			} else if (system_clock::now() + minutes(1) >= m_eventStartTime
				&& system_clock::now() + minutes(40) < m_eventStartTime
				&& !pTempleEvent.isAttackable) {
				pTempleEvent.isAttackable = true;
			} else if (system_clock::now() + minutes(40) >= m_eventStartTime
				&& pTempleEvent.isAttackable) {
				TerminationFinish();
				pTempleEvent.isAttackable = false;
			} else if (system_clock::now() + minutes(40) + seconds(20) >= m_eventStartTime
				&& pTempleEvent.isActive) {
				TempleEventFinish(0, 0);
				pTempleEvent.ActiveEvent = -1;
			}
		}
	}
}

void CGameServerDlg::TempleEventStart() {
	Packet result(WIZ_EVENT, uint8(TEMPLE_EVENT));
	pTempleEvent.StartTime = (uint32) UNIXTIME;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;
	pTempleEvent.AllUserCount = 0;
	memset(pTempleEvent.KarusDeathCount, 0, sizeof(pTempleEvent.KarusDeathCount));
	memset(pTempleEvent.ElmoDeathCount, 0, sizeof(pTempleEvent.ElmoDeathCount));
	result << (uint16) pTempleEvent.ActiveEvent << m_nTempleEventRemainSeconds;
	Send_All(&result);
}

void CGameServerDlg::JuraidTempleEventStart() {
	pTempleEvent.StartTime = (uint32) UNIXTIME;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;
	pTempleEvent.AllUserCount = 0;
	memset(pTempleEvent.isDevaFlag, 0, sizeof(pTempleEvent.isDevaFlag));
	memset(pTempleEvent.isDevaControl, 0, sizeof(pTempleEvent.isDevaControl));
	memset(pTempleEvent.DevaNation, 0, sizeof(pTempleEvent.DevaNation));
	memset(pTempleEvent.KarusDeathCount, 0, sizeof(pTempleEvent.KarusDeathCount));
	memset(pTempleEvent.ElmoDeathCount, 0, sizeof(pTempleEvent.ElmoDeathCount));
	memset(pTempleEvent.KarusDeathRoom1, 0, sizeof(pTempleEvent.KarusDeathRoom1));
	memset(pTempleEvent.KarusDeathRoom2, 0, sizeof(pTempleEvent.KarusDeathRoom2));
	memset(pTempleEvent.KarusDeathRoom3, 0, sizeof(pTempleEvent.KarusDeathRoom3));
	memset(pTempleEvent.ElmoDeathRoom1, 0, sizeof(pTempleEvent.ElmoDeathRoom1));
	memset(pTempleEvent.ElmoDeathRoom2, 0, sizeof(pTempleEvent.ElmoDeathRoom2));
	memset(pTempleEvent.ElmoDeathRoom3, 0, sizeof(pTempleEvent.ElmoDeathRoom3));
	Announcement(IDS_JURAD_EVENT_OPEN);
}

void CGameServerDlg::TempleEventTeleportUsers() {
	switch (pTempleEvent.ActiveEvent) {
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
		pTempleEvent.isAttackable = true;
		break;
	case TEMPLE_EVENT_CHAOS:
		pTempleEvent.isAttackable = false;
		break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
		pTempleEvent.isAttackable = true;
		break;
	}

	foreach_stlmap(itr, m_TempleEventUserArray) {
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser == nullptr
			|| !pUser->isInGame())
			continue;

		if (pTempleEvent.ZoneID == ZONE_CHAOS_DUNGEON) {
			if (pUser->CheckExistItem(CHAOS_MAP, 1))
				pUser->RobItem(CHAOS_MAP, 1);

			pUser->ZoneChange(pTempleEvent.ZoneID, 0.0f, 0.0f); // Start position is random...
		} else if (pTempleEvent.ZoneID == ZONE_JURAD_MOUNTAIN)
			pUser->ZoneChange(pTempleEvent.ZoneID, 0.0f, 0.0f);
		else if (pTempleEvent.ZoneID == ZONE_BORDER_DEFENSE_WAR)
			pUser->ZoneChange(pTempleEvent.ZoneID, 0.0f, 0.0f);
	}

	if (pTempleEvent.ZoneID == ZONE_BORDER_DEFENSE_WAR)
		TerminitionTime(ZONE_BORDER_DEFENSE_WAR, 1801);
	else if (pTempleEvent.ZoneID == ZONE_JURAD_MOUNTAIN)
		TerminitionTime(ZONE_JURAD_MOUNTAIN, 3001);
	else if (pTempleEvent.ZoneID == ZONE_CHAOS_DUNGEON)
		TerminitionTime(ZONE_CHAOS_DUNGEON, 1201);
}

uint8 CGameServerDlg::TempleEventGetRoomUsers(uint16 nEventRoom, uint8 nNation) {
	uint8 nEventRoomUserCount = 0;

	foreach_stlmap(itr, m_TempleEventUserArray) {
		if (itr->second->m_bEventRoom != nEventRoom)
			continue;

		CUser *pUser = GetUserPtr(itr->second->m_socketID);

		if (!pUser && pUser == nullptr)
			continue;

		if (pUser->isInGame() && pUser->GetEventRoom() == nEventRoom) {
			if (nNation == 0) {
				nEventRoomUserCount++;
				continue;
			}
			if (nNation != pUser->GetNation())
				continue;

			if (nNation == pUser->GetNation())
				nEventRoomUserCount++;
		}
	}

	return nEventRoomUserCount;
}

void CGameServerDlg::MonsterStoneSummon(uint16 RoomEvent, uint8 ZoneID) {
	foreach_stlmap_nolock(itr, m_MonsterRespawnListInformationArray) {
		if (itr->second->ZoneID == ZoneID && itr->second->sCount == 1) {
			if (itr->second->sSid != 7032
				&& itr->second->sSid != 7033
				&& itr->second->sSid != 7034)
				SpawnEventNpc(itr->second->sSid, true, itr->second->ZoneID, itr->second->X, itr->second->Y, itr->second->Z, itr->second->sCount, 2, 25 * 60, 0, 0, -1, RoomEvent);
			else
				SpawnEventNpc(itr->second->sSid, true, itr->second->ZoneID, itr->second->X, itr->second->Y, itr->second->Z, itr->second->sCount, 0, 25 * 60, 0, 0, -1, RoomEvent);
		} else if (itr->second->ZoneID == ZoneID && itr->second->sCount == 2) {
			SpawnEventNpc(itr->second->sSid, false, itr->second->ZoneID, itr->second->X, itr->second->Y, itr->second->Z, 1, 2, 25 * 60, 0, 0, -1, RoomEvent);
		}
	}
}

void CGameServerDlg::TempleEventSummon() {
	if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_CHAOS) {
		for (int i = 0; i < g_pMain->pTempleEvent.LastEventRoom; i++) {
			foreach_stlmap_nolock(itr, m_MonsterRespawnListInformationArray) {
				if (itr->second->ZoneID == ZONE_CHAOS_DUNGEON) {
					if (itr->second->sSid == CHAOS_CUBE_SSID)
						SpawnEventNpc(itr->second->sSid, false, itr->second->ZoneID, itr->second->X, itr->second->Y, itr->second->Z, itr->second->sCount, itr->second->bRadius, 60 * 60, 0, 0, -1, i + 1/* EventRoom */);
					else
						SpawnEventNpc(itr->second->sSid, true, itr->second->ZoneID, itr->second->X, itr->second->Y, itr->second->Z, itr->second->sCount, itr->second->bRadius, 60 * 60, 0, 0, -1, i + 1/* EventRoom */);
				}
			}
		}
	} else if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR) {
		for (int i = 0; i < g_pMain->pTempleEvent.LastEventRoom; i++) {
			foreach_stlmap_nolock(itr, m_MonsterRespawnListInformationArray) {
				if (itr->second->ZoneID == ZONE_BORDER_DEFENSE_WAR) {
					SpawnEventNpc(itr->second->sSid, false, itr->second->ZoneID, itr->second->X, itr->second->Y, itr->second->Z, itr->second->sCount, itr->second->bRadius, 60 * 60, 0, 0, -1, i + 1/* EventRoom */);
					SpawnEventNpc(itr->second->sSid, true, itr->second->ZoneID, itr->second->X, itr->second->Y, itr->second->Z, itr->second->sCount, itr->second->bRadius, 60 * 60, 0, 0, -1, i + 1/* EventRoom */);
				}
			}
		}
	} else if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_JURAD_MOUNTAIN) {
		for (int i = 0; i < g_pMain->pTempleEvent.LastEventRoom; i++) {
			foreach_stlmap_nolock(itr, m_MonsterRespawnListInformationArray) {
				if (itr->second->ZoneID == ZONE_JURAD_MOUNTAIN && itr->second->sCount == 1)
					SpawnEventNpc(itr->second->sSid, true, itr->second->ZoneID, itr->second->X, itr->second->Y, itr->second->Z, itr->second->sCount, itr->second->bRadius, 60 * 60, 0, 0, -1, i + 1/* EventRoom */);
				else if (itr->second->ZoneID == ZONE_JURAD_MOUNTAIN && itr->second->sCount == 2)
					SpawnEventNpc(itr->second->sSid, false, itr->second->ZoneID, itr->second->X, itr->second->Y, itr->second->Z, 1, 2, 60 * 60, 0, 0, -1, i + 1);
			}
		}
	}
}

void CGameServerDlg::TempleEventFinish(uint16 m_sEventRoom, uint8 m_bNation) {
	foreach_stlmap(itr, m_TempleEventUserArray) {
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser == nullptr)
			continue;

		if (m_sEventRoom > 0 && pUser->GetEventRoom() != m_sEventRoom)
			continue;

		_USER_RANKING * pRankInfo = m_UserRankingArray[pUser->GetNation() - 1].GetData(itr->second->m_socketID);

		if (pRankInfo) {
			int64 nChangeExp = -1;
			int32 nChangeLoyalty;
			int32 nChangeKnightCash;

			if (pRankInfo->pUser->GetZoneID() == ZONE_BORDER_DEFENSE_WAR) {
				if (m_bNation == 0 && g_pMain->pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] == pTempleEvent.KarusDeathCount[pUser->GetEventRoom()]) {
					int32 nChangeExpMax = 0, nChangeExpMin = 0;

					if (pRankInfo->pUser->GetLevel() > 20)
						nChangeExpMax = (40 + pRankInfo->pUser->m_iLoyaltyDaily)*(pRankInfo->pUser->GetLevel() - 19) * 1800;
					else
						nChangeExpMax = 100000;

					if (pRankInfo->pUser->GetLevel() > 20)
						nChangeExpMin = ((40 + pRankInfo->pUser->m_iLoyaltyDaily)*(pRankInfo->pUser->GetLevel() - 19) * 1260) - (40 + pRankInfo->pUser->m_iLoyaltyDaily);
					else
						nChangeExpMin = 50000;

					nChangeExp = nChangeExpMin;
					nChangeLoyalty = pTempleEvent.m_nBorderDefenseWarPrizeLoserLoyalty;
					nChangeKnightCash = pTempleEvent.m_nBorderDefenseWarPrizeLoserKnightCash;

					if (pTempleEvent.m_nBorderDefenseWarPrizeLoserItem > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeLoserItem, 1);

					if (nChangeLoyalty > 0)
						pUser->SendLoyaltyChange(nChangeLoyalty);

					if (nChangeExp > 0)
						pUser->ExpChange(nChangeExp);

					if (nChangeKnightCash > 0)
						g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), nChangeKnightCash);
				} else if (m_bNation == pUser->GetNation()) {
					int32 nChangeExpMax = 0, nChangeExpMin = 0;

					if (pRankInfo->pUser->GetLevel() > 20)
						nChangeExpMax = (40 + pRankInfo->pUser->m_iLoyaltyDaily)*(pRankInfo->pUser->GetLevel() - 19) * 1800;
					else
						nChangeExpMax = 100000;

					if (pRankInfo->pUser->GetLevel() > 20)
						nChangeExpMin = ((40 + pRankInfo->pUser->m_iLoyaltyDaily)*(pRankInfo->pUser->GetLevel() - 19) * 1260) - (40 + pRankInfo->pUser->m_iLoyaltyDaily);
					else
						nChangeExpMin = 50000;

					nChangeExp = nChangeExpMax;
					nChangeLoyalty = pTempleEvent.m_nBorderDefenseWarPrizeWonLoyalty;
					nChangeKnightCash = pTempleEvent.m_nBorderDefenseWarPrizeWonKnightCash;

					if (pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1 > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1, 1);

					if (pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2 > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2, 1);

					if (pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3 > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3, 1);

					if (pUser->GetNation() == KARUS && pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_K > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_K, 1);
					else if (pUser->GetNation() == ELMORAD && pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_H > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_H, 1);

					if (nChangeLoyalty > 0)
						pUser->SendLoyaltyChange(nChangeLoyalty);

					if (nChangeExp > 0)
						pUser->ExpChange(nChangeExp);

					if (nChangeKnightCash > 0)
						g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), nChangeKnightCash);
					pUser->AchieveType1(5);
				} else if ((m_bNation == pUser->GetNation() || m_bNation == 0) && (pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] > pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] && pUser->GetNation() == KARUS)) {
					int32 nChangeExpMax = 0, nChangeExpMin = 0;

					if (pRankInfo->pUser->GetLevel() > 20)
						nChangeExpMax = (40 + pRankInfo->pUser->m_iLoyaltyDaily)*(pRankInfo->pUser->GetLevel() - 19) * 1800;
					else
						nChangeExpMax = 100000;

					if (pRankInfo->pUser->GetLevel() > 20)
						nChangeExpMin = ((40 + pRankInfo->pUser->m_iLoyaltyDaily)*(pRankInfo->pUser->GetLevel() - 19) * 1260) - (40 + pRankInfo->pUser->m_iLoyaltyDaily);
					else
						nChangeExpMin = 50000;

					nChangeExp = nChangeExpMax;
					nChangeLoyalty = pTempleEvent.m_nBorderDefenseWarPrizeWonLoyalty;
					nChangeKnightCash = pTempleEvent.m_nBorderDefenseWarPrizeWonKnightCash;

					if (pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1 > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1, 1);

					if (pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2 > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2, 1);

					if (pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3 > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3, 1);

					if (pUser->GetNation() == KARUS && pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_K > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_K, 1);

					if (nChangeLoyalty > 0)
						pUser->SendLoyaltyChange(nChangeLoyalty);

					if (nChangeExp > 0)
						pUser->ExpChange(nChangeExp);

					if (nChangeKnightCash > 0)
						g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), nChangeKnightCash);

					pUser->AchieveType1(5);
				} else if ((m_bNation == pUser->GetNation() || m_bNation == 0) && (pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] > pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] && pUser->GetNation() == ELMORAD)) {
					int32 nChangeExpMax = 0, nChangeExpMin = 0;

					if (pRankInfo->pUser->GetLevel() > 20)
						nChangeExpMax = (40 + pRankInfo->pUser->m_iLoyaltyDaily)*(pRankInfo->pUser->GetLevel() - 19) * 1800;
					else
						nChangeExpMax = 100000;

					if (pRankInfo->pUser->GetLevel() > 20)
						nChangeExpMin = ((40 + pRankInfo->pUser->m_iLoyaltyDaily)*(pRankInfo->pUser->GetLevel() - 19) * 1260) - (40 + pRankInfo->pUser->m_iLoyaltyDaily);
					else
						nChangeExpMin = 50000;

					nChangeExp = nChangeExpMax;
					nChangeLoyalty = pTempleEvent.m_nBorderDefenseWarPrizeWonLoyalty;
					nChangeKnightCash = pTempleEvent.m_nBorderDefenseWarPrizeWonKnightCash;

					if (pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1 > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1, 1);

					if (pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2 > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2, 1);

					if (pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3 > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3, 1);

					if (pUser->GetNation() == ELMORAD && pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_H > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_H, 1);

					if (nChangeLoyalty > 0)
						pUser->SendLoyaltyChange(nChangeLoyalty);

					if (nChangeExp > 0)
						pUser->ExpChange(nChangeExp);

					if (nChangeKnightCash > 0)
						g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), nChangeKnightCash);

					pUser->AchieveType1(5);
				} else {
					int32 nChangeExpMax = 0, nChangeExpMin = 0;

					if (pRankInfo->pUser->GetLevel() > 20)
						nChangeExpMax = (40 + pRankInfo->pUser->m_iLoyaltyDaily)*(pRankInfo->pUser->GetLevel() - 19) * 1800;
					else
						nChangeExpMax = 100000;

					if (pRankInfo->pUser->GetLevel() > 20)
						nChangeExpMin = ((40 + pRankInfo->pUser->m_iLoyaltyDaily)*(pRankInfo->pUser->GetLevel() - 19) * 1260) - (40 + pRankInfo->pUser->m_iLoyaltyDaily);
					else
						nChangeExpMin = 50000;

					nChangeExp = nChangeExpMin;
					nChangeLoyalty = pTempleEvent.m_nBorderDefenseWarPrizeLoserLoyalty;
					nChangeKnightCash = pTempleEvent.m_nBorderDefenseWarPrizeLoserKnightCash;

					if (pTempleEvent.m_nBorderDefenseWarPrizeLoserItem > 0)
						pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeLoserItem, 1);

					if (nChangeLoyalty > 0)
						pUser->SendLoyaltyChange(nChangeLoyalty);

					if (nChangeExp > 0)
						pUser->ExpChange(nChangeExp);

					if (nChangeKnightCash > 0)
						g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), nChangeKnightCash);
				}
			} else if (pRankInfo->pUser->GetZoneID() == ZONE_JURAD_MOUNTAIN) {
				if (!pTempleEvent.m_nJuraidMountainOdulTipi) {
					if (((pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] > pTempleEvent.KarusDeathCount[pUser->GetEventRoom()]) || (m_bNation > 0 && m_bNation == KARUS)) && pUser->GetNation() == KARUS) {
						if (pUser->GetLevel() >= 70 && pUser->GetLevel() <= 72)
							pUser->GiveItem(GREEN_GEM);
						else if (pUser->GetLevel() >= 73 && pUser->GetLevel() <= 74)
							pUser->GiveItem(BLUE_GEM);
						else if (pUser->GetLevel() >= 75 && pUser->GetLevel() <= 76)
							pUser->GiveItem(YELLOW_GEM);
						else if (pUser->GetLevel() >= 77 && pUser->GetLevel() <= 78)
							pUser->GiveItem(RED_GEM);
						else if (pUser->GetLevel() >= 79 && pUser->GetLevel() <= 83)
							pUser->GiveItem(SILVERY_GEM);

						pUser->AchieveType1(6);
					} else if (((pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] > pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()]) || (m_bNation > 0 && m_bNation == ELMORAD)) && pUser->GetNation() == ELMORAD) {
						if (pUser->GetLevel() >= 70 && pUser->GetLevel() <= 72)
							pUser->GiveItem(GREEN_GEM);
						else if (pUser->GetLevel() >= 73 && pUser->GetLevel() <= 74)
							pUser->GiveItem(BLUE_GEM);
						else if (pUser->GetLevel() >= 75 && pUser->GetLevel() <= 76)
							pUser->GiveItem(YELLOW_GEM);
						else if (pUser->GetLevel() >= 77 && pUser->GetLevel() <= 78)
							pUser->GiveItem(RED_GEM);
						else if (pUser->GetLevel() >= 79 && pUser->GetLevel() <= 83)
							pUser->GiveItem(SILVERY_GEM);

						pUser->AchieveType1(6);
					} else {
						pUser->GiveItem(BLACK_GEM);
					}
				} else

				{
					if (((pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] > pTempleEvent.KarusDeathCount[pUser->GetEventRoom()]) || (m_bNation > 0 && m_bNation == KARUS)) && pUser->GetNation() == KARUS) {
						nChangeExp = pTempleEvent.m_nJuraidMountainPrizeWonExp;
						nChangeLoyalty = pTempleEvent.m_nJuraidMountainPrizeWonLoyalty;
						nChangeKnightCash = pTempleEvent.m_nJuraidMountainPrizeWonKnightCash;

						if (pTempleEvent.m_nJuraidMountainPrizeWonItemNo1 > 0)
							pUser->GiveItem(pTempleEvent.m_nJuraidMountainPrizeWonItemNo1, 1);

						if (pTempleEvent.m_nJuraidMountainPrizeWonItemNo2 > 0)
							pUser->GiveItem(pTempleEvent.m_nJuraidMountainPrizeWonItemNo2, 1);

						if (pTempleEvent.m_nJuraidMountainPrizeWonItemNo3 > 0)
							pUser->GiveItem(pTempleEvent.m_nJuraidMountainPrizeWonItemNo3, 1);

						if (pUser->GetNation() == KARUS && pTempleEvent.m_nJuraidMountainPrizeWonItemNo4_K > 0)
							pUser->GiveItem(pTempleEvent.m_nJuraidMountainPrizeWonItemNo4_K, 1);

						if (nChangeLoyalty > 0)
							pUser->SendLoyaltyChange(nChangeLoyalty);

						if (nChangeExp > 0)
							pUser->ExpChange(nChangeExp);

						if (nChangeKnightCash > 0)
							g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), nChangeKnightCash);

						pUser->AchieveType1(6);
					} else if (((pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] > pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()]) || (m_bNation > 0 && m_bNation == ELMORAD)) && pUser->GetNation() == ELMORAD) {
						nChangeExp = pTempleEvent.m_nJuraidMountainPrizeWonExp;
						nChangeLoyalty = pTempleEvent.m_nJuraidMountainPrizeWonLoyalty;
						nChangeKnightCash = pTempleEvent.m_nJuraidMountainPrizeWonKnightCash;

						if (pTempleEvent.m_nJuraidMountainPrizeWonItemNo1 > 0)
							pUser->GiveItem(pTempleEvent.m_nJuraidMountainPrizeWonItemNo1, 1);

						if (pTempleEvent.m_nJuraidMountainPrizeWonItemNo2 > 0)
							pUser->GiveItem(pTempleEvent.m_nJuraidMountainPrizeWonItemNo2, 1);

						if (pTempleEvent.m_nJuraidMountainPrizeWonItemNo3 > 0)
							pUser->GiveItem(pTempleEvent.m_nJuraidMountainPrizeWonItemNo3, 1);

						if (pUser->GetNation() == ELMORAD && pTempleEvent.m_nJuraidMountainPrizeWonItemNo4_H > 0)
							pUser->GiveItem(pTempleEvent.m_nJuraidMountainPrizeWonItemNo4_H, 1);

						if (nChangeLoyalty > 0)
							pUser->SendLoyaltyChange(nChangeLoyalty);

						if (nChangeExp > 0)
							pUser->ExpChange(nChangeExp);

						if (nChangeKnightCash > 0)
							g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), nChangeKnightCash);

						pUser->AchieveType1(6);
					} else {
						nChangeExp = pTempleEvent.m_nJuraidMountainPrizeLoserExp;
						nChangeLoyalty = pTempleEvent.m_nJuraidMountainPrizeLoserLoyalty;
						nChangeKnightCash = pTempleEvent.m_nJuraidMountainPrizeLoserKnightCash;

						if (pTempleEvent.m_nJuraidMountainPrizeLoserItem > 0)
							pUser->GiveItem(pTempleEvent.m_nJuraidMountainPrizeLoserItem, 1);

						if (nChangeLoyalty > 0)
							pUser->SendLoyaltyChange(nChangeLoyalty);

						if (nChangeExp > 0)
							pUser->ExpChange(nChangeExp);

						if (nChangeKnightCash > 0)
							g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), nChangeKnightCash);
					}
				}
			} else if (pRankInfo->pUser->GetZoneID() == ZONE_CHAOS_DUNGEON) {
				uint64 nGainedExp = uint64(pow(pUser->GetLevel(), 3) * 0.15 * (5 * pRankInfo->m_KillCount - pRankInfo->m_DeathCount));
				uint64 nPremiumGainedExp = nGainedExp * 2;

				if (nGainedExp > 8000000)
					nGainedExp = 8000000;

				if (nPremiumGainedExp > 8000000)
					nPremiumGainedExp = 8000000;

				nChangeExp = pUser->PremiumID != 0 ? nPremiumGainedExp : nGainedExp;

				uint8 nEventRoomUserCount = g_pMain->TempleEventGetRoomUsers(pUser->GetEventRoom());
				uint8 nItemRewardRankFirst = nEventRoomUserCount / 5;
				uint8 nItemRewardRankSecond = (nEventRoomUserCount - 1) * 2;

				nChangeExp = pTempleEvent.m_nChaosPrizeLoserExp;
				nChangeLoyalty = pTempleEvent.m_nChaosPrizeLoserLoyalty;
				nChangeKnightCash = pTempleEvent.m_nChaosPrizeLoserKnightCash;

				int32 nUserRank = pUser->GetPlayerRank(RANK_TYPE_CHAOS_DUNGEON);

				if (nUserRank == 1)
					pUser->AchieveType1(7);
				if (nUserRank == 2)
					pUser->AchieveType1(8);
				if (nUserRank == 3)
					pUser->AchieveType1(9);

				if (nUserRank > 0 && nUserRank <= nItemRewardRankFirst) {
					nChangeExp = pTempleEvent.m_nChaosPrizeWonExp;
					nChangeLoyalty = pTempleEvent.m_nChaosPrizeWonLoyalty;
					nChangeKnightCash = pTempleEvent.m_nChaosPrizeWonKnightCash;
					if (pTempleEvent.m_nChaosPrizeWonItemNo1 > 0)
						pUser->GiveItem(pTempleEvent.m_nChaosPrizeWonItemNo1, 1);

					if (pTempleEvent.m_nChaosPrizeWonItemNo2 > 0)
						pUser->GiveItem(pTempleEvent.m_nChaosPrizeWonItemNo2, 1);

					if (pTempleEvent.m_nChaosPrizeWonItemNo3 > 0)
						pUser->GiveItem(pTempleEvent.m_nChaosPrizeWonItemNo3, 1);

					if (pUser->GetNation() == ELMORAD && pTempleEvent.m_nChaosPrizeWonItemNo4_H > 0)
						pUser->GiveItem(pTempleEvent.m_nChaosPrizeWonItemNo4_H, 1);
					else if (pUser->GetNation() == KARUS && pTempleEvent.m_nChaosPrizeWonItemNo4_K > 0)
						pUser->GiveItem(pTempleEvent.m_nChaosPrizeWonItemNo4_K, 1);

					if (nChangeLoyalty > 0)
						pUser->SendLoyaltyChange(nChangeLoyalty);

					if (nChangeExp > 0)
						pUser->ExpChange(nChangeExp);

					if (nChangeKnightCash > 0)
						g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), nChangeKnightCash);
				} else {
					nChangeExp = pTempleEvent.m_nChaosPrizeLoserExp;
					nChangeLoyalty = pTempleEvent.m_nChaosPrizeLoserLoyalty;
					nChangeKnightCash = pTempleEvent.m_nChaosPrizeLoserKnightCash;

					if (pTempleEvent.m_nChaosPrizeLoserItem > 0)
						pUser->GiveItem(pTempleEvent.m_nChaosPrizeLoserItem, 1);

					if (nChangeLoyalty > 0)
						pUser->SendLoyaltyChange(nChangeLoyalty);

					if (nChangeExp > 0)
						pUser->ExpChange(nChangeExp);

					if (nChangeKnightCash > 0)
						g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), nChangeKnightCash);
				}
			}
		}
	}

	foreach_stlmap_nolock(itr, m_TempleEventUserArray) {
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser && pUser->isInGame()) {
			if (m_sEventRoom > 0 && pUser->GetEventRoom() != m_sEventRoom)
				continue;

			TempleEventKickOutUser(pUser);

			Packet result2(AG_USER_EVENTROOM);
			result2 << pUser->GetID() << uint16(0);
			g_pMain->Send_AIServer(&result2);

			pUser->SetUnitEventRoom(0);
			pUser->SetUserEventRoom(0);
		}
	}

	if (m_sEventRoom == 0)
		TempleEventReset();
}
void CGameServerDlg::TempleEventGetActiveEventTime(CUser *pUser) {
	if (pUser == nullptr)
		return;

	Packet result(WIZ_EVENT, uint8(TEMPLE_EVENT));
	result << (uint16) pTempleEvent.ActiveEvent << m_nTempleEventRemainSeconds;
	pUser->Send(&result);
}

void CGameServerDlg::TempleEventSendActiveEventTime(CUser *pUser) {
	if (pUser == nullptr)
		return;

	if (!pUser->isEventUser()) {
		TempleEventGetActiveEventTime(pUser);
		return;
	}

	Packet result(WIZ_EVENT, uint8(TEMPLE_EVENT_JOIN));
	result << uint8(1) << uint16(pTempleEvent.ActiveEvent);
	pUser->Send(&result);
	pUser->TempleOperations(TEMPLE_EVENT_COUNTER);
}

void CGameServerDlg::TempleEventKickOutUser(CUser *pUser) {
	if (pUser == nullptr
		|| !pUser->isInGame())
		return;

	uint8 nZoneID = 0;

	if (pUser->GetZoneID() == ZONE_BORDER_DEFENSE_WAR) {
		if (pUser->GetLevel() < 35)
			nZoneID = ZONE_MORADON;
		else
			nZoneID = pUser->GetNation();
	} else if (pUser->GetZoneID() == ZONE_CHAOS_DUNGEON) {
		if (pUser->GetLevel() < 35)
			nZoneID = ZONE_MORADON;
		else if (pUser->GetLevel() >= 35 && pUser->GetLevel() <= 59)
			nZoneID = ZONE_MORADON;
		else if (pUser->GetLevel() >= 60 && pUser->GetLevel() <= 69)
			nZoneID = ZONE_MORADON;
		else if (pUser->GetLevel() >= 70)
			nZoneID = ZONE_MORADON;
	} else if (pUser->GetZoneID() == ZONE_JURAD_MOUNTAIN) {
		nZoneID = pUser->GetNation() + 10;
	}
	if (nZoneID == 0)
		return;

	_START_POSITION * pStartPosition = GetStartPosition(nZoneID);

	if (pStartPosition == nullptr) {
		TRACE("### TempleEventKickOutUser - StartPosition not found : Zone ID=%d", nZoneID);
		return;
	}

	pUser->ZoneChange(nZoneID, 0.0f, 0.0f);
}

void CGameServerDlg::TempleEventReset() {
	pTempleEvent.ActiveEvent = -1;
	pTempleEvent.ZoneID = 0;
	pTempleEvent.LastEventRoom = 1;
	pTempleEvent.StartTime = 0;
	pTempleEvent.AllUserCount = 0;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;
	pTempleEvent.isAttackable = false;
	pTempleEvent.isActive = false;

	memset(pTempleEvent.DevaNation, 0, sizeof(pTempleEvent.DevaNation));
	memset(pTempleEvent.isDevaControl, 0, sizeof(pTempleEvent.isDevaControl));
	memset(pTempleEvent.isDevaFlag, 0, sizeof(pTempleEvent.isDevaFlag));
	memset(pTempleEvent.KarusDeathCount, 0, sizeof(pTempleEvent.KarusDeathCount));
	memset(pTempleEvent.ElmoDeathCount, 0, sizeof(pTempleEvent.ElmoDeathCount));
	memset(pTempleEvent.m_sBdwMiniTimer, -1, sizeof(pTempleEvent.m_sBdwMiniTimer));
	memset(pTempleEvent.m_sMiniTimerNation, -1, sizeof(pTempleEvent.m_sMiniTimerNation));
	memset(pTempleEvent.KarusDeathRoom1, 0, sizeof(pTempleEvent.KarusDeathRoom1));
	memset(pTempleEvent.KarusDeathRoom2, 0, sizeof(pTempleEvent.KarusDeathRoom2));
	memset(pTempleEvent.KarusDeathRoom3, 0, sizeof(pTempleEvent.KarusDeathRoom3));
	memset(pTempleEvent.ElmoDeathRoom1, 0, sizeof(pTempleEvent.ElmoDeathRoom1));
	memset(pTempleEvent.ElmoDeathRoom2, 0, sizeof(pTempleEvent.ElmoDeathRoom2));
	memset(pTempleEvent.ElmoDeathRoom3, 0, sizeof(pTempleEvent.ElmoDeathRoom3));

	m_TempleEventUserArray.DeleteAllData();
}

void CGameServerDlg::Announcement(uint16 type, int nation, int chat_type, CUser* pExceptUser, CNpc *pExpectNpc) {
	string chatstr;
	uint8 ZoneID = 0;
	std::string sEventName;

	switch (type) {
	case BATTLEZONE_OPEN:
		GetServerResource(IDP_BATTLEZONE_OPEN, &chatstr);
		break;
	case DECLARE_WINNER:
		if (m_bVictory == KARUS)
			GetServerResource(IDP_KARUS_VICTORY, &chatstr, m_sElmoradDead, m_sKarusDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDP_ELMORAD_VICTORY, &chatstr, m_sKarusDead, m_sElmoradDead);
		else
			return;
		break;
	case DECLARE_LOSER:
		if (m_bVictory == KARUS)
			GetServerResource(IDS_ELMORAD_LOSER, &chatstr, m_sKarusDead, m_sElmoradDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDS_KARUS_LOSER, &chatstr, m_sElmoradDead, m_sKarusDead);
		else
			return;
		break;
	case DECLARE_BAN:
		if (m_bVictory == KARUS || m_bVictory == ELMORAD)
			GetServerResource(IDS_BANISH_LOSER, &chatstr);
		else
			GetServerResource(IDS_BANISH_USER, &chatstr);
		break;
	case SNOW_BAN:
		if (m_bVictory == KARUS || m_bVictory == ELMORAD)
			GetServerResource(IDS_BANISH_LOSER, &chatstr);
		else
			GetServerResource(IDS_SNOWBATTLE_BANISH_USER, &chatstr);
		break;
	case DECLARE_BATTLE_ZONE_STATUS:
		if (m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE4) {
			GetServerResource(IDS_BATTLE_MONUMENT_STATUS, &chatstr, m_sKarusMonumentPoint, m_sElmoMonumentPoint, m_sKarusDead, m_sElmoradDead);
			SendNotice<PUBLIC_CHAT>(chatstr.c_str(), ZONE_BATTLE4, Nation::ALL, true);
		} else
			GetServerResource(IDS_BATTLEZONE_STATUS, &chatstr, m_sKarusDead, m_sElmoradDead);
		break;
	case DECLARE_BATTLE_MONUMENT_STATUS:
		if (pExceptUser) {
			GetServerResource(IDS_BATTLE_MONUMENT_WON_MESSAGE, &chatstr, GetBattleAndNationMonumentName(chat_type).c_str());
			g_pMain->SendNotice<PUBLIC_CHAT>(chatstr.c_str(), pExceptUser->GetZoneID());
			GetServerResource(IDS_BATTLE_MONUMENT_LOST_MESSAGE, &chatstr, GetBattleAndNationMonumentName(chat_type).c_str());
			g_pMain->SendNotice<PUBLIC_CHAT>(chatstr.c_str(), pExceptUser->GetZoneID());
			return;
		}
		break;
	case DECLARE_NATION_MONUMENT_STATUS:
		if (pExceptUser) {
			uint16 nTrapNumber = pExceptUser->GetZoneID() == ZONE_KARUS ? chat_type - LUFERSON_MONUMENT_SID : chat_type - ELMORAD_MONUMENT_SID;

			if ((pExceptUser->GetNation() == KARUS && pExceptUser->GetZoneID() == ELMORAD) || (pExceptUser->GetNation() == ELMORAD && pExceptUser->GetZoneID() == KARUS)) {
				GetServerResource(IDS_INFILTRATION_CONQUER, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExceptUser->GetZoneID()).c_str());
				g_pMain->SendAnnouncement(chatstr.c_str());
				return;
			} else if ((pExceptUser->GetNation() == KARUS && pExceptUser->GetZoneID() == KARUS) || (pExceptUser->GetNation() == ELMORAD && pExceptUser->GetZoneID() == ELMORAD)) {
				GetServerResource(IDS_INFILTRATION_RECAPTURE, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExceptUser->GetZoneID()).c_str());
				g_pMain->SendAnnouncement(chatstr.c_str());
				return;
			}
			return;
		}
		break;
	case DECLARE_NATION_REWARD_STATUS:
		if (pExpectNpc) {
			uint16 nTrapNumber = pExpectNpc->GetZoneID() == ZONE_KARUS ? chat_type - LUFERSON_MONUMENT_SID : chat_type - ELMORAD_MONUMENT_SID;

			GetServerResource(pExpectNpc->GetNation() == KARUS ? IDS_INFILTRATION_REWARD_KARUS : IDS_INFILTRATION_REWARD_ELMORAD, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExpectNpc->GetZoneID()).c_str());
			g_pMain->SendAnnouncement(chatstr.c_str(), Nation::ALL);
			return;
		}
		break;
	case SNOW_BATTLEZONE_OPEN:
		GetServerResource(IDS_SNOWBATTLE_OPEN, &chatstr);
		break;
	case UNDER_ATTACK_NOTIFY:
		if (m_bVictory == KARUS)
			GetServerResource(IDS_UNDER_ATTACK_ELMORAD, &chatstr, m_sKarusDead, m_sElmoradDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDS_UNDER_ATTACK_KARUS, &chatstr, m_sElmoradDead, m_sKarusDead);
		else
			return;
		break;
	case BATTLEZONE_CLOSE:
		GetServerResource(IDS_BATTLE_CLOSE, &chatstr);
		break;
	case SNOW_BATTLEZONE_CLOSE:
		GetServerResource(IDS_SNOWBATTLE_CLOSE, &chatstr);
		break;
	case KARUS_CAPTAIN_NOTIFY:
		GetServerResource(IDS_KARUS_CAPTAIN, &chatstr, m_strKarusCaptain.c_str());
		break;
	case ELMORAD_CAPTAIN_NOTIFY:
		GetServerResource(IDS_ELMO_CAPTAIN, &chatstr, m_strElmoradCaptain.c_str());
		break;
	case KARUS_CAPTAIN_DEPRIVE_NOTIFY:
		if (pExceptUser) {
			CKnights *pKnights = g_pMain->GetClanPtr(pExceptUser->GetClanID());
			GetServerResource(IDS_KARUS_CAPTAIN_DEPRIVE, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName().c_str(), pExceptUser->GetName().c_str());
		}
		break;
	case ELMORAD_CAPTAIN_DEPRIVE_NOTIFY:
		if (pExceptUser) {
			CKnights *pKnights = g_pMain->GetClanPtr(pExceptUser->GetClanID());
			GetServerResource(IDS_ELMO_CAPTAIN_DEPRIVE, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName().c_str(), pExceptUser->GetName().c_str());
		}
		break;
	case IDS_MONSTER_CHALLENGE_ANNOUNCEMENT:
		GetServerResource(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT, &chatstr, m_nForgettenTempleStartHour);
		break;
	case IDS_MONSTER_CHALLENGE_OPEN:
		GetServerResource(IDS_MONSTER_CHALLENGE_OPEN, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_START:
		ZoneID = ZONE_FORGOTTEN_TEMPLE;
		GetServerResource(IDS_MONSTER_CHALLENGE_START, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_VICTORY:
		ZoneID = ZONE_FORGOTTEN_TEMPLE;
		GetServerResource(IDS_MONSTER_CHALLENGE_VICTORY, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_CLOSE:
		GetServerResource(IDS_MONSTER_CHALLENGE_CLOSE, &chatstr);
		break;
	case IDS_EVENT_ZONE_OPEN:
		if (m_nEventZoneTime == 1)
			sEventName = "Dark Land";
		else if (m_nEventZoneTime == 2)
			sEventName = "Mini PVP Zone";
		else if (m_nEventZoneTime == 3)
			sEventName = "Devils Square";
		else if (m_nEventZoneTime == 4)
			sEventName = "Lost Castle";
		else if (m_nEventZoneTime == 6) {
			GetServerResource(IDS_JURAD_EVENT_OPEN, &chatstr, sEventName.c_str());
			break;
		} else if (m_nEventZoneTime == 5) {
			sEventName = "PVP Zone";
			GetServerResource(IDS_PVP_EVENT_ZONE_OPEN, &chatstr, sEventName.c_str());
			break;
		}

		GetServerResource(IDS_EVENT_ZONE_OPEN, &chatstr, sEventName.c_str());
		break;
	case IDS_EVENT_ZONE_START:
		if (m_nEventZoneTime == 1) {
			ZoneID = ZONE_DARK_LAND;
			sEventName = "Dark Land is infested of Demons. Rescue the city and fight for the Bosses!";
			GetServerResource(IDS_EVENT_ZONE_START, &chatstr, sEventName.c_str());
		} else if (m_nEventZoneTime == 5) {
			ZoneID = ZONE_ARDREAM;
			sEventName = "The first 3-ranked players(More NPs Gained) will receive award at the end. Go get the Monument and fight for the bosses!";
			GetServerResource(IDS_EVENT_ZONE_START, &chatstr, sEventName.c_str());
		} else if (m_nEventZoneTime == 2) {
			std::string sNotRank;
			ZoneID = ZONE_PVP_EVENT;
			sEventName = "The first 3-ranked players(More NPs Gained) will receive award at the end. Go get the Monument and fight for the bosses!";
			sNotRank = "You can't see the rank on Mini PvP event but it is working!";
			SendNotice(sNotRank.c_str());
			GetServerResource(IDS_EVENT_ZONE_START, &chatstr, sEventName.c_str());
		} else if (m_nEventZoneTime == 3) {
			ZoneID = ZONE_RONARK_LAND_BASE;
			sEventName = "The first 3-ranked players(More NPs Gained) will receive award at the end. Go get the Monument and fight for the bosses!";
			GetServerResource(IDS_EVENT_ZONE_START, &chatstr, sEventName.c_str());
		} else if (m_nEventZoneTime == 4) {
			ZoneID = ZONE_LOST_TEMPLE;
			sEventName = "Lost Castle is a chaos domined by Rey Satanas. Kill him to get the award!";
			GetServerResource(IDS_EVENT_ZONE_START, &chatstr, sEventName.c_str());
		} else if (m_nEventZoneTime == 6) {
			ZoneID = ZONE_JURAD_MOUNTAIN;
			sEventName = "Get the keys to go to the end. And then kill Lucifer to get the award!";
			GetServerResource(IDS_EVENT_ZONE_START, &chatstr, sEventName.c_str());
		}
		break;
	case IDS_EVENT_ZONE_VICTO:
		if (m_nEventZoneTime == 1)
			sEventName = "Dark Land";
		else if (m_nEventZoneTime == 2)
			sEventName = "Mini PVP Zone";
		else if (m_nEventZoneTime == 3)
			sEventName = "Devils Square";
		else if (m_nEventZoneTime == 4)
			sEventName = "Lost Castle";
		else if (m_nEventZoneTime == 5)
			sEventName = "PVP Zone";
		else if (m_nEventZoneTime == 6)
			sEventName = "Jurad Mountain";

		GetServerResource(IDS_EVENT_ZONE_VICTO, &chatstr, sEventName.c_str());
		break;
	case IDS_EVENT_ZONE_CLOSE:
		if (m_nEventZoneTime == 1)
			sEventName = "Dark Land";
		else if (m_nEventZoneTime == 2)
			sEventName = "Mini PVP Zone";
		else if (m_nEventZoneTime == 3)
			sEventName = "Devils Square";
		else if (m_nEventZoneTime == 4)
			sEventName = "Lost Castle";
		else if (m_nEventZoneTime == 5)
			sEventName = "PVP Zone";
		else if (m_nEventZoneTime == 6)
			sEventName = "Jurad Mountain";

		GetServerResource(IDS_EVENT_ZONE_CLOSE, &chatstr, sEventName.c_str());
		break;
	case IDS_SIEGE_WAR_READY_TIME_NOTICE:
		GetServerResource(IDS_SIEGE_WAR_READY_TIME_NOTICE, &chatstr, m_SiegeWarNoticeTime);
		break;
	case IDS_SIEGE_WAR_TIME_NOTICE:
		GetServerResource(IDS_SIEGE_WAR_TIME_NOTICE, &chatstr, m_SiegeWarNoticeTime);
		break;
	case IDS_SIEGE_WAR_START:
		GetServerResource(IDS_SIEGE_WAR_START, &chatstr);
		break;
	case IDS_SIEGE_WAR_END:
		GetServerResource(IDS_SIEGE_WAR_END, &chatstr);
		break;
	case IDS_NPC_GUIDON_DESTORY:
		GetServerResource(IDS_NPC_GUIDON_DESTORY, &chatstr, m_SiegeWarWinKnightsNotice.c_str());
		break;
	case IDS_SIEGE_WAR_VICTORY:
		GetServerResource(IDS_SIEGE_WAR_VICTORY, &chatstr, m_SiegeWarWinKnightsNotice.c_str());
		break;
	case IDS_JURAD_EVENT_OPEN:
		GetServerResource(IDS_JURAD_EVENT_OPEN, &chatstr);
		break;
	}

	Packet result;
	string finalstr;
	GetServerResource(IDP_ANNOUNCEMENT, &finalstr, chatstr.c_str());
	ChatPacket::Construct(&result, (uint8) chat_type, &finalstr);
	Send_All(&result, nullptr, nation, ZoneID);
}

/**
* @brief	Loads the specified user's NP ranks
* 			from the rankings tables.
*
* @param	pUser	The user.
*/
void CGameServerDlg::GetUserRank(CUser *pUser) {
	// Acquire the lock for thread safety
	//Guard lock(m_userRankingsLock);

	// Get character's name & convert it to upper case for case insensitivity
	string strUserID = pUser->GetName();
	STRTOUPPER(strUserID);

	// Grab the personal rank from the map, if applicable.
	UserNameRankMap::iterator itr = m_UserPersonalRankMap.find(strUserID);
	pUser->m_bPersonalRank = itr != m_UserPersonalRankMap.end() ? int8(itr->second->nRank) : -1;

	// Grab the knights rank from the map, if applicable.
	itr = m_UserKnightsRankMap.find(strUserID);
	pUser->m_bKnightsRank = itr != m_UserKnightsRankMap.end() ? int8(itr->second->nRank) : -1;
}

uint16 CGameServerDlg::GetKnightsAllMembers(uint16 sClanID, Packet & result, uint16 & pktSize, bool bClanLeader) {
	CKnights* pKnights = GetClanPtr(sClanID);
	if (pKnights == nullptr)
		return 0;

	uint16 count = 0;
	foreach_array(i, pKnights->m_arKnightsUser) {
		_KNIGHTS_USER *p = &pKnights->m_arKnightsUser[i];

		if (!p->byUsed)
			continue;

		CUser* pCurrent = p->pSession;

		if (pCurrent == nullptr) {
			if (p->m_lastLogined == 0)
				g_DBAgent.GetClanUserData(sClanID, pKnights->m_arKnightsUser[i]);

			uint32 HesaplaGun = 1;
			HesaplaGun = ((uint32(UNIXTIME) - (p->m_lastLogined)) / 3600);
			result << p->strUserName << uint16(p->m_sFame) << p->m_bLevel << p->m_sClass << uint8(0) << p->strUserMemo << uint32(HesaplaGun);
		} else {
			result << pCurrent->GetName() << uint16(pCurrent->GetFame()) << pCurrent->GetLevel() << pCurrent->m_sClass << uint8(pCurrent->isInGame() ? 1 : 0) << pCurrent->m_strMemo << uint32(UNIXTIME - p->m_lastLogin);
		}
		count++;
	}

	return count;
}
/**
* @brief	Calculates the clan grade from the specified
* 			loyalty points (NP).
*
* @param	nPoints	Loyalty points (NP).
* 					The points will be converted to clan points
* 					by this method.
*
* @return	The clan grade.
*/
int CGameServerDlg::GetKnightsGrade(uint32 nPoints) {
	uint32 nClanPoints = nPoints;

	if (nClanPoints >= m_Grade1)
		return 1;
	else if (nClanPoints >= m_Grade2)
		return 2;
	else if (nClanPoints >= m_Grade3)
		return 3;
	else if (nClanPoints >= m_Grade4)
		return 4;

	return 5;
}

/**
* @brief	Disconnects all players in the server.
*
* @return	The number of users who were in-game.
*/
int CGameServerDlg::KickOutAllUsers() {
	int count = 0;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser *pUser = TO_USER(itr.second);
		if (pUser->isInGame())
			count++;

		if (!g_pMain->OfflineMerchanting && !pUser->OfflineMerchant)
			pUser->Disconnect();
		Sleep(100);
	}
	return count;
}

/**
* @brief	Generates a new item serial.
*/
uint64 CGameServerDlg::GenerateItemSerial() {
	static std::recursive_mutex _mutex;

	MYINT64 serial;
	MYSHORT	increase;
	serial.i = 0;

	time_t t = UNIXTIME;
	struct tm * ptm;
	ptm = gmtime(&t);

	Guard lock(_mutex);
	increase.w = g_increase_serial++;

	serial.b[7] = (uint8) (m_nServerNo);
	serial.b[6] = (uint8) (ptm->tm_year % 100);
	serial.b[5] = (uint8) (ptm->tm_mon);
	serial.b[4] = (uint8) (ptm->tm_mday);
	serial.b[3] = (uint8) (ptm->tm_hour);
	serial.b[2] = (uint8) (ptm->tm_min);
	serial.b[1] = increase.b[1];
	serial.b[0] = increase.b[0];

	return serial.i;
}

/**
* @brief	Kick out all users from the specified zone
* 			to their home zone.
*
* @param	zone	The zone to kick users out from.
*/
void CGameServerDlg::KickOutZoneUsers(uint8 ZoneID, uint8 TargetZoneID, uint8 bNation) {
	// TODO: Make this localised to zones.
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	C3DMap	*pKarusMap = GetZoneByID(KARUS),
		*pElMoradMap = GetZoneByID(ELMORAD);

	ASSERT(pKarusMap != nullptr && pElMoradMap != nullptr);

	BOOST_FOREACH(auto itr, sessMap) {
		// Only kick users from requested zone.
		CUser * pUser = TO_USER(itr.second);
		if (!pUser->isInGame()
			|| pUser->GetZoneID() != ZoneID)
			continue;

		if (TargetZoneID > 0 && bNation == pUser->GetNation()) {
			pUser->ZoneChange(TargetZoneID, 0.0f, 0.0f);
			continue;
		} else if (TargetZoneID > 0 && bNation == ALL) {
			pUser->ZoneChange(TargetZoneID, 0.0f, 0.0f);
			continue;
		}

		C3DMap * pMap = (pUser->GetNation() == KARUS ? pKarusMap : pElMoradMap);
		if (TargetZoneID == 0)
			pUser->ZoneChange(pMap->m_nZoneNumber, pMap->m_fInitX, pMap->m_fInitZ);
	}
}

void CGameServerDlg::SendItemZoneUsers(uint8 ZoneID, uint32 nItemID, uint16 sCount /*= 1*/, uint32 Time/*= 0*/) {
	// TO-DO: Make this localised to zones.
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser * pUser = TO_USER(itr.second);
		if (!pUser->isInGame()
			|| pUser->GetZoneID() != ZoneID)
			continue;

		pUser->GiveItem(nItemID, sCount, true, Time);
	}
}

void CGameServerDlg::SendItemEventRoom(uint16 nEventRoom, uint32 nItemID, uint16 sCount /*= 1*/) {
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser * pUser = TO_USER(itr.second);
		if (!pUser->isInGame()
			|| pUser->GetEventRoom() != nEventRoom)
			continue;

		pUser->GiveItem(nItemID, sCount);
	}
}

void CGameServerDlg::Send_CommandChat(Packet *pkt, int nation, CUser* pExceptUser) {
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser * pUser = TO_USER(itr.second);
		if (pUser->isInGame() && (nation == 0 || nation == pUser->GetNation()))
			pUser->Send(pkt);
	}
}

/**
* @brief	Sends the flying santa/angel packet to all users in the server.
*/
void CGameServerDlg::SendFlyingSantaOrAngel() {
	Packet result(WIZ_SANTA, uint8(m_bSantaOrAngel));
	Send_All(&result);
}

CGameServerDlg::~CGameServerDlg() {
	printf("Waiting for timer threads to exit...");
	BOOST_FOREACH(auto itr, g_timerThreads) {
		(itr)->waitForExit();
		delete (itr);
	}
	printf(" exited.\n");

	// Cleanup our script pool & consequently ensure all scripts
	// finish execution before proceeding.
	// This prevents us from freeing data that's in use.
	printf("Shutting down Lua engine...");
	m_luaEngine.Shutdown();
	printf(" done.\n");

	printf("Shutting down database system...");
	DatabaseThread::Shutdown();
	printf(" done.\n");

	printf("Shutting down socket system...");
	m_aiSocketMgr.Shutdown();
	m_socketMgr.Shutdown();
	printf(" done.\n");

	CUser::CleanupChatCommands();
	CGameServerDlg::CleanupServerCommands();

	CleanupUserRankings();
	m_LevelUpArray.clear();

	if (m_fpDeathUser != nullptr)
		fclose(m_fpDeathUser);

	if (m_fpDeathNpc != nullptr)
		fclose(m_fpDeathNpc);

	if (m_fpMerchant != nullptr)
		fclose(m_fpMerchant);

	if (m_fpTrade != nullptr)
		fclose(m_fpTrade);

	if (m_fpChat != nullptr)
		fclose(m_fpChat);

	if (m_fpGM != nullptr)
		fclose(m_fpGM);

	if (m_upgradelog != nullptr)
		fclose(m_upgradelog);

	if (m_fpCheat != nullptr)
		fclose(m_fpCheat);
}

std::string CGameServerDlg::GetBattleAndNationMonumentName(int16 TrapNumber, uint8 ZoneID) {
	std::string sMonumentName = ZoneID == 0 ? "Nereid Monument" : "DECLARE_NATION_MONUMENT_STATUS";

	if (ZoneID == 0) {
		if (TrapNumber == 1)
			sMonumentName = "El Morad main territory";
		else if (TrapNumber == 2)
			sMonumentName = "El Morad provision line";
		else if (TrapNumber == 3)
			sMonumentName = "Lake of Life";
		else if (TrapNumber == 4)
			sMonumentName = "Foss Castle";
		else if (TrapNumber == 5)
			sMonumentName = "Karus main territory";
		else if (TrapNumber == 6)
			sMonumentName = "Karus provision line";
		else if (TrapNumber == 7)
			sMonumentName = "Swamp of Shadows";
	} else {
		if (ZoneID == ZONE_KARUS && TrapNumber == 1)
			TrapNumber = 2;
		else if (ZoneID == ZONE_KARUS && TrapNumber == 2)
			TrapNumber = 1;

		if (TrapNumber == 0)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Luferson" : "El Morad");
		else if (TrapNumber == 1)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Bellua" : "Asga Village");
		else if (TrapNumber == 2)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Linate" : "Raiba Village");
		else if (TrapNumber == 3)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Laon Camp" : "Dodo Camp");
	}

	return sMonumentName;
}
bool CGameServerDlg::CastleSiegeWarAttack(CUser *pUser, CUser *pTargetUser) {
	_KNIGHTS_SIEGE_WARFARE *pKnightSiege = g_pMain->GetSiegeMasterKnightsPtr(1);
	CKnights *pKnightsMaster = g_pMain->GetClanPtr(pKnightSiege->sMasterKnights);

	if (pUser == nullptr
		|| pTargetUser == nullptr)
		return false;

	CKnights *pClan1 = g_pMain->GetClanPtr(pUser->GetClanID());
	CKnights *pClan2 = g_pMain->GetClanPtr(pTargetUser->GetClanID());

	if (pClan1 == nullptr
		|| pClan2 == nullptr)
		return false;

	if (pUser->GetClanID() == pTargetUser->GetClanID()
		|| pUser->GetZoneID() != pTargetUser->GetZoneID()
		|| CKnightsManager::CheckAlliance(pClan1, pClan2))
		return false;

	if (pKnightsMaster != nullptr // Kale sahibi varsa ve
		&& (pUser->GetClanID() != pKnightsMaster->GetID() //  Atack yapan Kale sahibi clanda değilse
			&& pTargetUser->GetClanID() != pKnightsMaster->GetID()))// ve Atack yapılan Kale sahibi clanda değilse
		return false;

	return true;
}
void CGameServerDlg::CheckNationMonumentRewards() {
	std::vector<uint16> deleted;

	foreach_stlmap_nolock(itr, m_NationMonumentInformationArray) {
		if (int32(UNIXTIME) - itr->second->RepawnedTime < 20)
			continue;

		CNpc *pNpc = GetNpcPtr(itr->second->sNid);

		if (pNpc == nullptr) {
			deleted.push_back(itr->second->sSid);
			continue;
		}

		uint16 nTrapNumber = pNpc->GetZoneID() == ZONE_KARUS ? itr->second->sSid - LUFERSON_MONUMENT_SID : itr->second->sSid - ELMORAD_MONUMENT_SID;

		std::vector<Unit *> distributed_member;
		std::vector<uint16> unitList;
		g_pMain->GetUnitListFromSurroundingRegions(pNpc, &unitList);

		BOOST_FOREACH(auto itrx, unitList) {
			Unit * pTarget = g_pMain->GetUnitPtr(itrx);

			if (pTarget == nullptr
				|| pTarget->isNPC()
				|| pTarget->GetNation() == pNpc->GetZoneID())
				continue;

			if (pTarget->GetNation() == pNpc->GetNation() && pTarget->isInRangeSlow(pNpc, 50.0f))
				distributed_member.push_back(pTarget);
		}

		BOOST_FOREACH(auto itry, distributed_member) {
			Unit * pTarget = itry;

			if (pTarget == nullptr || pTarget->isNPC() || pTarget->GetNation() == pNpc->GetZoneID())
				continue;

			g_pMain->m_lastBlessTime = UNIXTIME;
			TO_USER(pTarget)->SendLoyaltyChange(nTrapNumber == 0 ? 200 : 50);
			Announcement(DECLARE_NATION_REWARD_STATUS, Nation::ALL, itr->second->sSid, nullptr, pNpc);
			ShowNpcEffect(itr->second->sNid, 20100, pNpc->GetZoneID(), pNpc->GetEventRoom());
		}
	}

	BOOST_FOREACH(auto itr, deleted)
		g_pMain->m_NationMonumentInformationArray.DeleteData(itr);
}

void CGameServerDlg::ShowNpcEffect(uint16 sNpcID, uint32 nEffectID, uint8 ZoneID, uint16 EventRoom) {
	Packet result(WIZ_OBJECT_EVENT, uint8(OBJECT_NPC));
	result << uint8(3) << sNpcID << nEffectID;
	g_pMain->Send_Zone(&result, ZoneID, nullptr, Nation::ALL, EventRoom);
}

void CGameServerDlg::WriteDeathUserLogFile(string & logMessage) {
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpDeathUser);
	fflush(m_fpDeathUser);
}

void CGameServerDlg::WriteDeathNpcLogFile(string & logMessage) {
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpDeathNpc);
	fflush(m_fpDeathNpc);
}

void CGameServerDlg::WriteMerchantUserLogFile(string & logMessage) {
	string logMessageEnd;
	DateTime time;
	logMessageEnd = string_format(_T("[ %d:%d:%d ] %s\n"), time.GetHour(), time.GetMinute(), time.GetSecond(), logMessage.c_str());

	fwrite(logMessageEnd.c_str(), logMessageEnd.length(), 1, m_fpMerchant);
	fflush(m_fpMerchant);
}

void CGameServerDlg::WriteTradeUserLogFile(string & logMessage) {
	string logMessageEnd;
	DateTime time;
	logMessageEnd = string_format(_T("[ %d:%d:%d ] %s\n"), time.GetHour(), time.GetMinute(), time.GetSecond(), logMessage.c_str());

	fwrite(logMessageEnd.c_str(), logMessageEnd.length(), 1, m_fpTrade);
	fflush(m_fpTrade);
}

void CGameServerDlg::WriteChatLogFile(string & logMessage) {
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpChat);
	fflush(m_fpChat);
}

void CGameServerDlg::WriteGMLogFile(string & logMessage) {
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpGM);
	fflush(m_fpGM);
}

void CGameServerDlg::WriteUpgradeLogFile(string & logMessage) {
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_upgradelog);
	fflush(m_upgradelog);
}

void CGameServerDlg::WriteCheatLogFile(string & logMessage) {
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpCheat);
	fflush(m_fpCheat);
}

bool CGameServerDlg::IsDuplicateItem(uint32 nItemID, uint64 nItemSerial) {
	return false;

	if (nItemID <= 0 || nItemSerial <= 0)
		return false;

	_USER_ITEM * pUserItem = g_pMain->m_UserItemArray.GetData(nItemID);

	if (pUserItem == nullptr)
		return false;

	uint32 nDuplicateCount = 0;

	BOOST_FOREACH(auto itr, pUserItem->nItemSerial) {
		if (itr == nItemSerial)
			nDuplicateCount++;

		if (nDuplicateCount > 1)
			return true;
	}

	return false;
}

void CGameServerDlg::AddUserItem(uint32 nItemID, uint64 nItemSerial) {
	if (nItemID <= 0 || nItemSerial <= 0)
		return;

	Guard lock(g_pMain->m_UserItemArray.m_lock);
	_USER_ITEM * pUserItem = g_pMain->m_UserItemArray.GetData(nItemID);

	if (pUserItem == nullptr) {
		pUserItem = new _USER_ITEM;
		pUserItem->nItemID = nItemID;
		pUserItem->nItemSerial.push_back(nItemSerial);

		if (!g_pMain->m_UserItemArray.PutData(pUserItem->nItemID, pUserItem))
			delete pUserItem;
	} else
		pUserItem->nItemSerial.push_back(nItemSerial);
}

void CGameServerDlg::ReloadKnightAndUserRanks() {
	g_DBAgent.UpdateRanks();

	// Update clan grades/rankings
	m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
	m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();

	ReloadAllKnights(true);
	LoadKnightsRankTable(false, true);

	// Update user rankings
	CleanupUserRankings();
	LoadUserRankings();
}

void CGameServerDlg::SetPlayerRankingRewards(uint16 ZoneID) {
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	BOOST_FOREACH(auto itr, sessMap) {
		CUser * pUser = TO_USER(itr.second);
		if (pUser == nullptr
			|| !pUser->isInGame()
			|| pUser->isGM()
			|| pUser->GetZoneID() != ZoneID)
			continue;

		if (pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) > 0
			&& pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) <= 10) {
			if (m_nPlayerRankingLoyaltyReward > 0)
				pUser->SendLoyaltyChange(m_nPlayerRankingLoyaltyReward, false, true, false);
			if (m_nPlayerRankingKnightCashReward > 0)
				g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), m_nPlayerRankingKnightCashReward);
		}
	}
}
void CGameServerDlg::TerminationFinish() {
	if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR) {
		Packet bresult(WIZ_EVENT);
		bresult << uint8(TEMPLE_EVENT_FINISH)
			<< uint8(2) << uint8(0)
			<< uint8(0) << uint8(20) << uint32(0);

		Packet cresult(WIZ_EVENT);
		cresult << uint8(TEMPLE_EVENT_FINISH)
			<< uint8(2) << uint8(0)
			<< uint8(1) << uint8(20) << uint32(0);

		Packet dresult(WIZ_EVENT);
		dresult << uint8(TEMPLE_EVENT_FINISH)
			<< uint8(1) << uint8(0)
			<< uint8(2) << uint8(20) << uint32(0);

		foreach_stlmap(itr, m_TempleEventUserArray) {
			CUser * pUser = GetUserPtr(itr->second->m_socketID);
			if (pUser == nullptr || !pUser->isInGame())
				continue;

			if (g_pMain->pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] == pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] && g_pMain->pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] == pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()])
				g_pMain->Send_Zone(&bresult, ZONE_BORDER_DEFENSE_WAR, pUser, Nation::ALL, pUser->GetEventRoom());

			if (pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] > pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] && pUser->GetNation() == KARUS)
				g_pMain->Send_Zone(&cresult, ZONE_BORDER_DEFENSE_WAR, pUser, Nation::ALL, pUser->GetEventRoom());

			if (pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] > pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] && pUser->GetNation() == ELMORAD)
				g_pMain->Send_Zone(&dresult, ZONE_BORDER_DEFENSE_WAR, pUser, Nation::ALL, pUser->GetEventRoom());
		}
	} else if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_CHAOS) {
		Packet bresult(WIZ_EVENT);
		bresult << uint8(TEMPLE_EVENT_FINISH)
			<< uint8(2) << uint8(0)
			<< uint8(1) << uint8(20) << uint32(0);

		foreach_stlmap(itr, m_TempleEventUserArray) {
			CUser * pUser = GetUserPtr(itr->second->m_socketID);

			if (pUser == nullptr || !pUser->isInGame())
				continue;

			g_pMain->Send_Zone(&bresult, ZONE_CHAOS_DUNGEON, pUser, Nation::ALL, pUser->GetEventRoom());
		}
	} else if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_JURAD_MOUNTAIN) {
		Packet bresult(WIZ_EVENT);
		bresult << uint8(TEMPLE_EVENT_FINISH)
			<< uint8(2) << uint8(0)
			<< uint8(0) << uint8(20) << uint32(0);

		Packet cresult(WIZ_EVENT);
		cresult << uint8(TEMPLE_EVENT_FINISH)
			<< uint8(2) << uint8(0)
			<< uint8(1) << uint8(20) << uint32(0);

		Packet dresult(WIZ_EVENT);
		dresult << uint8(TEMPLE_EVENT_FINISH)
			<< uint8(1) << uint8(0)
			<< uint8(2) << uint8(20) << uint32(0);

		foreach_stlmap(itr, m_TempleEventUserArray) {
			CUser * pUser = GetUserPtr(itr->second->m_socketID);

			if (pUser == nullptr || !pUser->isInGame())
				continue;

			if (g_pMain->pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] == pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] && g_pMain->pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] == pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()])
				g_pMain->Send_Zone(&bresult, ZONE_JURAD_MOUNTAIN, pUser, Nation::ALL, pUser->GetEventRoom());

			if (pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] > pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] && pUser->GetNation() == KARUS)
				g_pMain->Send_Zone(&cresult, ZONE_JURAD_MOUNTAIN, pUser, Nation::ALL, pUser->GetEventRoom());

			if (pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] > pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] && pUser->GetNation() == ELMORAD)
				g_pMain->Send_Zone(&dresult, ZONE_JURAD_MOUNTAIN, pUser, Nation::ALL, pUser->GetEventRoom());
		}
	}
}
void CGameServerDlg::TerminitionTime(uint8 sNewZone, uint16 Time) {
	if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_JURAD_MOUNTAIN) {
		Packet Inventory(WIZ_EVENT);
		Inventory << uint8(1); // Inventory Kontrol.
		Packet aresult(WIZ_SELECT_MSG);
		aresult << uint16(0) << uint8(7)
			<< uint32(0) << uint32(0)
			<< uint8(7) << uint16(0)
			<< uint8(0) << uint8(0x07)
			<< Time << uint16(0);

		Packet bresult(WIZ_BIFROST);
		bresult << uint8(0x05) << Time;

		Packet cresult(WIZ_EVENT);
		cresult << uint8(0x03) << uint32(0) << uint32(0) << uint32(0x05008504) << uint8(0);

		foreach_stlmap(itr, m_TempleEventUserArray) {
			CUser * pUser = GetUserPtr(itr->second->m_socketID);
			if (pUser == nullptr || !pUser->isInGame())
				continue;

			cresult << pUser->GetName();
		}
		cresult << uint32(0);

		foreach_stlmap_nolock(itr, m_TempleEventUserArray) {
			CUser * pUser = GetUserPtr(itr->second->m_socketID);
			if (pUser == nullptr || !pUser->isInGame())
				continue;

			pUser->Send(&Inventory);
			pUser->Send(&aresult);
			pUser->Send(&bresult);
			pUser->SendToRegion(&cresult, nullptr, pUser->GetEventRoom());
		}
	} else if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR) {
		Packet Inventory(WIZ_EVENT);
		Inventory << uint8(1); // Inventory Kontrol.

		Packet aresult(WIZ_SELECT_MSG);
		aresult << uint16(0) << uint8(7)
			<< uint32(0) << uint32(0)
			<< uint8(8) << uint16(0)
			<< uint8(0) << uint8(0x07)
			<< Time << uint16(0);

		Packet bresult(WIZ_BIFROST);
		bresult << uint8(0x05) << Time;

		Packet cresult(WIZ_EVENT);
		cresult << uint8(0x03) << uint32(0) << uint32(0) << uint32(0x05008504) << uint8(0);

		foreach_stlmap(itr, m_TempleEventUserArray) {
			CUser * pUser = GetUserPtr(itr->second->m_socketID);

			if (pUser == nullptr || !pUser->isInGame())
				continue;

			cresult << pUser->GetName();
		}
		cresult << uint32(0);

		foreach_stlmap_nolock(itr, m_TempleEventUserArray) {
			CUser * pUser = GetUserPtr(itr->second->m_socketID);

			if (pUser == nullptr || !pUser->isInGame())
				continue;

			pUser->Send(&Inventory);
			pUser->Send(&aresult);
			pUser->Send(&bresult);
			pUser->SendToRegion(&cresult, nullptr, pUser->GetEventRoom());
		}
	} else if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_CHAOS) {
		Packet aresult(WIZ_SELECT_MSG);
		aresult << uint16(0) << uint8(7)
			<< uint32(0) << uint32(0)
			<< uint8(9) << uint16(0)
			<< uint8(0) << uint8(0x18)
			<< Time << uint16(0);

		Packet bresult(WIZ_BIFROST);
		bresult << uint8(0x05) << Time;

		foreach_stlmap(itr, m_TempleEventUserArray) {
			CUser * pUser = GetUserPtr(itr->second->m_socketID);

			if (pUser == nullptr || !pUser->isInGame())
				continue;

			pUser->Send(&aresult);
			pUser->Send(&bresult);
		}
	}
}
void CGameServerDlg::UpdateSiege(int16 m_sCastleIndex, int16 m_sMasterKnights, int16 m_bySiegeType, int16 m_byWarDay, int16 m_byWarTime, int16 m_byWarMinute) {
	g_DBAgent.UpdateSiege(m_sCastleIndex, m_sMasterKnights, m_bySiegeType, m_byWarDay, m_byWarTime, m_byWarMinute);
}

void CGameServerDlg::UpdateSiegeTax(uint8 Zone, int16 ZoneTarrif) {
	g_DBAgent.UpdateSiegeTax(Zone, ZoneTarrif);
}
void CGameServerDlg::Send_Noah_Knights(Packet *pkt) {
	for (int i = 0; i < MAX_USER; i++) {
		CUser* pUser = GetUserPtr(i);

		if (pUser == nullptr ||
			pUser->GetLevel() > 50)
			continue;

		pUser->Send(pkt);
	}
}

void CGameServerDlg::LunarGoldShells(CUser *pUser) {
	if (pUser != nullptr) {
		Packet result(WIZ_MAP_EVENT);
		result << uint8(9) << uint8(1);

		pUser->Send(&result);
		return;
	}

	Packet result(WIZ_MAP_EVENT);
	result << uint8(9) << uint8(1); //wiz_map_event , 9 gold shell , 1 giydir
	Send_All(&result);
}

void CGameServerDlg::DeleteLunarGoldShells() {
	Packet result(WIZ_MAP_EVENT);
	result << uint8(9) << uint8(0); //wiz_map_event , 9 gold shell , 0 sildir
	Send_All(&result);
}

void CGameServerDlg::NereidsMonumentEvent(uint8 TrapNumber, uint8 bNation, CUser * pUser) {
	Packet result(WIZ_MAP_EVENT);
	result << uint8(0) << uint8(7);

	if (pUser == nullptr) {
		uint8 MonumentID = TrapNumber;
		m_sNereidsMonumentNation[MonumentID] = bNation;
	}

	for (int i = 0; i < 7; i++)
		result << m_sNereidsMonumentNation[i];

	if (pUser != nullptr)
		pUser->Send(&result);
	else
		Send_Zone(&result, ZONE_BATTLE4);
}