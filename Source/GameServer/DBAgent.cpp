#include "stdafx.h"
#include "../shared/database/OdbcConnection.h"
#include "KnightsManager.h"
#include "KingSystem.h"
#include "DBAgent.h"
#include "../shared/DateTime.h"

CDBAgent g_DBAgent;

using std::string;
using std::unique_ptr;

CDBAgent::CDBAgent()
{
	m_GameDB = new OdbcConnection();
	m_AccountDB = new OdbcConnection();
}

CDBAgent::~CDBAgent()
{
	delete m_GameDB;
	delete m_AccountDB;
}
bool CDBAgent::Startup(bool bMarsEnabled, 
					   tstring & strAccountDSN, tstring & strAccountUID, tstring & strAccountPWD,
					   tstring & strGameDSN, tstring & strGameUID, tstring & strGamePWD)
{
	if (!Connect(bMarsEnabled,
		strAccountDSN, strAccountUID, strAccountPWD,
		strGameDSN, strGameUID, strGamePWD))
	{
		// we should probably be a little more specific (i.e. *which* database server)
		printf(_T("ERROR: Failed to connect to the database server."));
		return false;
	}

	DatabaseThread::Startup();

	return true;
}

bool CDBAgent::Connect(bool bMarsEnabled, 
					   tstring & strAccountDSN, tstring & strAccountUID, tstring & strAccountPWD,
					   tstring & strGameDSN, tstring & strGameUID, tstring & strGamePWD)
{
	if (!m_AccountDB->Connect(strAccountDSN, strAccountUID, strAccountPWD, bMarsEnabled))
	{
		ReportSQLError(m_AccountDB->GetError());
		return false;
	}

	if (!m_GameDB->Connect(strGameDSN, strGameUID, strGamePWD, bMarsEnabled))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

void CDBAgent::ReportSQLError(OdbcError *pError)
{
	if (pError == nullptr)
		return;

	DateTime time;

	// This is *very* temporary.

	// I have to write an example code to this function as
	// save the query record to a map and try to get clear
	// this map repeatly.
	// About some seconds....

	// If query effects warehouse or userdata or tb_user
	// Disconnect the user that effected about this error
	// And add him to waiting list,
	// Get block his character while his query upload..



	string errorMessage = string_format(_T("[ ODBC Error - %d.%d.%d %d:%d:%d ] ] Source: %s Error: %s Description: %s\n"),
		time.GetDay(),time.GetMonth(),time.GetYear(),time.GetHour(),time.GetMinute(),time.GetSecond(),
		pError->Source.c_str(), pError->ExtendedErrorMessage.c_str(), pError->ErrorMessage.c_str());

	Guard lock(m_lock);
	FILE *fp = fopen("./Logs/GameServer.log", "a");
	if (fp != nullptr)
	{
		fwrite(errorMessage.c_str(), errorMessage.length(), 1, fp);
		fclose(fp);
	}

	TRACE("Database error: %s\n", errorMessage.c_str());
	delete pError;
}

int8 CDBAgent::AccountLogin(string & strAccountID, string & strPasswd)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strPasswd.c_str(), strPasswd.length());

	if (!dbCommand->Execute(_T("{? = CALL GAME_LOGIN(?, ?)}")))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

uint8 CDBAgent::NationSelect(string & strAccountID, uint8 bNation)
{
	uint8 bRet = 0;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL NATION_SELECT(?, %d)}"), bNation)))
		ReportSQLError(m_GameDB->GetError());

	return (bRet > 0 ? bNation : 0);
}

bool CDBAgent::GetAllCharID(string & strAccountID, string & strCharID1, string & strCharID2, string & strCharID3, string & strCharID4)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(_T("SELECT strCharID1, strCharID2, strCharID3, strCharID4 FROM ACCOUNT_CHAR WHERE strAccountID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchString(1, strCharID1);
	dbCommand->FetchString(2, strCharID2);
	dbCommand->FetchString(3, strCharID3);
	dbCommand->FetchString(4, strCharID4);
	return true;
}

bool CDBAgent::SetAllCharID(string & strAccountID, string & strCharID1, string & strCharID2, string & strCharID3, string & strCharID4)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	string ID1, ID2,ID3, ID4;

	if (strCharID1 == "")
		ID1 = "";
	else
		ID1 = strCharID1;

	if (strCharID2 == "")
		ID2 = "";
	else
		ID2 = strCharID2;

	if (strCharID3 == "")
		ID3 = "";
	else
		ID3 = strCharID3;

	if (strCharID4 == "")
		ID4 = "";
	else
		ID4 = strCharID4;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, ID1.c_str(), ID1.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, ID2.c_str(), ID2.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, ID3.c_str(), ID3.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, ID4.c_str(), ID4.length());

	if (!dbCommand->Execute(_T("{CALL UPDATE_ALL_CHAR_ID("
		"?, ?, ?, ?, ?)}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

void CDBAgent::LoadCharInfo(string & strCharID, ByteBuffer & result)
{
	uint32 nHair = 0;
	uint16 sClass = 0;
	uint8 bRace = 0, bLevel = 0, bFace = 0, bZone = 0; 
	char strItem[INVENTORY_TOTAL * 8];
	char strItemTime[INVENTORY_TOTAL * 8];
	ByteBuffer itemData;

	// ensure it's all 0'd out initially.
	memset(strItem, 0x00, sizeof(strItem));
	memset(strItemTime, 0x00, sizeof(strItemTime));

	if (!strCharID.empty())
	{
		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return;

		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

		if (!dbCommand->Execute(_T("SELECT Race, Class, HairRGB, [Level], Face, Zone, strItem, strItemTime FROM USERDATA WHERE strUserID = ?")))
			ReportSQLError(m_GameDB->GetError());

		if (dbCommand->hasData())
		{
			dbCommand->FetchByte(1, bRace);
			dbCommand->FetchUInt16(2, sClass);
			dbCommand->FetchUInt32(3, nHair);
			dbCommand->FetchByte(4, bLevel);
			dbCommand->FetchByte(5, bFace);
			dbCommand->FetchByte(6, bZone);
			dbCommand->FetchBinary(7, strItem, sizeof(strItem));
			dbCommand->FetchBinary(8, strItemTime, sizeof(strItemTime));
		}
	}

	itemData.append(strItem, sizeof(strItem));

	result	<< strCharID << bRace << sClass << bLevel << bFace << nHair << bZone;
	for (int i = 0; i < SLOT_MAX; i++) 
	{
		uint32 nItemID;
		uint16 sDurability, sCount;
		itemData >> nItemID >> sDurability >> sCount;
		if (i == HEAD || i == BREAST || i == SHOULDER || i == LEG || i == GLOVE || i == FOOT || i == RIGHTHAND || i == LEFTHAND)
			result << nItemID << sDurability;
	}
}

void CDBAgent::LoadCharSeal(string & strCharID, ByteBuffer & result)
{
	uint32 nHair = 0, Gold = 0;
	uint16 sClass = 0;
	uint8 bRace = 0, bLevel = 0, bFace = 0, bZone = 0, Sta = 0, Cha = 0, Intel = 0, Dex = 0, Strong = 0, Points = 0;
	uint8 strSkill[10];
	char strItem[INVENTORY_TOTAL * 8];
	char strItemTime[INVENTORY_TOTAL * 8];
	char strSerial[INVENTORY_TOTAL * 8];
	ByteBuffer itemData;
	ByteBuffer serialBuffer;
	ByteBuffer itemTimeBuffer;

	// ensure it's all 0'd out initially.
	memset(strItem, 0x00, sizeof(strItem));
	memset(strItemTime, 0x00, sizeof(strItemTime));
	memset(strSerial, 0x00, sizeof(strSerial));
	memset(strSkill, 0, sizeof(strSkill));

	if (!strCharID.empty())
	{
		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return;

		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

		if (!dbCommand->Execute(_T("SELECT Race, Class, [Level], strItem, strItemTime, strSerial, Strong, Sta, Dex, Cha, Intel, Gold, Points, strSkill  FROM USERDATA WHERE strUserID = ?")))
			ReportSQLError(m_GameDB->GetError());

		if (dbCommand->hasData())
		{
			dbCommand->FetchByte(1, bRace);
			dbCommand->FetchUInt16(2, sClass);
			dbCommand->FetchByte(3, bLevel);
			dbCommand->FetchBinary(4, strItem, sizeof(strItem));
			dbCommand->FetchBinary(5, strItemTime, sizeof(strItemTime));
			dbCommand->FetchBinary(6, strSerial, sizeof(strSerial));
			dbCommand->FetchByte(7, Strong);
			dbCommand->FetchByte(8, Sta);
			dbCommand->FetchByte(9, Dex);
			dbCommand->FetchByte(10, Cha);
			dbCommand->FetchByte(11, Intel);
			dbCommand->FetchUInt32(12, Gold);
			dbCommand->FetchByte(13, Points);
			dbCommand->FetchString(14, (char *)strSkill, sizeof(strSkill));
		}
	}

	itemData.append(strItem, sizeof(strItem));
	serialBuffer.append(strSerial, sizeof(strSerial));
	itemTimeBuffer.append(strItemTime, sizeof(strItemTime));

	result.SByte();
	result	<< strCharID << uint8(sClass) << uint16(bRace) << uint8(0)
	<< bLevel << Strong << Sta << Dex << Intel << Cha << Gold << Points
	<< uint8(0) << uint8(0) << uint8(0) << uint8(0)
	<< strSkill[SkillPointCat1] << strSkill[SkillPointCat2] << strSkill[SkillPointCat3] << strSkill[SkillPointMaster];

	UserItemSealMap m_sealedItemMap;
	LoadItemSealData(strCharID, strCharID, m_sealedItemMap);

	for (int i = 0; i < SLOT_MAX + HAVE_MAX; i++) 
	{
		uint32 nItemID, nItemTime;
		uint16 sDurability, sCount;
		uint64 nSerialNum;
		uint8 bFlag;

		itemData >> nItemID >> sDurability >> sCount;
		serialBuffer >> nSerialNum;
		itemTimeBuffer >> nItemTime;
		
		UserItemSealMap::iterator sealitr = m_sealedItemMap.find(nSerialNum);
		if (sealitr != m_sealedItemMap.end())
		{
			if (sealitr->second->bSealType == 1)
				bFlag = ITEM_FLAG_SEALED;
			else if (sealitr->second->bSealType == 3)
				bFlag = ITEM_FLAG_BOUND;
		}

		if (g_pMain->IsDuplicateItem(nItemID,nSerialNum))
			bFlag = ITEM_FLAG_DUPLICATE;

	
		if(sCount < 1) 
			result << uint32(0)	<< uint16(0) << uint16(0) << uint8(0);
		else
			result << nItemID << sDurability << sCount << bFlag;	// item type flag (e.g. rented)
	
	}
	
		result << uint8(0) << uint8(0) << uint8(0) << uint8(0) << uint8(0)/* Unique Level */ << uint8(0) /* Unique STR */;
}

uint8 CDBAgent::LoadCharRace(std::string & strCharID)
{
	uint8 sClass = 0;

	if (!strCharID.empty())
	{
		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return 0;

		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

		if (!dbCommand->Execute(_T("SELECT Race FROM USERDATA WHERE strUserID = ?")))
			ReportSQLError(m_GameDB->GetError());

		if (dbCommand->hasData())
		{
			dbCommand->FetchByte(1, sClass);
		}
	}

	return sClass;
}

uint64 CDBAgent::LoadCharExp(std::string & strCharID)
{
	uint64 sClass = 0;

	if (!strCharID.empty())
	{
		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return 0;

		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

		if (!dbCommand->Execute(_T("SELECT [exp] FROM USERDATA WHERE strUserID = ?")))
			ReportSQLError(m_GameDB->GetError());

		if (dbCommand->hasData())
		{
			dbCommand->FetchUInt64(1, sClass);
		}
	}

	return sClass;
}

uint8 CDBAgent::LoadYanCharHAS(string & strAccountID, std::string & strCharID)
{
	uint8 sClass = 0;

	if (!strAccountID.empty())
	{
		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return 0;

		dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

		if (!dbCommand->Execute(_T("SELECT COUNT(*) FROM ACCOUNT_CHAR WHERE strAccountID = ? AND (strCharID1 = ? or strCharID2 = ? or strCharID3 = ? or strCharID4 = ?)")))
			ReportSQLError(m_GameDB->GetError());

		if (dbCommand->hasData())
		{
			dbCommand->FetchByte(1, sClass);
		}
	}

	return sClass;
}

uint64 CDBAgent::GetSerialByID(uint32 ID)
{
	uint64 sClass = 0;

		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return 0;

		if (!dbCommand->Execute(string_format(_T("SELECT strSerial FROM USERDATA_CYPHER WHERE id = %d"),ID)))
			ReportSQLError(m_GameDB->GetError());

		if (dbCommand->hasData())
		{
			dbCommand->FetchUInt64(1, sClass);
		}
	

	return sClass;
}

uint8 CDBAgent::LoadCharLevel(string & strCharID)
{
	uint8 sClass = 0;

	if (!strCharID.empty())
	{
		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return 0;

		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

		if (!dbCommand->Execute(_T("SELECT [Level] FROM USERDATA WHERE strUserID = ?")))
			ReportSQLError(m_GameDB->GetError());

		if (dbCommand->hasData())
		{
			dbCommand->FetchByte(1, sClass);
		}
	}

	return sClass;
}

uint16 CDBAgent::LoadCharKnights(string & strCharID)
{
	uint16 sClass = 0;

	if (!strCharID.empty())
	{
		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return 0;

		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

		if (!dbCommand->Execute(_T("SELECT Knights FROM USERDATA WHERE strUserID = ?")))
			ReportSQLError(m_GameDB->GetError());

		if (dbCommand->hasData())
		{
			dbCommand->FetchUInt16(1, sClass);
		}
	}

	return sClass;
}


uint8 CDBAgent::LoadCharNation(string & strCharID)
{
	uint8 sClass = 0;

	if (!strCharID.empty())
	{
		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return 0;

		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

		if (!dbCommand->Execute(_T("SELECT [Nation] FROM USERDATA WHERE strUserID = ?")))
			ReportSQLError(m_GameDB->GetError());

		if (dbCommand->hasData())
		{
			dbCommand->FetchByte(1, sClass);
		}
	}

	return sClass;
}

uint16 CDBAgent::LoadAccountNTS(string & strCharID)
{
	uint16 sClass = 0;

	if (!strCharID.empty())
	{
		unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
		if (dbCommand.get() == nullptr)
			return 0;

		dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

		if (!dbCommand->Execute(_T("SELECT Class FROM USERDATA WHERE strUserID = ?")))
			ReportSQLError(m_GameDB->GetError());

		if (dbCommand->hasData())
		{
			dbCommand->FetchUInt16(1, sClass);
		}
	}

	return sClass;
}

int8 CDBAgent::CreateNewChar(string & strAccountID, int index, string & strCharID, uint8 bRace, uint16 sClass, uint32 nHair, uint8 bFace, uint8 bStr, uint8 bSta, uint8 bDex, uint8 bInt, uint8 bCha)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL CREATE_NEW_CHAR(?, %d, ?, %d, %d, %d, %d, %d, %d, %d, %d, %d)}"), 
		index, bRace, sClass, nHair, bFace, bStr, bSta, bDex, bInt, bCha)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

int8 CDBAgent::ChangeHair(std::string & strAccountID, std::string & strCharID, uint8 bOpcode, uint8 bFace, uint32 nHair)
{
	int8 bRet = 1; // failed
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL CHANGE_HAIR(?, ?, %d, %d, %d)}"), 
		bOpcode, bFace, nHair)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}


