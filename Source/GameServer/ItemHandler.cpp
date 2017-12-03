#include "stdafx.h"
#include "DBAgent.h"

using std::vector;

void CUser::WarehouseProcess(Packet & pkt) {
	Packet result(WIZ_WAREHOUSE);
	uint32 nItemID, nCount;
	uint16 sNpcId, reference_pos;
	uint8 page, bSrcPos, bDstPos;
	CNpc * pNpc = nullptr;
	_ITEM_TABLE * pTable = nullptr;
	_ITEM_DATA * pSrcItem = nullptr, *pDstItem = nullptr;
	uint8 opcode;
	bool bResult = false;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;


	pkt >> opcode;
	if (opcode == WAREHOUSE_OPEN) {
		result << opcode << uint8(1) << GetInnCoins();
		for (int i = 0; i < WAREHOUSE_MAX; i++) {
			_ITEM_DATA *pItem = &m_sWarehouseArray[i];

			if (pItem == nullptr)
				continue;

			result << pItem->nNum
				<< pItem->sDuration
				<< pItem->sCount
				<< pItem->bFlag;
			SetSpecialItemData(pItem, result);
			result << pItem->nExpirationTime;
		}
		if (isInPKZone()) {
			if (hasCoins(10000))
				GoldLose(10000);
			else {
				opcode = 1;
				goto fail_return;
			}
		}
		Send(&result);
		return;
	}

	pkt >> sNpcId >> nItemID >> page >> bSrcPos >> bDstPos;

	pNpc = g_pMain->GetNpcPtr(sNpcId);
	if (pNpc == nullptr
		|| pNpc->GetType() != NPC_WAREHOUSE
		|| !isInRange(pNpc, MAX_NPC_RANGE))
		goto fail_return;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr)
		goto fail_return;

	reference_pos = 24 * page;

	switch (opcode) {
		// Inventory -> inn
	case WAREHOUSE_INPUT:
		pkt >> nCount;

		// Handle coin input.
		if (nItemID == ITEM_GOLD) {
			if (!hasCoins(nCount)
				|| GetInnCoins() + nCount > COIN_MAX)
				goto fail_return;

			m_iBank += nCount;
			m_iGold -= nCount;
			break;
		}

		// Check for invalid slot IDs.
		if (bSrcPos > HAVE_MAX
			|| reference_pos + bDstPos > WAREHOUSE_MAX
			// Cannot be traded, sold or stored (note: don't check the race, as these items CAN be stored).
			|| (nItemID >= ITEM_NO_TRADE && pTable->m_bKind != 252)
			// Check that the source item we're moving is what the client says it is.
			|| (pSrcItem = GetItem(SLOT_MAX + bSrcPos))->nNum != nItemID
			// Rented items cannot be placed in the inn.
			|| pSrcItem->isRented()
			|| pSrcItem->isDuplicate()
			|| pSrcItem->nExpirationTime > 0 && (pSrcItem->nExpirationTime - uint32(UNIXTIME)) < (4 * DAY))
			goto fail_return;

		pDstItem = &m_sWarehouseArray[reference_pos + bDstPos];
		// Forbid users from moving non-stackable items into a slot already occupied by an item.
		if ((!pTable->isStackable() && pDstItem->nNum != 0)
			// Forbid users from moving stackable items into a slot already occupied by a different item.
			|| (pTable->isStackable()
				&& pDstItem->nNum != 0 // slot in use
				&& pDstItem->nNum != pSrcItem->nNum) // ... by a different item.
				// Ensure users have enough of the specified item to move.
			|| pSrcItem->sCount < nCount)
			goto fail_return;

		pDstItem->nNum = pSrcItem->nNum;
		pDstItem->sDuration = pSrcItem->sDuration;
		if (pTable->isStackable())
			pDstItem->sCount += (uint16) nCount;
		else
			pDstItem->sCount = (uint16) nCount;


		if (pTable->isStackable())
			pSrcItem->sCount -= nCount;
		else
			pSrcItem->sCount = 0;
		pDstItem->bFlag = pSrcItem->bFlag;
		pDstItem->sRemainingRentalTime = pSrcItem->sRemainingRentalTime;
		pDstItem->nExpirationTime = pSrcItem->nExpirationTime;

		if (!pTable->isStackable() || nCount == pDstItem->sCount)
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

		if (!pTable->isStackable() && pDstItem->nSerialNum == 0)
			pDstItem->nSerialNum = g_pMain->GenerateItemSerial();

		if (pSrcItem->sCount == 0 || pTable->m_bKind == 255)
			memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		SetUserAbility(false);
		SendItemWeight();
		break;

		// Inn -> inventory
	case WAREHOUSE_OUTPUT:
		pkt >> nCount;

		if (nItemID == ITEM_GOLD) {
			if (!hasInnCoins(nCount)
				|| GetCoins() + nCount > COIN_MAX)
				goto fail_return;

			m_iGold += nCount;
			m_iBank -= nCount;
			break;
		}

		// Ensure we're not being given an invalid slot ID.
		if (reference_pos + bSrcPos > WAREHOUSE_MAX
			|| bDstPos > HAVE_MAX
			// Check that the source item we're moving is what the client says it is.
			|| (pSrcItem = &m_sWarehouseArray[reference_pos + bSrcPos])->nNum != nItemID
			// Does the player have enough room in their inventory?
			|| !CheckWeight(pTable, nItemID, (uint16) nCount))
			goto fail_return;

		pDstItem = GetItem(SLOT_MAX + bDstPos);
		// Forbid users from moving non-stackable items into a slot already occupied by an item.
		if ((!pTable->isStackable() && pDstItem->nNum != 0)
			// Forbid users from moving stackable items into a slot already occupied by a different item.
			|| (pTable->isStackable()
				&& pDstItem->nNum != 0 // slot in use
				&& pDstItem->nNum != pSrcItem->nNum) // ... by a different item.
				// Ensure users have enough of the specified item to move.
			|| pSrcItem->sCount < nCount)
			goto fail_return;

		pDstItem->nNum = pSrcItem->nNum;
		pDstItem->sDuration = pSrcItem->sDuration;
		//pDstItem->sCount += (uint16) nCount;
		if (pTable->isStackable())
			pDstItem->sCount += (uint16) nCount;
		else
			pDstItem->sCount = (uint16) nCount;

		if (pTable->isStackable())
			pSrcItem->sCount -= nCount;
		else
			pSrcItem->sCount = 0;

		pDstItem->bFlag = pSrcItem->bFlag;
		pDstItem->sRemainingRentalTime = pSrcItem->sRemainingRentalTime;
		pDstItem->nExpirationTime = pSrcItem->nExpirationTime;

		if (!pTable->isStackable() || nCount == pDstItem->sCount)
			pDstItem->nSerialNum = pSrcItem->nSerialNum;

		if (!pTable->isStackable() && pDstItem->nSerialNum == 0)
			pDstItem->nSerialNum = g_pMain->GenerateItemSerial();

		if (pSrcItem->sCount == 0 || pTable->m_bKind == 255)
			memset(pSrcItem, 0, sizeof(_ITEM_DATA));

		SetUserAbility(false);
		SendItemWeight();
		break;

		// Inn -> inn
	case WAREHOUSE_MOVE:
		// Ensure we're not being given an invalid slot ID.
		if (reference_pos + bSrcPos > WAREHOUSE_MAX
			|| reference_pos + bDstPos > WAREHOUSE_MAX)
			goto fail_return;

		pSrcItem = &m_sWarehouseArray[reference_pos + bSrcPos];
		pDstItem = &m_sWarehouseArray[reference_pos + bDstPos];

		// Check that the source item we're moving is what the client says it is.
		if (pSrcItem->nNum != nItemID
			// You can't move a partial stack in the inn (the whole stack is moved).
			|| pDstItem->nNum != 0)
			goto fail_return;

		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
		memset(pSrcItem, 0, sizeof(_ITEM_DATA));
		break;

		// Inventory -> inventory (using the inn dialog)
	case WAREHOUSE_INVENMOVE:
		// Ensure we're not being given an invalid slot ID.
		if (bSrcPos > HAVE_MAX
			|| bDstPos > HAVE_MAX)
			goto fail_return;

		pSrcItem = GetItem(SLOT_MAX + bSrcPos);
		pDstItem = GetItem(SLOT_MAX + bDstPos);

		// Check that the source item we're moving is what the client says it is.
		if (pSrcItem->nNum != nItemID
			// You can't move a partial stack in the inventory (the whole stack is moved).
			|| pDstItem->nNum != 0)
			goto fail_return;

		memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
		memset(pSrcItem, 0, sizeof(_ITEM_DATA));
		break;
	}

	bResult = true;

fail_return: // hmm...
	result << opcode << bResult;
	Send(&result);
}

bool CUser::CheckWeight(uint32 nItemID, uint16 sCount) {
	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(nItemID);

	if (pTable == nullptr)
		return false;

	return CheckWeight(pTable, nItemID, sCount);
}

bool CUser::CheckWeight(_ITEM_TABLE * pTable, uint32 nItemID, uint16 sCount) {
	return (pTable != nullptr // Make sure the item exists
		// and that the weight doesn't exceed our limit
		&& (m_sItemWeight + (pTable->m_sWeight * sCount)) <= m_sMaxWeight
		// and we have room for the item.
		&& FindSlotForItem(nItemID, sCount) >= 0);
}

bool CUser::CheckExistItem(int itemid, short count /*= 1*/) {
	if (itemid < 100000000)
		return true;

	// Search for the existance of all items in the player's inventory storage and onwards (includes magic bags)
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++) {
		// This implementation fixes a bug where it ignored the possibility for multiple stacks.
		if (m_sItemArray[i].nNum == itemid
			&& m_sItemArray[i].sCount >= count)
			return true;
	}


	return false;
}

uint16 CUser::GetItemCount(uint32 nItemID) {
	uint32 result = 0;
	// Search for the existance of all items in the player's inventory storage and onwards (includes magic bags)
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++) {
		_ITEM_DATA *pItem = GetItem(i);
		if (pItem == nullptr
			|| pItem->isRented()
			|| pItem->isSealed()
			|| pItem->isBound()
			|| pItem->nExpirationTime > 0
			|| pItem->isDuplicate())
			continue;

		// This implementation fixes a bug where it ignored the possibility for multiple stacks.
		if (m_sItemArray[i].nNum == nItemID)
			result += m_sItemArray[i].sCount;
	}

	return result;
}

