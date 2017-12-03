#pragma once

#include "../shared/database/OdbcConnection.h"

enum UserUpdateType {
	UPDATE_LOGOUT,
	UPDATE_ALL_SAVE,
	UPDATE_PACKET_SAVE,
};

enum RentalType {
	RENTAL_TYPE_IN_LIST = 1,
	RENTAL_TYPE_LENDER = 2,
	RENTAL_TYPE_BORROWER = 3
};

struct _USER_RENTAL_ITEM {
	std::string strUserID;
	uint64 nSerialNum;
	uint32 nRentalIndex, nItemID, nRentalMoney;
	uint16 sDurability, sRentalTime;
	int16 sMinutesRemaining;
	uint8 byRentalType, byRegType;
	char szTimeRental[30];

	_USER_RENTAL_ITEM() {
		memset(&szTimeRental, 0, sizeof(szTimeRental));
	}
};

typedef std::map<uint64, _USER_RENTAL_ITEM *> UserRentalMap;

class Packet;
class CUser;
struct _ITEM_DATA;
class CDBAgent {
public:
	CDBAgent();


	bool Startup(bool bMarsEnabled,
		tstring & strAccountDSN, tstring & strAccountUID, tstring & strAccountPWD,
		tstring & strGameDSN, tstring & strGameUID, tstring & strGamePWD);

	bool Connect(bool bMarsEnabled,
		tstring & strAccountDSN, tstring & strAccountUID, tstring & strAccountPWD,
		tstring & strGameDSN, tstring & strGameUID, tstring & strGamePWD);

	void ReportSQLError(OdbcError *pError);


	int8 AccountLogin(std::string & strAccountID, std::string & strPasswd);
	uint8 NationSelect(std::string & strAccountID, uint8 bNation);
	bool GetAllCharID(std::string & strAccountID, std::string & strCharID1, std::string & strCharID2, std::string & strCharID3, std::string & strCharID4);
	bool SetAllCharID(std::string & strAccountID, std::string & strCharID1, std::string & strCharID2, std::string & strCharID3, std::string & strCharID4);
	void LoadCharInfo(std::string & strCharID, ByteBuffer & result);
	void LoadCharSeal(std::string & strCharID, ByteBuffer & result);
	uint16 LoadAccountNTS(std::string & strCharID);
	uint8 LoadYanCharHAS(std::string & strAccountID, std::string & strCharID);
	uint64 GetSerialByID(uint32 ID);
	uint8 LoadCharRace(std::string & strCharID);
	uint64 LoadCharExp(std::string & strCharID);
	uint8 LoadYanCharCount(std::string & strCharID);
	uint8 LoadCharLevel(std::string & strCharID);
	uint16 LoadCharKnights(std::string & strCharID);
	uint8 LoadCharNation(std::string & strCharID);
	void GetClanUserData(uint16 sClanID, _KNIGHTS_USER & p);
	int8 ChangeHair(std::string & strAccountID, std::string & strCharID, uint8 bOpcode, uint8 bFace, uint32 nHair);
	int8 CreateNewChar(std::string & strAccountID, int index, std::string & strCharID, uint8 bRace, uint16 sClass, uint32 nHair, uint8 bFace, uint8 bStr, uint8 bSta, uint8 bDex, uint8 bInt, uint8 bCha);
	void LoadRentalData(std::string & strAccountID, std::string & strCharID, UserRentalMap & rentalData);
	void LoadItemSealData(std::string & strAccountID, std::string & strCharID, UserItemSealMap & itemSealData);
	uint8 UpdatePetData(uint64 PetSerial, CPet *pPet);
	uint8 LoadPetData(uint64 PetSerial, CPet *pPet);
	uint8 InsertPetData(uint64 PetSerial, CPet *pPet);
	uint8 InsertCypherRingData(uint64 Serial, _CYPHERRING_DATA *pData, std::string strPasswd);
	uint8 InsertCypherRingChar(std::string strAccountID, uint64 Serial);
	bool LoadUserData(std::string & strAccountID, std::string & strCharID, CUser *pUser);
	bool LoadWarehouseData(std::string & strAccountID, CUser *pUser);
	bool LoadPremiumServiceUser(std::string & strAccountID, CUser *pUser);
	bool LoadSavedMagic(CUser *pUser);
	bool SetLogInInfo(std::string & strAccountID, std::string & strCharID, std::string & strServerIP, short sServerNo, std::string & strClientIP, uint8 bInit);
	bool SavePremiumServiceUser(CUser *pUser, _PREMIUM_TYPE * pPremium);