void CDBAgent::LoadRentalData(string & strAccountID, string & strCharID, UserRentalMap & rentalData)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(_T("{CALL LOAD_RENTAL_DATA(?)}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return;
	}

	if (!dbCommand->hasData())
		return;

	do
	{
		_USER_RENTAL_ITEM *pItem = new _USER_RENTAL_ITEM();
		_RENTAL_ITEM *pRentalItem = nullptr;

		dbCommand->FetchString(1, pItem->strUserID);
		if (STRCASECMP(pItem->strUserID.c_str(), strCharID.c_str()) != 0)
		{
			delete pItem;
			continue;
		}

		dbCommand->FetchByte(2, pItem->byRentalType);
		dbCommand->FetchByte(3, pItem->byRegType);
		dbCommand->FetchUInt32(4, pItem->nRentalIndex);
		dbCommand->FetchUInt32(5, pItem->nItemID);
		dbCommand->FetchUInt16(6, pItem->sDurability);
		dbCommand->FetchUInt64(7, pItem->nSerialNum);
		dbCommand->FetchUInt32(8, pItem->nRentalMoney);
		dbCommand->FetchUInt16(9, pItem->sRentalTime);
		dbCommand->FetchInt16(10, pItem->sMinutesRemaining);
		dbCommand->FetchString(11, pItem->szTimeRental, sizeof(pItem->szTimeRental));

		pRentalItem = g_pMain->m_RentalItemArray.GetData(pItem->nRentalIndex);
		if (pRentalItem == nullptr
			|| rentalData.find(pItem->nSerialNum) != rentalData.end())
			delete pItem;
		else
			rentalData.insert(std::make_pair(pItem->nSerialNum, pItem));

	} while (dbCommand->MoveNext());
}

void CDBAgent::LoadItemSealData(string & strAccountID, string & strCharID, UserItemSealMap & itemSealData)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(_T("SELECT nItemSerial, nItemID, bSealType FROM SEALED_ITEMS WHERE strAccountID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return;
	}

	if (!dbCommand->hasData())
		return;

	do
	{
		_USER_SEAL_ITEM *pItem = new _USER_SEAL_ITEM;

		dbCommand->FetchUInt64(1, pItem->nSerialNum);
		dbCommand->FetchUInt32(2, pItem->nItemID);
		dbCommand->FetchByte(3, pItem->bSealType);

		if (pItem == nullptr)
			delete pItem;
		else
			itemSealData.insert(std::make_pair(pItem->nSerialNum, pItem));

	} while (dbCommand->MoveNext());
}
uint8 CDBAgent::LoadPetData(uint64 PetSerial, CPet *pPet)
{
	uint8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	
	
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL LOAD_PET_DATA(" I64FMTD ", ?)}"), PetSerial)))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;
	//PET_NAME, PET_EXP, PET_LEVEL, PET_STOMACH, PET_ITEMID, PET_CLASS
	int i = 1;

	pPet->m_bIsKaul = true;
	dbCommand->FetchString(i++, pPet->m_strPetID);
	dbCommand->FetchUInt64(i++, pPet->m_iExp);
	dbCommand->FetchByte(i++, pPet->m_bLevel);
	dbCommand->FetchUInt16(i++, pPet->m_sSatisfaction);
	dbCommand->FetchUInt64(i++, pPet->m_Serial);
	dbCommand->FetchByte(i++, pPet->m_sClass);

	return bRet;
}

uint8 CDBAgent::UpdatePetData(uint64 PetSerial, CPet *pPet)
{
	uint8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	uint8 Slot = 0;
	uint32 FinishTime = 0, ItemNum = 0;

	for (int i=0; i < PET_MAX; i++)
	{
		if (pPet->m_sItemArray[i].nNum > 0)
		{
			Slot = i;
			FinishTime = pPet->m_sItemArray[i].nExpirationTime;
			ItemNum = pPet->m_sItemArray[i].nNum;
			break;
		}
	}
	
	dbCommand->AddParameter(SQL_PARAM_INPUT, pPet->m_strPetID.c_str(), pPet->m_strPetID.length());
	if (!dbCommand->Execute(string_format(_T("UPDATE PET_DATA SET PET_SLOT=%d, PET_FINISHTIME=%d,PET_ITEMID=%d,PET_NAME=?,PET_EXP=" I64FMTD ",PET_LEVEL=%d,PET_STOMACH=%d,PET_CLASS=%d WHERE PET_SERIAL = " I64FMTD ""),Slot,FinishTime,ItemNum,pPet->m_iExp,pPet->m_bLevel,pPet->m_sSatisfaction,pPet->m_sClass,PetSerial)))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	return 1;
}

uint8 CDBAgent::InsertPetData(uint64 PetSerial, CPet *pPet)
{
	uint8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	
	
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pPet->m_strPetID.c_str(), pPet->m_strPetID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL INSERT_PET_DATA(" I64FMTD ", ?, " I64FMTD ", %d, %d, %d, %d)}"), PetSerial, pPet->m_iExp, pPet->m_bLevel, pPet->m_sSatisfaction, pPet->m_sClass, pPet->SpecialPetID)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

uint8 CDBAgent::InsertCypherRingData(uint64 Serial, _CYPHERRING_DATA *pData, std::string strPasswd)
{
	uint8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	
	
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pData->UserName.c_str(), pData->UserName.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strPasswd.c_str(), strPasswd.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL INSERT_CYPHER_RING_DATA(" I64FMTD ", ?, " I64FMTD ", %d, %d, %d, %d, ?)}"), Serial, pData->iExp, pData->bLevel, pData->bRace, pData->sClass, pData->ID)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

uint8 CDBAgent::InsertCypherRingChar(std::string strAccountID, uint64 Serial)
{
	uint8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	
	
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL INSERT_CYPHER_RING_CHAR( ?, " I64FMTD ")}"), Serial)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

