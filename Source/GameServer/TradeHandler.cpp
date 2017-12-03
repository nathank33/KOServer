#include "stdafx.h"
#include "Map.h"
#include "../shared/DateTime.h"

using namespace std;

void CUser::ExchangeProcess(Packet & pkt) {
	uint8 opcode = pkt.read<uint8>();

	if (isStoreOpen() || isGM() || isMerchanting() || isSellingMerchant() || isBuyingMerchant() || isDead() || m_bMerchantStatex)
		opcode = 8;

	switch (opcode) {
	case EXCHANGE_REQ:
		ExchangeReq(pkt);
		break;
	case EXCHANGE_AGREE:
		ExchangeAgree(pkt);
		break;
	case EXCHANGE_ADD:
		ExchangeAdd(pkt);
		break;
	case EXCHANGE_DECIDE:
		ExchangeDecide();
		break;
	case EXCHANGE_CANCEL:
		ExchangeCancel();
		break;
	}
}

void CUser::ExchangeReq(Packet & pkt) {
	Packet result(WIZ_EXCHANGE);
	CUser * pUser;
	uint16 destid;

	if (isDead() || isStoreOpen() || isMerchanting() || m_bMerchantStatex)
		goto fail_return;

	else if (isTrading()) {
		ExchangeCancel();
		return;
	}

	destid = pkt.read<uint16>();
	pUser = g_pMain->GetUserPtr(destid);
	if (pUser == nullptr
		|| pUser == this
		|| pUser->isGM()
		|| pUser->isTrading()
		|| pUser->m_bMerchantStatex
		|| pUser->GetZoneID() != GetZoneID()
		|| !isInRange(pUser->GetX(), pUser->GetZ(), RANGE_50M)
		|| (GetNation() != GetNation() && GetMap()->canTradeWithOtherNation()))
		goto fail_return;

	m_sExchangeUser = destid;
	pUser->m_sExchangeUser = GetSocketID();

	if (pUser->isDead() || pUser->isStoreOpen() || pUser->isMerchanting())
		goto fail_return;

	if (GetLevel() < 20 || pUser->GetLevel() < 20) {
		Packet resultmer;
		std::string bufferpro = string_format("[Trade Message] must be at least 20 level");
		ChatPacket::Construct(&resultmer, 7, &bufferpro);
		Send(&resultmer);
		goto fail_return;
	}

	result << uint8(EXCHANGE_REQ) << GetSocketID();
	pUser->Send(&result);
	return;

fail_return:
	result << uint8(EXCHANGE_CANCEL);
	Send(&result);
}

void CUser::ExchangeAgree(Packet & pkt) {
	uint8 bResult = pkt.read<uint8>();
	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	if (pUser == nullptr || pUser == this) {
		m_sExchangeUser = -1;
		return;
	}

	if (pUser->isDead()
		|| isDead()
		|| pUser->isStoreOpen()
		|| isStoreOpen()
		|| pUser->isMerchanting()
		|| isMerchanting()
		|| pUser->m_bMerchantStatex
		|| m_bMerchantStatex
		|| !pUser->isTrading()
		|| !isTrading()
		|| pUser->GetZoneID() != GetZoneID()
		|| !isInRange(pUser->GetX(), pUser->GetZ(), RANGE_50M)) {
		m_sExchangeUser = -1;
		return;
	}

	if (!bResult || pUser->isDead()) // declined
	{
		m_sExchangeUser = -1;
		pUser->m_sExchangeUser = -1;
		bResult = 0;
	}

	string errorMessage = string_format(_T("TRADE uId-%s- tId-%s- r-%d- Z-%d- X-%d- Y-%d-"),
		GetName().c_str(), pUser->GetName().c_str(), bResult, GetZoneID(), uint16(GetX()), uint16(GetZ()));
	g_pMain->WriteTradeUserLogFile(errorMessage);

	Packet result(WIZ_EXCHANGE, uint8(EXCHANGE_AGREE));
	result << uint16(bResult);
	pUser->Send(&result);
}

