#include "stdafx.h"
#include <boost\foreach.hpp>	  

using std::string;

enum MerchantOpenResponseCodes
{
	MERCHANT_OPEN_SUCCESS = 1,
	MERCHANT_OPEN_NO_SESSION = -1,
	MERCHANT_OPEN_DEAD = -2,
	MERCHANT_OPEN_TRADING = -3,
	MERCHANT_OPEN_MERCHANTING = -4,
	MERCHANT_OPEN_INVALID_ZONE = -5,
	MERCHANT_OPEN_SHOPPING = -6,
	MERCHANT_OPEN_UNDERLEVELED = 30
};

void CUser::MerchantProcess(Packet & pkt)
{
	 uint8 opcode = pkt.read<uint8>();

	if (isTrading() 
		|| isStoreOpen() 
		|| isDead())
		CancelMerchant();
	
	switch (opcode)
	{
	// Regular merchants
	case MERCHANT_OPEN: 
		MerchantOpen(); 
		break;

	case MERCHANT_CLOSE: 
		MerchantClose(); 
		break;

	case MERCHANT_ITEM_ADD: 
		MerchantItemAdd(pkt); 
		break;

	case MERCHANT_ITEM_CANCEL: 
		MerchantItemCancel(pkt); 
		break;

	case MERCHANT_ITEM_LIST: 
		MerchantItemList(pkt); 
		break;

	case MERCHANT_ITEM_BUY: 
		MerchantItemBuy(pkt); 
		break;

	case MERCHANT_INSERT: 
		MerchantInsert(pkt); 
		break;

	case MERCHANT_TRADE_CANCEL: 
		CancelMerchant(); 
		break;

		// Buying merchants
	case MERCHANT_BUY_OPEN: 
		BuyingMerchantOpen(pkt); 
		break;

	case MERCHANT_BUY_CLOSE: 
		BuyingMerchantClose(); 
		break;

	case MERCHANT_BUY_LIST: 
		BuyingMerchantList(pkt); 
		break;

	case MERCHANT_BUY_INSERT: 
		BuyingMerchantInsert(pkt); 
		break;

	case MERCHANT_BUY_BUY: // seeya!
		BuyingMerchantBuy(pkt); 
		break;

	case MERCHANT_MENISIA_LIST:
		OfficialList(pkt);
		break;
	}

	
}

void CUser::MerchantOpen()
{
	int16 errorCode = 0;
	if (isDead())
		errorCode = MERCHANT_OPEN_DEAD;
	else if (isStoreOpen())
		errorCode = MERCHANT_OPEN_SHOPPING;
	else if (isTrading())
		errorCode = MERCHANT_OPEN_TRADING;
	else if (GetZoneID() != ZONE_MORADON 
		|| isMining())
		errorCode = MERCHANT_OPEN_INVALID_ZONE;
	else if (GetLevel() < 20)
		errorCode = MERCHANT_OPEN_UNDERLEVELED;
	else if (isBuyingMerchant()){
	BuyingMerchantClose();
	return;
	}else if(uint32(m_bLastMerchantTime+55) > uint32(UNIXTIME))
		errorCode = MERCHANT_OPEN_TRADING;
	else if (isMerchanting() || m_bMerchantStatex)
		errorCode = MERCHANT_OPEN_MERCHANTING;
	else 
	{
		errorCode = MERCHANT_OPEN_SUCCESS;
		m_bMerchantStatex = 1;
		m_bLastMerchantTime = uint32(UNIXTIME);
	}


	Packet result(WIZ_MERCHANT, uint8(MERCHANT_OPEN));
	result << errorCode;
	Send(&result);

	// If we're already merchanting, user may be desynced
	// so we need to close our current merchant first.
	if (errorCode == MERCHANT_OPEN_MERCHANTING)
	{
		MerchantClose();
	}
}

void CUser::MerchantClose()
{
	GiveMerchantItems(); // Give back to the user that which hasn't been sold, if any.
	
	m_bMerchantState = MERCHANT_STATE_NONE;

	m_bMerchantStatex = 0;

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_CLOSE));
	result << GetSocketID();
	SendToRegion(&result,nullptr,GetEventRoom());

	string errorMessage = string_format(_T("MERCHANT_CLOSE uId-%s- Z-%d- X-%d- Y-%d-"),
		GetName().c_str(),GetZoneID(), uint16(GetX()), uint16(GetZ()));
		g_pMain->WriteMerchantUserLogFile(errorMessage);
	
	_MERCHANT_LIST * pList = g_pMain->m_MerchantListArray.GetData(GetID());
	if(!pList)
		return;
	else
		g_pMain->m_MerchantListArray.DeleteData(GetID());
}