// Pretend you didn't see me. This really needs to go (just copying official)
bool CUser::CheckExistItemAnd(int32 nItemID1, int32 sCount1, int32 nItemID2, int32 sCount2,
	int32 nItemID3, int32 sCount3, int32 nItemID4, int32 sCount4, int32 nItemID5, int32 sCount5) {
	if (nItemID1
		&& !CheckExistItem(nItemID1, sCount1)
		&& nItemID1 > 99999999)
		if (nItemID1 != 900000000 || nItemID1 != 900001000 || nItemID1 != 900002000 || nItemID1 != 900003000 || nItemID1 != 900004000 || nItemID1 != 900005000 || nItemID1 != 900006000 || nItemID1 != 900007000 || nItemID1 != 900008000 || nItemID1 != 900009000 || nItemID1 != 900010000 || nItemID1 != 900011000)
			return false;

	if (nItemID2
		&& !CheckExistItem(nItemID2, sCount2)
		&& nItemID2 > 99999999)
		if (nItemID2 != 900000000 || nItemID2 != 900001000 || nItemID2 != 900002000 || nItemID2 != 900003000 || nItemID2 != 900004000 || nItemID2 != 900005000 || nItemID2 != 900006000 || nItemID2 != 900007000 || nItemID2 != 900008000 || nItemID2 != 900009000 || nItemID2 != 900010000 || nItemID2 != 900011000)
			return false;

	if (nItemID3
		&& !CheckExistItem(nItemID3, sCount3)
		&& nItemID3 > 99999999)
		if (nItemID3 != 900000000 || nItemID3 != 900001000 || nItemID3 != 900002000 || nItemID3 != 900003000 || nItemID3 != 900004000 || nItemID3 != 900005000 || nItemID3 != 900006000 || nItemID3 != 900007000 || nItemID3 != 900008000 || nItemID3 != 900009000 || nItemID3 != 900010000 || nItemID3 != 900011000)
			return false;

	if (nItemID4
		&& !CheckExistItem(nItemID4, sCount4)
		&& nItemID4 > 99999999)
		if (nItemID4 != 900000000 || nItemID4 != 900001000 || nItemID4 != 900002000 || nItemID4 != 900003000 || nItemID4 != 900004000 || nItemID4 != 900005000 || nItemID4 != 900006000 || nItemID4 != 900007000 || nItemID4 != 900008000 || nItemID4 != 900009000 || nItemID4 != 900010000 || nItemID4 != 900011000)
			return false;

	if (nItemID5
		&& !CheckExistItem(nItemID5, sCount5)
		&& nItemID5 > 99999999)
		if (nItemID5 != 900000000 || nItemID5 != 900001000 || nItemID5 != 900002000 || nItemID5 != 900003000 || nItemID5 != 900004000 || nItemID5 != 900005000 || nItemID5 != 900006000 || nItemID5 != 900007000 || nItemID5 != 900008000 || nItemID5 != 900009000 || nItemID5 != 900010000 || nItemID5 != 900011000)
			return false;


	return true;
}

bool CUser::CheckExistSpecialItemAnd(int32 nItemID1, int16 sCount1, int32 nItemID2, int16 sCount2,
	int32 nItemID3, int16 sCount3, int32 nItemID4, int16 sCount4, int32 nItemID5, int16 sCount5,
	int32 nItemID6, int16 sCount6, int32 nItemID7, int16 sCount7, int32 nItemID8, int16 sCount8,
	int32 nItemID9, int16 sCount9, int32 nItemID10, int16 sCount10) {
	if (nItemID1
		&& !CheckExistItem(nItemID1, sCount1)
		&& nItemID1 > 99999999)
		if (nItemID1 != 900000000 || nItemID1 != 900001000 || nItemID1 != 900002000 || nItemID1 != 900003000 || nItemID1 != 900004000 || nItemID1 != 900005000 || nItemID1 != 900006000 || nItemID1 != 900007000 || nItemID1 != 900008000 || nItemID1 != 900009000 || nItemID1 != 900010000 || nItemID1 != 900011000)
			return false;

	if (nItemID2
		&& !CheckExistItem(nItemID2, sCount2)
		&& nItemID2 > 99999999)
		if (nItemID2 != 900000000 || nItemID2 != 900001000 || nItemID2 != 900002000 || nItemID2 != 900003000 || nItemID2 != 900004000 || nItemID2 != 900005000 || nItemID2 != 900006000 || nItemID2 != 900007000 || nItemID2 != 900008000 || nItemID2 != 900009000 || nItemID2 != 900010000 || nItemID2 != 900011000)
			return false;

	if (nItemID3
		&& !CheckExistItem(nItemID3, sCount3)
		&& nItemID3 > 99999999)
		if (nItemID3 != 900000000 || nItemID3 != 900001000 || nItemID3 != 900002000 || nItemID3 != 900003000 || nItemID3 != 900004000 || nItemID3 != 900005000 || nItemID3 != 900006000 || nItemID3 != 900007000 || nItemID3 != 900008000 || nItemID3 != 900009000 || nItemID3 != 900010000 || nItemID3 != 900011000)
			return false;

	if (nItemID4
		&& !CheckExistItem(nItemID4, sCount4)
		&& nItemID4 > 99999999)
		if (nItemID4 != 900000000 || nItemID4 != 900001000 || nItemID4 != 900002000 || nItemID4 != 900003000 || nItemID4 != 900004000 || nItemID4 != 900005000 || nItemID4 != 900006000 || nItemID4 != 900007000 || nItemID4 != 900008000 || nItemID4 != 900009000 || nItemID4 != 900010000 || nItemID4 != 900011000)
			return false;

	if (nItemID5
		&& !CheckExistItem(nItemID5, sCount5)
		&& nItemID5 > 99999999)
		if (nItemID5 != 900000000 || nItemID5 != 900001000 || nItemID5 != 900002000 || nItemID5 != 900003000 || nItemID5 != 900004000 || nItemID5 != 900005000 || nItemID5 != 900006000 || nItemID5 != 900007000 || nItemID5 != 900008000 || nItemID5 != 900009000 || nItemID5 != 900010000 || nItemID5 != 900011000)
			return false;

	if (nItemID6
		&& !CheckExistItem(nItemID6, sCount6)
		&& nItemID6 > 99999999)
		if (nItemID6 != 900000000 || nItemID6 != 900001000 || nItemID6 != 900002000 || nItemID6 != 900003000 || nItemID6 != 900004000 || nItemID6 != 900005000 || nItemID6 != 900006000 || nItemID6 != 900007000 || nItemID6 != 900008000 || nItemID6 != 900009000 || nItemID6 != 900010000 || nItemID6 != 900011000)
			return false;

	if (nItemID7
		&& !CheckExistItem(nItemID7, sCount7)
		&& nItemID7 > 99999999)
		if (nItemID7 != 900000000 || nItemID7 != 900001000 || nItemID7 != 900002000 || nItemID7 != 900003000 || nItemID7 != 900004000 || nItemID7 != 900005000 || nItemID7 != 900006000 || nItemID7 != 900007000 || nItemID7 != 900008000 || nItemID7 != 900009000 || nItemID7 != 900010000 || nItemID7 != 900011000)
			return false;

	if (nItemID8
		&& !CheckExistItem(nItemID8, sCount8)
		&& nItemID8 > 99999999)
		if (nItemID8 != 900000000 || nItemID8 != 900001000 || nItemID8 != 900002000 || nItemID8 != 900003000 || nItemID8 != 900004000 || nItemID8 != 900005000 || nItemID8 != 900006000 || nItemID8 != 900007000 || nItemID8 != 900008000 || nItemID8 != 900009000 || nItemID8 != 900010000 || nItemID8 != 900011000)
			return false;

	if (nItemID9
		&& !CheckExistItem(nItemID9, sCount9)
		&& nItemID9 > 99999999)
		if (nItemID9 != 900000000 || nItemID9 != 900001000 || nItemID9 != 900002000 || nItemID9 != 900003000 || nItemID9 != 900004000 || nItemID9 != 900005000 || nItemID9 != 900006000 || nItemID9 != 900007000 || nItemID9 != 900008000 || nItemID9 != 900009000 || nItemID9 != 900010000 || nItemID9 != 900011000)
			return false;

	if (nItemID10
		&& !CheckExistItem(nItemID10, sCount10)
		&& nItemID10 > 99999999)
		if (nItemID10 != 900000000 || nItemID10 != 900001000 || nItemID10 != 900002000 || nItemID10 != 900003000 || nItemID10 != 900004000 || nItemID10 != 900005000 || nItemID10 != 900006000 || nItemID10 != 900007000 || nItemID10 != 900008000 || nItemID10 != 900009000 || nItemID10 != 900010000 || nItemID10 != 900011000)
			return false;

	return true;
}

bool CUser::RobItem(uint32 nItemID, uint16 sCount /*= 1*/, bool SendPacket /*=true*/) {

	// Allow unused exchanges.
	if (sCount == 0 || nItemID == 0)
		return false;

	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr)
		return false;
	if (nItemID == 900000000) //	Noah
	{
		GoldLose(sCount, true);
		return true;
	} else if (nItemID == 900001000) //	EXP
	{
		ExpChange(-(int64) sCount, true);
		return true;
	} else if (nItemID == 900002000 || nItemID == 900003000) //CountryCONT and LadderPoint
	{
		SendLoyaltyChange(-(int32) sCount, true);
		return true;
	} else if (nItemID == 900004000 ||	//	Random
		nItemID == 900005000 ||	//	Hunt
		nItemID == 900007000 ||	//	Skill
		nItemID == 900008000 ||	//	Killopponentcountry
		nItemID == 900009000 ||	//	Transport
		nItemID == 900010000 ||	//	LevelUp
		nItemID == 900011000)	//	War
		return true;

	// Search for the existance of all items in the player's inventory storage and onwards (includes magic bags)
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++) {
		if (RobItem(i, pTable, sCount, SendPacket))
			return true;
	}

	return false;
}

