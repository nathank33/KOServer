#include "stdafx.h"
#include "DBAgent.h"

using std::string;

#define ITEM_SEAL_PRICE 1000000

enum
{
	SEAL_TYPE_SEAL		= 1,
	SEAL_TYPE_UNSEAL	= 2,
	SEAL_TYPE_KROWAZ	= 3,
	SEAL_TYPE_OLD_ITEM	= 4
};

enum SealErrorCodes
{
	SealErrorNone			= 0, // no error, success!
	SealErrorFailed			= 2, // "Seal Failed."
	SealErrorNeedCoins		= 3, // "Not enough coins."
	SealErrorInvalidCode	= 4, // "Invalid Citizen Registry Number" (i.e. invalid code/password)
	SealErrorPremiumOnly	= 5, // "Only available to premium users"
	SealErrorFailed2		= 6, // "Seal Failed."
	SealErrorTooSoon		= 7, // "Please try again. You may not repeat this function instantly."
};

/**
* @brief	Packet handler for the item sealing system.
*
* @param	pkt	The packet.
*/
void CUser::ItemSealProcess(Packet & pkt)
{
	if (isDead() 
		|| isTrading() 
		|| isMerchanting() 
		|| isStoreOpen() 
		|| isSellingMerchant() 
		|| isBuyingMerchant() 
		|| isMining() 
		|| m_bMerchantStatex)
		return;

	// Seal type
	uint8 opcode = pkt.read<uint8>();

	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_SEAL));
	result << opcode;

	switch (opcode)
	{
		// Used when sealing an item.
	case SEAL_TYPE_SEAL:
		{
			string strPasswd;
			uint32 nItemID; 
			int16 unk0; // set to -1 in this case
			uint8 bSrcPos, bResponse = SealErrorNone;
			pkt >> unk0 >> nItemID >> bSrcPos >> strPasswd;

			/* 
			Most of these checks are handled client-side, so we shouldn't need to provide error messages.
			Also, item sealing requires certain premium types (gold, platinum, etc) - need to double-check 
			these before implementing this check.
			*/

			// is this a valid position? (need to check if it can be taken from new slots)
			if (bSrcPos >= HAVE_MAX 
				// does the item exist where the client says it does?
					|| GetItem(SLOT_MAX + bSrcPos)->nNum != nItemID
					// i ain't be allowin' no stealth items to be sealed!
					|| GetItem(SLOT_MAX + bSrcPos)->nSerialNum == 0
					|| GetItem(SLOT_MAX + bSrcPos)->nExpirationTime > 0)
					bResponse = SealErrorFailed;
			// is the password valid by client limits?
			else if (strPasswd.empty() || strPasswd.length() > 8)
				bResponse = SealErrorInvalidCode;
			// do we have enough coins?
			else if (!hasCoins(ITEM_SEAL_PRICE))
				bResponse = SealErrorNeedCoins;

			_ITEM_TABLE* pItem = g_pMain->m_ItemtableArray.GetData(nItemID);

			if(pItem == nullptr)
				return;

			// If no error, pass it along to the database.
			if (bResponse == SealErrorNone)
			{
				result << nItemID << bSrcPos << strPasswd << bResponse;
				g_pMain->AddDatabaseRequest(result, this);
			}
			// If there's an error, tell the client.
			// From memory though, there was no need -- it handled all of these conditions itself
			// so there was no need to differentiate (just ignore the packet). Need to check this.
			else 
			{
				result << bResponse;
				Send(&result);
			}
		} break;

		// Used when unsealing an item.
	case SEAL_TYPE_UNSEAL:
		{
			string strPasswd;
			uint32 nItemID; 
			int16 unk0; // set to -1 in this case
			uint8 bSrcPos, bResponse = SealErrorNone;
			pkt >> unk0 >> nItemID >> bSrcPos >> strPasswd;

			if (bSrcPos >= HAVE_MAX
				|| GetItem(SLOT_MAX+bSrcPos)->bFlag != ITEM_FLAG_SEALED
				|| GetItem(SLOT_MAX+bSrcPos)->nNum != nItemID)
				bResponse = SealErrorFailed;
			else if (strPasswd.empty() || strPasswd.length() > 8)
				bResponse = SealErrorInvalidCode;

			// If no error, pass it along to the database.
			if (bResponse == SealErrorNone)
			{
				result << nItemID << bSrcPos << strPasswd << bResponse;
				g_pMain->AddDatabaseRequest(result, this);
			}
			// If there's an error, tell the client.
			// From memory though, there was no need -- it handled all of these conditions itself
			// so there was no need to differentiate (just ignore the packet). Need to check this.
			else
			{
				result << bResponse;
				Send(&result);
			}
		} break;

		// Used when binding a Krowaz item (used to take it from not bound -> bound)
	case SEAL_TYPE_KROWAZ:
		{
			string strPasswd = "0"; //Dummy, not actually used.
			uint32 nItemID;
			uint8 bSrcPos = 0 , unk3, bResponse = SealErrorNone;
			uint16 unk1, unk2;
			pkt >> unk1 >> nItemID >> bSrcPos >> unk3 >> unk2;

			if (bSrcPos >= HAVE_MAX
				|| GetItem(SLOT_MAX+bSrcPos)->bFlag != ITEM_FLAG_NONE
				|| GetItem(SLOT_MAX+bSrcPos)->nNum != nItemID)
				bResponse = SealErrorFailed;

			if (bResponse == SealErrorNone)
			{
				result << nItemID << bSrcPos << strPasswd << bResponse;
				g_pMain->AddDatabaseRequest(result, this);
			}
		} 
		break;

		// Used when binding a Old item (used to take it from bound -> not bound)
		case SEAL_TYPE_OLD_ITEM:
		{
			string strPasswd, strPasswdA;
			uint32 nItemID; 
			int16 unk0; 
			uint8 bSrcPos, bResponse = SealErrorNone;
			pkt >> unk0 >> nItemID >> bSrcPos >> strPasswd;

			
			if (bSrcPos >= HAVE_MAX
				|| GetItem(SLOT_MAX+bSrcPos)->nNum != nItemID)
				bResponse = SealErrorFailed;

			if (bResponse == SealErrorNone)
			{
				_ITEM_TABLE * pTable = g_pMain->m_ItemtableArray.GetData(nItemID);
				if (pTable == nullptr)
					return;

				if (pTable->m_bKind == 93)
					RobItem(810890000, 10);
				else if (pTable->m_bKind >= 210 && pTable->m_bKind <= 240)
					RobItem(810890000, 70);

				result << nItemID << bSrcPos << strPasswd << bResponse;
				g_pMain->AddDatabaseRequest(result, this);
			}
			else
			{
				result << bResponse;
				Send(&result);
			}
		} 
		break;
	}
}