void CUser::MerchantItemAdd(Packet & pkt)
{
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_ADD));
	uint32 nGold, nItemID;
	uint16 sCount;
	uint16 bResult = 0;
	uint8 bSrcPos, // It sends the "actual" inventory slot (SLOT_MAX -> INVENTORY_MAX-SLOT_MAX), so need to allow for it. 
		bDstPos, 
		bMode; // Might be a flag for normal / "premium" merchant mode, once skills are implemented take another look at this.

	pkt >> nItemID >> sCount >> nGold >> bSrcPos >> bDstPos >> bMode;

	// TODO: Implement the possible error codes for these various error cases.
	if (bSrcPos >= HAVE_MAX
		|| bDstPos >= MAX_MERCH_ITEMS
		|| !m_bMerchantStatex
		|| isMerchanting())
		return;
	
	bSrcPos += SLOT_MAX;
	_ITEM_DATA *pSrcItem = GetItem(bSrcPos);

	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr)
		return;

	if	((nItemID >= ITEM_NO_TRADE && nItemID < ITEM_NO_TRADE_MAX) // Cannot be traded, sold or stored.
		|| pTable->m_bRace == RACE_UNTRADEABLE) // Cannot be traded or sold.
		goto fail_return;

	if (pSrcItem == nullptr)
		return;

	if (pSrcItem->nNum != nItemID
		|| pSrcItem->sCount < sCount
		|| pSrcItem->isRented()
		|| pSrcItem->isSealed()
		|| pSrcItem->isBound()
		|| pSrcItem->nExpirationTime != 0
		|| pSrcItem->isDuplicate())
		goto fail_return;

	_MERCH_DATA *pMerch = &m_arSellMerchantItems[bDstPos];

	if(pMerch == nullptr)
		return; 

	if (pSrcItem->IsSelling)
		goto fail_return;

	pMerch->nNum = nItemID;
	pMerch->nPrice = nGold;
	pMerch->sCount = sCount; // Selling Count
	pMerch->bCount = pSrcItem->sCount; // Original Count ( INVENTORY )
	pMerch->sDuration = pSrcItem->sDuration;
	pMerch->nSerialNum = pSrcItem->nSerialNum; // NOTE: Stackable items will have an issue with this.
	pMerch->bOriginalSlot = bSrcPos;
	pMerch->IsSoldOut = false;
	pSrcItem->IsSelling = true;
	bResult = 1;
	// Take the user's item.
	//memset(pSrcItem, 0, sizeof(_ITEM_DATA));

	result	<< bResult
		<< nItemID << sCount << pMerch->sDuration << nGold 
		<< bSrcPos << bDstPos;
	Send(&result);
	
fail_return:
	result  << bResult
		<< nItemID 
		<< sCount 
		<< pSrcItem->sDuration 
		<< nGold
		<< bSrcPos 
		<< bDstPos;
	Send(&result);
}
void CUser::MerchantItemCancel(Packet & pkt)
{
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_CANCEL));
	_MERCH_DATA *pMerch = nullptr;
	_ITEM_DATA *pItem = nullptr;

	if(isMerchanting() || !m_bMerchantStatex)
		return;

	uint8 bSrcPos = pkt.read<uint8>();

	/*	if (this == nullptr)
	result << int16(-1);*/
	// Invalid source position
	if (bSrcPos >= MAX_MERCH_ITEMS)
		result << int16(-2);
	// There's no item in that list..?
	else if ((pMerch = &m_arSellMerchantItems[bSrcPos])->nNum == 0)
		result << int16(-3);
	// Check to make sure we've got a valid stack
	else if (pMerch->bOriginalSlot > HAVE_MAX || (pItem = &m_sItemArray[pMerch->bOriginalSlot])->nNum != pMerch->nNum) 
		result << int16(-3); // custom error
	else
	{
		pItem->nNum = pMerch->nNum;
		pItem->sCount = pMerch->bCount;
		pItem->sDuration = pMerch->sDuration;
		pItem->IsSelling = false;
		pItem->nSerialNum = pMerch->nSerialNum; // NOTE: Stackable items will have an issue with this.



		string errorMessage = string_format(_T("MERCHANT_ITEM_CANCEL uId-%s- %d,%d,%d"),
		GetName().c_str(), pMerch->nNum, pMerch->bCount, pMerch->sDuration);
		g_pMain->WriteMerchantUserLogFile(errorMessage);

		memset(pMerch, 0, sizeof(pMerch));
		result << int16(1) << bSrcPos;

		
	}

	Send(&result);
}

