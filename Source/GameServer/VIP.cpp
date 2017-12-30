#include "stdafx.h"
#include "../shared/globals.h"
#include "../shared/DateTime.h"
#include "DBAgent.h"
#define VIP_VAULT_KEY	800442000

using std::vector;

void CUser::VipStorageOpenRequest(Packet & pkt) {
	uint16 NpcID;
	pkt >> NpcID;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	if (NpcID != 0) {
		auto pNpc = g_pMain->GetNpcPtr(NpcID);
		if (pNpc == nullptr)
			return;

		if (!pNpc->isInRange(this, MAX_NPC_RANGE))
			return;
	}

	if (VIPStoreFalseTrying == 3) {
		Packet result2(WIZ_VIP_STORAGE);
		result2 << uint8(11) // açýlýp açýlmayacaðý
			<< uint8(2)
			<< uint8(VIPStoreFalseTrying);
		Send(&result2);
		return;
	}

	if (VIPStorePassword.length() < 4) {
		Packet result(WIZ_VIP_STORAGE);
		result << uint8(1) << uint8(1);
		Send(&result);
	} else {
		Packet result(WIZ_VIP_STORAGE);
		result << uint8(0x0B) << uint8(1) << uint8(0);
		Send(&result);
	}
}

void CUser::VipStorageOpen(Packet & pkt) {
	std::string Password;
	pkt.SByte();
	pkt >> Password;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	Packet result(WIZ_VIP_STORAGE);
	if (Password != VIPStorePassword) {
		VIPStoreFalseTrying += 1;
		result << uint8(11) // açýlýp açýlmayacaðý
			<< uint8(2)
			<< uint8(VIPStoreFalseTrying);
		Send(&result);
		return;
	}

	VIPStoreFalseTrying = 0;
	result << uint8(1) << uint8(1) << uint8(1);

	if (nVIPExpirationTime > 0)
		result << uint8(1) << uint32(nVIPExpirationTime + 30 * DAY) << uint8(0) << uint32(0) << uint8(0) << uint32(0) << uint8(0) << uint32(0);
	else
		result << uint8(0) << uint32(0) << uint8(0) << uint32(0) << uint8(0) << uint32(0) << uint8(0) << uint32(0);

	for (size_t i = 0; i < MAX_SLOT_VIP_STORAGE; i++) {
		auto pSlot = GetVipStorageSlot((uint8) i);

		if (!pSlot)
			result << uint64(0) << uint64(0) << uint8(0);
		else {
			result << pSlot->nNum << pSlot->sDuration << pSlot->sCount << pSlot->bFlag;
			SetSpecialItemData(pSlot, result);
			result << pSlot->nExpirationTime;
		}
	}
	Send(&result);
}

void CUser::VipStorageItemInput(Packet & pkt) {
	Packet result(WIZ_VIP_STORAGE, uint8(2));
	uint8 bResult = true;
	uint32 ItemID;
	uint16 NpcID, Count;
	uint8 Page, SourcePosition, DestinationPosition;

	pkt >> NpcID >> ItemID >> Page >> SourcePosition >> DestinationPosition >> Count;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	if (NpcID != 0) {
		auto pNpc = g_pMain->GetNpcPtr(NpcID);

		if (pNpc == nullptr) {
			bResult = 2;
			goto SendPacket;
		}
	}

	if (SourcePosition > HAVE_MAX || DestinationPosition > MAX_SLOT_VIP_STORAGE) {
		bResult = 2;
		goto SendPacket;
	}

	auto pItem = g_pMain->GetItemPtr(ItemID);
	if (pItem == nullptr) {
		bResult = 2;
		goto SendPacket;
	}

	auto pInventoryItem = GetItem(SLOT_MAX + SourcePosition);

	if (pInventoryItem == nullptr || ItemID >= ITEM_NO_TRADE || pInventoryItem->nNum != ItemID || pInventoryItem->isRented() || pInventoryItem->isDuplicate()) {
		bResult = 2;
		goto SendPacket;
	}

	auto pVipStorAgeSlot = GetVipStorageSlot(DestinationPosition);

	if ((!pItem->isStackable() && pVipStorAgeSlot->nNum != 0)
		|| (pItem->isStackable() && pVipStorAgeSlot->nNum != 0 && pVipStorAgeSlot->nNum != pInventoryItem->nNum)
		|| pInventoryItem->sCount < Count) {
		bResult = 2;
		goto SendPacket;
	}

	if (pVipStorAgeSlot->sCount <= 0 || pVipStorAgeSlot->nNum == 0)
		memset(pVipStorAgeSlot, 0, sizeof(_ITEM_DATA));

	pVipStorAgeSlot->nNum = pInventoryItem->nNum;
	pVipStorAgeSlot->sDuration = pInventoryItem->sDuration;

	if (pItem->isStackable())
		pVipStorAgeSlot->sCount += (uint16) Count;
	else
		pVipStorAgeSlot->sCount = (uint16) Count;

	if (pItem->isStackable())
		pInventoryItem->sCount -= (uint16) Count;
	else
		pInventoryItem->sCount = 0;

	pVipStorAgeSlot->bFlag = pInventoryItem->bFlag;
	pVipStorAgeSlot->sRemainingRentalTime = pInventoryItem->sRemainingRentalTime;
	pVipStorAgeSlot->nExpirationTime = pInventoryItem->nExpirationTime;

	if (!pItem->isStackable() || Count == pVipStorAgeSlot->sCount)
		pVipStorAgeSlot->nSerialNum = pInventoryItem->nSerialNum;

	if (!pItem->isStackable() && pVipStorAgeSlot->nSerialNum == 0)
		pVipStorAgeSlot->nSerialNum = g_pMain->GenerateItemSerial();

	if (pInventoryItem->sCount <= 0)
		memset(pInventoryItem, 0, sizeof(_ITEM_DATA));

	SetUserAbility(false);
	SendItemWeight();

SendPacket:
	result << uint8(bResult);
	Send(&result);
}

