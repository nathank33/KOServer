#include "stdafx.h"
#include "../shared/DateTime.h"
#include "DBAgent.h"
#include "KingSystem.h"
#include "Map.h"
#include <cmath>

// Some item ID definitions
#define MIN_ITEM_ID 100000000
#define MAX_ITEM_ID 999999999

#define MAGE_EARRING 310310004
#define WARRIOR_EARRING 310310005
#define ROGUE_EARRING 310310006
#define PRIEST_EARRING 310310007

#define UPGRADE_RUBY_EARRING_MIN 310110005
#define UPGRADE_RUBY_EARRING_MAX 310110007

#define UPGRADE_PEARL_EARRING_MIN 310150005
#define UPGRADE_PEARL_EARRING_MAX 310150007

#define SHADOW_PIECE			700009000
#define TRINA_PIECE				700002000
#define KRAVDIS_PIECE			379258000

/**
* @brief	Packet handler for the assorted systems that
* 			were deemed to come under the 'upgrade' system.
*
* @param	pkt	The packet.
*/
void CUser::ItemUpgradeProcess(Packet & pkt) {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isStoreOpen()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	uint8 opcode = pkt.read<uint8>();

	switch (opcode) {
	case ITEM_UPGRADE:
		ItemUpgrade(pkt);
		break;

	case ITEM_ACCESSORIES:
		ItemUpgradeAccessories(pkt);
		break;

	case ITEM_BIFROST_EXCHANGE:
		BifrostPieceProcess(pkt);
		break;

	case ITEM_PET:
		ItemUpgradePet(pkt);
		break;

	case ITEM_UPGRADE_REBIRTH:
		ItemUpgradeRebirth(pkt);
		break;

	case ITEM_SEAL:
		ItemSealProcess(pkt);
		break;

	case ITEM_CHARACTER_SEAL:
		CharacterSealProcess(pkt);
		break;

	case ITEM_SPECIAL_EXCHANGE:
		SpecialItemExchange(pkt);
		break;

	case ITEM_DISASSEMBLING:
		ItemDisassembling(pkt);
		break;

	}
}