void CUser::MerchantItemList(Packet & pkt)
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

	if (m_sMerchantsSocketID >= 0)
		RemoveFromMerchantLookers(); //This check should never be hit...

	uint16 uid = pkt.read<uint16>();

	if(uid == GetID())
		return;

	CUser *pMerchant = g_pMain->GetUserPtr(uid);
	CBot *pMerchantBot = g_pMain->m_arBotArray.GetData(uid);
	if ((pMerchant == nullptr
		|| !pMerchant->isMerchanting()) 
		&& (pMerchantBot == nullptr || !pMerchantBot->isMerchanting())
		|| pMerchant->m_bMerchantState != MERCHANT_STATE_SELLING)
		return;

	m_sMerchantsSocketID = uid;

	if(pMerchant != nullptr)
	{
		if (pMerchant->m_arMerchantLookers.size() != 0) {
		Packet result(WIZ_MERCHANT, uint8(35));
		result << uint8(7);
		Send(&result);
		return;
		}

		pMerchant->m_arMerchantLookers.push_front(GetSocketID());

		Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_LIST));
		result << uint16(1) << uint16(uid);
		for (int i = 0; i < MAX_MERCH_ITEMS; i++)
		{
			_MERCH_DATA *pMerch = &pMerchant->m_arSellMerchantItems[i];
			result	<< pMerch->nNum << pMerch->sCount
				<< pMerch->sDuration << pMerch->nPrice;
			pMerchant->SetMerchantSpecialItemData(pMerch,result);
		}
		Send(&result);
	}
	else if(pMerchantBot != nullptr)
	{
			if (pMerchantBot->m_arMerchantLookers.size() != 0) {
		Packet result(WIZ_MERCHANT, uint8(35));
		result << uint8(7);
		Send(&result);
		return;
		}
			pMerchantBot->m_arMerchantLookers.push_front(GetSocketID());
			Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_LIST));
			result << uint16(1) << uint16(uid);
			for (int i = 0; i < MAX_MERCH_ITEMS; i++)
			{
				_MERCH_DATA *pMerch = &pMerchantBot->m_arSellMerchantItems[i];
				result	<< pMerch->nNum << pMerch->sCount
					<< pMerch->sDuration << pMerch->nPrice << uint32(0);
			}
			Send(&result);
	}
}

void CUser::MerchantItemBuy(Packet & pkt)
{
	uint32 itemid, req_gold;
	uint16 item_count, leftover_count;
	uint8 item_slot, dest_slot;

	CUser *pMerchant = g_pMain->GetUserPtr(m_sMerchantsSocketID),
		*pUser = g_pMain->GetUserPtr(GetID());
	CBot *pMerchantBot = g_pMain->m_arBotArray.GetData(m_sMerchantsSocketID);
	if (pMerchant == nullptr && pMerchantBot == nullptr)
		return;

	pkt >> itemid >> item_count >> item_slot >> dest_slot;

	// Make sure the slots are correct and that we're actually buying at least 1 item.
	if (item_slot >= MAX_MERCH_ITEMS
		|| dest_slot >= HAVE_MAX
		|| item_count == 0)
		return;

	if (isDead() 
		|| isTrading() 
		|| isMerchanting() 
		|| isStoreOpen() 
		|| isSellingMerchant() 
		|| isBuyingMerchant() 
		|| isMining() 
		|| m_bMerchantStatex)
		return;


	_MERCH_DATA *pMerch;
	// Grab pointers to the items.
	if(m_sMerchantsSocketID > MAX_USER)
	pMerch = &pMerchantBot->m_arSellMerchantItems[item_slot];
	else
	pMerch = &pMerchant->m_arSellMerchantItems[item_slot];

	_ITEM_DATA *pItem = GetItem(SLOT_MAX + dest_slot);
	_ITEM_DATA *pItemRob;
	if(m_sMerchantsSocketID < MAX_USER)
	pItemRob = &pMerchant->m_sItemArray[pMerch->bOriginalSlot];



	// Make sure the merchant actually has that item in that slot
	// and that they have enough
	if (pMerch->nNum != itemid
		|| pMerch->sCount < item_count)
		return;

	// If it's not stackable, and we're specifying something other than 1
	// we really don't care to handle this request...
	_ITEM_TABLE *proto = g_pMain->GetItemPtr(itemid);
	if (proto == nullptr
		|| !proto->m_bCountable && item_count != 1)
		return;

	// Do we have enough coins?
	req_gold = pMerch->nPrice * item_count;
	if (m_iGold < req_gold)
		return;

	// If the slot's not empty
	if (pItem->nNum != 0 
		// and we already have an item that isn't the same item
			// or it's the same item but the item's not stackable...
				&& (pItem->nNum != itemid || !proto->m_bCountable))
				return;

	leftover_count = pMerch->sCount - item_count;
	if(m_sMerchantsSocketID < MAX_USER)
	pMerchant->GoldChange(GetSocketID(), req_gold);
	else
	GoldLose(req_gold);
	pItem->nNum = itemid;
	pItem->sCount += item_count;
	pItem->sDuration = pMerch->sDuration;
	pItem->nSerialNum = pMerch->nSerialNum;

	string errorMessage = string_format(_T("MERCHANT_ITEM_BUY buyerId-%s- sellerId-%s- Gold-%d- I-%d-, C-%d-"),
		GetName().c_str(),m_sMerchantsSocketID < MAX_USER ? pMerchant->GetName().c_str() : pMerchantBot->GetName().c_str(), req_gold, itemid, item_count);
		g_pMain->WriteMerchantUserLogFile(errorMessage);

	pMerch->sCount -= item_count;
	pMerch->bCount -= item_count;

	
	if (pMerch->sCount == 0 && pMerch->bCount == 0 && m_sMerchantsSocketID < MAX_USER)
		memset(pItemRob, 0, sizeof(_ITEM_DATA));
	else if(m_sMerchantsSocketID < MAX_USER)
		pItemRob->sCount = pMerch->sCount;

	SendStackChangeSpecial(itemid, pItem->sCount, pItem->sDuration, dest_slot, 
		(pItem->sCount == item_count),0,pItem); // is it a new item?

	if(m_sMerchantsSocketID < MAX_USER)
	pMerchant->SendStackChange(itemid, pMerch->bCount, pMerch->sDuration,
		pMerch->bOriginalSlot- SLOT_MAX);

	if (pMerch->sCount == 0 && pMerch->bCount == 0)
		memset(pMerch, 0, sizeof(_MERCH_DATA));
	else if (pMerch->sCount == 0) // Countable item protect.
		pMerch->IsSoldOut = true;

	
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_ITEM_PURCHASED));
	if(m_sMerchantsSocketID < MAX_USER)
	{
	result << itemid << GetName();
	pMerchant->Send(&result);
	}
	result.clear();

	result	<< uint8(MERCHANT_ITEM_BUY) << uint16(1)
		<< itemid << leftover_count
		<< item_slot << dest_slot;

	Send(&result);

	if (item_slot < 4 && leftover_count == 0)
	{
		result.Initialize(WIZ_MERCHANT_INOUT);
		result << uint8(2) << m_sMerchantsSocketID << uint8(1) << uint8(0) << item_slot;
		if(m_sMerchantsSocketID < MAX_USER)
		pMerchant->SendToRegion(&result,nullptr,pMerchant->GetEventRoom());
		else
		pMerchantBot->SendToRegion(&result);
	}

	if(m_sMerchantsSocketID < MAX_USER)
	g_pMain->MerchantUserInOutForMe(pMerchant);

	g_pMain->MerchantUserInOutForMe(pUser);

	int nItemsRemaining = 0;
	if(m_sMerchantsSocketID < MAX_USER)
	{
	for (int i = 0; i < MAX_MERCH_ITEMS; i++)
	{
		if (pMerchant->m_arSellMerchantItems[i].nNum != 0 && !pMerchant->m_arSellMerchantItems[i].IsSoldOut)
			nItemsRemaining++;
	}

	if (nItemsRemaining == 0)
		pMerchant->MerchantClose();
	}
	else
	{
		for (int i = 0; i < MAX_MERCH_ITEMS; i++)
	{
		if (pMerchantBot->m_arSellMerchantItems[i].nNum != 0 && !pMerchantBot->m_arSellMerchantItems[i].IsSoldOut)
			nItemsRemaining++;
	}

	if (nItemsRemaining == 0)
	{
		pMerchantBot->LastWarpTime = UNIXTIME + 60;

		pMerchantBot->m_bMerchantState = MERCHANT_STATE_NONE;
		pMerchantBot->m_bMerchantStatex = 0;

	Packet result35(WIZ_MERCHANT, uint8(MERCHANT_CLOSE));
	result35 << pMerchantBot->GetID();
	pMerchantBot->SendToRegion(&result35);
	}
	}
}			