void CUser::SealItem(uint8 bSealType, uint8 bSrcPos)
{
	_ITEM_DATA * pItem = GetItem(SLOT_MAX + bSrcPos);
	if (pItem == nullptr)
		return;

	switch (bSealType)
	{
	case SEAL_TYPE_SEAL:
		pItem->bFlag = ITEM_FLAG_SEALED;
		GoldLose(ITEM_SEAL_PRICE);
		break;

	case SEAL_TYPE_UNSEAL:
		pItem->bFlag = 0;
		break;

	case SEAL_TYPE_KROWAZ:
		pItem->bFlag = ITEM_FLAG_BOUND;
		break;

	case SEAL_TYPE_OLD_ITEM:
		pItem->bFlag = ITEM_FLAG_NOT_BOUND; 
		break;
	}
}

void CUser::SendCharacterSealInfo(Packet & pkt)
{
	Packet result(WIZ_ITEM_UPGRADE,uint8(9));
	uint32 SpecialID;
	uint64 Serial;
	pkt >> SpecialID;

	Serial = g_DBAgent.GetSerialByID(SpecialID);

	if(Serial == 0)
		goto fail_return;

	_CYPHERRING_DATA * pRingData = g_pMain->GetCypherRingPtr(Serial);

	if(pRingData == nullptr)
		goto fail_return;


	result << uint8(4) << uint8(1);
	g_DBAgent.LoadCharSeal(pRingData->UserName,result);
	Send(&result);
fail_return:
	result << uint8(4) << uint8(2);
	Send(&result);
}