/**
* @brief	Packet handler for the standard item upgrade system.
*
* @param	pkt	The packet.
*/
void CUser::ItemUpgrade(Packet & pkt, uint8 nUpgradeType) {
	bool Follow = false;
	enum UpgradeErrorCodes {
		UpgradeFailed = 0,
		UpgradeSucceeded = 1,
		UpgradeTrading = 2,
		UpgradeNeedCoins = 3,
		UpgradeNoMatch = 4,
		UpgradeRental = 5
	};

	enum UpgradeType { UpgradeTypeNormal = 1, UpgradeTypePreview = 2 };

	Packet result(WIZ_ITEM_UPGRADE, nUpgradeType);
	_ITEM_DATA  * pOriginItem;
	_ITEM_TABLE * proto;
	int32 nItemID[10]; int8 bPos[10];
	uint16 sNpcID;
	int8 bType = UpgradeTypeNormal, bResult = UpgradeNoMatch, ItemClass = 0;
	bool Trina = false, Karivdis = false, Accessories = false;
	DateTime time;

	if (isDead() || isTrading() || isStoreOpen() || isMerchanting() || isSellingMerchant() || isBuyingMerchant() || isMining() || m_bMerchantStatex) {
		bResult = UpgradeTrading;
		goto fail_return;
	}

	pkt >> bType; // either preview or upgrade, need to allow for these types

	if (bType == UpgradeTypePreview) {
		bResult = UpgradeFailed;
		goto fail_return;
	}

	pkt >> sNpcID;
	for (int i = 0; i < 10; i++) {
		pkt >> nItemID[i] >> bPos[i];

		// Invalid slot ID
		if (bPos[i] != -1 && bPos[i] >= HAVE_MAX)
			return;
	}
	//printf("bPos1:%d,bPos2:%d,bPos3:%d,bPos4:%d,bPos5:%d\n",bPos[0],bPos[1],bPos[2],bPos[3],bPos[4]);
	//printf("nID1:%d,nID2:%d,nID3:%d,nID4:%d,nID5:%d\n",nItemID[0],nItemID[1],nItemID[2],nItemID[3],nItemID[4]);

	pOriginItem = GetItem(SLOT_MAX + bPos[0]);
	if (pOriginItem->nNum != nItemID[0]
		|| (proto = g_pMain->GetItemPtr(nItemID[0])) == nullptr)
		goto fail_return; // error with error code UpgradeNoMatch ("Items required for upgrade do not match")
	else if (pOriginItem->isRented()
		|| pOriginItem->isSealed()
		|| pOriginItem->nExpirationTime > 0) // unsure if there's another error code for sealed items
	{
		bResult = UpgradeRental;
		goto fail_return;
	}

	// Invalid item in slot.
	for (int x = 0; x < 10; x++) {
		if (bPos[x] != -1 && (nItemID[x] > 0 && nItemID[x] != GetItem(SLOT_MAX + bPos[x])->nNum))
			goto fail_return;
	}

	{ // scoped lock to prevent race conditions
		int nReqOriginItem = nItemID[0] % 100000;
		_ITEM_UPGRADE * pUpgrade = nullptr;
		foreach_stlmap_nolock(itr, g_pMain->m_ItemUpgradeArray) {
			pUpgrade = itr->second;

			if (pUpgrade->sOriginItem != nReqOriginItem)
				continue;



			if (((nItemID[0] / MIN_ITEM_ID) != 11) &&
				(nItemID[0] / MIN_ITEM_ID) != pUpgrade->nIndex / 100000 && pUpgrade->nIndex < 300000)
				continue;




			if (nItemID[1] == TRINA_PIECE || nItemID[2] == TRINA_PIECE)
				Trina = true;
			if (nItemID[1] == KRAVDIS_PIECE || nItemID[2] == KRAVDIS_PIECE)
				Karivdis = true;




			if (((nItemID[1] == TRINA_PIECE || nItemID[2] == TRINA_PIECE) && (pUpgrade->bRateType == 4))
				|| (nItemID[1] == KRAVDIS_PIECE || nItemID[2] == KRAVDIS_PIECE) && (pUpgrade->bRateType != 4))
				continue;


			if (pUpgrade->bRateType == 1)
				if (nItemID[1] == 379221000 ||
					nItemID[1] == 379222000 ||
					nItemID[1] == 379223000 ||
					nItemID[1] == 379224000 ||
					nItemID[1] == 379225000 ||
					nItemID[1] == 379226000 ||
					nItemID[1] == 379227000 ||
					nItemID[1] == 379228000 ||
					nItemID[1] == 379229000 ||
					nItemID[1] == 379230000 ||
					nItemID[1] == 379231000 ||
					nItemID[1] == 379232000 ||
					nItemID[1] == 379233000 ||
					nItemID[1] == 379234000 ||
					nItemID[1] == 379235000 ||
					nItemID[1] == 379255000 ||
					nItemID[2] == 379221000 ||
					nItemID[2] == 379222000 ||
					nItemID[2] == 379223000 ||
					nItemID[2] == 379224000 ||
					nItemID[2] == 379225000 ||
					nItemID[2] == 379226000 ||
					nItemID[2] == 379227000 ||
					nItemID[2] == 379228000 ||
					nItemID[2] == 379229000 ||
					nItemID[2] == 379230000 ||
					nItemID[2] == 379231000 ||
					nItemID[2] == 379232000 ||
					nItemID[2] == 379233000 ||
					nItemID[2] == 379234000 ||
					nItemID[2] == 379235000 ||
					nItemID[2] == 379255000)
					ItemClass = 1;
				else
					continue;

			if (pUpgrade->bRateType == 2)
				if (
					nItemID[1] == 379205000 ||
					nItemID[1] == 379206000 ||
					nItemID[1] == 379208000 ||
					nItemID[1] == 379209000 ||
					nItemID[1] == 379210000 ||
					nItemID[1] == 379211000 ||
					nItemID[1] == 379212000 ||
					nItemID[1] == 379213000 ||
					nItemID[1] == 379214000 ||
					nItemID[1] == 379215000 ||
					nItemID[1] == 379216000 ||
					nItemID[1] == 379217000 ||
					nItemID[1] == 379218000 ||
					nItemID[1] == 379219000 ||
					nItemID[1] == 379220000 ||
					nItemID[2] == 379205000 ||
					nItemID[2] == 379206000 ||
					nItemID[2] == 379208000 ||
					nItemID[2] == 379209000 ||
					nItemID[2] == 379210000 ||
					nItemID[2] == 379211000 ||
					nItemID[2] == 379212000 ||
					nItemID[2] == 379213000 ||
					nItemID[2] == 379214000 ||
					nItemID[2] == 379215000 ||
					nItemID[2] == 379216000 ||
					nItemID[2] == 379217000 ||
					nItemID[2] == 379218000 ||
					nItemID[2] == 379219000 ||
					nItemID[2] == 379220000
					)
					ItemClass = 2;
				else
					continue;

			if (pUpgrade->bRateType == 3)
				if (
					nItemID[1] == 379021000 ||
					nItemID[1] == 379022000 ||
					nItemID[1] == 379023000 ||
					nItemID[1] == 379024000 ||
					nItemID[1] == 379025000 ||
					nItemID[1] == 379030000 ||
					nItemID[1] == 379031000 ||
					nItemID[1] == 379032000 ||
					nItemID[1] == 379033000 ||
					nItemID[1] == 379034000 ||
					nItemID[1] == 379035000 ||
					nItemID[1] == 379138000 ||
					nItemID[1] == 379139000 ||
					nItemID[1] == 379140000 ||
					nItemID[1] == 379141000 ||
					nItemID[1] == 379016000 ||
					nItemID[2] == 379021000 ||
					nItemID[2] == 379022000 ||
					nItemID[2] == 379023000 ||
					nItemID[2] == 379024000 ||
					nItemID[2] == 379025000 ||
					nItemID[2] == 379030000 ||
					nItemID[2] == 379031000 ||
					nItemID[2] == 379032000 ||
					nItemID[2] == 379033000 ||
					nItemID[2] == 379034000 ||
					nItemID[2] == 379035000 ||
					nItemID[2] == 379138000 ||
					nItemID[2] == 379139000 ||
					nItemID[2] == 379140000 ||
					nItemID[2] == 379141000 ||
					nItemID[1] == 379016000
					)
					ItemClass = 3;
				else
					continue;

			if (pUpgrade->bRateType == 4)
				if (
					nItemID[1] == 379256000 ||
					nItemID[1] == 379257000 ||
					nItemID[2] == 379256000 ||
					nItemID[2] == 379257000)
					ItemClass = 4;
				else
					continue;


			if (proto->ItemClass == 8) {
				Accessories = true;
				if (pUpgrade->bRateType != 8
					|| (pUpgrade->bOriginType != -1
						&& pUpgrade->bOriginType != 23
						&& proto->ItemExt != pUpgrade->bOriginType))
					continue;
			}

			if (proto->ItemClass != 4
				&& (nItemID[1] == 379257000
					|| nItemID[2] == 379257000))
				goto fail_return;

			if ((proto->ItemClass == 4
				|| proto->ItemClass == 2
				|| proto->ItemClass == 1)
				&& (nItemID[1] == 379256000
					|| nItemID[2] == 379256000))
				goto fail_return;


			if (bPos[0] == bPos[1]
				|| bPos[1] == bPos[2]
				|| bPos[0] == bPos[2])
				continue;

			if (!(proto->ItemClass <= ItemClass)
				&& !Accessories)
				continue;




			if (pUpgrade->bOriginType != -1
				&& pUpgrade->nIndex < 200000 && pUpgrade->nIndex >= 100000) {
				switch (pUpgrade->bOriginType) {
				case 0:
					if (!proto->isDagger())
						continue;
					break;

				case 1:
					if (proto->m_bKind != 21)
						continue;
					break;

				case 2:
					if (proto->m_bKind != 22)
						continue;
					break;

				case 3:
					if (proto->m_bKind != 31)
						continue;
					break;

				case 4:
					if (proto->m_bKind != 32)
						continue;
					break;

				case 5:
					if (proto->m_bKind != 41)
						continue;
					break;

				case 6:
					if (proto->m_bKind != 42)
						continue;
					break;

				case 7:
					if (proto->m_bKind != 51)
						continue;
					break;

				case 8:
					if (proto->m_bKind != 52)
						continue;
					break;

				case 9:
					if (proto->m_bKind != 70 && proto->m_bKind != 71)
						continue;
					break;

				case 10:
					if (proto->m_bKind != 110)
						continue;
					break;

				case 11:
					if ((nItemID[0] / 10000000) != 19)
						continue;
					break;

				case 12:
					if (proto->m_bKind != 60)
						continue;
					break;

				case 13:
					if (proto->m_bKind != 210 && proto->m_bKind != 220 && proto->m_bKind != 230 && proto->m_bKind != 240)
						continue;
					break;

				case 14:
					if (proto->m_bKind != 11)
						continue;
					break;
				}
			}




			if ((nItemID[0] / MIN_ITEM_ID) != (pUpgrade->nIndex / 100000)
				&& ((nItemID[0] / MIN_ITEM_ID) != 11 && (pUpgrade->nIndex / 100000) == 1)
				&& ((pUpgrade->nIndex / 100000) == 1
					|| (pUpgrade->nIndex / 100000) == 2))
				continue;




			bool isValidMatch = true;
			// Does our upgrade attempt match the requirements for this upgrade entry?
			for (int x = 1; x < MAX_ITEMS_REQ_FOR_UPGRADE; x++) {
				if (bPos[x] == -1
					|| pUpgrade->nReqItem[x - 1] == 0)
					continue;

				_ITEM_DATA * pItem = GetItem(SLOT_MAX + bPos[x]);

				if (pItem == nullptr
					|| nItemID[x] != pItem->nNum
					|| (nUpgradeType != ITEM_ACCESSORIES
						&& nItemID[x] != pUpgrade->nReqItem[x - 1])) {
					if (!Trina && !Karivdis)
						isValidMatch = false;
					break;
				}
			}

			// Not a valid match, try another row.
			if (!isValidMatch)
				continue;

			if (!hasCoins(pUpgrade->nReqNoah)) {
				bResult = UpgradeNeedCoins;
				goto fail_return;
			}

			if (proto->ItemClass > 4 && !Accessories && pUpgrade->nReqItem[0] != 379257000 && pUpgrade->nReqItem[1] != 379257000 && (!Karivdis || !Trina)) {
				bResult = UpgradeNoMatch;
				goto fail_return;
			} else if (proto->ItemClass < 3 && !Accessories &&  pUpgrade->nReqItem[0] == 379257000 || pUpgrade->nReqItem[1] == 379257000 && (!Karivdis || !Trina)) {
				bResult = UpgradeNoMatch;
				goto fail_return;
			}


			int nNewItemID = pOriginItem->nNum + pUpgrade->nGiveItem;

			// Does this new item exist?
			_ITEM_TABLE * newProto = g_pMain->GetItemPtr(nNewItemID);
			if (newProto == nullptr)
				continue;

			bResult = UpgradeSucceeded;
			break;
		}

		// If we ran out of upgrades to search through, it failed.
		if (bResult != UpgradeSucceeded || pUpgrade == nullptr)
			goto fail_return;

		// Generate a random number, test if the item burned.
		int rand = myrand(0, myrand(9000, 10000)), GenRate = 0;

		if ((Trina || Karivdis) && pUpgrade->sGenRate < 10000) {
			GenRate = (pUpgrade->sGenRate + pUpgrade->sTrinaRate);

			if (GenRate > 10000)
				GenRate = 10000;
		} else
			GenRate = pUpgrade->sGenRate;

		if (bType == UpgradeTypeNormal && GenRate < rand) {
			bResult = UpgradeFailed;
			memset(pOriginItem, 0, sizeof(_ITEM_DATA));

			// Send upgrade notice.

			if ((pUpgrade->sGenRate < 2001
				&& nUpgradeType != ITEM_ACCESSORIES)
				|| nUpgradeType == ITEM_UPGRADE_REBIRTH
				|| (nUpgradeType == ITEM_ACCESSORIES
					&& pUpgrade->sGenRate < 5001))
				ItemUpgradeNotice(proto, UpgradeFailed);

			GoldLose(pUpgrade->nReqNoah, true);

			uint32 tariffTax = 0, nationalTax = 0;
			C3DMap * pMap = GetMap();
			_KNIGHTS_SIEGE_WARFARE *pSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(1);
			CKingSystem *pKingSystem = g_pMain->m_KingSystemArray.GetData(GetNation());

			if (pMap == nullptr)
				return;

			if (pMap->GetTariff())
				tariffTax = (pUpgrade->nReqNoah * GetMap()->GetTariff() / 100);

			switch (GetZoneID()) {
			case ZONE_KARUS:
				nationalTax = pUpgrade->nReqNoah * 10 / 100;
				if (tariffTax >= nationalTax)
					tariffTax -= nationalTax;

				if (pKingSystem->m_nTerritoryTax < COIN_MAX)
					pKingSystem->m_nTerritoryTax += (tariffTax);

				if (pKingSystem->m_nNationalTreasury < COIN_MAX)
					pKingSystem->m_nNationalTreasury += (nationalTax);

				if (pKingSystem->m_nTerritoryTax < COIN_MAX)
					InsertTaxUpEvent(ZONE_KARUS, (tariffTax));

				if (pKingSystem->m_nNationalTreasury < COIN_MAX)
					InsertTaxUpEvent(ZONE_KARUS_ESLANT, (nationalTax));
				break;
			case ZONE_ELMORAD:
				nationalTax = pUpgrade->nReqNoah * 10 / 100;
				if (tariffTax >= nationalTax)
					tariffTax -= nationalTax;

				if (pKingSystem->m_nTerritoryTax < COIN_MAX)
					pKingSystem->m_nTerritoryTax += (tariffTax);

				if (pKingSystem->m_nNationalTreasury < COIN_MAX)
					pKingSystem->m_nNationalTreasury += (nationalTax);

				if (pKingSystem->m_nTerritoryTax < COIN_MAX)
					InsertTaxUpEvent(ZONE_ELMORAD, (tariffTax));

				if (pKingSystem->m_nNationalTreasury < COIN_MAX)
					InsertTaxUpEvent(ZONE_ELMORAD_ESLANT, (nationalTax));
				break;
			case ZONE_ELMORAD_ESLANT:
				nationalTax = pUpgrade->nReqNoah * 10 / 100;
				if (tariffTax >= nationalTax)
					tariffTax -= nationalTax;

				if (pKingSystem->m_nTerritoryTax < COIN_MAX)
					pKingSystem->m_nTerritoryTax += (tariffTax);

				if (pKingSystem->m_nNationalTreasury < COIN_MAX)
					pKingSystem->m_nNationalTreasury += (nationalTax);

				if (pKingSystem->m_nTerritoryTax < COIN_MAX)
					InsertTaxUpEvent(ZONE_ELMORAD, (tariffTax));

				if (pKingSystem->m_nNationalTreasury < COIN_MAX)
					InsertTaxUpEvent(ZONE_ELMORAD_ESLANT, (nationalTax));
				break;
			case ZONE_MORADON:

				if (pSiegeWar->nMoradonTax < COIN_MAX)
					pSiegeWar->nMoradonTax += tariffTax;
				if (pSiegeWar->nMoradonTax < COIN_MAX)
					InsertTaxUpEvent(ZONE_MORADON, tariffTax);
				break;
			case ZONE_MORADONM2:
				if (pSiegeWar->nMoradonTax < COIN_MAX)
					pSiegeWar->nMoradonTax += tariffTax;
				if (pSiegeWar->nMoradonTax < COIN_MAX)
					InsertTaxUpEvent(ZONE_MORADON, tariffTax);
				break;
			default:
				nationalTax = pUpgrade->nReqNoah * 10 / 100;
				if (tariffTax >= nationalTax)
					tariffTax -= nationalTax;

				if (pKingSystem->m_nTerritoryTax < COIN_MAX)
					pKingSystem->m_nTerritoryTax += (tariffTax);

				if (pKingSystem->m_nNationalTreasury < COIN_MAX)
					pKingSystem->m_nNationalTreasury += (nationalTax);

				if (pKingSystem->m_nTerritoryTax < COIN_MAX)
					InsertTaxUpEvent(GetNation(), (tariffTax));

				if (pKingSystem->m_nNationalTreasury < COIN_MAX)
					InsertTaxUpEvent((GetNation() + 10), (nationalTax));
				break;
			}



			if (nUpgradeType == ITEM_UPGRADE_REBIRTH) {
				if (bResult == UpgradeFailed)
					g_pMain->WriteUpgradeLogFile(string_format("[ ItemRebirthUpgrade - %d:%d:%d ] %s :  Failed = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), proto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
			} else if (nUpgradeType == ITEM_ACCESSORIES && pUpgrade->sGenRate) {
				if (bResult == UpgradeFailed)
					g_pMain->WriteUpgradeLogFile(string_format("[ ItemAccessories - %d:%d:%d ] %s :  Failed = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), proto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
			} else {
				if (bResult == UpgradeFailed && pUpgrade->sGenRate)
					g_pMain->WriteUpgradeLogFile(string_format("[ ItemUpgrade - %d:%d:%d ] %s :  Failed = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), proto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
			}
		} else {

			if (isDead()
				|| isTrading()
				|| isStoreOpen()
				|| isMerchanting()
				|| m_bMerchantStatex
				|| isSellingMerchant()
				|| isBuyingMerchant()
				|| isMining()) {
				bResult = UpgradeTrading;
				goto fail_return;
			}
			// Generate the new item ID
			int nNewItemID = pOriginItem->nNum + pUpgrade->nGiveItem;

			// Does this new item exist?
			_ITEM_TABLE * newProto = g_pMain->GetItemPtr(nNewItemID);
			if (newProto == nullptr) { // if not, just say it doesn't match. No point removing the item anyway (like official :/).
				bResult = UpgradeNoMatch;
				goto fail_return;
			}

			if (bType != UpgradeTypePreview) {
				// Update the user's item in their inventory with the new item
				pOriginItem->nNum = nNewItemID;

				// Reset the durability also, to the new cap.
				pOriginItem->sDuration = newProto->m_sDuration;

				// Send upgrade notice.
				if ((pUpgrade->sGenRate < 2001
					&& nUpgradeType != ITEM_ACCESSORIES)
					|| nUpgradeType == ITEM_UPGRADE_REBIRTH
					|| (nUpgradeType == ITEM_ACCESSORIES
						&& pUpgrade->sGenRate < 5001))
					ItemUpgradeNotice(newProto, UpgradeSucceeded);

				// Rob gold upgrade noah
				GoldLose(pUpgrade->nReqNoah, true);

				if (nUpgradeType == ITEM_UPGRADE_REBIRTH) {
					if (bResult == UpgradeSucceeded)
						g_pMain->WriteUpgradeLogFile(string_format("[ ItemRebirthUpgrade - %d:%d:%d ] %s :  Succes = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), newProto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
				} else if (nUpgradeType == ITEM_ACCESSORIES) {
					if (bResult == UpgradeSucceeded)
						g_pMain->WriteUpgradeLogFile(string_format("[ ItemAccessories - %d:%d:%d ] %s :  Succes = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), newProto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
				} else {
					if (bResult == UpgradeSucceeded)
						g_pMain->WriteUpgradeLogFile(string_format("[ ItemUpgrade - %d:%d:%d ] %s :  Succes = %s ( Zone =%d, X=%d, Z=%d )\n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), newProto->m_sName.c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
				}
			}

			// Replace the item ID in the list for the packet
			nItemID[0] = nNewItemID;
		}

		// Remove the source item 
		if (bType != UpgradeTypePreview) {
			// Remove all required items, if applicable.
			for (int i = 0; i < 10; i++) {

				if (bPos[i] == -1 || bPos[i] >= HAVE_MAX || nItemID[i] < 1)
					continue;


				_ITEM_DATA * pItem = GetItem(SLOT_MAX + bPos[i]);

				if (pItem->nNum == 0 || pItem->sCount == 0 || (bResult == UpgradeSucceeded && pItem->nNum == pOriginItem->nNum))
					continue;

				_ITEM_TABLE * pTable = g_pMain->GetItemPtr(pItem->nNum);

				pItem->sCount--;
				if (pItem->sCount == 0 || (pTable != nullptr && pTable->isAccessory()))
					memset(pItem, 0, sizeof(pItem));
			}
		}
	} // end of scoped lock

	result << bType;

	if (Follow)
		printf("1x-%d\n", bResult);

	result << bResult;
	foreach_array(i, nItemID)
		result << nItemID[i] << bPos[i];
	Send(&result);

	// Send the result to everyone in the area
	// (i.e. make the anvil do its burned/upgraded indicator thing)
	result.Initialize(WIZ_OBJECT_EVENT);
	result << uint8(OBJECT_ANVIL) << bResult << sNpcID;
	SendToRegion(&result, nullptr, GetEventRoom());

	return;
fail_return:
	result << bResult;

	// The item data's only sent when not experiencing a general error
	if (bResult != 2) {
		foreach_array(i, nItemID)
			result << nItemID[i] << bPos[i];
	}
	Send(&result);
}


/**
* @brief	Upgrade notice.
*
* @param	pItem	The item.
*/
void CUser::ItemUpgradeNotice(_ITEM_TABLE * pItem, uint8 UpgradeResult) {
	bool bSendUpgradeNotice = false;
	std::string sUpgradeNotice;

	// Notice is only rebirth upgrade a Offical stuff.
	if (pItem->m_ItemType == 11 || pItem->m_ItemType == 12 || pItem->m_ItemType == 4)
		bSendUpgradeNotice = true;

	if (bSendUpgradeNotice) {
		Packet result(WIZ_LOGOSSHOUT, uint8(0x02));
		result.SByte();

		if (UpgradeResult == 0)
			result << uint8(0x05) << uint8(0x00) << GetName() << pItem->m_iNum;
		else if (UpgradeResult == 1)
			result << uint8(0x05) << uint8(0x01) << GetName() << pItem->m_iNum;

		g_pMain->Send_All(&result);
	}
}

/**
* @brief	Packet handler for the accessory upgrade system.
*
* @param	pkt	The packet.
*/
void CUser::ItemUpgradeAccessories(Packet & pkt) {
	ItemUpgrade(pkt, ITEM_ACCESSORIES);
}

void CUser::ItemUpgradePet(Packet & pkt) {

	Packet result(WIZ_ITEM_UPGRADE, uint8(ITEM_PET));
	uint16 unknown;
	uint32 ItemID;
	uint8 Slot;

	/*1 Familiar hatching failed.
	2 Invalid name.
	3 This Familiar cannot be incubated.
	4 Limit exceeded.*/
	uint8 Error = 2;
	std::string Name;
	pkt.DByte();
	pkt >> unknown >> ItemID >> Slot >> Name;

	if (Slot >= HAVE_MAX)
		goto fail_return;

	//printf("%d %d %d %s\n",unknown,ItemID,Slot,Name.c_str());

	_ITEM_DATA * pItem = GetItem(SLOT_MAX + Slot);

	if (ItemID != 600001000
		|| pItem == nullptr
		|| pItem->nNum != ItemID) {
		Error = 3;
		goto fail_return;
	}
	uint64 Serial = g_pMain->GenerateItemSerial();

	if (Name.length() > 21
		|| !g_pMain->WordGuardSystem(Name, (uint8) Name.length())
		|| Name.length() < 1) {
		Error = 2;
		goto fail_return;
	}


	CPet * pData = new CPet();
	pData->m_Serial = Serial;
	pData->m_strPetID = Name;
	pData->m_iExp = 0;
	pData->m_bLevel = 1;
	pData->m_sSatisfaction = 10000;
	pData->m_sClass = 101;
	g_pMain->LastPetID++;
	pData->SpecialPetID = g_pMain->LastPetID;

	uint8 DBResult = g_DBAgent.InsertPetData(Serial, pData);

	if (DBResult != 1) {
		Error = DBResult - 1;
		goto fail_return;
	}

	g_pMain->m_PetArray.insert(std::make_pair(pData->m_Serial, pData));

	pItem->nSerialNum = Serial;
	pItem->nNum = 610001000;

	result.DByte();
	result << uint8(1) << uint32(pItem->nNum) << Slot << uint32(pData->SpecialPetID) << Name << uint8(pData->m_sClass) << uint8(pData->m_bLevel) << uint16((pData->m_iExp * 10000) / g_pMain->GetPetExpByLevel(pData->m_bLevel)) << uint16(pData->m_sSatisfaction);
	Send(&result);

fail_return:
	result << uint8(0) << uint8(Error);
	Send(&result);
}

/**
* @brief	Packet handler for the Chaotic Generator system
* 			which is used to exchange Bifrost pieces/fragments.
*
* @param	pkt	The packet.
*/

void CUser::BifrostPieceProcess(Packet & pkt) {
	enum ResultOpCodes {
		Failed = 0,
		Success = 1
	};

	enum ResultMessages {
		EffectNone = 0, // No effect
		EffectRed = 1, // There will be better days.
		EffectGreen = 2, // Don't be too disappointed. You're luck isn't that bad.
		EffectWhite = 3 // It must be your lucky day.
	};

	uint16 nObjectID = 0;
	uint32 nExchangeItemID = 0;

	pkt >> nObjectID >> nExchangeItemID;

	std::vector<uint32> ExchangeIndexList;
	ResultOpCodes resultOpCode = Success;
	ResultMessages resultMessage = EffectNone;
	uint32 nItemID = 0;
	uint8 sItemSlot = 0;
	uint8 sExchangeItemSlot = 0;

	sExchangeItemSlot = FindSlotForItem(nExchangeItemID, 1) - SLOT_MAX;

	if (g_pMain->m_ItemExchangeArray.GetSize() > 0) {
		foreach_stlmap(itr, g_pMain->m_ItemExchangeArray) {
			if (itr->second->nOriginItemNum[0] == nExchangeItemID) {
				if (std::find(ExchangeIndexList.begin(), ExchangeIndexList.end(), itr->second->nIndex) == ExchangeIndexList.end())
					ExchangeIndexList.push_back(itr->second->nIndex);
			} else
				continue;
		}
	}

	if (ExchangeIndexList.size() > 0) {
		uint32 randIndex = myrand(0, (ExchangeIndexList.size() - 1));
		uint32 nExchangeID = ExchangeIndexList[randIndex];

		_ITEM_EXCHANGE * pExchange = g_pMain->m_ItemExchangeArray.GetData(nExchangeID);

		if (pExchange == nullptr
			|| !CheckExchange(nExchangeID)
			|| pExchange->bRandomFlag > 101
			|| !CheckExistItemAnd(pExchange->nOriginItemNum[0], pExchange->sOriginItemCount[0], 0, 0, 0, 0, 0, 0, 0, 0))
			resultOpCode = Failed;

		if (isTrading() || isMerchanting() || isMining() || isStoreOpen())
			resultOpCode = Failed;

		if (pExchange->bRandomFlag > 0 && resultOpCode == Success) {
			uint32 nTotalPercent = 0;
			for (int i = 0; i < ITEMS_IN_EXCHANGE_GROUP; i++)
				nTotalPercent += pExchange->sExchangeItemCount[i];

			if (nTotalPercent > 10000)
				resultOpCode = Failed;

			if (resultOpCode == Success) {
				uint8 bRandArray[20000];
				memset(&bRandArray, 0, sizeof(bRandArray));
				uint32 sExchangeCount[ITEMS_IN_EXCHANGE_GROUP];
				memcpy(&sExchangeCount, &pExchange->sExchangeItemCount, sizeof(pExchange->sExchangeItemCount));

				int offset = 0;
				for (int n = 0, i = 0; n < ITEMS_IN_EXCHANGE_GROUP; n++) {
					if (sExchangeCount[n] > 0) {
						memset(&bRandArray[offset], n, sExchangeCount[n]);
						offset += sExchangeCount[n];
					}
				}

				uint8 bRandSlot = bRandArray[myrand(0, 20000)];
				nItemID = pExchange->nExchangeItemNum[bRandSlot];

				sItemSlot = GetEmptySlot() - SLOT_MAX;
				RobItem(pExchange->nOriginItemNum[0], 1);
				GiveItem(nItemID, 1);


				_ITEM_TABLE *pItem = g_pMain->m_ItemtableArray.GetData(nItemID);

				if (pItem != nullptr) {
					if (pItem->m_ItemType == 4)
						resultMessage = EffectWhite;
					else if (pItem->m_ItemType == 5 || pItem->m_ItemType == 11 || pItem->m_ItemType == 12)
						resultMessage = EffectGreen;
					else
						resultMessage = EffectRed;
				}
			}
		}
	}

	Packet result(WIZ_ITEM_UPGRADE);
	result << (uint8) ITEM_BIFROST_EXCHANGE << (uint8) resultOpCode << nItemID << sItemSlot << nExchangeItemID << sExchangeItemSlot << (uint8) resultMessage;
	Send(&result);

	result.clear();
	result.SetOpcode(WIZ_OBJECT_EVENT);
	result << (uint8) OBJECT_ARTIFACT << (uint8) resultMessage << nObjectID;

	if (resultOpCode != Failed)
		SendToRegion(&result);
}

/**
* @brief	Packet handler for the Special exchange system
* 			which is used to exchange Krowaz meterials.
*
* @param	pkt	The packet.
*/
void CUser::SpecialItemExchange(Packet & pkt) {
	enum ResultOpCodes {
		WrongMaterial = 0,
		Success = 1,
		Failed = 2
	};

	ResultOpCodes resultOpCode = WrongMaterial;

	std::vector <uint32> ExchangingItems;
	uint16 sNpcID;
	uint32 nShadowPiece;
	uint8 nShadowPieceSlot;
	uint8 nMaterialCount;
	uint8 nItemSlot[10];
	uint8 nDownFlag;
	uint32 nItemID[10];
	uint8 nItemCount[10];

	uint32 nItemNumber = 0;
	uint8 sItemSlot = 0;

	pkt >> sNpcID >> nShadowPiece >> nShadowPieceSlot >> nMaterialCount;

	for (int i = 0; i < 10; i++) {
		nItemID[i] = 0;
		nItemCount[i] = 0;
	}

	for (int i = 0; i < nMaterialCount; i++)
		pkt >> nItemSlot[i];

	pkt >> nDownFlag;

	for (int i = 0; i < nMaterialCount; i++) {
		uint8 nReadByte;
		int nDigit = 100000000;
		nItemID[i] = 0;
		for (int x = 0; x < 9; x++) {
			pkt >> nReadByte;
			nItemID[i] += (nReadByte - 48) * nDigit;
			nDigit = nDigit / 10;
		}

		uint8 nCount[3] = {0, 0, 0};
		pkt >> nCount[0];
		pkt >> nCount[1];
		pkt >> nCount[2];
		int nCountFinish = 0;
		nCountFinish += (nCount[0] - 48) * 100;
		nCountFinish += (nCount[1] - 48) * 10;
		nCountFinish += (nCount[2] - 48) * 1;
		nItemCount[i] = nCountFinish;

		_ITEM_DATA * pItem = GetItem(nItemSlot[i] + SLOT_MAX);

		if (pItem == nullptr)
			return;

		if (pItem->nNum != nItemID[i]
			|| pItem->sCount < nItemCount[i])
			return;

		ExchangingItems.push_back(nItemID[i]);
	}

	std::vector<uint32> ExchangeIndexList;

	if (nMaterialCount >= 2) // Minimum Required : 2 Material
	{
		if (g_pMain->m_ItemMixArray.GetSize() > 0) {
			foreach_stlmap_nolock(itr, g_pMain->m_ItemMixArray) {

				uint8 nOriginItemCount = 0;
				uint8 nMatchCount = 0;
				bool bAddArray = false;

				if (nMaterialCount == 2)
					nMatchCount = (nShadowPiece == 0 ? 2 : 3);
				else if (nMaterialCount == 3)
					nMatchCount = (nShadowPiece == 0 ? 3 : 4);
				else if (nMaterialCount == 4)
					nMatchCount = (nShadowPiece == 0 ? 4 : 5);
				else if (nMaterialCount == 5)
					nMatchCount = (nShadowPiece == 0 ? 5 : 6);
				else if (nMaterialCount == 6)
					nMatchCount = (nShadowPiece == 0 ? 6 : 7);
				else if (nMaterialCount == 7)
					nMatchCount = (nShadowPiece == 0 ? 7 : 8);
				else if (nMaterialCount == 8)
					nMatchCount = (nShadowPiece == 0 ? 8 : 9);
				else if (nMaterialCount == 9)
					nMatchCount = (nShadowPiece == 0 ? 9 : 10);

				for (int i = 0; i < nMaterialCount; i++) {
					if (nItemID[i] != 0) {
						for (int x = 0; x < ITEMS_IN_SPECIAL_ORIGIN_GROUP; x++) {
							if (itr->second->nOriginItemNum[x] != 0
								&& nItemID[i] == itr->second->nOriginItemNum[x]) {
								nOriginItemCount++;
								break;
							}
						}
					}
				}

				if (nShadowPiece > 0) {
					if (nOriginItemCount == (nMatchCount - 1))
						bAddArray = true;
				}
				if (nShadowPiece == 0) {
					if (nOriginItemCount == nMatchCount)
						bAddArray = true;
				}

				if (bAddArray && std::find(ExchangeIndexList.begin(), ExchangeIndexList.end(), itr->second->nIndex) == ExchangeIndexList.end())
					ExchangeIndexList.push_back(itr->second->nIndex);
			}
		}
	}

	if (ExchangeIndexList.size() > 0) {
		uint32 randIndex = (ExchangeIndexList.size() - 1);
		uint32 nExchangeID = ExchangeIndexList[randIndex];

		_ITEM_MIX * pExchange = g_pMain->m_ItemMixArray.GetData(nExchangeID);

		if (pExchange == nullptr
			|| !CheckSpecialExchange(nExchangeID)
			|| pExchange->bType > 102
			|| !CheckExistSpecialItemAnd(
				pExchange->nOriginItemNum[0], pExchange->sOriginItemCount[0],
				pExchange->nOriginItemNum[1], pExchange->sOriginItemCount[1],
				pExchange->nOriginItemNum[2], pExchange->sOriginItemCount[2],
				pExchange->nOriginItemNum[3], pExchange->sOriginItemCount[3],
				pExchange->nOriginItemNum[4], pExchange->sOriginItemCount[4],
				pExchange->nOriginItemNum[5], pExchange->sOriginItemCount[5],
				pExchange->nOriginItemNum[6], pExchange->sOriginItemCount[6],
				pExchange->nOriginItemNum[7], pExchange->sOriginItemCount[7],
				pExchange->nOriginItemNum[8], pExchange->sOriginItemCount[8],
				pExchange->nOriginItemNum[9], pExchange->sOriginItemCount[9])) {
			resultOpCode = WrongMaterial;
		} else {
			bool bContinueExchange = true;

			for (int i = 0; i < nMaterialCount; i++) {
				if (!bContinueExchange)
					break;

				if (nItemID[i] != 0) {
					for (int x = 0; x < ITEMS_IN_SPECIAL_ORIGIN_GROUP; x++) {
						if (pExchange->nOriginItemNum[x] != 0
							&& nItemID[i] == pExchange->nOriginItemNum[x]
							&& nItemCount[i] != pExchange->sOriginItemCount[x]) {
							bContinueExchange = false;
							break;
						}
					}
				}
			}

			if (isTrading() || isMerchanting() || isSellingMerchant() || isBuyingMerchant() || isStoreOpen() || isMining())
				bContinueExchange = false;

			uint8 MatchingCase = 0;
			uint8 NeedCount = 0;
			for (int x = 0; x < ITEMS_IN_SPECIAL_ORIGIN_GROUP; x++) {
				std::vector<uint32> Used;

				if (pExchange->nOriginItemNum[x] > 0)
					NeedCount++;
				else
					continue;

				foreach(itr, ExchangingItems) {
					if ((*itr) == pExchange->nOriginItemNum[x]) {
						MatchingCase++;
						*itr = 0;
					}
				}


			}
			if (NeedCount != MatchingCase)
				bContinueExchange = false;

			if (!bContinueExchange)
				resultOpCode = WrongMaterial;
			else {
				uint32 nTotalPercent = 0;
				for (int i = 0; i < ITEMS_IN_SPECIAL_EXCHANGE_GROUP; i++)
					nTotalPercent += pExchange->sExchangeItemCount[i];

				if (nTotalPercent > 10000)
					resultOpCode = WrongMaterial;
				else {
					uint8 bRandArray[10000];
					memset(&bRandArray, 0, sizeof(bRandArray));
					uint16 sExchangeCount[ITEMS_IN_SPECIAL_EXCHANGE_GROUP];
					memcpy(&sExchangeCount, &pExchange->sExchangeItemCount, sizeof(pExchange->sExchangeItemCount));

					int offset = 0;
					for (int n = 0, i = 0; n < ITEMS_IN_SPECIAL_EXCHANGE_GROUP; n++) {
						if (sExchangeCount[n] > 0) {
							memset(&bRandArray[offset], n, sExchangeCount[n]);
							offset += sExchangeCount[n];
						}
					}

					uint8 bRandSlot = bRandArray[myrand(1, 5)];
					nItemNumber = pExchange->nExchangeItemNum[bRandSlot];
					uint16 nItemRate = pExchange->bSuccessRate;
					int rand = myrand(0, myrand(1, 5000));

					if (nShadowPiece > 0)
						nItemRate = 10000;

					if (nItemRate <= rand)
						resultOpCode = Failed;
					else {
						sItemSlot = GetEmptySlot() - SLOT_MAX;
						GiveItem(nItemNumber, 1);
						resultOpCode = Success;
					}

					for (int i = 0; i < ITEMS_IN_ORIGIN_GROUP; i++) {
						if (pExchange->nOriginItemNum[i] != 0)
							RobItem(pExchange->nOriginItemNum[i], pExchange->sOriginItemCount[i], false);
					}

				}
			}
		}
	} else
		return;

	Packet result(WIZ_ITEM_UPGRADE);
	result << (uint8) ITEM_SPECIAL_EXCHANGE << (uint8) resultOpCode << sNpcID;

	if (resultOpCode == Success)
		result << nItemNumber << sItemSlot;

	Send(&result);

	if (resultOpCode == Success)
		ShowNpcEffect(31033, true);
	else if (resultOpCode == Failed)
		ShowNpcEffect(31034, true);
}
/**
* @brief	Packet handler for the upgrading of 'rebirthed' items.
*
* @param	pkt	The packet.
*/
void CUser::ItemUpgradeRebirth(Packet & pkt) {
	ItemUpgrade(pkt, ITEM_UPGRADE_REBIRTH);
}

void CUser::ItemDisassembling(Packet & pkt) {
	enum ResultOpCodes {
		Failed = 0,
		Success = 1
	};

	enum ResultMessages {
		nExchangeIndexID1 = 1,
		nExchangeIndexID2 = 2,
		nExchangeIndexID3 = 3
	};
	uint8 nPos;
	uint8 nItemCount = 1;
	ResultOpCodes resultOpCode = Success;
	uint16 nObjectID = 0;
	uint32 nExchangeItemID = 0;
	std::vector<uint32> ExchangeIndexCrashList;
	pkt >> nExchangeItemID >> nPos >> nObjectID;
	Packet result(WIZ_ITEM_UPGRADE);

	_ITEM_DATA* pItemData = GetItem(nPos + SLOT_MAX);
	_ITEM_TABLE* pItem = g_pMain->GetItemPtr(nExchangeItemID);

	if (pItemData == nullptr)
		return;

	if (pItem == nullptr)
		return;

	if (g_pMain->m_ItemCrashArray.GetSize() > 0) {
		foreach_stlmap_nolock(itr, g_pMain->m_ItemCrashArray) {
			if (std::find(ExchangeIndexCrashList.begin(), ExchangeIndexCrashList.end(), itr->second->Index) == ExchangeIndexCrashList.end())
				ExchangeIndexCrashList.push_back(itr->second->Index);
			if (pItem->m_ItemType >= 0 && pItem->m_ItemType <= 1) {
				nItemCount = nExchangeIndexID1;
				itr->second->Flag = 2;
				continue;
			} else if (pItem->m_ItemType == 5 || pItem->m_ItemType == 11 && itr->second->Flag == 2) {
				nItemCount = nExchangeIndexID2;
				itr->second->Flag = 3;
				continue;
			} else if (pItem->m_ItemType == 4 || pItem->m_ItemType == 12 || pItem->m_ItemType == 13 && itr->second->Flag == 3) {
				nItemCount = nExchangeIndexID3;
				itr->second->Flag = 3;
				continue;
			} else
				return;

		}
	}

	if (ExchangeIndexCrashList.size() > 0) {

		uint32 randIndex[nExchangeIndexID3];
		uint32 nExchangeID[nExchangeIndexID3];

		if (pItem->m_ItemType >= 0 && pItem->m_ItemType <= 1)
			nItemCount = nExchangeIndexID1;
		else if (pItem->m_ItemType == 5 || pItem->m_ItemType == 11)
			nItemCount = nExchangeIndexID2;
		else if (pItem->m_ItemType == 4 || pItem->m_ItemType == 12 || pItem->m_ItemType == 13)
			nItemCount = nExchangeIndexID3;
		else
			return;

		for (int i = 0; i < nItemCount; i++) {
			randIndex[i] = myrand(0, (ExchangeIndexCrashList.size() - 1));
			nExchangeID[i] = ExchangeIndexCrashList[randIndex[i]];
		}

		bool CheckSlot = CheckGiveSlot(nItemCount);

		if (!CheckSlot
			|| !RobItem(nExchangeItemID, 1))
			return;

		RobItem(nExchangeItemID, 1);
		uint8 emptySlot = GetEmptySlot();
		result << (uint8) ITEM_DISASSEMBLING << (uint16) resultOpCode << nExchangeItemID << nPos << (uint16) nItemCount;

		if (emptySlot >= nItemCount) {
			for (int i = 0; i < nItemCount; i++) {
				_ITEM_CRASH * pExchangeCrashItem = g_pMain->m_ItemCrashArray.GetData(nExchangeID[i]);
				if (pExchangeCrashItem == nullptr)
					continue;

				int8 pos;
				if ((pos = FindSlotForItem(pExchangeCrashItem->ItemID, pExchangeCrashItem->ItemCount)) < 0)
					continue;

				GiveItem(pExchangeCrashItem->ItemID, pExchangeCrashItem->ItemCount, true);
				result << pExchangeCrashItem->ItemID << uint8(pos - SLOT_MAX) << (uint16) pExchangeCrashItem->ItemCount;
			}
		}
	}
	Send(&result);
}