void CUser::MerchantInsert(Packet & pkt)
{
	string advertMessage; // check here maybe to make sure they're not using it otherwise?
	pkt >> advertMessage;
	uint16 bResult = 0;
	if (advertMessage.size() > MAX_MERCH_MESSAGE)
		return;
	
	uint8 Count = 0, ListCount = 0, i = 1, TotalCount = 0;

	if (isDead() 
		|| isTrading() 
		|| isMerchanting() 
		|| isStoreOpen() 
		|| isSellingMerchant() 
		|| isBuyingMerchant() 
		|| isMining() 
		|| !m_bMerchantStatex)
		return;

	uint8 MerchantItemleri = 0;
		for (int i = 0; i < MAX_MERCH_ITEMS; i++)
	{
		if (m_arSellMerchantItems[i].nNum != 0)
			MerchantItemleri++;
	}

	if (MerchantItemleri == 0)
		return;

	m_bMerchantState = MERCHANT_STATE_SELLING;
	bResult = 1;

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_INSERT));	 

	if(isGM())
	{
		CBot * myBot = nullptr;
		uint16 ID = g_pMain->SpawnBot(3600,GetZoneID(),GetX(),GetY(),GetZ(),5,35);
			if(ID)
			{
				myBot = g_pMain->m_arBotArray.GetData(ID);
				if(myBot == nullptr)
					return;

				std::string asdasd = advertMessage;

				if(!asdasd.empty())
				myBot->MerchantChat = string_format("%s(Location:%d,%d)",asdasd.c_str(),myBot->GetSPosX()/10,myBot->GetSPosZ()/10);
				else
				myBot->MerchantChat.clear();
			

				myBot->m_iLoyalty = uint32(UNIXTIME) - 110;
				_MERCH_DATA * pMerch, * pMerchMe;
				for (int i = 0; i < MAX_MERCH_ITEMS; i++)
				{
					pMerch = &myBot->m_arSellMerchantItems[i];
					pMerchMe = &m_arSellMerchantItems[i];
					pMerch->nNum = pMerchMe->nNum;
					pMerch->nPrice = pMerchMe->nPrice;
					pMerch->sCount = pMerchMe->sCount; // Selling Count
					pMerch->bCount = pMerchMe->bCount; // Original Count ( INVENTORY )
					pMerch->sDuration = pMerchMe->sDuration;
					pMerch->nSerialNum = pMerchMe->nSerialNum; // NOTE: Stackable items will have an issue with this.
					pMerch->bOriginalSlot = pMerchMe->bOriginalSlot;
					pMerch->IsSoldOut = false;	
					bResult = 1;
				
				}
				MerchantClose();
				myBot->m_bPremiumMerchant = m_bPremiumMerchant;
				myBot->m_bMerchantState = MERCHANT_STATE_SELLING;

				 result << bResult << advertMessage << myBot->GetID()
				<< m_bPremiumMerchant; 
				for (int i = 0; i < MAX_MERCH_ITEMS; i++)
					result << myBot->m_arSellMerchantItems[i].nNum;
				
				if(bResult == 1)
					{
						_MERCHANT_LIST * pList = new _MERCHANT_LIST;

						for (int i = 0; i < MAX_MERCH_ITEMS; i++)
						{
		
						pList->ItemID[i] = myBot->m_arSellMerchantItems[i].nNum;
						pList->Price[i] = myBot->m_arSellMerchantItems[i].nPrice;
						pList->strUserID = myBot->GetID();
						pList->strUserName = myBot->m_strUserID;
						pList->Type = 0;

						}

						g_pMain->m_MerchantListArray.PutData(pList->strUserID, pList);
					}
				myBot->SendToRegion(&result);
				return;
			}
	}

	if(bResult == 1)
	{
		_MERCHANT_LIST * pList = new _MERCHANT_LIST;

		for (int i = 0; i < MAX_MERCH_ITEMS; i++)
		{
		
		pList->ItemID[i] = m_arSellMerchantItems[i].nNum;
		pList->Price[i] = m_arSellMerchantItems[i].nPrice;
		pList->strUserID = GetID();
		pList->strUserName = m_strUserID;
		pList->Type = 0;

		}

		g_pMain->m_MerchantListArray.PutData(pList->strUserID, pList);
	}
	 result << bResult << advertMessage << GetSocketID()
		<< m_bPremiumMerchant; 

	 string errorMessage = string_format(_T("MERCHANT_INSERT Msg-%s- uId-%s- %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"),
		advertMessage.c_str(),GetName().c_str(),m_arSellMerchantItems[0].nNum,m_arSellMerchantItems[1].nNum,m_arSellMerchantItems[2].nNum,m_arSellMerchantItems[3].nNum,
		m_arSellMerchantItems[4].nNum,m_arSellMerchantItems[5].nNum,m_arSellMerchantItems[6].nNum,m_arSellMerchantItems[7].nNum,m_arSellMerchantItems[8].nNum,m_arSellMerchantItems[9].nNum,
		m_arSellMerchantItems[10].nNum,m_arSellMerchantItems[11].nNum);

	g_pMain->WriteMerchantUserLogFile(errorMessage);

		for (int i = 0; i < MAX_MERCH_ITEMS; i++)
		result << m_arSellMerchantItems[i].nNum;
		
	SendToRegion(&result,nullptr,GetEventRoom());
}