	bool LoadWebItemMall(std::vector<_ITEM_DATA> & itemList, CUser *pUser);
	bool ClearWebItemMall(CUser *pUser);

	bool LoadSkillShortcut(Packet & result, CUser *pUser);
	void SaveSkillShortcut(uint16 sCount, char *buff, CUser *pUser);
	void RequestFriendList(std::vector<std::string> & friendList, CUser *pUser);
	FriendAddResult AddFriend(short sid, short tid);
	FriendRemoveResult RemoveFriend(std::string & strCharID, CUser *pUser);

	bool UpdateUser(std::string & strCharID, UserUpdateType type, CUser *pUser);
	bool UpdateWarehouseData(std::string & strAccountID, UserUpdateType type, CUser *pUser);
	bool UpdateSavedMagic(CUser *pUser);
	uint8 SealItem(std::string strSealPasswd, uint64 nItemSerial, uint32 nItemID, uint8 bSealType, CUser *pUser);

	int8 CreateKnights(uint16 sClanID, uint8 bNation, std::string & strKnightsName, std::string & strChief, uint8 bFlag = 1);
	int UpdateKnights(uint8 bType, std::string & strCharID, uint16 sClanID, uint8 bDomination);
	int DeleteKnights(uint16 sClanID);
	void UpdateVIPStoragePassword(std::string strUserID, std::string VIPPassword);
	void UpdateVIPStorageMinute(std::string strUserID, uint32 VIPMinute);
	uint16 LoadKnightsAllMembers(uint16 sClanID, Packet & result);
	int8 CreateAlliance(uint8 byType, uint16 shAlliancIndex, uint16 shKnightsIndex, uint8  byEmptyIndex, uint8 bySiegeFlag);
	int8 InsertAlliance(uint8 byType, uint16 shAlliancIndex, uint16 shKnightsIndex, uint8  byEmptyIndex, uint8 bySiegeFlag);
	int8 RemoveAlliance(uint8 byType, uint16 shAlliancIndex, uint16 shKnightsIndex, uint8  byEmptyIndex, uint8 bySiegeFlag);
	int8 DestoryAlliance(uint8 byType, uint16 shAlliancIndex, uint16 shKnightsIndex, uint8  byEmptyIndex, uint8 bySiegeFlag);
	bool LoadKnightsInfo(uint16 sClanID, uint8 & bNation, std::string & strKnightsName, uint16 & sMembers, uint32 & nPoints, uint8 & bRank);
	void LoadKnightsAllList();
	bool UpdateClanSymbol(uint16 sClanID, uint16 sSymbolSize, char *clanSymbol);
	bool DonateClanPoints(CUser * pUser, uint32 amountNP);
	void RefundNP(std::string & strUserID, uint32 nRefundNP);
	void UpdateUserAuthority(std::string & strUserID, uint16 nAuthority);
	void UpdateClanFund(uint16 sClanID, uint32 nClanPointFund);
	void UpdateClanNotice(uint16 sClanID, std::string & strClanNotice);
	NameChangeOpcode UpdateCharacterName(std::string & strAccountID, std::string & strUserID, std::string & strNewUserID);
	ClanNameChangeOpcode UpdateClanName(std::string & strUserID, uint16 sClanID, std::string & strNewKnightsID);
	void UpdateCape(uint16 sClanID, uint16 sCapeID, uint8 r, uint8 g, uint8 b);
	void UpdatePremiumType(uint8 bType, std::string StrAccountID);
	void UpdateClanGrade(uint16 sClanID, uint8 byFlag, uint16 sCapeID);
	void UpdateAccountKnightCash(std::string & strAccountID, uint32 KnightCash = 0);
	void UpdateBattleEvent(std::string & strCharID, uint8 bNation);
	void AccountLogout(std::string & strAccountID);