bool CDBAgent::LoadUserData(string & strAccountID, string & strCharID, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (pUser == nullptr 
		|| pUser->m_bLogout
		|| !pUser->GetName().empty()
		|| strCharID.length() > MAX_ID_SIZE)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(_T("{CALL LOAD_USER_DATA(?, ?)}")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	char	strItem[INVENTORY_TOTAL * 8] = {0}, strSerial[INVENTORY_TOTAL * 8] = {0},
		strVIPItem[MAX_SLOT_VIP_STORAGE * 8] = {0}, strVIPSerial[MAX_SLOT_VIP_STORAGE * 8] = {0}, strVIPItemTime[MAX_SLOT_VIP_STORAGE * 8], 
		strQuest[QUEST_ARRAY_SIZE],strQuestUse[100 * 6] = {0}, strRebStats[5] = {0},strAchieveQuest[ACHIEVE_QUEST_ARRAY_SIZE], strItemTime[INVENTORY_TOTAL * 8];;
	

	uint16  strAchieveQuestKill[ACHIEVE_QUEST_ARRAY_SIZE * 2];

	uint16 sQuestCount = 0;
	uint16 sQuestUseCount = 0;
	uint16 sAchieveQuestCount = 0;

	int field = 1;
	dbCommand->FetchByte(field++, pUser->m_bNation);
	dbCommand->FetchByte(field++, pUser->m_bRace);
	dbCommand->FetchUInt16(field++, pUser->m_sClass);
	dbCommand->FetchUInt32(field++, pUser->m_nHair);
	dbCommand->FetchByte(field++, pUser->m_bRank);
	dbCommand->FetchByte(field++, pUser->m_bTitle);
	dbCommand->FetchByte(field++, pUser->m_bLevel);
	dbCommand->FetchByte(field++, pUser->m_reblvl);
	dbCommand->FetchInt64(field++, pUser->m_iExp);
	dbCommand->FetchUInt32(field++, pUser->m_iLoyalty);
	dbCommand->FetchByte(field++, pUser->m_bFace);
	dbCommand->FetchByte(field++, pUser->m_bCity);
	dbCommand->FetchInt16(field++, pUser->m_bKnights);
	dbCommand->FetchByte(field++, pUser->m_bFame);
	dbCommand->FetchInt16(field++, pUser->m_sHp);
	dbCommand->FetchInt16(field++, pUser->m_sMp);
	dbCommand->FetchInt16(field++, pUser->m_sSp);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_STR]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_STA]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_DEX]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_INT]);
	dbCommand->FetchByte(field++, pUser->m_bStats[STAT_CHA]);
	dbCommand->FetchByte(field++, pUser->m_bAuthority);
	dbCommand->FetchInt16(field++, pUser->m_sPoints);
	dbCommand->FetchUInt32(field++, pUser->m_iGold);
	dbCommand->FetchByte(field++, pUser->m_bZone);
	dbCommand->FetchInt16(field++, pUser->m_sBind);
	pUser->m_curx = (float)(dbCommand->FetchInt32(field++) / 100.0f);
	pUser->m_curz = (float)(dbCommand->FetchInt32(field++) / 100.0f);
	pUser->m_cury = (float)(dbCommand->FetchInt32(field++) / 100.0f);
	pUser->m_oldx = pUser->m_curx;
	pUser->m_oldy = pUser->m_cury;
	pUser->m_oldz = pUser->m_curz;
	pUser->m_LastX = pUser->m_curx;
	pUser->m_LastZ = pUser->m_curz;
	dbCommand->FetchUInt32(field++, pUser->m_dwTime);
	dbCommand->FetchString(field++, (char *)pUser->m_bstrSkill, sizeof(pUser->m_bstrSkill));
	dbCommand->FetchBinary(field++, strItem, sizeof(strItem));
	dbCommand->FetchBinary(field++, strSerial, sizeof(strSerial));
	dbCommand->FetchBinary(field++, strVIPItem, sizeof(strVIPItem));
	dbCommand->FetchBinary(field++, strVIPSerial, sizeof(strVIPSerial));
	dbCommand->FetchUInt16(field++, sQuestCount);
	dbCommand->FetchUInt16(field++, sQuestUseCount);
	dbCommand->FetchUInt16(field++, sAchieveQuestCount);
	dbCommand->FetchBinary(field++, strQuest, sizeof(strQuest));
	dbCommand->FetchBinary(field++, strQuestUse, sizeof(strQuestUse));
	dbCommand->FetchBinary(field++, strAchieveQuest, sizeof(strAchieveQuest));
	dbCommand->FetchUInt32(field++, pUser->m_iMannerPoint);
	dbCommand->FetchUInt32(field++, pUser->m_iLoyaltyMonthly);
	dbCommand->FetchBinary(field++, strItemTime, sizeof(strItemTime));
	dbCommand->FetchBinary(field++, strVIPItemTime, sizeof(strVIPItemTime));
	dbCommand->FetchString(field++, pUser->m_strMemo);
	dbCommand->FetchUInt16(field++, pUser->m_GenieTime);
	dbCommand->FetchString(field++, (char *)pUser->m_GenieOptions, sizeof(pUser->m_GenieOptions));
	dbCommand->FetchString(field++, (char *)strAchieveQuestKill, sizeof(strAchieveQuestKill));
	dbCommand->FetchBinary(field++, strRebStats, sizeof(strRebStats));
	dbCommand->FetchUInt16(field++, pUser->m_sAchieveSkillTitle);
	dbCommand->FetchUInt16(field++, pUser->m_sAchieveCoverTitle);
	dbCommand->FetchString(field++, pUser->VIPStorePassword);
	dbCommand->FetchUInt32(field++, pUser->nVIPExpirationTime);

	dbCommand->FetchUInt32(field++, pUser->m_iMonsterDefeatedCount);
	dbCommand->FetchUInt32(field++, pUser->m_iUserDefeatedCount);
	dbCommand->FetchUInt32(field++, pUser->m_iUserDeathCount);
	dbCommand->FetchUInt32(field++, pUser->m_iAchievementPoint);
	dbCommand->FetchUInt32(field++, pUser->m_LastOnline);
	pUser->m_strUserID = strCharID;
	pUser->m_lastSaveTime = UNIXTIME;
	pUser->m_lastBonusTime = UNIXTIME;
	pUser->m_TimeOnline = UNIXTIME;
	
	ByteBuffer QuestUseBufer;
	QuestUseBufer.append(strQuestUse, sizeof(strQuestUse));

	// Convert the old quest storage format to the new one.
	pUser->m_questMap.clear();
	if (sQuestCount > QUEST_LIMIT)
		sQuestCount = QUEST_LIMIT;

	for (int i = 0, index = 0; i < sQuestCount; i++, index += 3)
	{
		uint16	sQuestID	= *(uint16 *)(strQuest + index);
		uint8	bQuestState	= *(uint8  *)(strQuest + index + 2);
		pUser->m_questMap.insert(std::make_pair(sQuestID, bQuestState));

	}

	// Start the Seed quest if it doesn't already exist.
	if (pUser->V3_CheckExistEvent(STARTER_SEED_QUEST, 0))
		pUser->V3_QuestEvent(STARTER_SEED_QUEST, 1);

	/*if (pUser->V3_CheckExistEvent(PET_QUEST, 0))
		pUser->V3_QuestEvent(PET_QUEST, 1);*/

	pUser->m_QuestMonMap.clear();
	for(int i = 0; i < sQuestUseCount; i++)
	{
		_QUEST_DATA * pData = new _QUEST_DATA;
		uint16 nQuestID;
		
		QuestUseBufer >> nQuestID;

		if (nQuestID == 0)
			continue;

		QuestUseBufer >> pData->dCKills[0];
		QuestUseBufer >> pData->dCKills[1];
		QuestUseBufer >> pData->dCKills[2];
		QuestUseBufer >> pData->dCKills[3];

		pUser->m_QuestMonMap.insert(std::make_pair(nQuestID, pData));

	}

	pUser->m_AchievequestMap.clear();
	if (sAchieveQuestCount > ACHIEVE_QUEST_LIMIT)
		sAchieveQuestCount = ACHIEVE_QUEST_LIMIT;

	for (int i = 0, index = 0; i < sAchieveQuestCount; i++, index += 3)
	{
		uint16	sAchieveQuestID	= *(uint16 *)(strAchieveQuest + index);
		uint8	bQuestState	= *(uint8  *)(strAchieveQuest + index + 2);

		pUser->m_AchievequestMap.insert(std::make_pair(sAchieveQuestID, bQuestState));	

		_ACHIEVE_MAIN * pMainData = g_pMain->GetAchieveMainPointer(sAchieveQuestID);

		if(pMainData == nullptr)
			continue;

		if (bQuestState == 2)
		{
			switch(pMainData->TabIndex)
			{
			case 0:
		 pUser->AchieveNormalCount++; // Normal Count
		 break;
			case 1:
		 pUser->AchieveQuestCount++; // Quest Count
		 break;
			case 2:
		 pUser->AchieveWarCount++; // War Count
		 break;
			case 3:
		 pUser->AchieveAdventureCount++; // Adventure Count
		 break;
			case 4:
		 pUser->AchieveChallengeCount++;// Challenge Count
		 break;
			default:
				break;
			}

		}


		if(bQuestState == 2)
		{
			if(pUser->AchieveLast3 == 0 
				&& pUser->AchieveLast2 == 0
				&& pUser->AchieveLast1 == 0)
				pUser->AchieveLast1 = sAchieveQuestID;
			else
			{
				pUser->AchieveLast3 = pUser->AchieveLast2;
				pUser->AchieveLast2 = pUser->AchieveLast1;
				pUser->AchieveLast1 = sAchieveQuestID;
			}

		}
	}

	for (int i = 0, index = 0; i < sAchieveQuestCount; i++, index += 6)
	{
		uint16 sQuestID2	=	strAchieveQuestKill[index];
		uint16 bCount		=	strAchieveQuestKill[index + 2];
		uint16 bCount1		=	strAchieveQuestKill[index + 4];
		

		if (sQuestID2 > 0){
			_ACHIEVE_QUEST * pAchieveQuest = new _ACHIEVE_QUEST;

			
			pAchieveQuest->ID = sQuestID2;
			pAchieveQuest->KillCount[0] = bCount;
			pAchieveQuest->KillCount[1] = bCount1;


				if(!pUser->m_bAchieveKillCount.PutData(pAchieveQuest->ID,pAchieveQuest))
				delete pAchieveQuest;

		}
	}

	ByteBuffer itemBuffer, serialBuffer, itemTimeBuffer,RebStatsBuffer;
	ByteBuffer VIPitemBuffer, VIPserialBuffer, VIPitemTimeBuffer;
	itemBuffer.append(strItem, sizeof(strItem));
	serialBuffer.append(strSerial, sizeof(strSerial));
	itemTimeBuffer.append(strItemTime, sizeof(strItemTime));
	VIPitemBuffer.append(strVIPItem, sizeof(strVIPItem));
	VIPserialBuffer.append(strVIPSerial, sizeof(strVIPSerial));
	VIPitemTimeBuffer.append(strVIPItemTime, sizeof(strVIPItemTime));
	RebStatsBuffer.append(strRebStats, sizeof(strRebStats));
	QuestUseBufer.append(strQuestUse, sizeof(strQuestUse));
	
	memset(pUser->m_sItemArray, 0x00, sizeof(pUser->m_sItemArray));
	memset(pUser->m_sVIPItemArray, 0x00, sizeof(pUser->m_sVIPItemArray));

	UserRentalMap rentalData;

	// Until this statement is cleaned up, 
	// no other statements can be processed.
	delete dbCommand.release();

	LoadRentalData(strAccountID, strCharID, rentalData);
	LoadItemSealData(strAccountID, strCharID, pUser->m_sealedItemMap);

	//RebStats Read Start
	uint8 RebStr, RebSta, RebDex, RebInt, RebCha;

	RebStatsBuffer >> RebStr >>  RebSta
		>> RebDex >> RebInt >> RebCha;

	pUser->SetRebStatBuff(STAT_STR, RebStr);
	pUser->SetRebStatBuff(STAT_STA, RebSta);
	pUser->SetRebStatBuff(STAT_DEX, RebDex);
	pUser->SetRebStatBuff(STAT_INT, RebInt);
	pUser->SetRebStatBuff(STAT_CHA, RebCha);
	//RebStats Read Ends

	for (int i = 0; i < INVENTORY_TOTAL; i++)
	{ 
		uint64 nSerialNum;
		uint32 nItemID;
		int16 sDurability, sCount;
		uint32 nItemTime;

		itemBuffer >> nItemID >> sDurability >> sCount;
		serialBuffer >> nSerialNum;
		itemTimeBuffer >> nItemTime;

		_ITEM_TABLE *pTable = g_pMain->GetItemPtr(nItemID);
		if (pTable == nullptr || sCount <= 0)
			continue;

		if (!pTable->m_bCountable && sCount > 1)
			sCount = 1;
		else if (sCount > ITEMCOUNT_MAX)
			sCount = ITEMCOUNT_MAX;

		if (nSerialNum == 0)
			nSerialNum = g_pMain->GenerateItemSerial();

		_ITEM_DATA *pItem = pUser->GetItem(i);
		pItem->nNum = nItemID;
		pItem->sDuration = pTable->isAccessory() ? pTable->m_sDuration : sDurability;
		pItem->sCount = sCount;
		pItem->nSerialNum = nSerialNum;
		pItem->nExpirationTime = nItemTime;

		// If the serial was found in the rental data, mark as rented.
		UserRentalMap::iterator itr = rentalData.find(nSerialNum);
		if (itr != rentalData.end())
		{
			pItem->bFlag = ITEM_FLAG_RENTED;
			pItem->sRemainingRentalTime = itr->second->sMinutesRemaining;
		}

		UserItemSealMap::iterator sealitr = pUser->m_sealedItemMap.find(nSerialNum);
		if (sealitr != pUser->m_sealedItemMap.end())
		{
			if (sealitr->second->bSealType == 1)
				pItem->bFlag = ITEM_FLAG_SEALED;
			else if (sealitr->second->bSealType == 3)
				pItem->bFlag = ITEM_FLAG_BOUND;
			else if (sealitr->second->bSealType == 4)
				pItem->bFlag = ITEM_FLAG_NOT_BOUND;
		}

		g_pMain->AddUserItem(nItemID, nSerialNum);

		if (g_pMain->IsDuplicateItem(nItemID,nSerialNum))
			pItem->bFlag = ITEM_FLAG_DUPLICATE;
	}


	for (int a = 0; a < MAX_SLOT_VIP_STORAGE; a++)
	{ 
		uint64 nSerialNum;
		uint32 nItemID;
		int16 sDurability, sCount;
		uint32 nItemTime;

		VIPitemBuffer >> nItemID >> sDurability >> sCount;
		VIPserialBuffer >> nSerialNum;
		VIPitemTimeBuffer >> nItemTime;


		_ITEM_TABLE *pTable = g_pMain->GetItemPtr(nItemID);
		if (pTable == nullptr || sCount <= 0)
			continue;

		if (!pTable->m_bCountable && sCount > 1)
			sCount = 1;
		else if (sCount > ITEMCOUNT_MAX)
			sCount = ITEMCOUNT_MAX;

		if (nSerialNum == 0)
			nSerialNum = g_pMain->GenerateItemSerial();

		_ITEM_DATA *pItem = pUser->GetVipStorageSlot(a);
		pItem->nNum = nItemID;
		pItem->sDuration = pTable->isAccessory() ? pTable->m_sDuration : sDurability;
		pItem->sCount = sCount;
		pItem->nSerialNum = nSerialNum;
		pItem->nExpirationTime = nItemTime;

		// If the serial was found in the rental data, mark as rented.
		UserRentalMap::iterator itr = rentalData.find(nSerialNum);
		if (itr != rentalData.end())
		{
			pItem->bFlag = ITEM_FLAG_RENTED;
			pItem->sRemainingRentalTime = itr->second->sMinutesRemaining;
		}

		UserItemSealMap::iterator sealitr = pUser->m_sealedItemMap.find(nSerialNum);
		if (sealitr != pUser->m_sealedItemMap.end())
		{
			if (sealitr->second->bSealType == 1)
				pItem->bFlag = ITEM_FLAG_SEALED;
			else if (sealitr->second->bSealType == 3)
				pItem->bFlag = ITEM_FLAG_BOUND;
			else if (sealitr->second->bSealType == 4)
				pItem->bFlag = ITEM_FLAG_NOT_BOUND;
		}

		g_pMain->AddUserItem(nItemID, nSerialNum);

		if (g_pMain->IsDuplicateItem(nItemID,nSerialNum))
			pItem->bFlag = ITEM_FLAG_DUPLICATE;

	}

	// Clean up the rental data
	foreach (itr, rentalData)
		delete itr->second;
	rentalData.clear();

	// Starter items. This needs fixing eventually.
	if (pUser->GetLevel() == 1 && pUser->m_iExp == 0) // going back to their initial bugginess
	{
		uint32 nItemID = 0;
		uint16 sDurability = 0;

		switch (pUser->m_sClass)
		{
		case 101:
			nItemID = 120010000;
			sDurability = 5000;
			break;
		case 102:
			nItemID = 110010000;
			sDurability = 4000;
			break;
		case 103:
			nItemID = 180010000;
			sDurability = 5000;
			break;
		case 104:
			nItemID = 190010000;
			sDurability = 10000;
			break;
		case 201:
			nItemID = 120050000;
			sDurability = 5000;
			break;
		case 202:
			nItemID = 110050000;
			sDurability = 4000;
			break;
		case 203:
			nItemID = 180050000;
			sDurability = 5000;
			break;
		case 204:
			nItemID = 190050000;
			sDurability = 10000;
			break;
		}

		if (nItemID && !pUser->CheckExistItem(nItemID, 1))
		{
			int slot = pUser->GetEmptySlot();
			if (slot < 0)
				return true;

			_ITEM_DATA * pItem = pUser->GetItem(slot);
			pItem->nNum = nItemID;
			pItem->sDuration = sDurability;
			pItem->sCount = 1;
			pItem->nSerialNum = g_pMain->GenerateItemSerial();
		}
	}

	return true;
}