void CUser::GiveMerchantItems()
{
	for (int i = 0; i < MAX_MERCH_ITEMS; i++)
	{
		_MERCH_DATA *pMerch = &m_arSellMerchantItems[i];
		if (pMerch->nNum == 0)
			continue;

		_ITEM_DATA *pItem = &m_sItemArray[pMerch->bOriginalSlot];
		pItem->nNum = pMerch->nNum;
		pItem->nSerialNum = pMerch->nSerialNum;
		pItem->sCount = pMerch->bCount;
		pItem->sDuration = pMerch->sDuration;
		pItem->IsSelling = false;
		// NOTE: Don't need to update the client, the client doesn't see any change.
	}

	// remove the items from the array now that they've been restored to the user
	memset(&m_arSellMerchantItems, 0, sizeof(m_arSellMerchantItems));
}

void CUser::CancelMerchant()
{
	if (m_sMerchantsSocketID < 0)
		return;
	CUser *pMerchant = g_pMain->GetUserPtr(GetID());
	if(pMerchant == nullptr)
		return;
	RemoveFromMerchantLookers();
	memset(&m_arSellMerchantItems, 0, sizeof(m_arSellMerchantItems));
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_TRADE_CANCEL));
	result << uint16(1);
	Send(&result);


}

void CUser::BuyingMerchantOpen(Packet & pkt)
{
	int16 errorCode = 0;
	if (isDead())
		errorCode = MERCHANT_OPEN_DEAD;
	else if (isStoreOpen())
		errorCode = MERCHANT_OPEN_SHOPPING;
	else if (isTrading())
		errorCode = MERCHANT_OPEN_TRADING;
	else if ((GetZoneID() != ZONE_MORADON && GetZoneID() != ZONE_MORADONM2) || isMining())
		errorCode = MERCHANT_OPEN_INVALID_ZONE;
	else if (GetLevel() < 20)
		errorCode = MERCHANT_OPEN_UNDERLEVELED;
	else if (isMerchanting() || m_bMerchantStatex)
		errorCode = MERCHANT_OPEN_MERCHANTING;
	else if(uint32(m_bLastMerchantTime+55) > uint32(UNIXTIME))
		errorCode = MERCHANT_OPEN_TRADING;
	else {
		errorCode = MERCHANT_OPEN_SUCCESS;
		m_bMerchantStatex = 1;
		m_bLastMerchantTime = uint32(UNIXTIME);
	}
	memset(&m_arBuyMerchantItems, 0, sizeof(m_arBuyMerchantItems));
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_OPEN));
	result << errorCode;
	Send(&result);

	if (errorCode == MERCHANT_OPEN_MERCHANTING)
		BuyingMerchantClose();
}