	void InsertRepurchase(uint32 nItemID, time_t RepTime, uint64 nSerial, CUser *pUser);
	void DeleteRepurchase(uint32 nItemID, time_t RepTime, CUser *pUser);
	void LoadRepurchase(CUser *pUser);


	void UpdateConCurrentUserCount(int nServerNo, int nZoneNo, int nCount);

	uint8 GetUnreadLetterCount(std::string & strCharID);
	bool GetLetterList(std::string & strCharID, Packet & result, bool bNewLettersOnly = true);
	int8 SendLetter(std::string & strSenderID, std::string & strRecipientID, std::string & strSubject, std::string & strMessage, uint8 bType, _ITEM_DATA * pItem, int32 nCoins);
	bool ReadLetter(std::string & strCharID, uint32 nLetterID, std::string & strMessage);
	int8 GetItemFromLetter(std::string & strCharID, uint32 nLetterID, uint32 & nItemID, uint16 & sCount, uint16 & sDurability, uint32 & nCoins, uint64 & nSerialNum, uint32 & nExpirationTime);
	void DeleteLetter(std::string & strCharID, uint32 nLetterID);
	void ResendLetter(std::string & strCharID, uint32 nLetterID);
	void SendUDP_ElectionStatus(uint8 m_byType);
	bool GetElectionResult(uint8 byNation, CKingSystem *KingSystem);
	void UpdateElectionStatus(uint8 byType, uint8 byNation);
	void UpdateElectionList(uint8 byDBType, uint8 byType, uint8 byNation, uint16 sKnights, uint32 nAmount, std::string & strNominee, CUser * pUser);
	int16 UpdateElectionProc(CUser * pUser);
	int16 UpdateCandidacyRecommend(std::string & strNominator, std::string & strNominee, uint8 byNation);
	void UpdateCandidacyNoticeBoard(std::string & strCharID, uint8 byNation, std::string & strNotice);
	void UpdateNoahOrExpEvent(uint8 byType, uint8 byNation, uint8 byAmount, uint8 byDay, uint8 byHour, uint8 byMinute, uint16 sDuration);
	void InsertPrizeEvent(uint8 byType, uint8 byNation, uint32 nCoins, std::string & strCharID);
	void InsertTaxEvent(uint8 KingNationTax, uint8 Nation, uint32 TerritoryTax = 0);
	void InsertTaxUpEvent(uint8 Nation, uint32 TerritoryTax);
	void ResetLoyaltyMonthly();
	void ClearRemainUsers();
	void InsertUserDailyOp(_USER_DAILY_OP * pUserDailyOp);
	void UpdateUserDailyOp(std::string strUserId, uint8 type, int32 sUnixTime);
	void UpdateRanks();
	void UpdateSiege(int16 m_sCastleIndex, int16 m_sMasterKnights, int16 m_bySiegeType, int16 m_byWarDay, int16 m_byWarTime, int16 m_byWarMinute);
	void UpdateSiegeTax(uint8 Zone, int16 ZoneTarrif);

	int8 NationTransfer(std::string strAccountID, uint16 Race1, uint16 Race2, uint16 Race3, uint16 Race4);
	int8 YanChar(std::string strAccountID);
	int8 YanCharClan(std::string strAccountID);

	~CDBAgent();

private:
	OdbcConnection *m_GameDB, *m_AccountDB;
	std::recursive_mutex m_lock;

	friend class CGameServerDlg;
};

extern CDBAgent g_DBAgent;