bool CDBAgent::LoadWarehouseData(string & strAccountID, CUser *pUser)
{
	char strItem[WAREHOUSE_MAX * 8], strSerial[WAREHOUSE_MAX * 8], strItemTime[WAREHOUSE_MAX * 8];

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (pUser == nullptr 
		|| pUser->m_bLogout)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(_T("SELECT nMoney, WarehouseData, strSerial, WarehouseDataTime FROM WAREHOUSE WHERE strAccountID = ?")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	memset(strItem, 0x00, sizeof(strItem));
	memset(strSerial, 0x00, sizeof(strSerial));
	memset(strItemTime, 0x00, sizeof(strItemTime));

	dbCommand->FetchUInt32(1, pUser->m_iBank);
	dbCommand->FetchBinary(2, strItem, sizeof(strItem));
	dbCommand->FetchBinary(3, strSerial, sizeof(strSerial));
	dbCommand->FetchBinary(4, strItemTime, sizeof(strItemTime));

	ByteBuffer itemBuffer, serialBuffer, itemTimeBuffer;
	itemBuffer.append(strItem, sizeof(strItem));
	serialBuffer.append(strSerial, sizeof(strSerial));
	itemTimeBuffer.append(strItemTime, sizeof(strItemTime));

	memset(pUser->m_sWarehouseArray, 0x00, sizeof(pUser->m_sWarehouseArray));

	for (int i = 0; i < WAREHOUSE_MAX; i++) 
	{
		uint64 nSerialNum;
		uint32 nItemID, nItemTime;
		int16 sDurability, sCount;

		itemBuffer >> nItemID >> sDurability >> sCount;
		serialBuffer >> nSerialNum;
		itemTimeBuffer >> nItemTime;

		_ITEM_TABLE *pTable = g_pMain->GetItemPtr(nItemID);
		if (pTable == nullptr || sCount <= 0)
			continue;

		if (!pTable->m_bCountable && sCount > 1)
			sCount = 1;
		else if (sCount > ITEMCOUNT_MAX)
			sCount = ITEMCOUNT_MAX;

		pUser->m_sWarehouseArray[i].nNum = nItemID;
		pUser->m_sWarehouseArray[i].sDuration = sDurability;
		pUser->m_sWarehouseArray[i].sCount = sCount;
		pUser->m_sWarehouseArray[i].nSerialNum = nSerialNum;
		pUser->m_sWarehouseArray[i].nExpirationTime = nItemTime;

		UserItemSealMap::iterator sealitr = pUser->m_sealedItemMap.find(nSerialNum);
		if (sealitr != pUser->m_sealedItemMap.end())
		{
			if (sealitr->second->bSealType == 1)
				pUser->m_sWarehouseArray[i].bFlag = ITEM_FLAG_SEALED;
			else if (sealitr->second->bSealType == 3)
				pUser->m_sWarehouseArray[i].bFlag = ITEM_FLAG_BOUND;
			else if (sealitr->second->bSealType == 4)
				pUser->m_sWarehouseArray[i].bFlag = ITEM_FLAG_NOT_BOUND;
		}

		g_pMain->AddUserItem(nItemID, nSerialNum);

		if (g_pMain->IsDuplicateItem(nItemID,nSerialNum))
			pUser->m_sWarehouseArray[i].bFlag = ITEM_FLAG_DUPLICATE;
	}

	return true;
}

bool CDBAgent::LoadPremiumServiceUser(string & strAccountID, CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &pUser->PremiumID);

	if (!dbCommand->Execute(_T("{CALL LOAD_PREMIUM_SERVICE_USER(?,?)}")))
		ReportSQLError(m_AccountDB->GetError());
	
	


	uint16 count = 0;
	do
	{
		_PREMIUM_TYPE * pPremium = new _PREMIUM_TYPE;

		dbCommand->FetchByte(1, pPremium->PremiumType);
		dbCommand->FetchUInt16(2, pPremium->PremiumTime);

		pUser->PremiumList.PutData(pPremium->PremiumType,pPremium);
		count++;
	} while (dbCommand->MoveNext());

	

	// this is hardcoded because we don't really care about the other mode
	if (pUser->PremiumID != 0)
		pUser->m_bAccountStatus = 1; // normal premium with expiry time
	else
		pUser->m_bAccountStatus = 0;

	return true;
}


bool CDBAgent::LoadSavedMagic(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	if (!dbCommand->Execute(_T("SELECT "
		"nSkill1, nDuring1, nSkill2, nDuring2, nSkill3, nDuring3, nSkill4, nDuring4, nSkill5, nDuring5, "
		"nSkill6, nDuring6, nSkill7, nDuring7, nSkill8, nDuring8, nSkill9, nDuring9, nSkill10, nDuring10 "
		"FROM USER_SAVED_MAGIC WHERE strCharID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	pUser->m_savedMagicMap.clear();
	if (!dbCommand->hasData())
		return true;

	for (int i = 1; i <= 20; i += 2)
	{
		uint32	nSkillID;
		uint16	nExpiry;
		dbCommand->FetchUInt32(i, nSkillID);
		dbCommand->FetchUInt16(i + 1, nExpiry);

		if (nSkillID != 0)
			pUser->m_savedMagicMap[nSkillID] = (nExpiry + UNIXTIME);
	}

	return true;
}

bool CDBAgent::UpdateSavedMagic(CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	Guard lock(pUser->_unitlock);
	uint32 nSkillID[10] = {0};
	uint16 tExpiryTime[10] = {0};
	uint32 i = 0;
	foreach (itr, pUser->m_savedMagicMap)
	{
		nSkillID[i]		= itr->first;
		if (itr->first != 0)
			tExpiryTime[i]	= uint16(itr->second - UNIXTIME);

		if (++i == 10)
			break;
	}

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_SAVED_MAGIC(?, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)}"), 
		nSkillID[0], tExpiryTime[0], nSkillID[1], tExpiryTime[1], nSkillID[2], tExpiryTime[2], nSkillID[3], tExpiryTime[3], nSkillID[4], tExpiryTime[4],
		nSkillID[5], tExpiryTime[5], nSkillID[6], tExpiryTime[6], nSkillID[7], tExpiryTime[7], nSkillID[8], tExpiryTime[8], nSkillID[9], tExpiryTime[9])))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

bool CDBAgent::SetLogInInfo(string & strAccountID, string & strCharID, string & strServerIP, short sServerNo, string & strClientIP, uint8 bInit)
{
	uint8 result = 0;
	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &result);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strServerIP.c_str(), strServerIP.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strClientIP.c_str(), strClientIP.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL SET_LOGIN_INFO(?, ?, %d, ?, ?, %d)}"), sServerNo, bInit)))
		ReportSQLError(m_AccountDB->GetError());

	return (bool)(result == 0 ? false : true);
}

bool CDBAgent::ClearWebItemMall(CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());

	// TODO: Add an arg for the free slot count so we only need to pull/delete what we can hold.
	if (!dbCommand->Execute(_T("{CALL CLEAR_WEB_ITEMMALL(?)}")))
	{
		ReportSQLError(m_AccountDB->GetError());
		return false;
	}else
		return true;
}


bool CDBAgent::LoadWebItemMall(std::vector<_ITEM_DATA> & itemList, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());

	// TODO: Add an arg for the free slot count so we only need to pull/delete what we can hold.
	if (!dbCommand->Execute(_T("{CALL LOAD_WEB_ITEMMALL(?)}")))
		ReportSQLError(m_AccountDB->GetError());

	if (dbCommand->hasData())
	{
		do
		{
			_ITEM_DATA item;
			dbCommand->FetchUInt32(2, item.nNum); // 1 is the account name, which we don't need to use unless we're logging	
			dbCommand->FetchUInt16(3, item.sCount);
			dbCommand->FetchUInt32(5, item.nExpirationTime);
			itemList.push_back(item);
		} while (dbCommand->MoveNext());
	}

	return true;
}

bool CDBAgent::LoadSkillShortcut(Packet & result, CUser *pUser)
{
	if (pUser == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	uint16 sCount;
	char strSkillData[260];

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());
	if (!dbCommand->Execute(_T("SELECT nCount, strSkillData FROM USERDATA_SKILLSHORTCUT WHERE strCharID = ?")))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchUInt16(1, sCount);
	dbCommand->FetchString(2, strSkillData, sizeof(strSkillData));

	result << sCount;
	for (uint32 i = 0; i < sCount; i++)
		result << *(uint32 *)(strSkillData + (i * sizeof(uint32)));

	return true;
}