void CUser::BuyingMerchantClose()
{
	
	if (isMerchanting() && !isBuyingMerchant()){
		MerchantClose();
		return;
	}
	if (isMerchanting())
		m_bMerchantState = MERCHANT_STATE_NONE;
	else if (m_sMerchantsSocketID >= 0)
		RemoveFromMerchantLookers();
	else
		return;

	m_bMerchantStatex = 0;
	memset(&m_arBuyMerchantItems, 0, sizeof(m_arBuyMerchantItems));
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_CLOSE));
	result << GetSocketID();
	SendToRegion(&result,nullptr,GetEventRoom());
	
	_MERCHANT_LIST * pList = g_pMain->m_MerchantListArray.GetData(GetID());
	if(!pList)
		return;
	else
		g_pMain->m_MerchantListArray.DeleteData(GetID());
}

void CUser::BuyingMerchantInsert(Packet & pkt)
{
	uint8 amount_of_items;
	uint32 itemid, buying_price;
	uint32 totalamount = 0;
	uint16 item_count;
	_ITEM_TABLE *pItem = nullptr;
	uint8 bResult = 0;

	pkt >> amount_of_items;


		if (isDead() 
		|| isTrading() 
		|| isMerchanting() 
		|| isStoreOpen() 
		|| isSellingMerchant() 
		|| isBuyingMerchant() 
		|| isMining() 
		|| !m_bMerchantStatex)
		return;

	uint8 MerchantItemleri = 0;



	for (int i = 0; i < amount_of_items; i++)
	{
		pkt >> itemid >> item_count >> buying_price;
		pItem = g_pMain->m_ItemtableArray.GetData(itemid);
		if (pItem == nullptr){
			BuyingMerchantClose();
			return;
		}
		m_arBuyMerchantItems[i].nNum = itemid;
		m_arBuyMerchantItems[i].sCount = item_count;
		m_arBuyMerchantItems[i].nPrice = buying_price;
		m_arBuyMerchantItems[i].sDuration = pItem->m_sDuration;
		totalamount += buying_price;
		MerchantItemleri++;
		
	}
	
	if (MerchantItemleri == 0)
			return;

	if (!hasCoins(totalamount))
		return;

	m_bMerchantState = MERCHANT_STATE_BUYING;
	bResult = 1;
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_INSERT));
	result << bResult;
	Send(&result);

	if(bResult == 1)
	{
		_MERCHANT_LIST * pList = new _MERCHANT_LIST;

		for (int i = 0; i < amount_of_items; i++)
		{
		
		pList->ItemID[i] = m_arBuyMerchantItems[i].nNum;
		pList->Price[i] = m_arBuyMerchantItems[i].nPrice;
		pList->strUserID = GetID();
		pList->strUserName = m_strUserID;
		pList->Type = 1;

		}

		g_pMain->m_MerchantListArray.PutData(pList->strUserID, pList);
	}
	BuyingMerchantInsertRegion();
}

void CUser::BuyingMerchantInsertRegion()
{
	Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_REGION_INSERT));
	result << GetSocketID();

	for (int i = 0; i < 4; i++)
	{
		result << m_arBuyMerchantItems[i].nNum;
	}

	SendToRegion(&result,nullptr,GetEventRoom());
}

void CUser::BuyingMerchantList(Packet & pkt)
{
	if (m_sMerchantsSocketID >= 0)
		RemoveFromMerchantLookers(); //This check should never be hit...

	if (isDead() 
		|| isTrading() 
		|| isMerchanting() 
		|| isStoreOpen() 
		|| isSellingMerchant() 
		|| isBuyingMerchant() 
		|| isMining() 
		|| m_bMerchantStatex)
		return;

	uint16 uid = pkt.read<uint16>();

	if(uid == GetID())
		return;

	CUser *pMerchant = g_pMain->GetUserPtr(uid);
	if (pMerchant == nullptr
		|| !pMerchant->isMerchanting() 
		|| pMerchant->m_bMerchantState != MERCHANT_STATE_BUYING)
		return;

	m_sMerchantsSocketID = uid;

	if (pMerchant->m_arMerchantLookers.size() != 0) {
		Packet result(WIZ_MERCHANT, uint8(35));
		result << uint8(7);
		Send(&result);
	return;
	}

	pMerchant->m_arMerchantLookers.push_front(GetSocketID());

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_LIST));
	result << uint8(1) << uint16(uid);
	for (int i = 0; i < MAX_MERCH_ITEMS; i++)
	{
		_MERCH_DATA *pMerch = &pMerchant->m_arBuyMerchantItems[i];
		result	<< pMerch->nNum << pMerch->sCount
			<< pMerch->sDuration << pMerch->nPrice;
	}
	Send(&result);
}