bool CUser::RobItem(uint8 bPos, _ITEM_TABLE * pTable, uint16 sCount /*= 1*/, bool SendPacket /*=true*/) {


	// Allow unused exchanges.
	if (sCount == 0)
		return false;

	if (pTable == nullptr)
		return false;

	_ITEM_DATA *pItem = GetItem(bPos);

	if (pItem == nullptr)
		return false;

	if (pItem->nNum != pTable->m_iNum
		|| pItem->sCount < sCount)
		return false;

	// Consumable "scrolls" (with some exceptions) use the duration/durability as a usage count
	// instead of the stack size. Interestingly, the client shows this instead of the stack size in this case.
	bool bIsConsumableScroll = (pTable->m_bKind == 255); /* include 97? not sure how accurate this check is... */
	if (bIsConsumableScroll)
		pItem->sDuration -= sCount;
	else if (pTable->isStackable())
		pItem->sCount -= sCount;
	else
		pItem->sCount = 0;


	std::string errorMessage = string_format(_T("ROB_ITEM uId-%s- I-%d- C-%d- Z-%d- X-%d- Y-%d-"),
		GetName().c_str(), pItem->nNum, sCount, GetZoneID(), uint16(GetX()), uint16(GetZ()));
	g_pMain->WriteTradeUserLogFile(errorMessage);
	// Delete the item if the stack's now 0
	// or if the item is a consumable scroll and its "duration"/use count is now 0.
	if (pItem->sCount == 0
		|| (bIsConsumableScroll && pItem->sDuration == 0))
		memset(pItem, 0, sizeof(_ITEM_DATA));




	if (SendPacket)
		SendStackChange(pTable->m_iNum, pItem->sCount, pItem->sDuration, bPos - SLOT_MAX);
	return true;
}

/**
* @brief	Checks if all players in the party have sCount of item nItemID
* 			and if so, removes it.
*
* @param	nItemID	Identifier for the item.
* @param	sCount 	Stack size.
*
* @return	true if the required items were taken, false if not.
*/
bool CUser::RobAllItemParty(uint32 nItemID, uint16 sCount /*= 1*/) {
	// Allow unused exchanges.
	if (sCount == 0)
		return false;

	_PARTY_GROUP * pParty = g_pMain->GetPartyPtr(GetPartyID());
	if (pParty == nullptr)
		return RobItem(nItemID, sCount);

	// First check to see if all users in the party have enough of the specified item.
	std::vector<CUser *> partyUsers;
	for (int i = 0; i < MAX_PARTY_USERS; i++) {
		CUser * pUser = g_pMain->GetUserPtr(pParty->uid[i]);
		if (pUser != nullptr
			&& !pUser->CheckExistItem(nItemID, sCount))
			return false;

		partyUsers.push_back(pUser);
	}

	// Since all users have the required item, we can now remove them. 
	foreach(itr, partyUsers)
		(*itr)->RobItem(nItemID, sCount);

	return true;
}

bool CUser::GiveItem(uint32 itemid, uint32 count, bool send_packet /*= true*/, uint32 Time) {
	std::string errorMessage = string_format(_T("GIVE_ITEM uId-%s- I-%d- C-%d- Z-%d- X-%d- Y-%d-"),
		GetName().c_str(), itemid, count, GetZoneID(), uint16(GetX()), uint16(GetZ()));
	g_pMain->WriteTradeUserLogFile(errorMessage);

	if (itemid == 900000000)//noah
	{
		GoldGain(count, true, false);
		return true;
	} else if (itemid == 900001000)//exp
	{
		ExpChange(count, false);
		return true;
	} else if (itemid == 900002000 || itemid == 900003000)//Country CONT Ladder Point
	{
		SendLoyaltyChange(count, false);
		return true;
	}
	int8 pos;
	bool bNewItem = true;
	_ITEM_TABLE* pTable = g_pMain->GetItemPtr(itemid);
	if (pTable == nullptr)
		return false;

	pos = FindSlotForItem(itemid, count);
	if (pos < 0)
		return false;

	_ITEM_DATA *pItem = GetItem(pos);
	if (pItem->nNum != 0 || pItem == nullptr)
		bNewItem = false;

	if (bNewItem)
		pItem->nSerialNum = g_pMain->GenerateItemSerial();

	pItem->nNum = itemid;
	pItem->sCount += count;
	if (pItem->sCount > MAX_ITEM_COUNT)
		pItem->sCount = MAX_ITEM_COUNT;

	//if (pItem->nNum == CHAOS_MAP)
	//pItem->nExpirationTime = int32(UNIXTIME) + 86400; // 1 day */

	pItem->sDuration = pTable->m_sDuration;
	if (Time != 0) {
		pItem->nExpirationTime = uint32(UNIXTIME) + ((60 * 60 * 24) * Time);
	} else {
		pItem->nExpirationTime = 0;
	}


	// This is really silly, but match the count up with the duration
	// for this special items that behave this way.
	/*if (pTable->m_bKind == 255)
	pItem->sCount = pItem->sDuration;*/

	if (send_packet) {
		SendStackChange(itemid, m_sItemArray[pos].sCount, m_sItemArray[pos].sDuration, pos - SLOT_MAX, true, Time);
	} else {
		SetUserAbility(false);
		SendItemWeight();
	}

	return true;
}

void CUser::SendItemWeight() {
	Packet result(WIZ_WEIGHT_CHANGE);
	result << m_sItemWeight;
	Send(&result);
}

bool CUser::ItemEquipAvailable(_ITEM_TABLE *pTable) {
	return (pTable != nullptr
		&& GetLevel() >= pTable->m_bReqLevel
		&& GetLevel() <= pTable->m_bReqLevelMax
		&& m_bRank >= pTable->m_bReqRank // this needs to be verified
		&& m_bTitle >= pTable->m_bReqTitle // this is unused
		&& GetStat(STAT_STR) >= pTable->m_bReqStr
		&& GetStat(STAT_STA) >= pTable->m_bReqSta
		&& GetStat(STAT_DEX) >= pTable->m_bReqDex
		&& GetStat(STAT_INT) >= pTable->m_bReqIntel
		&& GetStat(STAT_CHA) >= pTable->m_bReqCha);
}