void CUser::SendCharacterSealProcess()
{
	Packet result(WIZ_ITEM_UPGRADE,uint8(9));

	if(!CheckExistItem(800111000) && !CheckExistItem(800112000))
		return;


	std::string strCharID1, strCharID2, strCharID3, strCharID4;
	uint16 Class1 = 0, Class2 = 0, Class3 = 0, Class4 = 0;
	uint8 Level1 = 0, Level2 = 0, Level3 = 0, Level4 = 0;

	g_DBAgent.GetAllCharID(GetAccountName(), strCharID1, strCharID2, strCharID3, strCharID4);

	for (int i = 1; i < 4; i++)
{
	if(strCharID1.empty())
	{
		strCharID1 = strCharID2;
		strCharID2.clear();
	}

	if(strCharID2.empty())
	{
		strCharID2 = strCharID3;
		strCharID3.clear();
	}

	if(strCharID3.empty())
	{
		strCharID3 = strCharID4;
		strCharID4.clear();
	}
}
	


	if(!strCharID1.empty())
	Class1 = g_DBAgent.LoadAccountNTS(strCharID1);
	if(!strCharID2.empty())
	Class2 = g_DBAgent.LoadAccountNTS(strCharID2);
	if(!strCharID3.empty())
	Class3 = g_DBAgent.LoadAccountNTS(strCharID3);
	if(!strCharID4.empty())
	Class4 = g_DBAgent.LoadAccountNTS(strCharID4);

	if(!strCharID1.empty())
	Level1 = g_DBAgent.LoadCharLevel(strCharID1);
	if(!strCharID2.empty())
	Level2 = g_DBAgent.LoadCharLevel(strCharID2);
	if(!strCharID3.empty())
	Level3 = g_DBAgent.LoadCharLevel(strCharID3);
	if(!strCharID4.empty())
	Level4 = g_DBAgent.LoadCharLevel(strCharID4);
	result.DByte();
	result << uint8(1) << uint8(1);

	for (int a = 0; a < 4; a++)
	{
		if(a == 0)
		{
			if (strCharID1.empty())
				continue;

			result << strCharID1 << uint8(1) << Class1 << Level1 << uint8(1) << uint32(1) << uint8(1);
			result << uint64(0) << uint64(0) << uint64(0) << uint64(0) << uint64(0) << uint64(0);

		}else if(a == 1)
		{
			if (strCharID2.empty())
				continue;

			result << strCharID2 << uint8(1) << Class2 << Level2 << uint8(1) << uint32(1) << uint8(1);
			result << uint64(0) << uint64(0) << uint64(0) << uint64(0) << uint64(0) << uint64(0);

		}else if(a == 2)
		{
			if (strCharID3.empty())
				continue;

			result << strCharID3 << uint8(1) << Class3 << Level3 << uint8(1) << uint32(1) << uint8(1);
			result << uint64(0) << uint64(0) << uint64(0) << uint64(0) << uint64(0) << uint64(0);

		}else if(a == 3)
		{
			if (strCharID4.empty())
				continue;

			result << strCharID4 << uint8(1) << Class4 << Level4 << uint8(1) << uint32(1) << uint8(1);
			result << uint64(0) << uint64(0) << uint64(0) << uint64(0) << uint64(0) << uint64(0);
		}

	}

	Send(&result);
}
/**
* @brief	Packet handler for the character sealing system.
*
* @param	pkt	The packet.
*/
void CUser::CharacterSealProcess(Packet & pkt)
{
	uint8 command = pkt.read<uint8>();
	
	if (isDead() 
		|| !isInGame()
		|| isTrading() 
		|| isMerchanting() 
		|| isStoreOpen() 
		|| isSellingMerchant() 
		|| isBuyingMerchant() 
		|| isMining() 
		|| m_bMerchantStatex)
		return;

	switch (command)
	{
	case 1:
		SendCharacterSealProcess();
		break;
	case 2:
		CharacterGetSealed(pkt);
		break;
	case 3:
		CharacterGetUnSealed(pkt);
		break;
	case 4:
		SendCharacterSealInfo(pkt);
		break;
	}



	
}