void CUser::BuyingMerchantBuy(Packet & pkt)
{
	uint32 nPrice;
	uint16 sStackSize, sRemainingStackSize;
	uint8 bSellerSrcSlot, bMerchantListSlot;

	CUser *pMerchant = g_pMain->GetUserPtr(m_sMerchantsSocketID);
	if (pMerchant == nullptr)
		return;

	pkt >> bSellerSrcSlot >> bMerchantListSlot >> sStackSize;

	if (bSellerSrcSlot >= HAVE_MAX
		|| bMerchantListSlot >= MAX_MERCH_ITEMS)
		return;

	if (isDead() 
		|| isTrading() 
		|| isMerchanting() 
		|| isStoreOpen() 
		|| isSellingMerchant() 
		|| isBuyingMerchant() 
		|| isMining() 
		|| m_bMerchantStatex)
		return;

	_MERCH_DATA *pWantedItem = &pMerchant->m_arBuyMerchantItems[bMerchantListSlot];
	_ITEM_DATA *pSellerItem = GetItem(SLOT_MAX + bSellerSrcSlot);

	// Make sure the merchant actually has that item in that slot
	// and that they want enough, and the selling user has enough
	if (pWantedItem == nullptr 
		|| pSellerItem == nullptr
		|| pWantedItem->nNum != pSellerItem->nNum
		|| pWantedItem->sCount < sStackSize
		|| pSellerItem->sCount < sStackSize
		|| pSellerItem->nExpirationTime > 0
		// For scrolls, this will ensure you can only sell a full stack of scrolls.
		// For everything else, this will ensure you cannot sell items that need repair.
		|| pSellerItem->sDuration != pWantedItem->sDuration
		|| pSellerItem->isDuplicate()
		|| sStackSize <= 0)
		return;

	// If it's not stackable, and we're specifying something other than 1
	// we really don't care to handle this request...
	_ITEM_TABLE *proto = g_pMain->GetItemPtr(pWantedItem->nNum);
	if (proto == nullptr
		|| !proto->m_bCountable && sStackSize != 1)
		return;
	

	// Do they have enough coins?
	nPrice = pWantedItem->nPrice * sStackSize;
	if (!pMerchant->hasCoins(nPrice))
		return;

	// Now find the buyer a home for their item
	int8 bDstPos = pMerchant->FindSlotForItem(pWantedItem->nNum, sStackSize);
	if (bDstPos < 0)
		return;

	_ITEM_DATA *pMerchantItem = pMerchant->GetItem(bDstPos);

	// Take coins off the buying merchant
	if (!pMerchant->GoldLose(nPrice))
		return;

	// and give them all to me, me, me!
	GoldGain(nPrice);

	// Get the remaining stack size after purchase.
	sRemainingStackSize = pSellerItem->sCount - sStackSize;

	// Now we give the buying merchant their wares.
	pMerchantItem->nNum = pSellerItem->nNum;
	pMerchantItem->sDuration = pSellerItem->sDuration;
	pSellerItem->sCount -= sStackSize;
	pMerchantItem->sCount += sStackSize;

	// Update how many items the buyer still needs.
	pWantedItem->sCount -= sStackSize;

	// If the buyer needs no more, remove this item from the wanted list.
	if (pWantedItem->sCount == 0)
		memset(pWantedItem, 0, sizeof(_MERCH_DATA));

	// If the seller's all out, remove their item.
	if (pSellerItem->sCount == 0)
		memset(pSellerItem, 0, sizeof(_ITEM_DATA));

	// TODO : Proper checks for the removal of the items in the array, we're now assuming everything gets bought

	// Update players
	SendStackChange(pSellerItem->nNum, pSellerItem->sCount, pSellerItem->sDuration, bSellerSrcSlot);
	pMerchant->SendStackChange(pMerchantItem->nNum, pMerchantItem->sCount, pMerchantItem->sDuration, bDstPos - SLOT_MAX,
		pMerchantItem->sCount == sStackSize); 	// if the buying merchant only has what they wanted, it's a new item.
	// (otherwise it was a stackable item that was merged into an existing slot)

	Packet result(WIZ_MERCHANT, uint8(MERCHANT_BUY_BOUGHT));
	result << bMerchantListSlot << uint16(0) << GetName();
	pMerchant->Send(&result);

	result.clear();
	result << uint8(MERCHANT_BUY_SOLD) << uint8(1) << bMerchantListSlot << pWantedItem->sCount << bSellerSrcSlot << pSellerItem->sCount;
	Send(&result);

	result.clear();
	result << uint8(MERCHANT_BUY_BUY) << uint8(1);
	Send(&result);


	if (bMerchantListSlot < 4 && pWantedItem->sCount == 0)
	{
		result.Initialize(WIZ_MERCHANT_INOUT);
		result << uint8(2) << m_sMerchantsSocketID << uint8(1) << uint8(0) << bMerchantListSlot;
		pMerchant->SendToRegion(&result,nullptr,pMerchant->GetEventRoom());
	}		

	int nItemsRemaining = 0;
	for (int i = 0; i < MAX_MERCH_ITEMS; i++)
	{
		if (pMerchant->m_arBuyMerchantItems[i].nNum != 0)
			nItemsRemaining++;
	}

	if (nItemsRemaining == 0)
		pMerchant->BuyingMerchantClose();

}