void CUser::ItemMove(Packet & pkt) {
	_ITEM_TABLE *pTable, *pTableSrc;
	_ITEM_DATA *pSrcItem, *pDstItem, tmpItem, *pRightHand, *pLeftHand;
	uint32 nItemID;
	uint8 bSrcPos, bDstPos, dir, type;
	memset(&tmpItem, 0, sizeof(_ITEM_DATA));

	bool isSlotInven = false;

	pkt >> type >> dir >> nItemID >> bSrcPos >> bDstPos;


	if (isTrading()
		|| isMerchanting()
		|| m_bMerchantStatex
		|| isMining()
		|| GetZoneID() == ZONE_CHAOS_DUNGEON)
		goto fail_return;


	if (type == 2) {
		if (isTrading()
			|| isMerchanting()
			|| m_bMerchantStatex
			|| isMining()
			|| GetZoneID() == ZONE_CHAOS_DUNGEON
			|| isStoreOpen())
			goto fail_return;

		Packet IteAuto(WIZ_ITEM_MOVE, uint8(0x02));

		if ((UNIXTIME - lastArrangeTime) < 30) {
			IteAuto << uint8(0);
			Send(&IteAuto);
			return;
		}

		IteAuto << uint8(1);
		lastArrangeTime = UNIXTIME;
		_ITEM_DATA m_sCopyItemArray[INVENTORY_TOTAL];
		memset(m_sCopyItemArray, 0x00, sizeof(m_sCopyItemArray));

		uint8 lastCount = 0;


		for (int i = 0; i < INVENTORY_TOTAL; i++) {
			_ITEM_DATA  pItems = m_sItemArray[i];
			_ITEM_DATA * pItem = GetItem(i);

			if ((i < SLOT_MAX)
				|| (i >= (SLOT_MAX + HAVE_MAX))) {
				m_sCopyItemArray[i] = pItems;
				continue;
			}


			_ITEM_TABLE * pTable = g_pMain->GetItemPtr(pItem->nNum);

			if (pItem == nullptr
				|| pItem->nNum == 0
				|| pItem->sCount == 0
				|| pTable == nullptr)
				continue;

			m_sCopyItemArray[lastCount + SLOT_MAX] = pItems;
			lastCount++;
		}

		for (int i = 0; i < INVENTORY_TOTAL; i++)
			m_sItemArray[i] = m_sCopyItemArray[i];


		for (int i = SLOT_MAX; i < (SLOT_MAX + HAVE_MAX); i++) {
			_ITEM_DATA * pItem = GetItem(i);

			if (pItem->nNum == 0
				|| pItem->sCount == 0)
				IteAuto << uint32(0) << uint16(0) << uint16(0) << uint8(0) << uint16(0) << uint32(0) << uint32(0);
			else
				IteAuto << pItem->nNum << pItem->sDuration << pItem->sCount << pItem->bFlag	// item type flag (e.g. rented)
				<< pItem->sRemainingRentalTime;	// remaining time
			SetSpecialItemData(pItem, IteAuto);
			IteAuto << pItem->nExpirationTime;


		}
		Send(&IteAuto);
	}


	_ITEM_TABLE * pItemData = nullptr;
	CPet *newPet = nullptr;
	_ITEM_DATA *pItem = nullptr;



	pTable = g_pMain->GetItemPtr(nItemID);
	if (pTable == nullptr
		//  || dir == ITEM_INVEN_SLOT && ((pTable->m_sWeight + m_sItemWeight) > m_sMaxWeight))
			//  || dir > ITEM_MBAG_TO_MBAG || bSrcPos >= SLOT_MAX+HAVE_MAX+COSP_MAX+MBAG_MAX || bDstPos >= SLOT_MAX+HAVE_MAX+COSP_MAX+MBAG_MAX
		|| ((dir == ITEM_INVEN_SLOT || dir == ITEM_SLOT_SLOT)
			&& (bDstPos > SLOT_MAX || !ItemEquipAvailable(pTable)))
		|| (dir == ITEM_SLOT_INVEN && bSrcPos > SLOT_MAX)
		|| ((dir == ITEM_INVEN_SLOT || dir == ITEM_SLOT_SLOT) && bDstPos == RESERVED))
		goto fail_return;

	switch (dir) {
	case ITEM_MBAG_TO_MBAG:
		if (bDstPos >= MBAG_TOTAL || bSrcPos >= MBAG_TOTAL
			// We also need to make sure that if we're setting an item in a magic bag, we need to actually
				// have a magic back to put the item in!
			|| (INVENTORY_MBAG + bDstPos < INVENTORY_MBAG2 && m_sItemArray[BAG1].nNum == 0)
			|| (INVENTORY_MBAG + bDstPos > INVENTORY_MBAG2 && m_sItemArray[BAG2].nNum == 0)
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[INVENTORY_MBAG + bSrcPos].nNum)
			goto fail_return;

		pSrcItem = &m_sItemArray[INVENTORY_MBAG + bSrcPos];
		pDstItem = &m_sItemArray[INVENTORY_MBAG + bDstPos];
		break;


	case ITEM_MBAG_TO_INVEN:
		if (bDstPos >= HAVE_MAX || bSrcPos >= MBAG_TOTAL
			// We also need to make sure that if we're taking an item from a magic bag, we need to actually
				// have a magic back to take it from!
			|| (INVENTORY_MBAG + bSrcPos < INVENTORY_MBAG2 && m_sItemArray[BAG1].nNum == 0)
			|| (INVENTORY_MBAG + bSrcPos > INVENTORY_MBAG2 && m_sItemArray[BAG2].nNum == 0)
			// Make sure that the item actually exists there.
			|| m_sItemArray[SLOT_MAX + bDstPos].nNum != 0
			|| nItemID != m_sItemArray[INVENTORY_MBAG + bSrcPos].nNum)
			goto fail_return;

		pSrcItem = &m_sItemArray[INVENTORY_MBAG + bSrcPos];
		pDstItem = &m_sItemArray[INVENTORY_INVENT + bDstPos];
		break;

	case ITEM_INVEN_TO_MBAG:
		if (bDstPos >= MBAG_TOTAL || bSrcPos >= HAVE_MAX
			// We also need to make sure that if we're adding an item to a magic bag, we need to actually
				// have a magic back to put the item in!
			|| (INVENTORY_MBAG + bDstPos < INVENTORY_MBAG2 && m_sItemArray[BAG1].nNum == 0)
			|| (INVENTORY_MBAG + bDstPos > INVENTORY_MBAG2 && m_sItemArray[BAG2].nNum == 0)
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[INVENTORY_INVENT + bSrcPos].nNum)
			goto fail_return;

		pSrcItem = &m_sItemArray[INVENTORY_INVENT + bSrcPos];
		pDstItem = &m_sItemArray[INVENTORY_MBAG + bDstPos];
		break;

	case ITEM_COSP_TO_INVEN:
		if (bDstPos >= HAVE_MAX || bSrcPos >= COSP_MAX + MBAG_COUNT
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[INVENTORY_COSP + bSrcPos].nNum)
			goto fail_return;

		pSrcItem = &m_sItemArray[INVENTORY_COSP + bSrcPos];
		pDstItem = &m_sItemArray[SLOT_MAX + bDstPos];

		pTableSrc = g_pMain->GetItemPtr(pDstItem->nNum);
		if (pDstItem->nNum != 0
			&& (pTableSrc == nullptr || pTableSrc->m_bSlot != pTable->m_bSlot || !IsValidSlotPos(pTableSrc, bSrcPos)))
			goto fail_return;

		break;

	case ITEM_PET_TO_INVEN:
		if ((pItemData = GetItemPrototype(SHOULDER)) == nullptr
			|| !pItemData->isPet())
			goto fail_return;

		if ((pItem = GetItem(SHOULDER)) == nullptr
			|| pItem->nNum != pItemData->Getnum()
			|| nItemID != 700012000)
			goto fail_return;

		newPet = g_pMain->GetPetPtr(pItem->nSerialNum);
		if (newPet == nullptr || newPet->m_pNpc == nullptr)
			return;

		if (bSrcPos >= PET_MAX || bDstPos >= HAVE_MAX
			|| nItemID != newPet->m_sItemArray[bSrcPos].nNum
			|| m_sItemArray[SLOT_MAX + bDstPos].nNum > 0)
			goto fail_return;

		pDstItem = &m_sItemArray[SLOT_MAX + bDstPos];
		pSrcItem = &newPet->m_sItemArray[bSrcPos];
		break;

	case ITEM_INVEN_TO_PET:
		if ((pItemData = GetItemPrototype(SHOULDER)) == nullptr
			|| !pItemData->isPet())
			goto fail_return;

		if ((pItem = GetItem(SHOULDER)) == nullptr
			|| pItem->nNum != pItemData->Getnum()
			|| nItemID != 700012000)
			goto fail_return;

		newPet = g_pMain->GetPetPtr(pItem->nSerialNum);
		if (newPet == nullptr || newPet->m_pNpc == nullptr)
			return;

		if (bSrcPos >= HAVE_MAX || bDstPos >= PET_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[SLOT_MAX + bSrcPos].nNum
			|| newPet->m_sItemArray[bDstPos].nNum > 0)
			goto fail_return;

		pDstItem = &newPet->m_sItemArray[bDstPos];
		pSrcItem = &m_sItemArray[SLOT_MAX + bSrcPos];
		break;


	case ITEM_INVEN_TO_COSP:
		if (bDstPos >= COSP_MAX + MBAG_COUNT || bSrcPos >= HAVE_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[SLOT_MAX + bSrcPos].nNum
			|| !IsValidSlotPos(pTable, bDstPos)) {
			goto fail_return;
		}

		pSrcItem = &m_sItemArray[SLOT_MAX + bSrcPos];
		pDstItem = &m_sItemArray[INVENTORY_COSP + bDstPos];

		// If we're setting a magic bag...
		if (bDstPos == COSP_BAG1 || bDstPos == COSP_BAG2) {
			// Can't replace existing magic bag.
			if (pDstItem->nNum != 0
				// Can't set any old item in the bag slot, it must be a bag.
				|| pTable->m_bSlot != ItemSlotBag)
				goto fail_return;
		}

		pTableSrc = g_pMain->GetItemPtr(pDstItem->nNum);
		if (pDstItem->nNum != 0 && !IsValidSlotPos(pTableSrc, bDstPos))
			goto fail_return;
		break;

	case ITEM_INVEN_SLOT:
		if (bDstPos >= SLOT_MAX || bSrcPos >= HAVE_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[INVENTORY_INVENT + bSrcPos].nNum
			// Disable duplicate item moving to slot.
			|| m_sItemArray[INVENTORY_INVENT + bSrcPos].isDuplicate()
			// Ensure the item is able to be equipped in that slot
			|| !IsValidSlotPos(pTable, bDstPos))
			goto fail_return;

		isSlotInven = true;

		pSrcItem = &m_sItemArray[INVENTORY_INVENT + bSrcPos];
		pDstItem = &m_sItemArray[bDstPos];
		pRightHand = &m_sItemArray[RIGHTHAND];
		pLeftHand = &m_sItemArray[LEFTHAND];

		if (pTable->m_bSlot == 0x01 || (pTable->m_bSlot == 0x00 && bDstPos == RIGHTHAND)) {
			if (pLeftHand->nNum != 0) {
				_ITEM_TABLE* pTable2 = g_pMain->GetItemPtr(pLeftHand->nNum);

				if (pTable2 == nullptr)
					goto fail_return;

				if (pTable2->m_bSlot == 0x04) {
					// Sol elde tek el item var ise kalkan vs item geldiğinde sol eldeki itemi düşür.
					memcpy(pRightHand, pSrcItem, sizeof(_ITEM_DATA));
					memcpy(pSrcItem, pLeftHand, sizeof(_ITEM_DATA));
					memset(pLeftHand, 0, sizeof(_ITEM_DATA));
				} else {
					memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA));
					memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
					memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA));
				}
			} else {
				memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA));
				memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
				memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA));
			}
		} else if (pTable->m_bSlot == 0x02 || (pTable->m_bSlot == 0x00 && bDstPos == LEFTHAND)) {
			if (pRightHand->nNum != 0) {
				_ITEM_TABLE* pTable2 = g_pMain->GetItemPtr(pRightHand->nNum);

				if (pTable2 == nullptr)
					goto fail_return;

				if (pTable2->m_bSlot == 0x03) {
					memcpy(pLeftHand, pSrcItem, sizeof(_ITEM_DATA));
					memcpy(pSrcItem, pRightHand, sizeof(_ITEM_DATA));
					memset(pRightHand, 0, sizeof(_ITEM_DATA));
				} else {
					memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA));
					memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
					memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA));
				}
			} else {
				memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA));
				memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
				memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA));
			}
		} else if (pTable->m_bSlot == 0x03) {
			if (pLeftHand->nNum != 0 && pRightHand->nNum != 0) // her iki el de dolu ise işlem yapma
				goto fail_return;
			else if (pLeftHand->nNum != 0) // sol el dolu ise 
			{
				memcpy(pRightHand, pSrcItem, sizeof(_ITEM_DATA));
				memcpy(pSrcItem, pLeftHand, sizeof(_ITEM_DATA));
				memset(pLeftHand, 0, sizeof(_ITEM_DATA));
			} else // her iki ihtimal de yok ise normal işlem yap
			{
				memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA));
				memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
				memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA));
			}
		} else if (pTable->m_bSlot == 0x04) {
			if (pLeftHand->nNum != 0 && pRightHand->nNum != 0) // her iki el de dolu ise işlem yapma
				goto fail_return;
			else if (pRightHand->nNum != 0) {
				memcpy(pLeftHand, pSrcItem, sizeof(_ITEM_DATA));
				memcpy(pSrcItem, pRightHand, sizeof(_ITEM_DATA));
				memset(pRightHand, 0, sizeof(_ITEM_DATA));
			} else // her iki ihtimal de yok ise normal işlem yap
			{
				memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA));
				memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
				memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA));
			}
		} else {
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA));
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA));
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA));
		}
		break;

	case ITEM_SLOT_INVEN:
		if (bDstPos >= HAVE_MAX || bSrcPos >= SLOT_MAX
			// Make sure that the item actually exists there.
			|| m_sItemArray[SLOT_MAX + bDstPos].nNum != 0
			|| nItemID != m_sItemArray[bSrcPos].nNum)
			goto fail_return;

		if (isSummonPet && nItemID == 610001000)
			goto fail_return;

		pSrcItem = &m_sItemArray[bSrcPos];
		pDstItem = &m_sItemArray[INVENTORY_INVENT + bDstPos];
		break;

	case ITEM_INVEN_INVEN:
		if (bDstPos >= HAVE_MAX || bSrcPos >= HAVE_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[INVENTORY_INVENT + bSrcPos].nNum)
			goto fail_return;

		pSrcItem = &m_sItemArray[INVENTORY_INVENT + bSrcPos];
		pDstItem = &m_sItemArray[INVENTORY_INVENT + bDstPos];
		break;

	case ITEM_SLOT_SLOT:
		if (bDstPos >= SLOT_MAX || bSrcPos >= SLOT_MAX
			// Make sure that the item actually exists there.
			|| nItemID != m_sItemArray[bSrcPos].nNum
			// Ensure the item is able to be equipped in that slot
			|| !IsValidSlotPos(pTable, bDstPos))
			goto fail_return;

		pSrcItem = &m_sItemArray[bSrcPos];
		pDstItem = &m_sItemArray[bDstPos];
		break;

	default:
		goto fail_return;
		break;
		//return;
	}

	if (!isSlotInven) {
		// If there's an item already in the target slot already, we need to just swap the items
		if (dir == ITEM_INVEN_TO_MBAG && pSrcItem->nNum == pDstItem->nNum) {
			if (pTable->m_bCountable && pSrcItem->sCount + pDstItem->sCount < ITEMCOUNT_MAX) {
				pDstItem->sCount += pSrcItem->sCount;
				memset(pSrcItem, 0, sizeof(_ITEM_DATA)); // Clear out the source item's data
			} else
				goto fail_return;
		} else if (pDstItem->nNum != 0) {
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
		// Since there's no way to move a partial stack using this handler, just overwrite the destination.
		else {
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Shift the item over
			memset(pSrcItem, 0, sizeof(_ITEM_DATA)); // Clear out the source item's data
		}
	}

	// If equipping/de-equipping an item
	if (dir == ITEM_INVEN_SLOT || dir == ITEM_SLOT_INVEN
		// or moving an item to/from our cospre item slots
		|| dir == ITEM_INVEN_TO_COSP || dir == ITEM_COSP_TO_INVEN
		|| dir == ITEM_SLOT_SLOT) {
		// Re-update item stats
		SetUserAbility(false);
	}

	SendItemMove(1);
	SendItemWeight();



	// Update everyone else, so that they can see your shiny new items (you didn't take them off did you!? DID YOU!?)
	switch (dir) {
	case ITEM_INVEN_SLOT:
	case ITEM_INVEN_TO_COSP:
		UserLookChange(bDstPos, nItemID, pDstItem->sDuration);
		break;
	case ITEM_SLOT_INVEN:
		UserLookChange(bSrcPos, pSrcItem->nNum, pSrcItem->sDuration);
		break;
	case ITEM_COSP_TO_INVEN:
		UserLookChange(bSrcPos, pSrcItem->nNum, pSrcItem->sDuration);
		break;

	case ITEM_SLOT_SLOT:
		UserLookChange(bSrcPos, pSrcItem->nNum, pSrcItem->sDuration);
		UserLookChange(bDstPos, pDstItem->nNum, pDstItem->sDuration);
		break;
	}

	Send2AI_UserUpdateInfo();
	return;

fail_return:
	SendItemMove(0);
	return;
}