void CDBAgent::SaveSkillShortcut(uint16 sCount, char *buff, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, buff, 260);

	if (!dbCommand->Execute(string_format(_T("{CALL SKILLSHORTCUT_SAVE(?, %d, ?)}"), sCount)))
		ReportSQLError(m_GameDB->GetError());
}

bool CDBAgent::SavePremiumServiceUser(CUser *pUser, _PREMIUM_TYPE * pPremium)
{
	if (pUser == nullptr)
		return false;

	if (pPremium->PremiumTime == 0 
		&& pPremium->PremiumType == 0)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetAccountName().c_str(), pUser->GetAccountName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());

	if (!dbCommand->Execute(string_format(_T("{CALL SAVE_PREMIUM_SERVICE_USER(?, ?, %d, %d)}"), 
		pPremium->PremiumType, pPremium->PremiumTime)))
		ReportSQLError(m_GameDB->GetError());

	return true;
}

uint8 CDBAgent::SealItem(string strSealPasswd, uint64 nItemSerial, uint32 nItemID, uint8 bSealType, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 3;

	uint8 bRet = 1;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetAccountName().c_str(), pUser->GetAccountName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strSealPasswd.c_str(), strSealPasswd.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL USER_ITEM_SEAL(?, ?, ?, " I64FMTD ", %d, %d)}"), nItemSerial, nItemID, bSealType)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

void CDBAgent::RequestFriendList(std::vector<string> & friendList, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());
	if (!dbCommand->Execute(_T("SELECT * FROM FRIEND_LIST WHERE strUserID = ?")))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return;

	string strCharID;
	for (int i = 2; i <= 25; i++)
	{
		if (dbCommand->FetchString(i, strCharID)
			&& strCharID.length())
			friendList.push_back(strCharID);
	}
}

FriendAddResult CDBAgent::AddFriend(short sid, short tid)
{
	CUser *pSrcUser = g_pMain->GetUserPtr(sid), *pTargetUser = g_pMain->GetUserPtr(tid);
	CBot *pTargetBot = g_pMain->m_arBotArray.GetData(tid);
	if (pSrcUser == nullptr 
		|| (pTargetUser == nullptr && pTargetBot == nullptr))
		return FRIEND_ADD_ERROR;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return FRIEND_ADD_ERROR;

	int16 sRet = (int16)FRIEND_ADD_ERROR;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pSrcUser->GetName().c_str(), pSrcUser->GetName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, pTargetUser == nullptr ? pTargetBot->GetName().c_str() : pTargetUser->GetName().c_str(), pTargetUser == nullptr ? pTargetBot->GetName().length() : pTargetUser->GetName().length());

	if (!dbCommand->Execute(_T("{? = CALL INSERT_FRIEND_LIST(?, ?)}")))
		ReportSQLError(m_GameDB->GetError());

	return (FriendAddResult)sRet;
}

FriendRemoveResult CDBAgent::RemoveFriend(string & strCharID, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return FRIEND_REMOVE_ERROR;

	int16 sRet = (int16)FRIEND_REMOVE_ERROR;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(_T("{? = CALL DELETE_FRIEND_LIST(?, ?)}")))
		ReportSQLError(m_GameDB->GetError());

	return (FriendRemoveResult)sRet;
}

bool CDBAgent::UpdateUser(string & strCharID, UserUpdateType type, CUser *pUser)
{
	if (strCharID != pUser->GetName())
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;
	
	pUser->m_dwTime += (uint32(UNIXTIME - pUser->m_TimeOnline) / 60);
	pUser->m_TimeOnline = UNIXTIME;
	char strQuest[QUEST_ARRAY_SIZE];
	char strAchieveQuest[QUEST_ARRAY_SIZE];
	uint16 strAchieveQuestKill[QUEST_ARRAY_SIZE];
	memset(strQuest, 0, sizeof(strQuest));
	memset(strAchieveQuest, 0, sizeof(strAchieveQuest));
	memset(strAchieveQuestKill, 0, sizeof(strAchieveQuestKill));
	int index = 0;
	foreach (itr, pUser->m_questMap)
	{
		*(uint16 *)(strQuest + index) = itr->first;
		*(uint8  *)(strQuest + index + 2) = itr->second;
		index += 3;
	}

	int index2 = 0;
	foreach (itr, pUser->m_AchievequestMap)
	{

		if(itr->first > 32000 
			|| itr->first < 0
			|| itr->second == 0)
			continue;

		*(uint16 *)(strAchieveQuest + index2) = itr->first;
		*(uint8  *)(strAchieveQuest + index2 + 2) = itr->second;
		//printf("UpdateUser ID:%d ST:%d\n",itr->first,itr->second);
		index2 += 3;
	}

	index2 = 0;
	int AchieveQuestSize = 0;
	foreach(itr,pUser->m_AchievequestMap)
	{
		if (itr->second == 1 || itr->second == 2) 
		{
			Guard lock(pUser->m_bAchieveKillCount.m_lock);
			_ACHIEVE_QUEST * pAchieveQuest = pUser->m_bAchieveKillCount.GetData(itr->first);
			if (pAchieveQuest == nullptr)
				continue;

		strAchieveQuestKill[index2] = itr->first; // 0,1
		strAchieveQuestKill[index2 + 2] = pAchieveQuest->KillCount[0]; // 2,3
		strAchieveQuestKill[index2 + 4] = pAchieveQuest->KillCount[1]; // 4,5
		AchieveQuestSize++;
		index2 += 6;
		}

	}
	

	ByteBuffer itemBuffer, serialBuffer, itemTimeBuffer,RebStatsBuffer,QuestBuffer;
	ByteBuffer VIPitemBuffer, VIPserialBuffer, VIPitemTimeBuffer;
	
	uint8 kcount = 0;
	foreach(itr, pUser->m_QuestMonMap)
	{
		QuestBuffer << itr->first;

		QuestBuffer << itr->second->dCKills[0];
		QuestBuffer << itr->second->dCKills[1];
		QuestBuffer << itr->second->dCKills[2];
		QuestBuffer << itr->second->dCKills[3];

		kcount++;
	}

	if (kcount == 0)
		QuestBuffer << uint16(0);

	for (int i = 0; i < INVENTORY_TOTAL; i++)
	{
		_ITEM_DATA *pItem = &pUser->m_sItemArray[i];
		itemBuffer << pItem->nNum << pItem->sDuration << pItem->sCount;
		serialBuffer << pItem->nSerialNum;
		itemTimeBuffer << pItem->nExpirationTime;
	}
	for (int i = CWING; i <= CTOP; i++)
	{
		_ITEM_DATA *pItem = &pUser->m_sItemArray[i];
		itemBuffer << pItem->nNum << pItem->sDuration << pItem->sCount;
		serialBuffer << pItem->nSerialNum;
		itemTimeBuffer << pItem->nExpirationTime;
	}
	for (int a = 0; a < MAX_SLOT_VIP_STORAGE; a++)
	{
		_ITEM_DATA *pItem = &pUser->m_sVIPItemArray[a];
		VIPitemBuffer << pItem->nNum << pItem->sDuration << pItem->sCount;
		VIPserialBuffer << pItem->nSerialNum;
		VIPitemTimeBuffer << pItem->nExpirationTime;
	}
	
	RebStatsBuffer << pUser->GetRebStatBuff(STAT_STR) << pUser->GetRebStatBuff(STAT_STA)
		<< pUser->GetRebStatBuff(STAT_DEX) << pUser->GetRebStatBuff(STAT_INT) << pUser->GetRebStatBuff(STAT_CHA);

	
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)pUser->m_bstrSkill, sizeof(pUser->m_bstrSkill));
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)itemBuffer.contents(), itemBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)serialBuffer.contents(), serialBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)VIPitemBuffer.contents(), VIPitemBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)VIPserialBuffer.contents(), VIPserialBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)strQuest, sizeof(strQuest), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)QuestBuffer.contents(), QuestBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)strAchieveQuest, sizeof(strAchieveQuest), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)itemTimeBuffer.contents(), itemTimeBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)VIPitemTimeBuffer.contents(), VIPitemTimeBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strMemo.c_str(), pUser->m_strMemo.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)pUser->m_GenieOptions, sizeof(pUser->m_GenieOptions));
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)strAchieveQuestKill, sizeof(strAchieveQuestKill));
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)RebStatsBuffer.contents(), RebStatsBuffer.size(), SQL_BINARY);
	
	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_USER_DATA("
		"?, " // strCharID 
		"%d, %d, %d, %d, %d, "		// nation, race, class, hair, rank
		"%d, %d, %d, " I64FMTD ", %d, %d, "		// title, level,rebLevel, exp, loyalty, face
		"%d, %d, %d, "				// city, knights, fame
		"%d, %d, %d, "				// hp, mp, sp
		"%d, %d, %d, %d, %d, "		// str, sta, dex, int, cha
		"%d, %d, %d, %d, %d, "		// authority, free points, gold, zone, bind
		"%d, %d, %d, %d, %d, %d, %d, "	// x, z, y, dwTime, sQuestCount, sQuestMonsterCount, sAchieveQuestCount
		"?, ?, ?, ?, ?, ?,?, ?,"		// strSkill, strItem, strSerial, strVIPItem, strVIPSerial, strQuest, strQuestKill, strAchieveQuest
		"%d, %d, ?, ?,"				// manner points, monthly NP, strItemTime, strVIPItemTime
		"?, %d, ?, ?, ?, %d, %d, %d, " // ClanMemo Genie Time , Genie Options, strAchieveQuestKill, rebirthstats, dtUpdateClan, AchieveSkill, AchieveCover
		"%d,%d, %d, %d, %d, "			// vipExTime,m_iMonsterDefeatedCount, m_iUserDefeatedCount, m_iUserDeathCount, m_iAchievementPoint,LastOnline
		"%d)}"),
		pUser->m_bNation, pUser->m_bRace, pUser->m_sClass, pUser->m_nHair, pUser->m_bRank, 
		pUser->m_bTitle, pUser->m_bLevel, pUser->m_reblvl, pUser->m_iExp /* temp hack, database needs to support it */, pUser->m_iLoyalty, pUser->m_bFace, 
		pUser->m_bCity,	pUser->m_bKnights, pUser->m_bFame, 
		pUser->m_sHp, pUser->m_sMp, pUser->m_sSp, 
		pUser->m_bStats[STAT_STR], pUser->m_bStats[STAT_STA], pUser->m_bStats[STAT_DEX], pUser->m_bStats[STAT_INT], pUser->m_bStats[STAT_CHA], 
		pUser->m_bAuthority, pUser->m_sPoints, pUser->m_iGold, pUser->m_bZone, pUser->m_sBind, 
		(int)(pUser->m_curx*100), (int)(pUser->m_curz*100), (int)(pUser->m_cury*100), pUser->m_dwTime, pUser->m_questMap.size(), pUser->m_QuestMonMap.size(), AchieveQuestSize,
		pUser->m_iMannerPoint, pUser->m_iLoyaltyMonthly, pUser->m_GenieTime, uint32(UNIXTIME), pUser->m_sAchieveSkillTitle,pUser->m_sAchieveCoverTitle,
		pUser->nVIPExpirationTime,pUser->m_iMonsterDefeatedCount,pUser->m_iUserDefeatedCount,pUser->m_iUserDeathCount,pUser->m_iAchievementPoint,
		pUser->m_LastOnline)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}


	pUser->m_lastSaveTime = UNIXTIME;
	pUser->m_lastBonusTime = UNIXTIME;
	return true;
}

