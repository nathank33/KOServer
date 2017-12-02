#pragma once

class C3DMap;

#include "../shared/types.h"
#include "../shared/STLMap.h"
#include "../shared/STLMapOnlyLoad.h"
#include "GameDefine.h"
#include "Knights.h"
#include "Npc.h"
#include "Pet.h"

class CKingSystem;

typedef CSTLMapOnlyLoad <C3DMap>					ZoneArray;
typedef std::map<int, int64>				LevelUpArray;
typedef CSTLMapOnlyLoad <_CLASS_COEFFICIENT>		CoefficientArray;
typedef CSTLMapOnlyLoad <_ITEM_TABLE>				ItemtableArray;
typedef CSTLMapOnlyLoad <_MAGIC_TABLE>				MagictableArray;
typedef CSTLMapOnlyLoad <_MAGIC_TYPE1>				Magictype1Array;
typedef CSTLMapOnlyLoad <_MAGIC_TYPE2>				Magictype2Array;
typedef CSTLMapOnlyLoad <_MAGIC_TYPE3>				Magictype3Array;
typedef CSTLMapOnlyLoad	<_MAGIC_TYPE4>				Magictype4Array;
typedef CSTLMapOnlyLoad <_MAGIC_TYPE5>				Magictype5Array;
typedef CSTLMapOnlyLoad <_MAGIC_TYPE6>				Magictype6Array;
typedef CSTLMapOnlyLoad <_MAGIC_TYPE7>				Magictype7Array;
typedef CSTLMapOnlyLoad <_MAGIC_TYPE8>				Magictype8Array; 
typedef CSTLMapOnlyLoad <_MAGIC_TYPE9>				Magictype9Array;
typedef CSTLMap <CNpc>						NpcArray;
typedef CSTLMap <CBot>						BotArray;
typedef std::map<uint64, CPet*>				PetArray;
typedef CSTLMap <_PARTY_GROUP>				PartyArray;
typedef CSTLMap <CKnights>					KnightsArray;
typedef CSTLMap <_KNIGHTS_RATING>			KnightsRatingArray;
typedef CSTLMap <_KNIGHTS_ALLIANCE>			KnightsAllianceArray;
typedef CSTLMap <_KNIGHTS_USER *>				KnightsUserArray;
typedef CSTLMapOnlyLoad <_ZONE_SERVERINFO>			ServerArray;
typedef CSTLMapOnlyLoad <_KNIGHTS_CAPE>				KnightsCapeArray;
typedef CSTLMapOnlyLoad <_START_POSITION>			StartPositionArray;
typedef	CSTLMapOnlyLoad	<_SERVER_RESOURCE>			ServerResourceArray;
typedef	CSTLMapOnlyLoad	<_QUEST_HELPER>				QuestHelperArray;
typedef	CSTLMapOnlyLoad	<_QUEST_MONSTER>			QuestMonsterArray;
typedef	CSTLMap	<_RENTAL_ITEM>				RentalItemArray;
typedef CSTLMapOnlyLoad <_ITEM_EXCHANGE>			ItemExchangeArray;
typedef CSTLMap <_ITEM_EXCHANGE_EXP>		ItemExchangeExpArray;
typedef CSTLMapOnlyLoad <_ITEM_UPGRADE>				ItemUpgradeArray;
typedef CSTLMap <_ITEM_OP>					ItemOpArray;
typedef CSTLMapOnlyLoad <CKingSystem>				KingSystemArray;
typedef CSTLMapOnlyLoad <_SET_ITEM>					SetItemArray;
typedef CSTLMapOnlyLoad <_MINING_ITEM>				ItemMiningArray;
typedef CSTLMapOnlyLoad <_ACHIEVE_NORMAL>			AchieveNormalArray;
typedef CSTLMapOnlyLoad <_ACHIEVE_TITLE>			AchieveTitleArray;
typedef CSTLMapOnlyLoad <_ACHIEVE_WAR>				AchieveWarArray;
typedef CSTLMapOnlyLoad <_ACHIEVE_COM>				AchieveComArray;
typedef CSTLMapOnlyLoad <_ACHIEVE_MAIN>				AchieveMainArray;
typedef CSTLMapOnlyLoad <_ACHIEVE_MONSTER>			AchieveMonsterArray;
typedef std::map<uint64,_CYPHERRING_DATA *>	CypherRingArray;
typedef CSTLMap <_ITEM_DUPER>				ItemDupersArray;
typedef CSTLMap <_ILEGAL_ITEMS>				IlegalItemsArray;

typedef CSTLMap <_MERCHANT_LIST>				MerchantListArray;


typedef std::map<std::string, _USER_RANK *>			UserNameRankMap; 
typedef std::map<uint8, _USER_RANK *>				UserRankMap;

typedef std::vector<_QUEST_HELPER *>				QuestHelperList;
typedef std::map<uint16, QuestHelperList>			QuestNpcList;

typedef std::vector<_MONSTER_SUMMON_LIST>			MonsterSummonList;
typedef CSTLMapOnlyLoad <MonsterSummonList>					MonsterSummonListArray;
typedef CSTLMapOnlyLoad <_MONSTER_SUMMON_LIST_ZONE>			MonsterSummonListZoneArray;
typedef CSTLMapOnlyLoad <_MONSTER_RESPAWN_LIST>				MonsterRespawnListArray;
typedef CSTLMapOnlyLoad <_MONSTER_RESPAWN_LIST_INFORMATION>	MonsterRespawnListInformationArray;
typedef CSTLMapOnlyLoad <_MONSTER_RESPAWN_LIST_RANDOM>		MonsterRespawnListRandomArray;

typedef CSTLMapOnlyLoad <_PREMIUM_ITEM>						PremiumItemArray;
typedef CSTLMapOnlyLoad <_PREMIUM_ITEM_EXP>					PremiumItemExpArray;

typedef CSTLMap <_USER_RANKING>						UserRankingArray;
typedef std::map<std::string, _USER_DAILY_OP *>		UserDailyOpMap; 

typedef CSTLMap <_TEMPLE_EVENT_USER>				TempleEventUserArray;
typedef CSTLMapOnlyLoad <_KNIGHTS_SIEGE_WARFARE>			KnightsSiegeWarfareArray;
typedef CSTLMapOnlyLoad <_EVENT_TRIGGER>					EventTriggerArray;
typedef CSTLMapOnlyLoad <_MONUMENT_INFORMATION>				NationMonumentInformationArray;
typedef CSTLMapOnlyLoad <_MONSTER_CHALLENGE>				MonsterChallengeArray;
typedef CSTLMap <_EVENT_TIMES>						EventTimesArray;
typedef CSTLMapOnlyLoad <_MONSTER_CHALLENGE_SUMMON_LIST>	MonsterChallengeSummonListArray;
typedef CSTLMapOnlyLoad <_START_POSITION_RANDOM>			StartPositionRandomArray;
typedef CSTLMapOnlyLoad <_USER_ITEM>						UserItemArray;
typedef CSTLMapOnlyLoad <_OBJECT_EVENT>						ObjectEventArray;
typedef CSTLMap <_CHAT_ROOM>						ChatRoomArray;
typedef CSTLMap <_ITEM_CRASH>						ItemCrashArray;
typedef CSTLMap <_ITEM_MIX>					ItemMixArray;