bool CUser::CheckExchange(int nExchangeID) {
	// Does the exchange exist?

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return false;

	_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);
	if (pExchange == nullptr)
		return false;
	uint16 sFreeSlot = -1;

	// Find free slots in the inventory, so that we can check against this later.
	uint8 bFreeSlots = 0;
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++) {
		if (m_sItemArray[i].nNum == 0
			&& ++bFreeSlots >= ITEMS_IN_EXCHANGE_GROUP)
			break;
	}

	// Add up the rates for this exchange to obtain a total percentage
	int nTotalPercent = 0;
	for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
		nTotalPercent += pExchange->sExchangeItemCount[i];

	if (nTotalPercent > 9000)
		return (bFreeSlots > 0);

	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++) {
		if (m_sItemArray[i].nNum == 0) {
			sFreeSlot = i;
			break;
		}
	}

	if (sFreeSlot < 0) {
		return false;
	}

	// Can we hold all of these items? If we can't, we have a problem.
	uint8 bReqSlots = 0;
	uint32 nReqWeight = 0;
	for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++) {
		uint32 nItemID = pExchange->nExchangeItemNum[i];

		// Does the item exist? If not, we'll ignore it (NOTE: not official behaviour).
		_ITEM_TABLE * pTable = nullptr;
		if (nItemID == 0
			|| (pTable = g_pMain->GetItemPtr(nItemID)) == nullptr)
			continue;

		// Try to find a slot for the item.
		// If we can't find an applicable slot with our inventory as-is,
		// there's no point even checking further.
		int pos;
		if ((pos = FindSlotForItem(nItemID, 1)) < 0)
			return false;

		// Now that we have our slot, see if it's in use (i.e. if adding a stackable item)
		// If it's in use, then we don't have to worry about requiring an extra slot for this item.
		// The only caveat here is with having multiple of the same stackable item: 
		// theoretically we could give them OK, but when it comes time to adding them, we'll find that
		// there's too many of them and they can't fit in the same slot. 
		// As this isn't an issue with real use cases, we can ignore it.
		_ITEM_DATA *pItem = GetItem(pos);
		if (pItem->nNum == 0)
			bReqSlots++; // new item? new required slot.

		// Also take into account weight (not official behaviour)
		nReqWeight += pTable->m_sWeight;
	}

	// Holding too much already?
	/*if (m_sItemWeight + nReqWeight > m_sMaxWeight)
	return false;*/
	// Chaotic Generator ağırlık bugu

	if (isTrading() || isMerchanting() || isMining() || m_bMerchantStatex)
		return false;
	// Do we have enough slots?
	return (bFreeSlots >= bReqSlots);
}

bool CUser::CheckSpecialExchange(int nExchangeID) {
	// Does the exchange exist?

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return false;

	_ITEM_MIX * pExchange = g_pMain->m_ItemMixArray.GetData(nExchangeID);
	if (pExchange == nullptr)
		return false;
	uint16 sFreeSlot = -1;

	// Find free slots in the inventory, so that we can check against this later.
	uint8 bFreeSlots = 0;
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++) {
		if (m_sItemArray[i].nNum == 0
			&& ++bFreeSlots >= ITEMS_IN_SPECIAL_EXCHANGE_GROUP)
			break;
	}

	// Add up the rates for this exchange to obtain a total percentage
	int nTotalPercent = 0;
	for (int i = 0; i < ITEMS_IN_SPECIAL_EXCHANGE_GROUP; i++)
		nTotalPercent += pExchange->sExchangeItemCount[i];

	if (nTotalPercent > 9000)
		return (bFreeSlots > 0);

	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++) {
		if (m_sItemArray[i].nNum == 0) {
			sFreeSlot = i;
			break;
		}
	}

	if (sFreeSlot < 0) {
		return false;
	}

	// Can we hold all of these items? If we can't, we have a problem.
	uint8 bReqSlots = 0;
	uint32 nReqWeight = 0;
	for (int i = 0; i < ITEMS_IN_SPECIAL_EXCHANGE_GROUP; i++) {
		uint32 nItemID = pExchange->nExchangeItemNum[i];

		// Does the item exist? If not, we'll ignore it (NOTE: not official behaviour).
		_ITEM_TABLE * pTable = nullptr;
		if (nItemID == 0
			|| (pTable = g_pMain->GetItemPtr(nItemID)) == nullptr)
			continue;

		// Try to find a slot for the item.
		// If we can't find an applicable slot with our inventory as-is,
		// there's no point even checking further.
		int pos;
		if ((pos = FindSlotForItem(nItemID, 1)) < 0)
			return false;

		// Now that we have our slot, see if it's in use (i.e. if adding a stackable item)
		// If it's in use, then we don't have to worry about requiring an extra slot for this item.
		// The only caveat here is with having multiple of the same stackable item: 
		// theoretically we could give them OK, but when it comes time to adding them, we'll find that
		// there's too many of them and they can't fit in the same slot. 
		// As this isn't an issue with real use cases, we can ignore it.
		_ITEM_DATA *pItem = GetItem(pos);
		if (pItem->nNum == 0)
			bReqSlots++; // new item? new required slot.

		// Also take into account weight (not official behaviour)
		nReqWeight += pTable->m_sWeight;
	}

	// Holding too much already?
	/*if (m_sItemWeight + nReqWeight > m_sMaxWeight)
	return false;*/
	// Chaotic Generator ağırlık bugu

	if (isTrading() || isMerchanting() || isMining() || m_bMerchantStatex)
		return false;
	// Do we have enough slots?
	return (bFreeSlots >= bReqSlots);
}