void CUser::RemoveFromMerchantLookers()
{
	CUser *pPreviousMerchantUser = g_pMain->GetUserPtr(m_sMerchantsSocketID);

	CBot *pPreviousMerchantBot = g_pMain->m_arBotArray.GetData(m_sMerchantsSocketID);
	if (pPreviousMerchantUser == nullptr && pPreviousMerchantBot == nullptr)
		return;
	if ( m_sMerchantsSocketID < MAX_USER)
		pPreviousMerchantUser->m_arMerchantLookers.remove(GetSocketID());
	else
		pPreviousMerchantBot->m_arMerchantLookers.remove(GetSocketID());
	m_sMerchantsSocketID = -1;
}

void  CUser::OfficialList(Packet & pkt)
{
	if (GetZoneID() < ZONE_MORADON || GetZoneID() > ZONE_MORADONM2)
	{
		Packet result(WIZ_MERCHANT, uint8(MERCHANT_MENISIA_LIST));
		result << uint8(1) << uint8(0) << uint8(3);
		Send(&result);
		return;
	}

	uint8 opcode = pkt.read<uint8>();

	switch (opcode)
	{
	case 1:
		ListSend(pkt);
		break;

	case 2:
		ListMoveProcess(pkt);
		break;
	}	
}

void CUser::ListSend(Packet & pkt)
{
	uint32 RecvItemID = pkt.read<uint32>();

	if (RecvItemID != ITEM_MENICIAS_LIST)
		return;

	if (CheckExistItem(RecvItemID))
	{
		uint8 nCount = 0;
		
		Packet result(WIZ_MERCHANT,uint8(MERCHANT_MENISIA_LIST));
		result << uint8(0x01) << uint8(0x01) << nCount;
		Send(&result);

		int16 nSize = g_pMain->m_MerchantListArray.GetSize();

		if(nSize == 0)
		{
			uint8 nCount = 0;
			Packet result2(WIZ_MERCHANT,uint8(MERCHANT_MENISIA_LIST));
			result2 << uint8(0x01) << uint8(0x02) << nCount;
			Send(&result2);
			return;
		}

		while (nSize > 0)
		{
			nCount++;
			result.clear();

			result << uint8(MERCHANT_MENISIA_LIST) << uint8(1) << uint8(2) << nCount;

			if(nSize > 50)
				result << uint8(50);
			else
				result << uint8(nSize);
			
			result.SByte();

			foreach_stlmap(itr, g_pMain->m_MerchantListArray)
			{
				result << itr->second->strUserID << itr->second->strUserName << itr->second->Type;

				for(int i = 0; i < 12; i++)
				{
					if(itr->second->ItemID[i] == 3452816845)
						result << uint32(0) << uint32(0);
					else
					{
						result << itr->second->ItemID[i] << itr->second->Price[i];
					}
				}
			}
			Send(&result);
			nSize = nSize - 50;
		}
	}
	else
	{
		Packet result(WIZ_MERCHANT,uint8(MERCHANT_MENISIA_LIST));
		result << uint8(0x01) << uint8(0x00) << uint8(0x00);
		Send(&result);
	}
}

void CUser::ListMoveProcess(Packet & pkt)
{
	if (isDead() 
		|| isTrading() 
		|| isMerchanting() 
		|| isStoreOpen() 
		|| isSellingMerchant() 
		|| isBuyingMerchant() 
		|| isMining() 
		|| m_bMerchantStatex)
	{
		Packet result(WIZ_MERCHANT, uint8(MERCHANT_MENISIA_LIST));
		result << uint8(1) << uint8(0) << uint8(3);
		Send(&result);
		return;
	}

	uint16 TargetID = pkt.read<uint16>();
	CUser *pUser = g_pMain->GetUserPtr(TargetID);
	CBot * pBot = g_pMain->m_arBotArray.GetData(TargetID);

	if (!pUser)
	{
		if(pBot)
		{
			ZoneChange(pBot->m_bZone, pBot->GetX(), pBot->GetZ());
		}
		else
		{
		Packet error(WIZ_MERCHANT, uint8(MERCHANT_MENISIA_LIST));
		error << uint8(2) << uint8(0) << uint8(0);
		Send(&error);
		return;
		}
		


	}
	else if(!pBot)
	{
		if(pUser)
		{
			ZoneChange(pUser->GetZoneID(), pUser->GetX(), pUser->GetZ());
		}
		else
		{
		Packet error(WIZ_MERCHANT, uint8(MERCHANT_MENISIA_LIST));
		error << uint8(2) << uint8(0) << uint8(0);
		Send(&error);
		return;
		}
	}

	
}