bool CDBAgent::UpdateWarehouseData(string & strAccountID, UserUpdateType type, CUser *pUser)
{
	if (strAccountID.length() == 0)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;


	// This *should* be padded like the database field is (unnecessarily), but I want to see how MSSQL responds.
	ByteBuffer itemBuffer, serialBuffer, itemTimeBuffer;
	for (int i = 0; i < WAREHOUSE_MAX; i++)
	{
		_ITEM_DATA *pItem = &pUser->m_sWarehouseArray[i];
		itemBuffer << pItem->nNum << pItem->sDuration << pItem->sCount;
		serialBuffer << pItem->nSerialNum;
		itemTimeBuffer << pItem->nExpirationTime;
	}

	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)itemBuffer.contents(), itemBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)serialBuffer.contents(), serialBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)itemTimeBuffer.contents(), itemTimeBuffer.size(), SQL_BINARY);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(string_format(_T("UPDATE WAREHOUSE SET nMoney=%d, dwTime=%d, WarehouseData=?, strSerial=?, WarehouseDataTime=? WHERE strAccountID=?"), 
		pUser->m_iBank, pUser->m_dwTime)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

int8 CDBAgent::CreateAlliance(uint8 byType, uint16 shAlliancIndex, uint16 shKnightsIndex, uint8  byEmptyIndex, uint8 bySiegeFlag)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;
	

	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_KNIGHTS_ALLIANCE(%d, %d, %d, %d, %d)}"), byType, shAlliancIndex, shKnightsIndex, byEmptyIndex,bySiegeFlag)))
	ReportSQLError(m_GameDB->GetError());

	return bRet;
}

int8 CDBAgent::InsertAlliance(uint8 byType, uint16 shAlliancIndex, uint16 shKnightsIndex, uint8  byEmptyIndex, uint8 bySiegeFlag)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;
	

	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_KNIGHTS_ALLIANCE(%d, %d, %d, %d, %d)}"), byType, shAlliancIndex, shKnightsIndex, byEmptyIndex,bySiegeFlag)))
	ReportSQLError(m_GameDB->GetError());

	return bRet;
}

int8 CDBAgent::RemoveAlliance(uint8 byType, uint16 shAlliancIndex, uint16 shKnightsIndex, uint8  byEmptyIndex, uint8 bySiegeFlag)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;
	

	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_KNIGHTS_ALLIANCE(%d, %d, %d, %d, %d)}"), byType, shAlliancIndex, shKnightsIndex, byEmptyIndex,bySiegeFlag)))
	ReportSQLError(m_GameDB->GetError());

	return bRet;
}

int8 CDBAgent::DestoryAlliance(uint8 byType, uint16 shAlliancIndex, uint16 shKnightsIndex, uint8  byEmptyIndex, uint8 bySiegeFlag)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;
	

	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_KNIGHTS_ALLIANCE(%d, %d, %d, %d, %d)}"), byType, shAlliancIndex, shKnightsIndex, byEmptyIndex,bySiegeFlag)))
	ReportSQLError(m_GameDB->GetError());

	return bRet;
}

int8 CDBAgent::CreateKnights(uint16 sClanID, uint8 bNation, string & strKnightsName, string & strChief, uint8 bFlag)
{
	int8 bRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strKnightsName.c_str(), strKnightsName.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strChief.c_str(), strChief.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL CREATE_KNIGHTS(%d, %d, %d, ?, ?)}"), sClanID, bNation, bFlag)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

int CDBAgent::UpdateKnights(uint8 bType, string & strCharID, uint16 sClanID, uint8 bDomination)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL UPDATE_KNIGHTS(%d, ?, %d, %d)}"), bType + 0x10, sClanID, bDomination)))
		ReportSQLError(m_GameDB->GetError());

	if (bType == KNIGHTS_VICECHIEF && sRet == 0)
	{
		CKnights * pKnights = g_pMain->GetClanPtr(sClanID);
		if (pKnights != nullptr)
		{
			if (pKnights->m_strViceChief_1 == "")
				pKnights->m_strViceChief_1 = strCharID;
			else if (pKnights->m_strViceChief_2 == "")
				pKnights->m_strViceChief_2 = strCharID;
			else if (pKnights->m_strViceChief_3 == "")
				pKnights->m_strViceChief_3 = strCharID;
		}
	}

	return sRet;
}

int CDBAgent::DeleteKnights(uint16 sClanID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	int16 sRet = -1;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	if (!dbCommand->Execute(string_format(_T("{? = CALL DELETE_KNIGHTS (%d)}"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

void CDBAgent::UpdateVIPStoragePassword(std::string strUserID, std::string VIPPassword)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return ;
		
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, VIPPassword.c_str(), VIPPassword.length());
	if (!dbCommand->Execute(string_format(_T("EXEC UPDATE_USER_VIP_PASSWORD  ?,?"))))
		ReportSQLError(m_GameDB->GetError());

	return;
}

void CDBAgent::UpdateVIPStorageMinute(std::string strUserID, uint32 VIPMinute)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;
		
	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	if (!dbCommand->Execute(string_format(_T("EXEC UPDATE_USER_VIP_MINUTE  ?,%d"), VIPMinute)))
		ReportSQLError(m_GameDB->GetError());

	return;
}


uint16 CDBAgent::LoadKnightsAllMembers(uint16 sClanID, Packet & result)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	if (!dbCommand->Execute(string_format(_T("SELECT strUserID, Fame, [Level], Class FROM USERDATA WHERE Knights = %d"), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return 0;

	uint16 count = 0;
	do
	{
		string strCharID; uint16 sClass; uint8 bFame, bLevel;
		dbCommand->FetchString(1, strCharID);
		dbCommand->FetchByte(2, bFame);
		dbCommand->FetchByte(3, bLevel);
		dbCommand->FetchUInt16(4, sClass);

		result << strCharID << bFame << bLevel << sClass 
			// check if user's logged in (i.e. grab logged in state)
			<< uint8(g_pMain->GetUserPtr(strCharID, TYPE_CHARACTER) == nullptr ? 0 : 1);
		count++;
	} while (dbCommand->MoveNext());

	return count;
}

bool CDBAgent::LoadKnightsInfo(uint16 sClanID, uint8 & bNation, std::string & strKnightsName, uint16 & sMembers, uint32 & nPoints, uint8 & bRank)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	if (!dbCommand->Execute(string_format(_T("SELECT Nation, IDName, Members, Points, Ranking FROM KNIGHTS WHERE IDNum=%d" ), sClanID)))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchByte(1, bNation);
	dbCommand->FetchString(2, strKnightsName);
	dbCommand->FetchUInt16(3, sMembers);
	dbCommand->FetchUInt32(4, nPoints);
	dbCommand->FetchByte(5, bRank);

	return true;
}

void CDBAgent::LoadKnightsAllList()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	const tstring szSQL = _T("SELECT IDNum, Points, Ranking FROM KNIGHTS WHERE Points != 0 ORDER BY Points DESC"); 

	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(szSQL))
		ReportSQLError(m_GameDB->GetError());

	if (!dbCommand->hasData())
		return;

	Packet result(WIZ_KNIGHTS_PROCESS);
	uint8 bCount = 0;
	int offset;

	do
	{
		if (bCount == 0)
		{
			result.clear();
			offset = result.wpos();
			result << uint8(0);
		}

		uint32 nPoints; uint16 sClanID; uint8 bRanking;
		dbCommand->FetchUInt16(1, sClanID);
		dbCommand->FetchUInt32(2, nPoints);
		dbCommand->FetchByte(3, bRanking);

		result << sClanID << nPoints << bRanking;

		// only send 100 clans at a time (no shared memory limit, yay!)
		if (++bCount >= 100)
		{
			// overwrite the count
			result.put(offset, bCount);

			CKnightsManager::RecvKnightsAllList(result);
			bCount = 0;
		}
	} while (dbCommand->MoveNext());

	// didn't quite make it in the last batch (if any)? send the remainder.
	if (bCount < 100)
	{
		result.put(offset, bCount);
		CKnightsManager::RecvKnightsAllList(result);
	}
}

bool CDBAgent::UpdateClanSymbol(uint16 sClanID, uint16 sSymbolSize, char *clanSymbol)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, clanSymbol, MAX_KNIGHTS_MARK, SQL_BINARY);
	if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS SET Mark=?, sMarkVersion += 1, sMarkLen=%d WHERE IDNum=%d"), sSymbolSize, sClanID)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

/**
* @brief	Handles the clan NP donations database request.
* 			It is essentially the client packet's real handler
* 			as the packet is simply forwarded here.
*
* @param	pUser	The user.
* @param	pkt  	The packet.
*/
void CKnightsManager::ReqDonateNP(CUser *pUser, Packet & pkt)
{
	if (pUser == nullptr || !pUser->isInClan())
		return;

	uint32 amountNP;
	pkt >> amountNP;

	// Ensure the user has enough NP to donate to the clan.
	if (amountNP > pUser->GetLoyalty()
		// Users must have at least MIN_NP_TO_DONATE to donate.
			|| (pUser->GetLoyalty() - amountNP) < MIN_NP_TO_DONATE)
			return;

	// Ensure the clan exists
	CKnights * pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
	if (pKnights == nullptr)
		return;

	// Take player's donated NP. Don't affect monthly NP. 
	if (g_DBAgent.DonateClanPoints(pUser, amountNP))
	{
		// Update the user's donated NP
		CKnightsManager::AddUserDonatedNPinGame(pUser->GetClanID(), pUser, amountNP);


		// Take the NP from the user and update the client.
		pUser->m_iLoyalty -= amountNP;
		Packet result(WIZ_LOYALTY_CHANGE, uint8(LOYALTY_NATIONAL_POINTS));
		result << pUser->m_iLoyalty << pUser->m_iLoyaltyMonthly
			<< uint32(0) // Clan donations(? Donations made by this user? For the clan overall?)
			<< amountNP; // Premium NP(? Additional NP gained?)
		pUser->Send(&result);
	}
}

/**
* @brief	Donates (clanPoints) clan points to the specified user's clan.
* 			Also increases the user's total NP donated.
*
* @param	pUser	  	The donor user.
* @param	amountNP  	The number of national points being donated by the user.
*
* @return	true if it succeeds, false if it fails.
*/
bool CDBAgent::DonateClanPoints(CUser * pUser,  uint32 amountNP)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL DONATE_CLAN_POINTS(?, %d, %d)}"), pUser->GetClanID(), amountNP)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	return true;
}

/**
* @brief	Handles the database request to refund the specified
* 			logged out character's donated NP.
* 			
* 			NOTE: Logged in players don't need to be handled as their NP is
* 			refunded in-game.
*
* @param	pkt	The packet.
*/
void CKnightsManager::ReqRefundNP(Packet & pkt)
{
	string strUserID;
	uint32 nRefundNP;
	pkt >> strUserID >> nRefundNP;
	g_DBAgent.RefundNP(strUserID, nRefundNP);
}

/**
* @brief	Handles the database request to update the
* 			specified clan's clan point fund.
*
* @param	pkt	The packet.
*/
void CKnightsManager::ReqUpdateNP(Packet & pkt)
{
	uint16 sClanID;
	uint32 nClanPointFund;
	pkt >> sClanID >> nClanPointFund;
	g_DBAgent.UpdateClanFund(sClanID, nClanPointFund);
}

/**
* @brief	Refunds the specified amount of NP to a logged out character.
*
* @param	strUserID	Character's name.
* @param	nRefundNP	The amount of NP to refund.
*/
void CDBAgent::RefundNP(string & strUserID, uint32 nRefundNP)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	if (!dbCommand->Execute(string_format(_T("UPDATE USERDATA SET Loyalty += %d WHERE strUserID = ?"), nRefundNP)))
		ReportSQLError(m_GameDB->GetError());
}



/**
* @brief	Change authority is logged out character.
*
* @param	strUserID	Character's name.
* @param	nAuthority	New user authority.
*/
void CDBAgent::UpdateUserAuthority(string & strUserID, uint16 nAuthority)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	if (!dbCommand->Execute(string_format(_T("UPDATE USERDATA SET Authority = %d WHERE strUserID = ?"), nAuthority)))
		ReportSQLError(m_GameDB->GetError());
}

/**
* @brief	Updates the clan fund.
*
* @param	sClanID		  	Identifier for the clan.
* @param	nClanPointFund	The current clan point fund.
*/
void CDBAgent::UpdateClanFund(uint16 sClanID, uint32 nClanPointFund)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS SET ClanPointFund = %d WHERE IDNum = %d"), nClanPointFund, sClanID)))
		ReportSQLError(m_GameDB->GetError());
}