bool CUser::RunExchange(int nExchangeID, uint16 count) {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return false;


	_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);

	uint16 temp_sOriginItemCount0 = 0;
	uint16 temp_sOriginItemCount1 = 0;
	uint16 temp_sOriginItemCount2 = 0;
	uint16 temp_sOriginItemCount3 = 0;
	uint16 temp_sOriginItemCount4 = 0;

	uint16 temp_sCount = 0;

	if (pExchange != nullptr) {
		uint16 sItemCount[5];
		sItemCount[0] = GetItemCount(pExchange->nOriginItemNum[0]);
		sItemCount[1] = GetItemCount(pExchange->nOriginItemNum[1]);
		sItemCount[2] = GetItemCount(pExchange->nOriginItemNum[2]);
		sItemCount[3] = GetItemCount(pExchange->nOriginItemNum[3]);
		sItemCount[4] = GetItemCount(pExchange->nOriginItemNum[4]);
		temp_sCount = sItemCount[0];

		for (int i = 1; i < 5; i++) {
			if (sItemCount[i] < temp_sCount && sItemCount[i] != 0)
				temp_sCount = sItemCount[i];
		}

		if (temp_sCount >= count)
			temp_sCount = count;

		temp_sOriginItemCount0 = pExchange->nOriginItemNum[0] == 0 ? 0 : (count == 0 ? pExchange->sOriginItemCount[0] : temp_sCount);
		temp_sOriginItemCount1 = pExchange->nOriginItemNum[1] == 0 ? 0 : (count == 0 ? pExchange->sOriginItemCount[1] : temp_sCount);
		temp_sOriginItemCount2 = pExchange->nOriginItemNum[2] == 0 ? 0 : (count == 0 ? pExchange->sOriginItemCount[2] : temp_sCount);
		temp_sOriginItemCount3 = pExchange->nOriginItemNum[3] == 0 ? 0 : (count == 0 ? pExchange->sOriginItemCount[3] : temp_sCount);
		temp_sOriginItemCount4 = pExchange->nOriginItemNum[4] == 0 ? 0 : (count == 0 ? pExchange->sOriginItemCount[4] : temp_sCount);
	}

	if (isTrading() || isMerchanting() || isMining() || m_bMerchantStatex)
		return false;

	if (pExchange == nullptr
		// Is it a valid exchange (do we have room?)
		|| !CheckExchange(nExchangeID)
		// We handle flags from 0-101 only. Anything else is broken.
		|| pExchange->bRandomFlag > 101)
		return false;

	// Do we have all of the required items?
	if (CheckExistItemAnd(
		pExchange->nOriginItemNum[0], temp_sOriginItemCount0,
		pExchange->nOriginItemNum[1], temp_sOriginItemCount1,
		pExchange->nOriginItemNum[2], temp_sOriginItemCount2,
		pExchange->nOriginItemNum[3], temp_sOriginItemCount3,
		pExchange->nOriginItemNum[4], temp_sOriginItemCount4)) {
		// These checks are a little pointless, but remove the required items as well.
		RobItem(pExchange->nOriginItemNum[0], temp_sOriginItemCount0);
		RobItem(pExchange->nOriginItemNum[1], temp_sOriginItemCount1);
		RobItem(pExchange->nOriginItemNum[2], temp_sOriginItemCount2);
		RobItem(pExchange->nOriginItemNum[3], temp_sOriginItemCount3);
		RobItem(pExchange->nOriginItemNum[4], temp_sOriginItemCount4);
	}

	if (pExchange->nOriginItemNum[2] == 900000000) {
		for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++) {
			if (pExchange->nOriginItemNum[i] == 900000000)
				GoldLose(pExchange->sOriginItemCount[i], true);

			if (pExchange->nOriginItemNum[i] != 900000000)
				RobItem(pExchange->nOriginItemNum[i], pExchange->sOriginItemCount[i]);

		}
	}

	// No random element? We're just exchanging x items for y items.
	if (!pExchange->bRandomFlag || pExchange->bRandomFlag == 10 || pExchange->bRandomFlag == 11 || pExchange->bRandomFlag == 12 || pExchange->bRandomFlag == 0) {
		for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++) {
			bool item = false;
			if (count == 0) {
				if (pExchange->nExchangeItemNum[i] == 900000000)//noah
				{
					GoldGain(pExchange->sExchangeItemCount[i], true, false);
					item = true;
				} else if (pExchange->nExchangeItemNum[i] == 900001000)//exp
				{
					ExpChange(pExchange->sExchangeItemCount[i], true);
					item = true;
				} else if (pExchange->nExchangeItemNum[i] == 900002000 || pExchange->nExchangeItemNum[i] == 900003000)//Country CONT Ladder Point
				{
					SendLoyaltyChange(pExchange->sExchangeItemCount[i], false);
					item = true;
				} else if (pExchange->nExchangeItemNum[i] == 900004000   //random
					|| pExchange->nExchangeItemNum[i] == 900005000 //hunt
					|| pExchange->nExchangeItemNum[i] == 900006000 //Jobchange
					|| pExchange->nExchangeItemNum[i] == 900007000 //Skill
					|| pExchange->nExchangeItemNum[i] == 900008000 //Killopponentcountry
					|| pExchange->nExchangeItemNum[i] == 900009000 //Transport
					|| pExchange->nExchangeItemNum[i] == 900010000 //LevelUp
					|| pExchange->nExchangeItemNum[i] == 900011000 //War
					) {
					item = true;
				}

				else if (!item) {
					GiveItem(pExchange->nExchangeItemNum[i], pExchange->sExchangeItemCount[i]);
				}
			} else if (pExchange->nExchangeItemNum[i] == 900000000)//noah
			{
				GoldGain(pExchange->sExchangeItemCount[i] * count, true, false);
				item = true;
			} else if (pExchange->nExchangeItemNum[i] == 900001000)//exp
			{
				ExpChange(pExchange->sExchangeItemCount[i] * count, true);
				item = true;
			} else if (pExchange->nExchangeItemNum[i] == 900002000 || pExchange->nExchangeItemNum[i] == 900003000)//Country CONT Ladder Point
			{
				SendLoyaltyChange(pExchange->sExchangeItemCount[i] * count, false);
				item = true;
			} else
				GiveItem(pExchange->nExchangeItemNum[i], temp_sCount);

			V3_QuestShowGiveItem(pExchange->nExchangeItemNum[0], pExchange->sExchangeItemCount[0],
				pExchange->nExchangeItemNum[1], pExchange->sExchangeItemCount[1],
				pExchange->nExchangeItemNum[2], pExchange->sExchangeItemCount[2],
				pExchange->nExchangeItemNum[3], pExchange->sExchangeItemCount[3],
				pExchange->nExchangeItemNum[4], pExchange->sExchangeItemCount[4]);
		}
	} else if (pExchange->bRandomFlag == 20) {
		if (PremiumID > 0) {
			if (pExchange->nExchangeItemNum[3] == 900001000)
				ExpChange(pExchange->sExchangeItemCount[3], true);
			V3_QuestShowGiveItem(pExchange->nExchangeItemNum[3], pExchange->sExchangeItemCount[3], 0, 0, 0, 0, 0, 0, 0, 0);
		} else {
			if (pExchange->nExchangeItemNum[0] == 900001000)
				ExpChange(pExchange->sExchangeItemCount[0], true);
			V3_QuestShowGiveItem(pExchange->nExchangeItemNum[0], pExchange->sExchangeItemCount[0], 0, 0, 0, 0, 0, 0, 0, 0);
		}
	} else if (pExchange->bRandomFlag == 30) {
		if (PremiumID > 0) {
			if (pExchange->nExchangeItemNum[3] == 900001000)
				ExpChange(pExchange->sExchangeItemCount[3], true);
			V3_QuestShowGiveItem(pExchange->nExchangeItemNum[3], pExchange->sExchangeItemCount[3], 0, 0, 0, 0, 0, 0, 0, 0);
		} else {
			if (pExchange->nExchangeItemNum[0] == 900001000)
				ExpChange(pExchange->sExchangeItemCount[0], true);
			V3_QuestShowGiveItem(pExchange->nExchangeItemNum[0], pExchange->sExchangeItemCount[0], 0, 0, 0, 0, 0, 0, 0, 0);
		}
	}
	// For these items the rate set by bRandomFlag.
	else if (pExchange->bRandomFlag <= 100) {
		int rand = myrand(0, 1000 * pExchange->bRandomFlag) / 1000;
		if (rand >= 5)
			rand = 4;

		if (rand <= 4)
			GiveItem(pExchange->nExchangeItemNum[rand], pExchange->sExchangeItemCount[rand]);
	}
	// For 101, the rates are determined by sExchangeItemCount.
	else if (pExchange->bRandomFlag == 101) {
		uint32 nTotalPercent = 0;
		for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
			nTotalPercent += pExchange->sExchangeItemCount[i];

		// If they add up to more than 100%, 
		if (nTotalPercent > 10000) {
			TRACE("Exchange %d is invalid. Rates add up to more than 100%% (%d%%)", nExchangeID, nTotalPercent / 100);
			return false;
		}

		// Holy stack batman! We're just going ahead and copying official for now.
		// NOTE: Officially they even use 2 bytes per element. Yikes.
		uint8 bRandArray[10000];
		memset(&bRandArray, 0, sizeof(bRandArray)); // default to 0 in case it's lower than 100% (in which case, first item's rate increases)

		// Copy the counts, as we're going to adjust them locally.
		uint16 sExchangeCount[ITEMS_IN_EXCHANGE_GROUP];

		memcpy(&sExchangeCount, &pExchange->sExchangeItemCount, sizeof(pExchange->sExchangeItemCount));

		// Build array of exchange item slots (0-4)
		int offset = 0;
		for (int n = 0, i = 0; n < ITEMS_IN_EXCHANGE_GROUP; n++) {
			if (sExchangeCount[n] > 0) {
				memset(&bRandArray[offset], n, sExchangeCount[n]);
				offset += sExchangeCount[n];
			}
		}

		// Pull our exchange item slot out of our hat (the array we generated).
		uint8 bRandSlot = bRandArray[myrand(0, 9999)];
		uint32 nItemID = pExchange->nExchangeItemNum[bRandSlot];

		// Finally, give our item.
		GiveItem(nItemID, 1);

		V3_QuestShowGiveItem(nItemID, 1, 0, 0, 0, 0, 0, 0, 0, 0);
	}

	return true;
}