void CUser::CharacterGetSealed(Packet & pkt)
{
	Packet result(WIZ_ITEM_UPGRADE, uint8(9));
	uint16 UnkNow1;
	uint32 ItemNum;
	uint8 Slot, ErrorCode = 0;
	std::string strUserID, strPasswd;
	pkt.DByte();
	pkt >> UnkNow1 >> Slot >> ItemNum;

	pkt >> strUserID >> strPasswd;

	if (strPasswd.empty() || strPasswd.length() > 8
		|| strUserID.empty())
	{
		result << uint8(2) << uint8(2);// Character seal failed.	14032
		Send(&result);
		return;
	}

	_ITEM_DATA * pItem = GetItem(SLOT_MAX + Slot);

	if(ItemNum != 800111000 
		|| pItem == nullptr
		|| pItem->nNum != ItemNum)
	{
		result << uint8(2) << uint8(2);// Character seal failed.	14032
		Send(&result);
		return;
	}

	uint8 YanCharimi = g_DBAgent.LoadYanCharHAS(GetAccountName(),strUserID);

	if (YanCharimi == 0)
	{
		result << uint8(2) << uint8(2);// Character seal failed.	14032
		Send(&result);
		return;
	}
	
	uint64 Serial = g_pMain->GenerateItemSerial();

	_CYPHERRING_DATA * pData = new _CYPHERRING_DATA;
		pData->m_Serial = Serial;
		pData->UserName = strUserID;
		pData->iExp = g_DBAgent.LoadCharExp(strUserID);
		pData->bRace = g_DBAgent.LoadCharRace(strUserID);
		pData->sClass = uint8(g_DBAgent.LoadAccountNTS(strUserID));
		pData->bLevel = g_DBAgent.LoadCharLevel(strUserID);
		g_pMain->LastPetID++;
		pData->ID = g_pMain->LastPetID;

		uint8 DBResult = g_DBAgent.InsertCypherRingData(Serial,pData, strPasswd);

	if(DBResult != 1)
	{
		result << uint8(2) << uint8(0) << uint8(20 + DBResult);
		Send(&result);
		return;
	}

	g_pMain->m_CypherRingArray.insert(std::make_pair(pData->m_Serial, pData));

		pItem->nSerialNum = Serial;
		pItem->nNum = 800112000;

	// Character seal successful. 14031
	// When a bound item is equipped or stored on a character it cannot be sealed. 14044
	// When a rental item is equipped or stored on a character it cannot be sealed. 14045
	

	// 2 0 20 >> There is no password set for the VIP Vault. Visit the Inn Hostess to set the VIP Vault password.
	// 2 0 22 >> Invalid Citizen Registry Number.


	result.DByte();
	result << uint8(2) << uint8(1) << Slot << uint32(pItem->nNum)  << uint32(pData->ID) << pData->UserName << uint8(pData->sClass) << uint8(pData->bLevel) << uint16((pData->iExp * 10000) / g_pMain->GetExpByLevel(pData->bLevel)) << uint16(pData->bRace);
	Send(&result);

	// 2 2 hatalar

	// uint16 -4 14044
	// uint16 -5 14045
	// nothing 14032
}


void CUser::CharacterGetUnSealed(Packet & pkt)
{
	Packet result(WIZ_ITEM_UPGRADE,uint8(9));
	uint16 unkNown;
	uint32 ItemID;
	uint8 Slot, CharacterSlot;

	result << uint8(3);

	if(!CheckExistItem(800112000))
		goto fail_return;

	pkt >> unkNown >> Slot >> ItemID >> CharacterSlot;

	if(Slot >= HAVE_MAX || ItemID != 800112000)
		goto fail_return;

	_ITEM_DATA * pItem = GetItem(Slot + SLOT_MAX);
	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(ItemID);

	if (pItem == nullptr || pTable == nullptr)
		goto fail_return;

	_CYPHERRING_DATA * pRingData = g_pMain->GetCypherRingPtr(pItem->nSerialNum);

	if(pRingData == nullptr)
		goto fail_return;

	if(pRingData->m_Serial != pItem->nSerialNum)
		goto fail_return;	

	uint8 YanChar = g_DBAgent.YanChar(GetAccountName());
	uint8 Nation = g_DBAgent.LoadCharNation(pRingData->UserName);

	if(YanChar > 3 || YanChar < 1 || Nation < 1 || Nation > 2 || Nation != GetNation())
		goto fail_return;

	uint8 DBResult = g_DBAgent.InsertCypherRingChar(GetAccountName(), pItem->nSerialNum);

	if(DBResult != 1)
		goto fail_return;

	g_pMain->m_CypherRingArray.erase(pItem->nSerialNum);
	RobItem(SLOT_MAX + Slot,pTable,1);

	result << uint8(1);

fail_return:
	result << uint8(2);
	Send(&result);
}