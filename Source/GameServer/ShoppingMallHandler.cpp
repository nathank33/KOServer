#include "stdafx.h"
#include "DBAgent.h"

void CUser::ShoppingMall(Packet & pkt) {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	uint8 opcode = pkt.read<uint8>();

	switch (opcode) {
	case STORE_OPEN:
		HandleStoreOpen(pkt);
		break;

	case STORE_CLOSE:
		HandleStoreClose();
		break;

	case STORE_BUY:
	case STORE_MINI: // not sure what this is
	case STORE_PROCESS:
		/* fairly certain there's no need to emulate these as they're unused */
		break;

	case STORE_LETTER:
		LetterSystem(pkt);
		break;

	default:
		TRACE("Unknown shoppingmall packet: %X\n", opcode);
	}
}

// We're opening the PUS...
void CUser::HandleStoreOpen(Packet & pkt) {
	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_OPEN));
	int16 sErrorCode = 1, sFreeSlot = -1;

	if (isDead())
		sErrorCode = -2;
	else if (isTrading())
		sErrorCode = -3;
	else if (isMerchanting() || m_bMerchantStatex)
		sErrorCode = -4;
	// Not allowed in private arenas
	else if (GetZoneID() >= 40 && GetZoneID() <= 45)
		sErrorCode = -5;
	else if (isStoreOpen())
		sErrorCode = -7;
	if (sErrorCode != 1)
		goto fail_return;

	for (int i = SLOT_MAX; i < INVENTORY_TOTAL; i++) {
		if (GetItem(i)->nNum == 0) {
			sFreeSlot = i;
			break;
		}
	}

	if (sFreeSlot < 0) {
		sErrorCode = -8;
		goto fail_return;
	}

	m_bStoreOpen = true;

fail_return:
	result << sErrorCode << sFreeSlot;
	Send(&result);
}

// We're closing the PUS so that we can call LOAD_WEB_ITEMMALL and load the extra items.
void CUser::HandleStoreClose() {
	if (!m_bStoreOpen)
		return;
	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_CLOSE));
	m_bStoreOpen = false;
	g_pMain->AddDatabaseRequest(result, this);
}

void CUser::ReqLoadWebItemMall() {
	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_CLOSE));
	std::vector<_ITEM_DATA> itemList;

	if (!g_DBAgent.LoadWebItemMall(itemList, this))
		return;


	if (int(itemList.size()) > GetEmptySlotCount())
		return;
	else
		g_DBAgent.ClearWebItemMall(this);



	// reuse the GiveItem() method for giving them the item, just don't send the packet
	// as it's handled by STORE_CLOSE.

	foreach(itr, itemList)
		GiveItem(itr->nNum, itr->sCount, true, itr->nExpirationTime);

	for (int i = SLOT_MAX; i < INVENTORY_TOTAL; i++) {
		_ITEM_DATA *pItem = GetItem(i);
		result << pItem->nNum
			<< pItem->sDuration
			<< pItem->sCount
			<< pItem->bFlag	// item type flag (e.g. rented)
			<< pItem->sRemainingRentalTime;
	}

	Send(&result);
}