bool CUser::RunSelectExchange(int nExchangeID, uint32 Count) {
	if (isDead() || isTrading() || isStoreOpen() || isMerchanting() || isSellingMerchant() || isBuyingMerchant() || isMining())
		return false;

	if (bMenuID >= 0 && bySelectedReward == -1) {
		_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);

		if (pExchange == nullptr || !CheckExchange(nExchangeID))
			return false;

		// These checks are a little pointless, but remove the required items as well.
		RobItem(pExchange->nOriginItemNum[bMenuID], pExchange->sOriginItemCount[bMenuID]);

		// No random element? We're just exchanging x items for y items.
		if (!pExchange->bRandomFlag
			|| pExchange->bRandomFlag == 10
			|| pExchange->bRandomFlag == 11
			|| pExchange->bRandomFlag == 12
			|| pExchange->bRandomFlag == 20
			|| pExchange->bRandomFlag == 0) {
			bool m_ItemExchange = false;
			if (pExchange->nExchangeItemNum[bMenuID] == 900000000)//noah
			{
				GoldGain(pExchange->sExchangeItemCount[bMenuID]);
				m_ItemExchange = true;
			} else if (pExchange->nExchangeItemNum[bMenuID] == 900001000)//exp
			{
				ExpChange(pExchange->sExchangeItemCount[bMenuID], true);
				m_ItemExchange = true;
			} else if (pExchange->nExchangeItemNum[bMenuID] == 900002000 || pExchange->nExchangeItemNum[bMenuID] == 900003000)//Country CONT Ladder Point
			{
				SendLoyaltyChange(pExchange->sExchangeItemCount[bMenuID]);
				m_ItemExchange = true;
			} else if (pExchange->nExchangeItemNum[bMenuID] == 900004000   //random
				|| pExchange->nExchangeItemNum[bMenuID] == 900005000 //hunt
				|| pExchange->nExchangeItemNum[bMenuID] == 900006000 //Jobchange
				|| pExchange->nExchangeItemNum[bMenuID] == 900007000 //Skill
				|| pExchange->nExchangeItemNum[bMenuID] == 900008000 //Killopponentcountry
				|| pExchange->nExchangeItemNum[bMenuID] == 900009000 //Transport
				|| pExchange->nExchangeItemNum[bMenuID] == 900010000 //LevelUp
				|| pExchange->nExchangeItemNum[bMenuID] == 900011000 //War
				) {
				m_ItemExchange = true;
			}

			else if (!m_ItemExchange) {
				GiveItem(pExchange->nExchangeItemNum[bMenuID], pExchange->sExchangeItemCount[bMenuID]);
			}
		}

		_ITEM_EXCHANGE_EXP * pExchangeExp = g_pMain->m_ItemExchangeExpArray.GetData(nExchangeID);

		if (pExchangeExp == nullptr)
			return false;

		// No random element? We're just exchanging x items for y items.
		if (!pExchangeExp->bRandomFlag
			|| pExchangeExp->bRandomFlag == 10
			|| pExchangeExp->bRandomFlag == 11
			|| pExchangeExp->bRandomFlag == 12
			|| pExchangeExp->bRandomFlag == 20
			|| pExchangeExp->bRandomFlag == 0) {
			for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++) {
				bool m_ItemExchangeExp = false;
				if (pExchangeExp->nExchangeItemNum[i] == 900000000)//noah
				{
					GoldGain(pExchangeExp->sExchangeItemCount[i]);
					m_ItemExchangeExp = true;
				} else if (pExchangeExp->nExchangeItemNum[i] == 900001000)//exp
				{
					ExpChange(pExchangeExp->sExchangeItemCount[i], true);
					m_ItemExchangeExp = true;
				} else if (pExchangeExp->nExchangeItemNum[i] == 900002000
					|| pExchangeExp->nExchangeItemNum[i] == 900003000)//Country CONT Ladder Point
				{
					SendLoyaltyChange(pExchangeExp->sExchangeItemCount[i]);
					m_ItemExchangeExp = true;
				} else if (pExchangeExp->nExchangeItemNum[i] == 900004000   //random
					|| pExchangeExp->nExchangeItemNum[i] == 900005000 //hunt
					|| pExchangeExp->nExchangeItemNum[i] == 900006000 //Jobchange
					|| pExchangeExp->nExchangeItemNum[i] == 900007000 //Skill
					|| pExchangeExp->nExchangeItemNum[i] == 900008000 //Killopponentcountry
					|| pExchangeExp->nExchangeItemNum[i] == 900009000 //Transport
					|| pExchangeExp->nExchangeItemNum[i] == 900010000 //LevelUp
					|| pExchangeExp->nExchangeItemNum[i] == 900011000 //War
					) {
					m_ItemExchangeExp = true;
				}

				else if (!m_ItemExchangeExp) {
					GiveItem(pExchangeExp->nExchangeItemNum[i], pExchangeExp->sExchangeItemCount[i]);

					V3_QuestShowGiveItem(pExchangeExp->nExchangeItemNum[0], pExchangeExp->sExchangeItemCount[0],
						pExchangeExp->nExchangeItemNum[1], pExchangeExp->sExchangeItemCount[1],
						pExchangeExp->nExchangeItemNum[2], pExchangeExp->sExchangeItemCount[2],
						pExchangeExp->nExchangeItemNum[3], pExchangeExp->sExchangeItemCount[3],
						pExchange->nExchangeItemNum[bMenuID], pExchange->sExchangeItemCount[bMenuID]);
				}
			}
		}
	} else if (bMenuID >= 0 && bySelectedReward != -1) {

		_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);

		if (pExchange == nullptr || !CheckExchange(nExchangeID))
			return false;

		_ITEM_EXCHANGE_EXP * pExchangeExp = g_pMain->m_ItemExchangeExpArray.GetData(nExchangeID);

		if (pExchangeExp == nullptr)
			return false;

		// No random element? We're just exchanging x items for y items.
		if (!pExchangeExp->bRandomFlag
			|| pExchangeExp->bRandomFlag == 10
			|| pExchangeExp->bRandomFlag == 11
			|| pExchangeExp->bRandomFlag == 12
			|| pExchangeExp->bRandomFlag == 20
			|| pExchangeExp->bRandomFlag == 0) {
			bool m_ItemExchange = false;
			if (pExchangeExp->nExchangeItemNum[bySelectedReward] == 900000000)//noah
			{
				GoldGain(pExchangeExp->sExchangeItemCount[bySelectedReward]);
				m_ItemExchange = true;
			} else if (pExchangeExp->nExchangeItemNum[bySelectedReward] == 900001000)//exp
			{
				ExpChange(pExchangeExp->sExchangeItemCount[bySelectedReward], true);
				m_ItemExchange = true;
			} else if (pExchangeExp->nExchangeItemNum[bySelectedReward] == 900002000
				|| pExchangeExp->nExchangeItemNum[bySelectedReward] == 900003000)//Country CONT Ladder Point
			{
				SendLoyaltyChange(pExchangeExp->sExchangeItemCount[bySelectedReward]);
				m_ItemExchange = true;
			} else if (pExchangeExp->nExchangeItemNum[bySelectedReward] == 900004000   //random
				|| pExchangeExp->nExchangeItemNum[bySelectedReward] == 900005000 //hunt
				|| pExchangeExp->nExchangeItemNum[bySelectedReward] == 900006000 //Jobchange
				|| pExchangeExp->nExchangeItemNum[bySelectedReward] == 900007000 //Skill
				|| pExchangeExp->nExchangeItemNum[bySelectedReward] == 900008000 //Killopponentcountry
				|| pExchangeExp->nExchangeItemNum[bySelectedReward] == 900009000 //Transport
				|| pExchangeExp->nExchangeItemNum[bySelectedReward] == 900010000 //LevelUp
				|| pExchangeExp->nExchangeItemNum[bySelectedReward] == 900011000 //War
				) {
				m_ItemExchange = true;
			}

			else if (!m_ItemExchange) {

				GiveItem(pExchangeExp->nExchangeItemNum[bySelectedReward], pExchangeExp->sExchangeItemCount[bySelectedReward]);
				V3_QuestShowGiveItem(pExchange->nExchangeItemNum[0], pExchange->sExchangeItemCount[0],
					pExchange->nExchangeItemNum[1], pExchange->sExchangeItemCount[1],
					pExchange->nExchangeItemNum[2], pExchange->sExchangeItemCount[2],
					pExchange->nExchangeItemNum[3], pExchange->sExchangeItemCount[3],
					pExchangeExp->nExchangeItemNum[bySelectedReward], pExchangeExp->sExchangeItemCount[bySelectedReward]);
			}
		}

		_ITEM_EXCHANGE * pExchangeExps = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);

		if (pExchangeExps == nullptr)
			return false;

		// No random element? We're just exchanging x items for y items.
		if (!pExchangeExps->bRandomFlag
			|| pExchangeExps->bRandomFlag == 10
			|| pExchangeExps->bRandomFlag == 11
			|| pExchangeExps->bRandomFlag == 12
			|| pExchangeExps->bRandomFlag == 20
			|| pExchangeExps->bRandomFlag == 0) {
			for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++) {
				bool m_ItemExchangeExps = false;
				if (pExchangeExps->nExchangeItemNum[i] == 900000000)//noah
				{
					GoldGain(pExchangeExps->sExchangeItemCount[i]);
					m_ItemExchangeExps = true;
				} else if (pExchangeExps->nExchangeItemNum[i] == 900001000)//exp
				{
					ExpChange(pExchangeExps->sExchangeItemCount[i], true);
					m_ItemExchangeExps = true;
				} else if (pExchangeExps->nExchangeItemNum[i] == 900002000
					|| pExchangeExps->nExchangeItemNum[i] == 900003000)//Country CONT Ladder Point
				{
					SendLoyaltyChange(pExchangeExps->sExchangeItemCount[i]);
					m_ItemExchangeExps = true;
				} else if (pExchangeExps->nExchangeItemNum[i] == 900004000   //random
					|| pExchangeExps->nExchangeItemNum[i] == 900005000 //hunt
					|| pExchangeExps->nExchangeItemNum[i] == 900006000 //Jobchange
					|| pExchangeExps->nExchangeItemNum[i] == 900007000 //Skill
					|| pExchangeExps->nExchangeItemNum[i] == 900008000 //Killopponentcountry
					|| pExchangeExps->nExchangeItemNum[i] == 900009000 //Transport
					|| pExchangeExps->nExchangeItemNum[i] == 900010000 //LevelUp
					|| pExchangeExps->nExchangeItemNum[i] == 900011000 //War
					) {
					m_ItemExchangeExps = true;
				}

				else if (m_ItemExchangeExps) {
					GiveItem(pExchangeExps->nExchangeItemNum[i], pExchangeExps->sExchangeItemCount[i]);
					V3_QuestShowGiveItem(pExchangeExps->nExchangeItemNum[0], pExchangeExps->sExchangeItemCount[0],
						pExchangeExps->nExchangeItemNum[1], pExchangeExps->sExchangeItemCount[1],
						pExchangeExps->nExchangeItemNum[2], pExchangeExps->sExchangeItemCount[2],
						pExchangeExps->nExchangeItemNum[3], pExchangeExps->sExchangeItemCount[3],
						pExchange->nExchangeItemNum[i], pExchange->sExchangeItemCount[i]);
				}
			}
		}

	}
	return true;
}