/**
* @brief	Updates the clan notice.
*
* @param	sClanID		 	Identifier for the clan.
* @param	strClanNotice	The clan notice.
*/
void CDBAgent::UpdateClanNotice(uint16 sClanID, std::string & strClanNotice)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strClanNotice.c_str(), strClanNotice.length());
	if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS SET strClanNotice = ? WHERE IDNum = %d"), sClanID)))
		ReportSQLError(m_GameDB->GetError());
}


NameChangeOpcode CDBAgent::UpdateCharacterName(std::string & strAccountID, std::string & strUserID, std::string & strNewUserID)
{
	 unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	 if (dbCommand.get() == nullptr)
	  return NameChangeInvalidName;

	 int16 sRet = 1;

	 dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	 dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	 dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	 dbCommand->AddParameter(SQL_PARAM_INPUT, strNewUserID.c_str(), strNewUserID.length());

	 if (!dbCommand->Execute(string_format(_T("{? = CALL CHANGE_NEW_ID(%d, ?, ?, ?, 1)}"), g_pMain->m_nServerNo)))
	  ReportSQLError(m_GameDB->GetError());

	 return NameChangeOpcode(sRet);
}


ClanNameChangeOpcode CDBAgent::UpdateClanName(std::string & strUserID, uint16 sClanID, std::string & strNewKnightsID)
{
	 unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	 if (dbCommand.get() == nullptr)
		 return ClanNameChangeInvalidName;

	 int16 sRet = 1;

	 dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	 dbCommand->AddParameter(SQL_PARAM_INPUT, strUserID.c_str(), strUserID.length());
	 dbCommand->AddParameter(SQL_PARAM_INPUT, strNewKnightsID.c_str(), strNewKnightsID.length());

	 if (!dbCommand->Execute(string_format(_T("{? = CALL CHANGE_NEW_KNIGHTS_ID(%d, ?, ?, %d, 1)}"), g_pMain->m_nServerNo, sClanID)))
	  ReportSQLError(m_GameDB->GetError());

	 return ClanNameChangeOpcode(sRet);
}

/**
* @brief	Handles clan cape database updates.
*
* @param	sClanID	Identifier for the clan.
* @param	sCapeID	Identifier for the cape.
* @param	r 	Red colour component.
* @param	g 	Green colour component.
* @param	b 	Blue colour component.
*
* @return	true if it succeeds, false if it fails.
*/
void CDBAgent::UpdateCape(uint16 sClanID, uint16 sCapeID, uint8 r, uint8 g, uint8 b)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS SET sCape=%d, bCapeR=%d, bCapeG=%d, bCapeB=%d WHERE IDNum=%d"), 
		sCapeID, r, g, b, sClanID)))
		ReportSQLError(m_GameDB->GetError());
}


void CDBAgent::UpdatePremiumType(uint8 bType, std::string StrAccountID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, StrAccountID.c_str(), StrAccountID.length());

	if (!dbCommand->Execute(string_format(_T("UPDATE TB_USER SET bPremiumType=%d WHERE strAccountID=?"), 
		bType)))
		ReportSQLError(m_GameDB->GetError());
}
/**
* @brief	Updates the clan grade.
*
* @param	sClanID	Identifier for the clan.
* @param	byFlag 	The clan type (training, promoted, etc).
* @param	sCapeID	Identifier for the cape.
*/
void CDBAgent::UpdateClanGrade(uint16 sClanID, uint8 byFlag, uint16 sCapeID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS SET sCape=%d, Flag=%d WHERE IDNum=%d"), 
		sCapeID, byFlag, sClanID)))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::UpdateBattleEvent(string & strCharID, uint8 bNation)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("UPDATE BATTLE SET byNation=%d, strUserName=? WHERE sIndex=%d"), bNation, g_pMain->m_nServerNo)))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::AccountLogout(string & strAccountID)
{
	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(_T("{CALL ACCOUNT_LOGOUT(?)}")))
		ReportSQLError(m_AccountDB->GetError());
}

void CDBAgent::UpdateConCurrentUserCount(int nServerNo, int nZoneNo, int nCount)
{
	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("UPDATE CONCURRENT SET zone%d_count = %d WHERE serverid = %d"), nZoneNo, nCount, nServerNo)))
		ReportSQLError(m_AccountDB->GetError());
}

// This is what everything says it should do, 
// but the client doesn't seem to care if it's over 1
uint8 CDBAgent::GetUnreadLetterCount(string & strCharID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	uint8 bCount = 0;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bCount);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(_T("{? = CALL MAIL_BOX_CHECK_COUNT(?)}")))
	{
		ReportSQLError(m_GameDB->GetError());
		return 0;
	}

	return bCount;
}

bool CDBAgent::GetLetterList(string & strCharID, Packet & result, bool bNewLettersOnly /* = true*/)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	int8 bCount = 0;
	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bCount);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL MAIL_BOX_REQUEST_LIST(?, %d)}"), bNewLettersOnly)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	result << uint8(1);
	int offset = result.wpos();
	result << bCount; // placeholder for count

	if (!dbCommand->hasData())
		return true;

	result.SByte();
	do
	{
		string strSubject, strSender;
		uint32 nLetterID, nItemID, nCoins, nDate;
		uint16 sCount, sDaysRemaining;
		uint8 bStatus, bType;

		dbCommand->FetchUInt32(1, nLetterID);
		dbCommand->FetchByte(2, bStatus);
		dbCommand->FetchByte(3, bType);
		dbCommand->FetchString(4, strSubject);
		dbCommand->FetchString(5, strSender);
		dbCommand->FetchByte(6, bType);
		dbCommand->FetchUInt32(7, nItemID);
		dbCommand->FetchUInt16(8, sCount);
		dbCommand->FetchUInt32(9, nCoins);
		dbCommand->FetchUInt32(10, nDate);
		dbCommand->FetchUInt16(11, sDaysRemaining); 

		result	<< nLetterID // letter ID
			<< bStatus  // letter status, doesn't seem to affect anything
			<< strSubject << strSender
			<< bType;	

		if (bType == 2)
			result	<< nItemID << sCount << nCoins;

		result	<< nDate // date (yy*10000 + mm*100 + dd)
			<< uint8(sDaysRemaining) << uint8(1);

	} while (dbCommand->MoveNext());

	result.put(offset, bCount); // set count now that the result set's been read

	return true;
}

int8 CDBAgent::SendLetter(string & strSenderID, string & strRecipientID, string & strSubject, string & strMessage, uint8 bType, _ITEM_DATA * pItem, int32 nCoins)
{
	uint64 nSerialNum = 0;
	uint32 nItemID = 0;
	uint16 sCount = 0, sDurability = 0;
	uint32 nExpirationTime = 0;
	int8 bRet = 0;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return 0;

	// This is a little bit redundant, but best to be sure.
	if (bType == 2 
		&& pItem != nullptr)
	{
		nItemID = pItem->nNum;
		sCount = pItem->sCount;
		sDurability = pItem->sDuration;
		nSerialNum = pItem->nSerialNum;
		nExpirationTime = pItem->nExpirationTime;
	}

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strSenderID.c_str(), strSenderID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strRecipientID.c_str(), strRecipientID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strSubject.c_str(), strSubject.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strMessage.c_str(), strMessage.length());

	// MSSQL uses signed types.
	if (!dbCommand->Execute(string_format(_T("{? = CALL MAIL_BOX_SEND(?, ?, ?, ?, %d, %d, %d, %d, " I64FMTD ",%d, %d)}"), 
		bType, nItemID, sCount, sDurability, nSerialNum,nExpirationTime, nCoins)))
	{
		ReportSQLError(m_GameDB->GetError());
		return 0;
	}

	return bRet;
}

bool CDBAgent::ReadLetter(string & strCharID, uint32 nLetterID, string & strMessage)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL MAIL_BOX_READ(?, %d)}"), nLetterID)))
	{
		ReportSQLError(m_GameDB->GetError());
		return false;
	}

	if (!dbCommand->hasData())
		return false;

	dbCommand->FetchString(1, strMessage);
	return true;
}

int8 CDBAgent::GetItemFromLetter(string & strCharID, uint32 nLetterID, uint32 & nItemID, uint16 & sCount, uint16 & sDurability, uint32 & nCoins, uint64 & nSerialNum, uint32 & nExpirationTime)
{
	int8 bRet = -1; // error
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return bRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{? = CALL MAIL_BOX_GET_ITEM(?, %d)}"), nLetterID)))
		ReportSQLError(m_GameDB->GetError());

	if (dbCommand->hasData())
	{
		dbCommand->FetchUInt32(1, nItemID);
		dbCommand->FetchUInt16(2, sCount);
		dbCommand->FetchUInt16(3, sDurability);
		dbCommand->FetchUInt32(4, nCoins);
		dbCommand->FetchUInt64(5, nSerialNum);
		dbCommand->FetchUInt32(6, nExpirationTime);
	}

	return bRet=1;
}

void CDBAgent::DeleteLetter(string & strCharID, uint32 nLetterID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	// NOTE: The official implementation passes all 5 letter IDs.
	if (!dbCommand->Execute(string_format(_T("UPDATE MAIL_BOX SET bDeleted = 1 WHERE nLetterID = %d AND strRecipientID = ?"), nLetterID)))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::ResendLetter(string & strCharID, uint32 nLetterID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	// NOTE: The official implementation passes all 5 letter IDs.
	if (!dbCommand->Execute(string_format(_T("UPDATE MAIL_BOX SET bStatus = 1 WHERE nLetterID = %d AND strRecipientID = ?"), nLetterID)))
		ReportSQLError(m_GameDB->GetError());
}

/**
* @brief	Updates the election status.
*
* @param	byType  	Election status.
* @param	byNation	Electoral nation.
*/
void CDBAgent::UpdateElectionStatus(uint8 byType, uint8 byNation)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("{CALL KING_UPDATE_ELECTION_STATUS(%d, %d)}"), byType, byNation)))
		ReportSQLError(m_GameDB->GetError());
}

/**
* @brief	Updates the election list.
*
* @param	byDBType  	Procedure-specific database action.
* 						If 1, insert. If 2, delete.
* @param	byType	  	Flag to specify what the user's in the election list for (election, impeachment, and thereof).
* @param	byNation  	Electoral nation.
* @param	sKnights  	The nominee's clan ID.
* @param	nAmount		Vote count.
* @param	strNominee	The nominee's name.
*/
void CDBAgent::UpdateElectionList(uint8 byDBType, uint8 byType, uint8 byNation, uint16 sKnights, uint32 nAmount, string & strNominee, CUser * pUser)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr || pUser == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strAccountID.c_str(), pUser->m_strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strNominee.c_str(), strNominee.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KING_ELECTION_PROC(?, ?, %d, ?, %d)}"), pUser->GetNation(), sRet)))
		ReportSQLError(m_GameDB->GetError());

	Packet result(WIZ_KING, uint8(1));
	result << byType << uint8(2) << sRet;
	pUser->Send(&result);
}

int16 CDBAgent::UpdateElectionProc(CUser *pUser)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr || pUser == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strAccountID.c_str(), pUser->m_strAccountID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->GetName().c_str(), pUser->GetName().length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KING_ELECTION_PROC(?, ?, %d, ?, 0)}"),pUser->GetNation())))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

void CDBAgent::SendUDP_ElectionStatus(uint8 m_byType)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;
	
	if (!dbCommand->Execute(string_format(_T("UPDATE KING_SYSTEM SET byType = %d"), m_byType)))
	ReportSQLError(m_GameDB->GetError());
}

bool CDBAgent::GetElectionResult(uint8 byNation, CKingSystem *KingSystem)
{
	if (KingSystem == nullptr)
		return false;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

		if (!dbCommand->Execute(string_format(_T("{CALL KING_ELECTION_FINISH(%d)}"),byNation)))
		ReportSQLError(m_GameDB->GetError());

		if (!dbCommand->hasData())
		return false;

		

		

		dbCommand->FetchString(1, KingSystem->m_strOldKingName);
		dbCommand->FetchString(2, KingSystem->m_strKingName);
		dbCommand->FetchUInt16(3, KingSystem->winPercent);
		
		return true;
}