void CUser::ExchangeAdd(Packet & pkt) {
	if (!isTrading()
		|| isDead()
		|| isStoreOpen()
		|| isMerchanting()
		|| m_bMerchantStatex)
		return;

	Packet result(WIZ_EXCHANGE, uint8(EXCHANGE_ADD));
	uint64 nSerialNum;
	uint32 nItemID, count = 0;
	uint16 duration = 0;
	_ITEM_DATA * pSrcItem = nullptr;
	list<_EXCHANGE_ITEM*>::iterator	Iter;
	uint8 pos;
	bool bAdd = true, bGold = false;

	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	if (pUser == nullptr
		|| pUser == this
		|| pUser->isDead()
		|| isDead()) {
		ExchangeCancel();
		return;
	}

	if (pUser->isDead() || pUser->isStoreOpen()
		|| pUser->isMerchanting()
		|| pUser->m_bMerchantStatex
		|| !pUser->isTrading())
		goto add_fail;

	pkt >> pos >> nItemID >> count;

	if (count == 0)
		goto add_fail;

	_ITEM_TABLE *pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr
		|| (nItemID != ITEM_GOLD
			&& (pos >= HAVE_MAX // Invalid position
				|| (nItemID >= ITEM_NO_TRADE && nItemID < ITEM_NO_TRADE_MAX) // Cannot be traded, stored or sold.
				|| pTable->m_bRace == RACE_UNTRADEABLE)) // Cannot be traded or sold.
		|| m_bExchangeOK)
		goto add_fail;

	if (nItemID == ITEM_GOLD) {
		if (count <= 0 || count > m_iGold)
			goto add_fail;

		// If we have coins in the list already
		// add to the amount of coins listed.
		foreach(itr, m_ExchangeItemList) {
			if ((*itr)->nItemID == ITEM_GOLD) {
				(*itr)->nCount += count;
				bAdd = false; /* don't need to add a new entry */
				break;
			}
		}

		m_iGold -= count;
	} else if ((pSrcItem = GetItem(SLOT_MAX + pos)) != nullptr && pSrcItem->nNum == nItemID) {
		if (pSrcItem->sCount < count
			|| pSrcItem->isRented()
			|| pSrcItem->isSealed()
			|| pSrcItem->isBound()
			|| pSrcItem->isDuplicate()
			|| pSrcItem->nExpirationTime != 0)
			goto add_fail;

		if (pTable->m_bCountable) {
			foreach(itr, m_ExchangeItemList) {
				if ((*itr)->nItemID == nItemID) {
					(*itr)->nCount += count;
					bAdd = false;
					break;
				}
			}
		}

		if (pTable->isStackable())
			pSrcItem->sCount -= count;
		else
			pSrcItem->sCount = 0;


		duration = pSrcItem->sDuration;
		nSerialNum = pSrcItem->nSerialNum;
	} else
		goto add_fail;

	foreach(itr, m_ExchangeItemList) {
		if ((*itr)->nItemID == ITEM_GOLD) {
			bGold = true;
			break;
		}
	}
	if ((int) m_ExchangeItemList.size() > (bGold ? 13 : 12))
		goto add_fail;

	if (bAdd) {
		_EXCHANGE_ITEM * pItem = new _EXCHANGE_ITEM;
		pItem->nItemID = nItemID;
		pItem->sDurability = duration;
		pItem->nCount = count;
		pItem->nSerialNum = nSerialNum;
		pItem->bSrcPos = SLOT_MAX + pos;
		m_ExchangeItemList.push_back(pItem);
	}

	result << uint8(1);
	Send(&result);

	result.clear();

	result << uint8(EXCHANGE_OTHERADD)
		<< nItemID << count << duration;
	SetSpecialItemData(pSrcItem, result);
	pUser->Send(&result);
	return;

add_fail:
	result << uint8(0);
	Send(&result);
}