uint32 CUser::GetMaxExchange(int nExchangeID) {
	uint16 sResult = 0;
	_ITEM_TABLE * pTable;
	uint16 temp_sCount = 0;

	_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);

	if (pExchange != nullptr) {
		for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++) {
			pTable = g_pMain->GetItemPtr(pExchange->nOriginItemNum[i]);
			if (pTable != nullptr)
				temp_sCount += GetItemCount(pExchange->nOriginItemNum[i]);
		}

		sResult = temp_sCount;
	}

	return sResult;
}

bool CUser::IsValidSlotPos(_ITEM_TABLE* pTable, int destpos) {
	if (pTable == nullptr)
		return false;


	bool bOneHandedItem = false;
	switch (pTable->m_bSlot) {
	case ItemSlot1HEitherHand:
		if (destpos != RIGHTHAND && destpos != LEFTHAND)
			return false;

		bOneHandedItem = true;
		break;

	case ItemSlot1HRightHand:
		if (destpos != RIGHTHAND)
			return false;

		bOneHandedItem = true;
		break;

		// If we're equipping a 2H item in our right hand, there must
		// be no item in our left hand.
	case ItemSlot2HRightHand: //
		if (destpos != RIGHTHAND || GetItem(LEFTHAND)->nNum != 0) {
			_ITEM_DATA *pSrcItem = GetItem(LEFTHAND), *pDstItem = GetItem(RIGHTHAND), tmpItem;
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
		break;

	case ItemSlot1HLeftHand:
		if (destpos != LEFTHAND)
			return false;
		bOneHandedItem = true;
		break;

		// If we're equipping a 2H item in our left hand, there must
		// be no item in our right hand.
	case ItemSlot2HLeftHand://
		if (destpos != LEFTHAND || GetItem(RIGHTHAND)->nNum != 0) {
			_ITEM_DATA *pSrcItem = GetItem(RIGHTHAND), *pDstItem = GetItem(LEFTHAND), tmpItem;
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
		break;

	case ItemSlotPauldron:
		if (destpos != BREAST)
			return false;
		break;

	case ItemSlotPads:
		if (destpos != LEG)
			return false;
		break;

	case ItemSlotHelmet:
		if (destpos != HEAD)
			return false;
		break;

	case ItemSlotGloves:
		if (destpos != GLOVE)
			return false;
		break;

	case ItemSlotBoots:
		if (destpos != FOOT)
			return false;
		break;

	case ItemSlotEarring:
		if (destpos != RIGHTEAR && destpos != LEFTEAR)
			return false;
		break;

	case ItemSlotNecklace:
		if (destpos != NECK)
			return false;
		break;

	case ItemSlotRing:
		if (destpos != RIGHTRING && destpos != LEFTRING)
			return false;
		break;

	case ItemSlotPet:
	case ItemSlotShoulder:
		if (destpos != SHOULDER)
			return false;
		break;

	case ItemSlotBelt:
		if (destpos != WAIST)
			return false;
		break;

	case ItemSlotCospreGloves:
		if (destpos != COSP_GLOVE && destpos != COSP_GLOVE2)
			return false;
		break;

	case ItemSlotCosprePauldron:
		if (destpos != COSP_BREAST)
			return false;
		break;

	case ItemSlotCospreHelmet:
		if (destpos != COSP_HELMET)
			return false;
		break;

	case ItemSlotCospreWings:
		if (destpos != COSP_WINGS)
			return false;
		break;

	case ItemSlotCospreFairy:
		if (destpos != COSP_FAIRY)
			return false;
		break;

	case ItemSlotBag:
		if (destpos != COSP_BAG1 && destpos != COSP_BAG2)
			return false;
		break;
	default:
		return false;
	}

	// 1H items can only be equipped when a 2H item isn't equipped.
	if (bOneHandedItem) {
		_ITEM_DATA * pItem;
		_ITEM_TABLE * pTable2 = GetItemPrototype(destpos == LEFTHAND ? RIGHTHAND : LEFTHAND, pItem);
		if (pTable2 != nullptr && pTable2->is2Handed()) {
			_ITEM_DATA *pSrcItem = GetItem(RIGHTHAND), *pDstItem = GetItem(LEFTHAND), tmpItem;
			memcpy(&tmpItem, pDstItem, sizeof(_ITEM_DATA)); // Temporarily store the target item
			memcpy(pDstItem, pSrcItem, sizeof(_ITEM_DATA)); // Replace the target item with the source
			memcpy(pSrcItem, &tmpItem, sizeof(_ITEM_DATA)); // Now replace the source with the old target (swapping them)
		}
	}

	return true;
}

void CUser::SendStackChangeSpecial(uint32 nItemID, uint32 nCount /* needs to be 4 bytes, not a bug */, uint16 sDurability, uint8 bPos, bool bNewItem /* = false */, uint32 Time, _ITEM_DATA * pItem) {
	Packet result(WIZ_ITEM_COUNT_CHANGE);

	result << uint16(1);
	result << uint8(1);
	result << uint8(bPos);
	result << nItemID << nCount;
	result << uint8(bNewItem ? 100 : 0);
	result << sDurability;

	if (Time != 0) {
		Time = int32(UNIXTIME) + ((60 * 60 * 24) * Time);
	} else {
		Time = 0;
	}

	if (pItem != nullptr) {
		_CYPHERRING_DATA * pRingData = g_pMain->GetCypherRingPtr(pItem->nSerialNum);
		CPet * pPet = g_pMain->GetPetPtr(pItem->nSerialNum);
		if (pRingData != nullptr)
			result << pRingData->ID;
		if (pPet != nullptr)
			result << pPet->SpecialPetID;
	}

	if (Time != 0) {
		result << Time << Time;
	}

	SetUserAbility(false);
	SendItemWeight();

	Send(&result);
}


void CUser::SendStackChange(uint32 nItemID, uint32 nCount /* needs to be 4 bytes, not a bug */, uint16 sDurability, uint8 bPos, bool bNewItem /* = false */, uint32 Time) {
	Packet result(WIZ_ITEM_COUNT_CHANGE);

	result << uint16(1);
	result << uint8(1);
	result << uint8(bPos);
	result << nItemID << nCount;
	result << uint8(bNewItem ? 100 : 0);
	result << sDurability;

	if (Time != 0) {
		Time = int32(UNIXTIME) + ((60 * 60 * 24) * Time);
	} else {
		Time = 0;
	}



	if (Time != 0) {
		result << Time << Time;
	}

	SetUserAbility(false);
	SendItemWeight();

	Send(&result);
}
void CUser::ItemRemove(Packet & pkt) {
	Packet result(WIZ_ITEM_REMOVE);
	_ITEM_DATA * pItem;
	uint8 bType, bPos;
	uint32 nItemID;

	pkt >> bType >> bPos >> nItemID;

	// Inventory
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		goto fail_return;
	else if (bType == 0) {
		if (bPos >= HAVE_MAX)
			goto fail_return;

		bPos += SLOT_MAX;
	}
	// Equipped items
	else if (bType == 1) {
		if (bPos >= SLOT_MAX)
			goto fail_return;
	} else if (bType == 2) {
		if (bPos >= HAVE_MAX)
			goto fail_return;
		bPos += SLOT_MAX;
	}

	pItem = GetItem(bPos);

	// Make sure the item matches what the client says it is
	if (pItem == nullptr)
		goto fail_return;

	if (pItem->sCount < 1
		|| pItem->nNum != nItemID
		|| pItem->isSealed()
		|| pItem->isRented()
		|| pItem->IsSelling
		|| GetZoneID() == ZONE_CHAOS_DUNGEON)
		goto fail_return;

	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(pItem->nNum);

	if (pTable == nullptr)
		goto fail_return;

	if (pTable->m_iSellPrice != SellTypeFullPrice
		&& !pTable->isStackable()
		&& pItem->nExpirationTime < 1
		&& !pTable->isPet()) {
		g_DBAgent.InsertRepurchase(pItem->nNum, uint32(UNIXTIME + 24 * 60 * 60 * 3), pItem->nSerialNum, this);
	}

	memset(pItem, 0, sizeof(_ITEM_DATA));

	SetUserAbility();
	SendItemWeight();

	result << uint8(1);
	Send(&result);

	return;
fail_return:
	result << uint8(0);
	Send(&result);
}

bool CUser::CheckGiveSlot(uint8 sSlot) {
	// Find free slots in the inventory, so that we can check against this later.

	if (isDead() || isTrading() || isStoreOpen() || isMerchanting() || isSellingMerchant() || isBuyingMerchant() || isMining())
		return false;

	uint8 bFreeSlots = 0;
	for (int i = SLOT_MAX; i < SLOT_MAX + HAVE_MAX; i++) {
		if (GetItem(i)->nNum == 0 && ++bFreeSlots >= sSlot)
			break;
	}

	// Can we hold all of these items? If we can't, we have a problem.
	uint8 bReqSlots = 0;
	for (int i = 0; i < sSlot; i++)
		bReqSlots++; // new item? new required slot.

	if (bFreeSlots < bReqSlots)
		return false;

	// Do we have enough slots?
	return (bFreeSlots >= bReqSlots);
}