/**
* @brief	Nominates/recommends strNominee as King.
*
* @param	strNominator	The nominator.
* @param	strNominee  	The nominee.
* @param	byNation		Their nation.
*
* @return	.
*/
int16 CDBAgent::UpdateCandidacyRecommend(std::string & strNominator, std::string & strNominee, uint8 byNation)
{
	int16 sRet = -1;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return sRet;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &sRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strNominee.c_str(), strNominee.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strNominator.c_str(), strNominator.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL KING_CANDIDACY_RECOMMEND(?, ?, %d, 0)}"), byNation)))
		ReportSQLError(m_GameDB->GetError());

	return sRet;
}

/**
* @brief	Updates the candidacy notice board.
*
* @param	strCharID	Candidate's name.
* @param	byNation 	Candidate's nation.
* @param	strNotice	The notice.
*/
void CDBAgent::UpdateCandidacyNoticeBoard(string & strCharID, uint8 byNation, string & strNotice)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	// Field is 1024 bytes in the database.
	uint8 strBinaryNotice[1024] = {0};
	uint16 sNoticeLen = strNotice.length();

	// This shouldn't happen, but... in the horribly unlikely event it does... we're ready.
	if (sNoticeLen > sizeof(strBinaryNotice))
		sNoticeLen = sizeof(strBinaryNotice);

	memcpy(strBinaryNotice, strNotice.c_str(), sNoticeLen);

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, strBinaryNotice, sizeof(strBinaryNotice));

	if (!dbCommand->Execute(string_format(_T("{CALL KING_CANDIDACY_NOTICE_BOARD_PROC(?, %d, %d, ?)}"), 
		sNoticeLen, byNation)))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::UpdateNoahOrExpEvent(uint8 byType, uint8 byNation, uint8 byAmount, uint8 byDay, uint8 byHour, uint8 byMinute, uint16 sDuration)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("{CALL KING_UPDATE_NOAH_OR_EXP_EVENT(%d, %d, %d, %d, %d, %d, %d)}"), 
		byType, byNation, byAmount, byDay, byHour, byMinute, sDuration)))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::InsertPrizeEvent(uint8 byType, uint8 byNation, uint32 nCoins, std::string & strCharID)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strCharID.c_str(), strCharID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL KING_INSERT_PRIZE_EVENT(%d, %d, %d, ?)}"), 
		byType, byNation, nCoins)))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::InsertTaxEvent(uint8 KingNationTax, uint8 Nation, uint32 TerritoryTax)
{	
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (TerritoryTax > 0)
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE KING_SYSTEM SET nTerritoryTax -= %d WHERE byNation = %d"), TerritoryTax, Nation)))
		ReportSQLError(m_GameDB->GetError());
	}
	else
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE KING_SYSTEM SET byTerritoryTariff = %d WHERE byNation = %d"), KingNationTax, Nation)))
		ReportSQLError(m_GameDB->GetError());
	}
}

void CDBAgent::InsertTaxUpEvent(uint8 Nation, uint32 TerritoryTax)
{	
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;
	if (Nation == ZONE_ELMORAD || Nation == ZONE_KARUS)
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE KING_SYSTEM SET nTerritoryTax += %d WHERE byNation = %d"), TerritoryTax, Nation)))
		ReportSQLError(m_GameDB->GetError());
	}else if(Nation == ZONE_ELMORAD_ESLANT || Nation == ZONE_KARUS_ESLANT)
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE KING_SYSTEM SET nNationalTreasury += %d WHERE byNation = %d"), TerritoryTax, (Nation - 10))))
		ReportSQLError(m_GameDB->GetError());
	}
	else if (Nation == ZONE_MORADON)
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS_SIEGE_WARFARE SET nMoradonTax += %d WHERE sCastleIndex = 1"), TerritoryTax)))
		ReportSQLError(m_GameDB->GetError());
	}
	else if (Nation == ZONE_DELOS)
	{
		if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS_SIEGE_WARFARE SET nDellosTax += %d WHERE sCastleIndex = 1"), TerritoryTax)))
		ReportSQLError(m_GameDB->GetError());
	}
}
/**
* @brief	Resets the monthly NP total accumulated in the last month.
*/
void CDBAgent::ResetLoyaltyMonthly()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(_T("{CALL RESET_LOYALTY_MONTHLY}")))
		ReportSQLError(m_GameDB->GetError());
}

/**
* @brief	Clears the remaining users who were connected to this server
from the logged in user list that may still be there as the 
result of an improper shutdown.
*/
void CDBAgent::ClearRemainUsers()
{
	_ZONE_SERVERINFO * pInfo = g_pMain->m_ServerArray.GetData(g_pMain->m_nServerNo);
	if (pInfo == nullptr)
		return;

	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pInfo->strServerIP.c_str(), pInfo->strServerIP.length());
	if (!dbCommand->Execute(_T("{CALL CLEAR_REMAIN_USERS(?)}")))
		ReportSQLError(m_AccountDB->GetError());
}

void CDBAgent::InsertUserDailyOp(_USER_DAILY_OP * pUserDailyOp)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUserDailyOp->strUserId.c_str(), pUserDailyOp->strUserId.length());
	if (!dbCommand->Execute(string_format(_T("{CALL INSERT_USER_DAILY_OP(?, %d, %d, %d, %d, %d, %d, %d, %d)}"), 
		pUserDailyOp->ChaosMapTime, pUserDailyOp->UserRankRewardTime, pUserDailyOp->PersonalRankRewardTime, pUserDailyOp->KingWingTime)))
		ReportSQLError(m_GameDB->GetError());	
}

void CDBAgent::UpdateUserDailyOp(std::string strUserId, uint8 type, int32 sUnixTime)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strUserId.c_str(), strUserId.length());
	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_USER_DAILY_OP(?, %d, %d)}"), type, sUnixTime)))
		ReportSQLError(m_GameDB->GetError());	
}

void CDBAgent::UpdateRanks()
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(_T("{CALL UPDATE_RANKS}")))
		ReportSQLError(m_GameDB->GetError());
}

int8 CDBAgent::NationTransfer(std::string strAccountID, uint16 Race1, uint16 Race2, uint16 Race3, uint16 Race4)
{
	int8 bRet = 3;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(string_format(_T("{? = CALL ACCOUNT_NATION_TRANSFER(?,%d,%d,%d,%d)}"),Race1,Race2,Race3,Race4)))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}


int8 CDBAgent::YanChar(std::string strAccountID)
{
	int8 bRet = 0;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(_T("{? = CALL ACCOUNT_NATION_CHAR_COUNT(?)}")))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}


int8 CDBAgent::YanCharClan(std::string strAccountID)
{
	int8 bRet = 0;
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return false;

	dbCommand->AddParameter(SQL_PARAM_OUTPUT, &bRet);
	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());

	if (!dbCommand->Execute(_T("{? = CALL ACCOUNT_NATION_CHAR_CLAN(?)}")))
		ReportSQLError(m_GameDB->GetError());

	return bRet;
}

/**
* @brief	Change knight cash is account.
*
* @param	strAccountID	Character's Account name.
* @param	KnightCash		Added knight cash amount
*/
void CDBAgent::UpdateAccountKnightCash(string & strAccountID, uint32 KnightCash)
{
	unique_ptr<OdbcCommand> dbCommand(m_AccountDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, strAccountID.c_str(), strAccountID.length());
	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_KNIGHT_CASH (?, %d)}"), KnightCash)))
		ReportSQLError(m_AccountDB->GetError());	
}

void CDBAgent::GetClanUserData(uint16 sClanID, _KNIGHTS_USER & p)
{
	
	_KNIGHTS_USER * pUser = &p;

	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->strUserName.c_str(), pUser->strUserName.length());
	if (!dbCommand->Execute(string_format(_T("SELECT Fame, [Level], Class, strMemo, dtUpdateClan FROM USERDATA WHERE Knights = %d AND strUserID = ?"), sClanID)))
	{
		ReportSQLError(m_GameDB->GetError());
		return;
	}

	if (!dbCommand->hasData())
		return;
	
		 int i = 1;
		 
		 dbCommand->FetchByte(i++,pUser->m_sFame);
		 dbCommand->FetchByte(i++,pUser->m_bLevel);
		 dbCommand->FetchUInt16(i++, pUser->m_sClass);
		 dbCommand->FetchString(i++, pUser->strUserMemo);
		 dbCommand->FetchUInt32(i++, pUser->m_lastLogined);

		 return;

}


void CDBAgent::UpdateSiege(int16 m_sCastleIndex, int16 m_sMasterKnights, int16 m_bySiegeType, int16 m_byWarDay, int16 m_byWarTime, int16 m_byWarMinute)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (!dbCommand->Execute(string_format(_T("{CALL UPDATE_SIEGE (%d, %d, %d, %d, %d, %d)}"), m_sCastleIndex, m_sMasterKnights, m_bySiegeType, m_byWarDay, m_byWarTime, m_byWarMinute)))
		ReportSQLError(m_AccountDB->GetError());	
}

void CDBAgent::UpdateSiegeTax(uint8 Zone, int16 ZoneTarrif)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;
	if (Zone == ZONE_DELOS)
	{
	if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS_SIEGE_WARFARE SET sDellosTariff = %d"), ZoneTarrif)))
		ReportSQLError(m_GameDB->GetError());
	}
	else if (Zone == ZONE_MORADON)
	{
	if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS_SIEGE_WARFARE SET sMoradonTariff = %d"), ZoneTarrif)))
		ReportSQLError(m_GameDB->GetError());
	}
	else if (Zone == 0)
	{
	if (!dbCommand->Execute(string_format(_T("UPDATE KNIGHTS_SIEGE_WARFARE SET nDungeonCharge = 0, nMoradonTax = 0, nDellosTax = 0"))))
		ReportSQLError(m_GameDB->GetError());
	}
}

void CDBAgent::InsertRepurchase(uint32 nItemID, time_t RepTime, uint64 nSerial, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (pUser == nullptr
		|| pUser->m_bLogout)
		return;

	ByteBuffer rSerial;
	rSerial << nSerial;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());
	dbCommand->AddParameter(SQL_PARAM_INPUT, (char *)rSerial.contents(), rSerial.size(), SQL_BINARY);

	if (!dbCommand->Execute(string_format(_T("{CALL INSERT_REPURCHASE(?, %d, %d, ?)}"), nItemID, RepTime)))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::DeleteRepurchase(uint32 nItemID, time_t RepTime, CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (pUser == nullptr
		|| pUser->m_bLogout)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length()); 

	if (!dbCommand->Execute(string_format(_T("DELETE FROM REPURCHASE_ITEMS WHERE strUserID = ? and nItemID = %d and nRepTime = %d"), nItemID, RepTime)))
		ReportSQLError(m_GameDB->GetError());
}

void CDBAgent::LoadRepurchase(CUser *pUser)
{
	unique_ptr<OdbcCommand> dbCommand(m_GameDB->CreateCommand());
	if (dbCommand.get() == nullptr)
		return;

	if (pUser == nullptr
		|| pUser->m_bLogout)
		return;

	dbCommand->AddParameter(SQL_PARAM_INPUT, pUser->m_strUserID.c_str(), pUser->m_strUserID.length());

	if (!dbCommand->Execute(_T("SELECT nItemID, nRepTime FROM REPURCHASE_ITEMS WHERE strUserID = ? ORDER BY nRepTime ASC")))
		ReportSQLError(m_GameDB->GetError());

	pUser->m_RepurchaseMap.clear();

	uint32 rCount = 1, rTime, rItemID;;
	if (dbCommand->hasData())
	{
		do
		{
			dbCommand->FetchUInt32(1, rItemID);
			dbCommand->FetchUInt32(2, rTime);

			if (rTime <= UNIXTIME)
			{
				DeleteRepurchase(rItemID, rTime, pUser);
				continue;
			}

			_ITEM_REPURCHASE * rItem = new _ITEM_REPURCHASE;

			rItem->nNum = rItemID;
			rItem->tRepTime = rTime;

			pUser->m_RepurchaseMap.insert(std::make_pair(rCount, rItem));
			rCount++;
		} while (dbCommand->MoveNext());
	}
}