void CUser::VipStorageItemOutput(Packet & pkt) {
	Packet result(WIZ_WAREHOUSE, uint8(3));
	uint8 bResult = 1;

	uint32 ItemID;
	uint16 NpcID, Count;
	uint8 Page, SourcePosition, DestinationPosition;

	pkt >> NpcID >> ItemID >> Page >> SourcePosition >> DestinationPosition >> Count;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	if (NpcID != 0) {
		auto pNpc = g_pMain->GetNpcPtr(NpcID);

		if (pNpc == nullptr) {
			bResult = 2;
			goto SendPacket;
		}
	}

	if (SourcePosition > MAX_SLOT_VIP_STORAGE || DestinationPosition > HAVE_MAX) {
		bResult = 2;
		goto SendPacket;
	}

	auto pItem = g_pMain->GetItemPtr(ItemID);
	if (pItem == nullptr) {
		bResult = 2;
		goto SendPacket;
	}

	auto pVipStorAgeSlot = GetVipStorageSlot(SourcePosition);

	if (pVipStorAgeSlot == nullptr
		|| ItemID >= ITEM_NO_TRADE
		|| pVipStorAgeSlot->nNum != ItemID
		|| pVipStorAgeSlot->nNum == 0/* || !CheckWeight(pItem, ItemID, (uint16)Count)*/) {
		bResult = 2;
		goto SendPacket;
	}

	auto pInventoryItem = GetItem(SLOT_MAX + DestinationPosition);

	if ((!pItem->isStackable() && pInventoryItem->nNum != 0)
		|| (pItem->isStackable() && pInventoryItem->nNum != 0 && pInventoryItem->nNum != pVipStorAgeSlot->nNum)
		|| pVipStorAgeSlot->sCount < Count) {
		bResult = 2;
		goto SendPacket;
	}

	if (pInventoryItem->sCount <= 0 || pInventoryItem->nNum == 0)
		memset(pInventoryItem, 0, sizeof(_ITEM_DATA));

	pInventoryItem->nNum = pVipStorAgeSlot->nNum;
	pInventoryItem->sDuration = pVipStorAgeSlot->sDuration;

	if (pItem->isStackable())
		pInventoryItem->sCount += (uint16) Count;
	else
		pInventoryItem->sCount = (uint16) Count;

	if (pItem->isStackable())
		pVipStorAgeSlot->sCount -= (uint16) Count;
	else
		pVipStorAgeSlot->sCount = 0;

	pInventoryItem->bFlag = pVipStorAgeSlot->bFlag;
	pInventoryItem->sRemainingRentalTime = pVipStorAgeSlot->sRemainingRentalTime;
	pInventoryItem->nExpirationTime = pVipStorAgeSlot->nExpirationTime;

	if (!pItem->isStackable() || Count == pInventoryItem->sCount)
		pInventoryItem->nSerialNum = pVipStorAgeSlot->nSerialNum;

	if (!pItem->isStackable() && pInventoryItem->nSerialNum == 0)
		pInventoryItem->nSerialNum = g_pMain->GenerateItemSerial();

	if (pVipStorAgeSlot->sCount <= 0)
		memset(pVipStorAgeSlot, 0, sizeof(_ITEM_DATA));

	SetUserAbility(false);
	SendItemWeight();

SendPacket:
	result << uint8(bResult);
	Send(&result);
}

void CUser::VipStorageItemMove(Packet & pkt) {
	Packet result(WIZ_VIP_STORAGE, uint8(4));
	auto bResult = true;

	uint32 ItemID;
	uint16 NpcID;
	uint8 Page, SourcePosition, DestinationPosition;

	pkt >> NpcID >> ItemID >> Page >> SourcePosition >> DestinationPosition;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	if (NpcID != 0) {
		auto pNpc = g_pMain->GetNpcPtr(NpcID);
		if (pNpc == nullptr) {
			bResult = false;
			goto SendPacket;
		}
	}

	auto pItem = g_pMain->GetItemPtr(ItemID);
	if (pItem == nullptr) {
		bResult = false;
		goto SendPacket;
	}

	auto pSourceItem = GetVipStorageSlot(SourcePosition);
	auto pDestinationItem = GetVipStorageSlot(DestinationPosition);

	if (pSourceItem->nNum != ItemID || pDestinationItem->nNum != 0) {
		bResult = false;
		goto SendPacket;
	}

	memcpy(pDestinationItem, pSourceItem, sizeof(_ITEM_DATA));
	memset(pSourceItem, 0, sizeof(_ITEM_DATA));

SendPacket:
	result << uint8(bResult);
	Send(&result);
}