void CUser::ExchangeDecide() {
	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	if (pUser == nullptr
		|| pUser->isDead()
		|| pUser == this
		|| isDead()
		|| !isTrading()
		|| m_bMerchantStatex
		|| isStoreOpen()
		|| isMerchanting()
		|| m_bExchangeOK
		|| m_sExchangeUser == -1) {
		ExchangeCancel();
		return;
	}

	if (pUser->isDead()
		|| !pUser->isTrading()
		|| pUser->m_bMerchantStatex
		|| pUser->isStoreOpen()
		|| pUser->isMerchanting()) {
		pUser->ExchangeCancel();
		ExchangeCancel();
		return;
	}

	Packet result(WIZ_EXCHANGE);
	if (!pUser->m_bExchangeOK) {
		m_bExchangeOK = 1;
		result << uint8(EXCHANGE_OTHERDECIDE);
		pUser->Send(&result);
		return;
	}

	// Did the exchange requirements fail?
	if (!CheckExchange() || !pUser->CheckExchange()) {
		// At this stage, neither user has their items exchanged.
		// However, their coins were removed -- these will be removed by ExchangeFinish().
		result << uint8(EXCHANGE_DONE) << uint8(0);
		Send(&result);
		pUser->Send(&result);

		ExchangeCancel();
		pUser->ExchangeCancel();
	} else {
		ExecuteExchange();
		pUser->ExecuteExchange();

		Packet result(WIZ_EXCHANGE);
		result << uint8(EXCHANGE_DONE) << uint8(1)
			<< GetCoins()
			<< uint16(pUser->m_ExchangeItemList.size());

		foreach(itr, pUser->m_ExchangeItemList) {
			result << (*itr)->bDstPos << (*itr)->nItemID
				<< uint16((*itr)->nCount) << (*itr)->sDurability;
			pUser->SetExchangeSpecialItemData((*itr), result);
		}
		Send(&result);

		result.clear();

		result << uint8(EXCHANGE_DONE) << uint8(1)
			<< pUser->GetCoins()
			<< uint16(m_ExchangeItemList.size());

		foreach(itr, m_ExchangeItemList) {

			result << (*itr)->bDstPos << (*itr)->nItemID
				<< uint16((*itr)->nCount) << (*itr)->sDurability;
			SetExchangeSpecialItemData((*itr), result);
		}
		pUser->Send(&result);
		DateTime time;
		foreach(itr, pUser->m_ExchangeItemList) {
			g_pMain->WriteTradeUserLogFile(string_format("[ %d:%d:%d ] User Trade: %s - Recv Item User: %s  - Item: %d , count %d , Serial: %I64d \n", time.GetHour(), time.GetMinute(), time.GetSecond(), pUser->GetName().c_str(), GetName().c_str(), (*itr)->nItemID, (*itr)->nCount, (*itr)->nSerialNum));
		}

		foreach(itr, m_ExchangeItemList) {
			g_pMain->WriteTradeUserLogFile(string_format("[ %d:%d:%d ] User Trade: %s - Recv Item User: %s  - Item: %d , count %d , Serial: %I64d \n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), pUser->GetName().c_str(), (*itr)->nItemID, (*itr)->nCount, (*itr)->nSerialNum));
		}

		SetUserAbility(false);
		SendItemWeight();
		ExchangeFinish();

		pUser->SetUserAbility(false);
		pUser->SendItemWeight();
		pUser->ExchangeFinish();
	}
}

void CUser::ExchangeCancel(bool bIsOnDeath) {
	if (!isTrading()
		|| (!bIsOnDeath && isDead()))
		return;

	// Restore coins and items...
	while (m_ExchangeItemList.size()) {
		_EXCHANGE_ITEM *pItem = m_ExchangeItemList.front();
		if (pItem != nullptr) {
			// Restore coins to owner
			if (pItem->nItemID == ITEM_GOLD)
				m_iGold += pItem->nCount;
			// Restore items to owner
			else if (GetItem(pItem->bSrcPos)->nNum == pItem->nItemID)
				GetItem(pItem->bSrcPos)->sCount += pItem->nCount;

			delete pItem;
		}

		m_ExchangeItemList.pop_front();
	}

	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	ExchangeFinish();

	if (pUser != nullptr) {
		pUser->ExchangeCancel();

		string errorMessage = string_format(_T("TRADE_CANCEL uId-%s- tId-%s- Z-%d- X-%d- Y-%d-"),
			GetName().c_str(), pUser->GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ()));
		g_pMain->WriteTradeUserLogFile(errorMessage);

		Packet result(WIZ_EXCHANGE, uint8(EXCHANGE_CANCEL));
		pUser->Send(&result);
	}
}

void CUser::ExchangeFinish() {
	m_sExchangeUser = -1;
	m_bExchangeOK = 0;
	m_ExchangeItemList.clear();
}

/**
* @brief	Determines if a trade will be successful.
* 			If it's successful, we can exchange the items.
*
* @return	true if it succeeds, false if it fails.
*/
bool CUser::CheckExchange() {
	uint32 money = 0;
	uint16 weight = 0;

	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	if (pUser == nullptr || pUser == this)
		return false;

	if (pUser->isDead()
		|| pUser->isStoreOpen()
		|| pUser->isMerchanting()
		|| pUser->m_bMerchantStatex)
		return false;

	if (isDead()
		|| isStoreOpen()
		|| isMerchanting()
		|| m_bMerchantStatex)
		return false;


	// Get the total number of free slots in the player's inventory
	uint8 bFreeSlots = 0, bItemCount = 0;
	for (uint8 i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++) {
		_ITEM_DATA * pItem = GetItem(i);
		if (pItem->nNum == 0)
			bFreeSlots++;
	}

	// Loop through the other user's list of items up for trade.
	foreach(Iter, pUser->m_ExchangeItemList) {
		// If we're trading coins, ensure we don't exceed our limit.
		if ((*Iter)->nItemID == ITEM_GOLD) {
			money += (*Iter)->nCount;
			if ((m_iGold + money) > COIN_MAX)
				return false;

			continue;
		}

		// Does this item exist?
		_ITEM_TABLE *pTable = g_pMain->GetItemPtr((*Iter)->nItemID);
		if (pTable == nullptr)
			return false;

		if (pTable->isStackable()
			&& (*Iter)->nItemID != 0 // slot in use
			&& (*Iter)->nItemID != pTable->m_iNum)
			return false;

		// Is there enough room for this item?
		// NOTE: Also ensures we have enough space in our inventory (with one exchange in mind anyway)
		if (!CheckWeight(pTable, (*Iter)->nItemID, (*Iter)->nCount))
			return false;

		// Total up the weight.
		weight += pTable->m_sWeight;
		bItemCount++;
	}

	// Do we have enough free slots for all these items?
	if (bItemCount > bFreeSlots)
		return false; /* note: ignores item stacks for now */

	// Ensure the total combined item weight does not exceed our weight limit
	return ((weight + m_sItemWeight) <= m_sMaxWeight);
}

bool CUser::ExecuteExchange() {
	CUser *pUser = g_pMain->GetUserPtr(m_sExchangeUser);
	if (pUser == nullptr)
		return false;

	ItemList::iterator coinItr = pUser->m_ExchangeItemList.end();
	foreach(Iter, pUser->m_ExchangeItemList) {
		if ((*Iter)->nItemID == ITEM_GOLD) {
			m_iGold += (*Iter)->nCount;
			coinItr = Iter;
			continue;
		}

		_ITEM_TABLE *pTable = g_pMain->GetItemPtr((*Iter)->nItemID);
		if (pTable == nullptr)
			continue;

		int nSlot = FindSlotForItem((*Iter)->nItemID, (*Iter)->nCount);
		ASSERT(nSlot > 0); /* this shouldn't happen, CheckExchange() prevents this */

		_ITEM_DATA * pDstItem = GetItem(nSlot);
		_ITEM_DATA * pSrcItem = pUser->GetItem((*Iter)->bSrcPos);

		if (pDstItem->nNum != pSrcItem->nNum
			&& pDstItem->nNum != 0)
			continue;

		pDstItem->nNum = pSrcItem->nNum;
		if (pTable->isStackable())
			pDstItem->sCount += (*Iter)->nCount;
		else
			pDstItem->sCount = (*Iter)->nCount;

		if (pDstItem->sCount > MAX_ITEM_COUNT)
			pDstItem->sCount = MAX_ITEM_COUNT;

		pDstItem->sDuration = pSrcItem->sDuration;

		if (!pTable->isStackable() || (*Iter)->nCount == pDstItem->sCount)
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

		if (!pTable->isStackable() && pDstItem->nSerialNum == 0)
			pSrcItem->nSerialNum = g_pMain->GenerateItemSerial();

		// This is really silly, but match the count up with the duration
		// for this special items that behave this way.
		if (pTable->m_bKind == 255)
			pDstItem->sCount = pDstItem->sDuration;

		// Set destination position for use in packet to client
		// to let them know where the item is.
		(*Iter)->bDstPos = (uint8) (nSlot - SLOT_MAX);


		string errorMessage = string_format(_T("TRADE_FINISH uId-%s- tId-%s- I-%d- Z-%d- X-%d- Y-%d-"),
			GetName().c_str(), pUser->GetName().c_str(), pSrcItem->nNum, GetZoneID(), uint16(GetX()), uint16(GetZ()));
		g_pMain->WriteTradeUserLogFile(errorMessage);

		// Remove the item from the other player.
		if (pSrcItem->sCount == 0 || pTable->m_bKind == 255)
			memset(pSrcItem, 0, sizeof(_ITEM_DATA));
	}

	// Remove coins from the list so it doesn't get sent
	// with the rest of the packet.
	if (coinItr != pUser->m_ExchangeItemList.end()) {
		delete *coinItr;
		pUser->m_ExchangeItemList.erase(coinItr);
	}

	return true;
}