void CUser::VipStorageInventoryItemMove(Packet & pkt) {
	Packet result(WIZ_VIP_STORAGE, uint8(5));
	auto bResult = true;

	uint32 ItemID;
	uint16 NpcID;
	uint8 Page, SourcePosition, DestinationPosition;

	pkt >> NpcID >> ItemID >> Page >> SourcePosition >> DestinationPosition;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	if (NpcID != 0) {
		auto pNpc = g_pMain->GetNpcPtr(NpcID);
		if (pNpc == nullptr) {
			bResult = false;
			goto SendPacket;
		}
	}

	auto pItem = g_pMain->GetItemPtr(ItemID);
	if (pItem == nullptr) {
		bResult = false;
		goto SendPacket;
	}

	auto pSourceItem = GetItem(SLOT_MAX + SourcePosition);
	auto pDestinationItem = GetItem(SLOT_MAX + DestinationPosition);

	if (pSourceItem->nNum != ItemID || pDestinationItem->nNum != 0) {
		bResult = false;
		goto SendPacket;
	}

	memcpy(pDestinationItem, pSourceItem, sizeof(_ITEM_DATA));
	memset(pSourceItem, 0, sizeof(_ITEM_DATA));

SendPacket:
	result << uint8(bResult);
	Send(&result);
}

void CUser::VipStorageUseKey(Packet & pkt) {
	Packet result(WIZ_VIP_STORAGE, uint8(6));
	/*
	2 - You have failed to use [VIP]Storage Key.
	4 - It is not [VIP]Storage Key.
	5 - You cannot extent the period if the storage has more than 60 days.
	*/
	uint8 ReturnValue = 1;
	uint16 NpcID;
	uint32 ItemID;
	uint8 UnKnow1, UnKnow2, SourcePosition;

	pkt >> NpcID >> ItemID >> UnKnow1 >> SourcePosition >> UnKnow2;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	if (VIPStorePassword.length() < 4) {
		ReturnValue = 2;
		goto failed_return;
	}

	if (ItemID != VIP_VAULT_KEY) {
		ReturnValue = 4;
		goto failed_return;
	}
	_ITEM_DATA *pItem = GetItem(SLOT_MAX + SourcePosition);

	if (pItem == nullptr) {
		ReturnValue = 2;
		goto failed_return;
	}

	_ITEM_TABLE *pTable = g_pMain->GetItemPtr(pItem->nNum);

	if (pTable == nullptr) {
		ReturnValue = 2;
		goto failed_return;
	}

	if (pItem->nNum != VIP_VAULT_KEY) {
		ReturnValue = 4;
		goto failed_return;
	}

	if (nVIPExpirationTime > 0) {
		if (nVIPExpirationTime + (DAY * 30) > (UNIXTIME + (DAY * 60))) {
			ReturnValue = 5;
			goto failed_return;
		}

		nVIPExpirationTime += (DAY * 30);
	} else
		nVIPExpirationTime += uint32(UNIXTIME + 24 * 60 * 60 * 30);

	RobItem(SourcePosition + SLOT_MAX, pTable, 1);
	memset(pItem, 0, sizeof(pItem));

	result << ReturnValue << uint8(1) << uint32(nVIPExpirationTime);
	Send(&result);
	return;
failed_return:
	result << ReturnValue;
	Send(&result);
}
void CUser::VipStorageNewPassword(Packet & pkt) {
	if (VIPStorePassword.size() == 1 || VIPStorePassword.size() < 4 || VIPStorePassword.size() > 4)
		VipStorageChangePassword(pkt);
	else
		return;
}

void CUser::VipStorageChangePassword(Packet & pkt) {
	Packet result(WIZ_VIP_STORAGE, uint8(8));
	std::string Password;
	uint8 ReturnValue = 2;

	pkt.SByte();
	pkt >> Password;
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	if (Password.size() == 0 || Password.size() < 4 || Password.size() > 4) {
		ReturnValue = 2;
		goto failed_return;
	}

	VIPStoreFalseTrying = 0;
	VIPStorePassword = Password;

	g_DBAgent.UpdateVIPStoragePassword(GetName(), VIPStorePassword);
	ReturnValue = 1;
	goto failed_return;
	/*
	1 - Password change has set. VIP Storage will be closed.
	2 - Password change has failed.
	*/

failed_return:
	result << ReturnValue;
	Send(&result);
}