#include "stdafx.h"
#include "Map.h"
#include "KnightsManager.h"
#include "KingSystem.h"
#include "DBAgent.h"
#include <boost\foreach.hpp>
#include "../shared/DateTime.h"

using namespace std;
using std::string;

struct ITEMS {
	int ITEMID;
	uint8 IPOS;
	uint16 _ICOUNT;
}
pItems;

void CUser::ItemRepair(Packet & pkt) {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	Packet result(WIZ_ITEM_REPAIR);
	uint32 money, itemid;
	uint16 durability, quantity, sNpcID;
	_ITEM_TABLE* pTable = nullptr;
	CNpc *pNpc = nullptr;
	uint8 sPos, sSlot;

	pkt >> sPos >> sSlot >> sNpcID >> itemid;
	if (sPos == 1) {	// SLOT
		if (sSlot >= SLOT_MAX)
			goto fail_return;

		if (m_sItemArray[sSlot].nNum != itemid)
			goto fail_return;
	} else if (sPos == 2) {	// INVEN
		if (sSlot >= HAVE_MAX)
			goto fail_return;

		if (m_sItemArray[SLOT_MAX + sSlot].nNum != itemid)
			goto fail_return;
	}

	pNpc = g_pMain->GetNpcPtr(sNpcID);
	if (pNpc == nullptr || !isInRange(pNpc, MAX_NPC_RANGE))
		return;

	if (pNpc->GetType() == NPC_TINKER || pNpc->GetType() == NPC_MERCHANT) {
		pTable = g_pMain->GetItemPtr(itemid);
		if (pTable == nullptr
			|| pTable->m_iSellPrice == SellTypeNoRepairs)
			goto fail_return;

		durability = pTable->m_sDuration;
		if (durability == 1)
			goto fail_return;

		if (sPos == 1)
			quantity = pTable->m_sDuration - m_sItemArray[sSlot].sDuration;
		else if (sPos == 2)
			quantity = pTable->m_sDuration - m_sItemArray[SLOT_MAX + sSlot].sDuration;

		money = (unsigned int) ((((pTable->m_iBuyPrice - 10) / 10000.0f) + pow((float) pTable->m_iBuyPrice, 0.75f)) * quantity / (double) durability);

		if (GetPremiumProperty(PremiumRepairDiscountPercent) > 0)
			money = money * GetPremiumProperty(PremiumRepairDiscountPercent) / 100;

		if (!GoldLose(money, false))
			goto fail_return;

		if (sPos == 1)
			m_sItemArray[sSlot].sDuration = durability;
		else if (sPos == 2)
			m_sItemArray[SLOT_MAX + sSlot].sDuration = durability;

		result << uint8(1) << GetCoins();
		Send(&result);
		return;
	}

fail_return:
	result << uint8(0) << GetCoins();
	Send(&result);
}

void CUser::ClientEvent(uint16 sNpcID) {
	// Ensure AI's loaded
	if (!g_pMain->m_bPointCheckFlag)
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

	int32 iEventID = 0;
	CNpc *pNpc = g_pMain->GetNpcPtr(sNpcID);
	if (pNpc == nullptr
		|| !isInRange(pNpc, MAX_NPC_RANGE))
		return;

	m_sEventNid = sNpcID;
	m_sEventSid = pNpc->GetProtoID(); // For convenience purposes with Lua scripts.

	if (pNpc->GetProtoID() == SAW_BLADE_SSID) {
		HpChange(-5000 / 10);
		return;
	} else if (pNpc->GetProtoID() == CHAOS_CUBE_SSID && !pNpc->isDead() && pNpc->cubeisactive < UNIXTIME) {
		pNpc->cubeisactive = UNIXTIME + 10;
		uint8 nEventRoomUserCount = g_pMain->TempleEventGetRoomUsers(GetEventRoom());
		uint8 nItemRewardRankFirst = nEventRoomUserCount / 3;
		uint8 nItemRewardRankSecond = (nEventRoomUserCount - 1) * 2;

		int32 nUserRank = GetPlayerRank(RANK_TYPE_CHAOS_DUNGEON);
		uint32 nItemID = 0;
		int rand = 0;
		rand = myrand(1, 100);

		if (rand > 0 && rand < 11)
			nItemID = ITEM_KILLING_BLADE;
		else if (rand > 10 && rand < 56)
			nItemID = ITEM_LIGHT_PIT;
		else if (rand > 55 && rand < 93)
			nItemID = ITEM_DRAIN_RESTORE;
		else
			nItemID = ITEM_KILLING_BLADE;

		GiveItem(nItemID, 1);
		g_pMain->ShowNpcEffect(GetSocketID(), 251, GetZoneID(), GetEventRoom());
		/// AG_CHAOS_CUBE
		Packet result(AG_CHAOS_CUBE);
		result << sNpcID;
		g_pMain->Send_AIServer(&result);
		return;
	} else if (pNpc->GetProtoID() == CHAOS_CUBE_SSID)
		return;
	else if (pNpc->GetType() == NPC_KISS) {
		KissUser();
		return;
	}

	Guard lock(g_pMain->m_questNpcLock);
	QuestNpcList::iterator itr = g_pMain->m_QuestNpcList.find(pNpc->GetProtoID());
	if (itr == g_pMain->m_QuestNpcList.end())
		return;

	QuestHelperList & pList = itr->second;
	_QUEST_HELPER * pHelper = nullptr;
	foreach(itr, pList) {
		if ((*itr) == nullptr
			|| (*itr)->sEventDataIndex
			|| (*itr)->bEventStatus
			|| ((*itr)->bNation != 3 && (*itr)->bNation != GetNation())
			|| ((*itr)->bClass != 5 && !JobGroupCheck((*itr)->bClass)))
			continue;

		pHelper = (*itr);
		break;
	}

	if (pHelper == nullptr)
		return;

	V3_QuestRunEvent(pHelper, pHelper->nEventTriggerIndex);
}

void CUser::KissUser() {
	Packet result(WIZ_KISS);
	result << uint32(GetID()) << m_sEventNid;
	GiveItem(910014000); // aw, you got a 'Kiss'. How literal.
	SendToRegion(&result, nullptr, GetEventRoom());
}

void CUser::ClassChange(Packet & pkt, bool bFromClient /*= true */) {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	Packet result(WIZ_CLASS_CHANGE);
	bool bSuccess = false;
	uint8 opcode = pkt.read<uint8>();
	/*printf ("%d %s\n",opcode,GetName().c_str());*/
	if (opcode == CLASS_CHANGE_REQ) {
		ClassChangeReq();
		return;
	} else if (opcode == ALL_POINT_CHANGE) {
		AllPointChange(false);
		return;
	} else if (opcode == ALL_SKILLPT_CHANGE) {
		AllSkillPointChange(false);
		return;
	} else if (opcode == CHANGE_MONEY_REQ) {
		uint8 sub_type = pkt.read<uint8>(); // type is irrelevant
		uint32 money = (uint32) pow((GetLevel() * 2.0f), 3.4f);

		if (GetLevel() < 30)
			money = (uint32) (money * 0.4f);
		else if (GetLevel() >= 60)
			money = (uint32) (money * 1.5f);

		// If nation discounts are enabled (1), and this nation has won the last war, get it half price.
		// If global discounts are enabled (2), everyone can get it for half price.
		if ((g_pMain->m_sDiscount == 1 && g_pMain->m_byOldVictory == GetNation())
			|| g_pMain->m_sDiscount == 2)
			money /= 2;

		result << uint8(CHANGE_MONEY_REQ) << money;
		Send(&result);
		return;
	} else if (opcode == CHANGE_REBIRTH_STAT) {
		if (GetRebLevel() >= 5)
			return;

		if (!CheckExistItem(900579000, 1))
			return;

		uint8 RecStr, RecSta, RecDex, RecInt, RecCha;

		uint8 rStr = GetRebStatBuff(STAT_STR),
			rSta = GetRebStatBuff(STAT_STA),
			rDex = GetRebStatBuff(STAT_DEX),
			rInt = GetRebStatBuff(STAT_INT),
			rCha = GetRebStatBuff(STAT_CHA);

		pkt >> RecStr >> RecSta >> RecDex >> RecInt >> RecCha;

		if (RecStr + RecSta + RecDex + RecInt + RecCha < 2)
			return;

		SetRebStatBuff(STAT_STR, rStr += RecStr);
		SetRebStatBuff(STAT_STA, rSta += RecSta);
		SetRebStatBuff(STAT_DEX, rDex += RecDex);
		SetRebStatBuff(STAT_INT, rInt += RecInt);
		SetRebStatBuff(STAT_CHA, rCha += RecCha);

		m_questMap[52] = 0;
		m_questMap[53] = 0;
		m_questMap[54] = 0;

		m_reblvl++;
		m_iExp = 0;

		RobItem(900579000, 1);
		result << uint8(CHANGE_REBIRTH_STAT) << uint16(1);
		Send(&result);
		Disconnect();
		return;
	}
	// If this packet was sent from the client, ignore it.
	else if (bFromClient)
		return;

	uint8 classcode = pkt.read<uint8>();
	switch (m_sClass) {
	case KARUWARRIOR:
		if (classcode == BERSERKER || classcode == GUARDIAN)
			bSuccess = true;
		break;
	case KARUROGUE:
		if (classcode == HUNTER || classcode == PENETRATOR)
			bSuccess = true;
		break;
	case KARUWIZARD:
		if (classcode == SORSERER || classcode == NECROMANCER)
			bSuccess = true;
		break;
	case KARUPRIEST:
		if (classcode == SHAMAN || classcode == DARKPRIEST)
			bSuccess = true;
		break;
	case ELMORWARRRIOR:
		if (classcode == BLADE || classcode == PROTECTOR)
			bSuccess = true;
		break;
	case ELMOROGUE:
		if (classcode == RANGER || classcode == ASSASSIN)
			bSuccess = true;
		break;
	case ELMOWIZARD:
		if (classcode == MAGE || classcode == ENCHANTER)
			bSuccess = true;
		break;
	case ELMOPRIEST:
		if (classcode == CLERIC || classcode == DRUID)
			bSuccess = true;
		break;
	case BERSERKER:
		if (classcode == GUARDIAN)
			bSuccess = true;
		break;
	case HUNTER:
		if (classcode == PENETRATOR)
			bSuccess = true;
		break;
	case SORSERER:
		if (classcode == NECROMANCER)
			bSuccess = true;
		break;
	case SHAMAN:
		if (classcode == DARKPRIEST)
			bSuccess = true;
		break;
	case BLADE:
		if (classcode == PROTECTOR)
			bSuccess = true;
		break;
	case PORUTU:
		if (classcode == PORUTUSKILLED)
			bSuccess = true;
		break;
	case PORUTUSKILLED:
		if (classcode == PORUTUMASTER)
			bSuccess = true;
		break;
	case KURIAN:
		if (classcode == KURIANSKILLED)
			bSuccess = true;
		break;
	case KURIANSKILLED:
		if (classcode == KURIANMASTER)
			bSuccess = true;
		break;
	case RANGER:
		if (classcode == ASSASSIN)
			bSuccess = true;
		break;
	case MAGE:
		if (classcode == ENCHANTER)
			bSuccess = true;
		break;
	case CLERIC:
		if (classcode == DRUID)
			bSuccess = true;
		break;
	}

	// Not allowed this job change
	if (!bSuccess) {
		result << uint8(CLASS_CHANGE_RESULT) << uint8(0);
		Send(&result);
		return;
	}

	m_sClass = classcode;
	if (isInParty()) {
		// TO-DO: Move this somewhere better.
		result.SetOpcode(WIZ_PARTY);
		result << uint8(PARTY_CLASSCHANGE) << GetSocketID() << uint16(classcode);
		g_pMain->Send_PartyMember(GetPartyID(), &result);
	}
}

void CUser::RecvSelectMsg(Packet & pkt)	// Receive menu reply from client.
{
	string szLuaFilename;
	pkt.SByte();
	pkt >> bMenuID >> szLuaFilename >> bySelectedReward;

	if (isTrading()
		|| isMerchanting()
		|| isMining()
		|| m_bMerchantStatex
		|| isDead()
		|| isStoreOpen())
		return;

	if (!AttemptSelectMsg(bMenuID, bySelectedReward))
		memset(&m_iSelMsgEvent, -1, sizeof(m_iSelMsgEvent));
}

bool CUser::AttemptSelectMsg(uint8 bMenuID, int8 bySelectedReward) {
	_QUEST_HELPER * pHelper = nullptr;
	if (bMenuID >= MAX_MESSAGE_EVENT
		|| isDead()
		|| m_nQuestHelperID == 0)
		return false;

	// Get the event number that needs to be processed next.
	int32 selectedEvent = m_iSelMsgEvent[bMenuID];
	if (selectedEvent < 0
		|| (pHelper = g_pMain->m_QuestHelperArray.GetData(m_nQuestHelperID)) == nullptr
		|| !V3_QuestRunEvent(pHelper, selectedEvent, bySelectedReward))
		return false;

	return true;
}

void CUser::SendSay(int32 nTextID[8]) {
	Packet result(WIZ_NPC_SAY);
	result << int32(-1) << int32(-1);
	foreach_array_n(i, nTextID, 8)
		result << nTextID[i];
	Send(&result);
}

void CUser::SelectMsg(uint8 bFlag, int32 nQuestID, int32 menuHeaderText,
	int32 menuButtonText[MAX_MESSAGE_EVENT], int32 menuButtonEvents[MAX_MESSAGE_EVENT]) {
	_QUEST_HELPER * pHelper = g_pMain->m_QuestHelperArray.GetData(m_nQuestHelperID);
	if (pHelper == nullptr)
		return;

	// Send the menu to the client
	Packet result(WIZ_SELECT_MSG);
	result.SByte();

	// There seems to be a problem with bFlags and the packet format.
	// A packet with value 4 often doesn't get sent
	if (bFlag == 4) {
		bFlag = 4;
	}

	result << m_sEventSid << bFlag << nQuestID << menuHeaderText;
	foreach_array_n(i, menuButtonText, MAX_MESSAGE_EVENT)
		result << menuButtonText[i];
	result << pHelper->strLuaFilename;
	Send(&result);

	// and store the corresponding event IDs.
	memcpy(&m_iSelMsgEvent, menuButtonEvents, sizeof(int32) * MAX_MESSAGE_EVENT);
}

void CUser::NpcEvent(Packet & pkt) {
	// Ensure AI is loaded first
	if (!g_pMain->m_bPointCheckFlag)
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

	Packet result;
	uint8 bUnknown = pkt.read<uint8>();
	uint16 sNpcID = pkt.read<uint16>();
	int32 nQuestID = pkt.read<int32>();

	CNpc *pNpc = g_pMain->GetNpcPtr(sNpcID);
	if (pNpc == nullptr
		|| !isInRange(pNpc, MAX_NPC_RANGE))
		return;

	switch (pNpc->GetType()) {
	case NPC_LOYALTY_MERCHANT:
		result.SetOpcode(WIZ_TRADE_NPC);
		result << pNpc->m_iSellingGroup;
		Send(&result);
		break;
	case NPC_MERCHANT:
	case NPC_TINKER:
		result.SetOpcode(pNpc->GetType() == NPC_MERCHANT ? WIZ_TRADE_NPC : WIZ_REPAIR_NPC);
		result << pNpc->m_iSellingGroup;
		Send(&result);
		break;

	case NPC_MARK:
		result.SetOpcode(WIZ_KNIGHTS_PROCESS);
		result << uint8(KNIGHTS_CAPE_NPC);
		Send(&result);
		break;

	case NPC_RENTAL:
		result.SetOpcode(WIZ_RENTAL);
		result << uint8(RENTAL_NPC)
			<< uint16(1) // 1 = enabled, -1 = disabled
			<< pNpc->m_iSellingGroup;
		Send(&result);
		break;

	case NPC_ELECTION:
	case NPC_TREASURY:
	{
		CKingSystem * pKingSystem = g_pMain->m_KingSystemArray.GetData(GetNation());
		result.SetOpcode(WIZ_KING);
		if (pNpc->GetType() == NPC_ELECTION) {
			// Ensure this still works as per official without a row in the table.
			string strKingName = (pKingSystem == nullptr ? "" : pKingSystem->m_strKingName);
			result.SByte();
			result << uint8(KING_NPC) << strKingName;
		} else {
			// Ensure this still works as per official without a row in the table.
			uint32 nTribute = (pKingSystem == nullptr ? 0 : pKingSystem->m_nTribute + pKingSystem->m_nTerritoryTax);
			uint32 nTreasury = (pKingSystem == nullptr ? 0 : pKingSystem->m_nNationalTreasury);
			result << uint8(KING_TAX) << uint8(1) // success
				<< uint16(isKing() ? 1 : 2) // 1 enables king-specific stuff (e.g. scepter), 2 is normal user stuff
				<< nTribute << nTreasury;
		}
		Send(&result);
	} break;

	case NPC_SIEGE:
	{
		_KNIGHTS_SIEGE_WARFARE *pKnightSiegeWarFare = g_pMain->GetSiegeMasterKnightsPtr(1);
		result.SetOpcode(WIZ_SIEGE);
		result << uint8(3) << uint8(7);
		Send(&result);
	}
	break;
	case NPC_SIEGE_1:
	{
		_KNIGHTS_SIEGE_WARFARE *pKnightSiegeWarFare = g_pMain->GetSiegeMasterKnightsPtr(1);
		if (pKnightSiegeWarFare->sMasterKnights == GetClanID()) {
			result.SetOpcode(WIZ_SIEGE);
			result << uint8(4) << uint8(1)

				<< pKnightSiegeWarFare->nDungeonCharge
				<< pKnightSiegeWarFare->nMoradonTax
				<< pKnightSiegeWarFare->nDellosTax;
			Send(&result);
		}
	}
	break;

	case NPC_VICTORY_GATE:
		switch (GetWarVictory()) {
		case KARUS:
			if (GetNation() == KARUS)
				ZoneChange(2, 222, 1846);
			break;
		case ELMORAD:
			if (GetNation() == ELMORAD)
				ZoneChange(1, 1865, 168);
			break;
		}
		break;

	case NPC_CAPTAIN:
		result.SetOpcode(WIZ_CLASS_CHANGE);
		result << uint8(CLASS_CHANGE_REQ);
		Send(&result);
		break;

	case NPC_WAREHOUSE:
		result.SetOpcode(WIZ_WAREHOUSE);
		result << uint8(WAREHOUSE_REQ);
		Send(&result);
		break;

	case NPC_CHAOTIC_GENERATOR:
	case NPC_CHAOTIC_GENERATOR2:
		SendAnvilRequest(sNpcID, ITEM_BIFROST_REQ);//Burda deniyoruz
		break;
	case NPC_KJWAR:
		result.SetOpcode(WIZ_CAPTURE);
		result << uint8(1) << uint8(7);
		Send(&result);
		break;
	case NPC_BORDER_MONUMENT:
		CaptureEvent();
		break;
	case NPC_CLAN: // this HAS to go.
		result << uint16(0); // page 0
		CKnightsManager::AllKnightsList(this, result);
	default:
		ClientEvent(sNpcID);
	}
}

void CUser::CaptureEvent() {
	if (GetZoneID() != ZONE_BORDER_DEFENSE_WAR ||
		g_pMain->pTempleEvent.m_sMiniTimerNation[GetEventRoom()] == GetNation())
		return;

	Packet Border;
	Packet Reg;

	m_tBorderCapure = UNIXTIME;

	Border.Initialize(WIZ_QUEST);
	Border << uint8(0x03) << uint32(9993);
	Send(&Border);

	Reg.Initialize(WIZ_CAPTURE);
	Reg << uint8(CAPURE_RIGHT_CLICK) << GetSocketID() << GetName();
	Send(&Reg);
}

// NPC Shop

void CUser::ShopHackerBan() {
	std::string sNoticeMessage;

	sNoticeMessage = string_format("%s is currently blocked by cheating.", GetName().c_str());
	if (!sNoticeMessage.empty())
		g_pMain->SendNotice(sNoticeMessage.c_str(), Nation::ALL);

	m_bAuthority = AUTHORITY_BANNED;
	Disconnect();
}

void CUser::ItemTrade(Packet & pkt) {
	Packet result(WIZ_ITEM_TRADE);
	uint32 transactionPrice;
	int group = 0;
	uint16 npcid;
	uint16 line;
	_ITEM_TABLE* pTable = nullptr;
	CNpc* pNpc = nullptr;
	uint8 type = 0, errorCode = 1, purchased_item_count;
	bool bSuccess = true;
	std::vector<ITEMS> pItems(0);
	_KNIGHTS_SIEGE_WARFARE *pSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(1);
	CKingSystem *pKingSystem = g_pMain->m_KingSystemArray.GetData(GetNation());
	DateTime time;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex) {
		errorCode = 1;
		goto send_packet;
	}

	pkt >> type;

	if (type == 5) {
		RecvRepurchase(pkt);
		return;
	}

	// Buy == 1, Sell == 2
	if (type == 1 || type == 2) {
		pkt >> group >> npcid;

		if (!g_pMain->m_bPointCheckFlag
			|| (pNpc = g_pMain->GetNpcPtr(npcid)) == nullptr
			|| (pNpc->GetType() != NPC_MERCHANT && pNpc->GetType() != NPC_TINKER && pNpc->GetType() != NPC_LOYALTY_MERCHANT && pNpc->m_iSellingGroup != 232000)
			|| pNpc->m_iSellingGroup != group
			|| !isInRange(pNpc, MAX_NPC_RANGE))
			goto fail_return;
	}

	pkt >> purchased_item_count;

	if (purchased_item_count > 14)
		purchased_item_count = 14;
	else if (purchased_item_count < 1)
		goto fail_return;

	if (type == 1) {
		for (int i = 0; i < purchased_item_count; i++) {
			ITEMS iItems;
			pkt >> iItems.ITEMID;
			pkt >> iItems.IPOS;
			pkt >> iItems._ICOUNT;
			pkt >> line;  // WtF ?
			pItems.resize(i + 1, iItems);

			_ITEM_TABLE * cItem = g_pMain->m_ItemtableArray.GetData(iItems.ITEMID);			// Shop Hacker's Fucked by Terry

			if (cItem == nullptr) {
				g_pMain->WriteCheatLogFile(string_format("[ ShopHack - %d:%d:%d ] Error Invalid: User %s tried to buy the Invalid Item %d NpcID %d \n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), iItems.ITEMID, npcid));
				goto fail_return;
			}

			if (cItem->m_bSellingGroup != group / 1000) {
				g_pMain->WriteCheatLogFile(string_format("[ ShopHack - %d:%d:%d ] Error Group: User %s tried to buy the Item %d in ShopGroup %d, NpcID %d \n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), iItems.ITEMID, group, npcid));
				//ShopHackerBan();
				goto fail_return;
			}

			if (iItems.IPOS >= HAVE_MAX || iItems._ICOUNT < 1 // Dupe Fixed.
				|| iItems._ICOUNT > MAX_ITEM_COUNT
				|| (iItems._ICOUNT > 200 && cItem->m_iBuyPrice >= 100000)) {
				errorCode = 2;
				goto fail_return;
			}
		}
	} else {
		for (int i = 0; i < purchased_item_count; i++) {
			ITEMS iItems;
			pkt >> iItems.ITEMID;
			pkt >> iItems.IPOS;
			pkt >> iItems._ICOUNT;
			pItems.resize(i + 1, iItems);

			_ITEM_TABLE * cItem = g_pMain->m_ItemtableArray.GetData(iItems.ITEMID);			// Shop Hacker's Fucked by Terry

			if (cItem == nullptr) {
				g_pMain->WriteCheatLogFile(string_format("[ ShopHack - %d:%d:%d ] Error Invalid: User %s tried to buy the Invalid Item %d NpcID %d \n", time.GetHour(), time.GetMinute(), time.GetSecond(), GetName().c_str(), iItems.ITEMID, npcid));
				goto fail_return;
			}

			if (iItems.IPOS >= HAVE_MAX || iItems._ICOUNT < 1
				|| iItems._ICOUNT > MAX_ITEM_COUNT
				|| (iItems._ICOUNT > 200 && cItem->m_iSellPrice >= 100000)) {
				errorCode = 2;
				goto fail_return;
			}
		}
	}

	uint32 real_price = 0;
	uint32 total_price = 0;
	// Buying from an NPC Gold
	if (type == 1 && pNpc->m_iSellingGroup != 249000) {
		for (int i = 0; i < purchased_item_count; i++) {
			transactionPrice = 0;
			if (pItems[i].ITEMID != 0) {
				if (isTrading()
					|| (pTable = g_pMain->GetItemPtr(pItems[i].ITEMID)) == nullptr
					|| (type == 2
						&& ((pItems[i].ITEMID >= ITEM_NO_TRADE && pItems[i].ITEMID < ITEM_NO_TRADE_MAX)
							|| pTable->m_bRace == RACE_UNTRADEABLE)))
					goto fail_return;

				if (pTable->m_iBuyPrice > 200000
					&& pItems[i]._ICOUNT > 100
					|| (pItems[i]._ICOUNT > 2000
						&& pTable->m_iBuyPrice > 2)
					|| pItems[i]._ICOUNT > 9999) {
					errorCode = 4;
					goto fail_return;
				}

				if (pTable->m_bSellingGroup == 0
					|| pTable->m_iNPBuyPrice > 0) {
					errorCode = 2;
					goto fail_return;
				}

				if (pItems[i].IPOS >= HAVE_MAX
					|| pItems[i]._ICOUNT <= 0
					|| pItems[i]._ICOUNT > MAX_ITEM_COUNT) {
					errorCode = 2;
					goto fail_return;
				}

				if (i == 0) {
					_ITEM_TABLE *prTable = nullptr;
					uint32 totalweight = 0;
					for (int j = 0; j < purchased_item_count; j++) {
						prTable = g_pMain->GetItemPtr(pItems[j].ITEMID);

						if (prTable == nullptr)
							goto fail_return;

						total_price = total_price + uint32(((uint32) prTable->m_iBuyPrice * pItems[j]._ICOUNT));
						totalweight += prTable->m_sWeight * pItems[i]._ICOUNT;

						if (prTable->m_bSellingGroup == 0)
							goto fail_return;
					}

					if ((totalweight + m_sItemWeight) > m_sMaxWeight) {
						errorCode = 4;
						total_price = 0;
						goto fail_return;
					}

					if (!hasCoins(total_price)) {
						errorCode = 3;
						total_price = 0;
						goto fail_return;
					}
				}

				if (pItems[i].ITEMID != 0) {
					if (m_sItemArray[SLOT_MAX + pItems[i].IPOS].nNum != 0) {
						if (m_sItemArray[SLOT_MAX + pItems[i].IPOS].nNum != pItems[i].ITEMID) {
							errorCode = 2;
							goto fail_return;
						}

						if (!pTable->m_bCountable || pItems[i]._ICOUNT <= 0) {
							errorCode = 2;
							goto fail_return;
						}

						if (pTable->m_bCountable
							&& (pItems[i]._ICOUNT + m_sItemArray[SLOT_MAX + pItems[i].IPOS].sCount) > MAX_ITEM_COUNT) {
							errorCode = 4;
							goto fail_return;
						}
					}

					uint32 tariffTax = 0/*Kiþiye*/, nationalTax = 0/*Irka*/;
					uint32 BuyPrice = pTable->m_iBuyPrice * 90 / 100;
					if (GetMap()->GetTariff() > 0)
						tariffTax = (pTable->m_iBuyPrice * GetMap()->GetTariff() / 100 * pItems[i]._ICOUNT);

					switch (GetZoneID()) {
					case ZONE_MORADON:
					case ZONE_MORADONM2:
						if (pSiegeWar->nMoradonTax < COIN_MAX)
							pSiegeWar->nMoradonTax += tariffTax;
						if (pSiegeWar->nMoradonTax < COIN_MAX)
							InsertTaxUpEvent(ZONE_MORADON, tariffTax);
						break;
					case ZONE_DELOS:
					case ZONE_HELL_ABYSS:
					case ZONE_DESPERATION_ABYSS:
						if (pSiegeWar->nMoradonTax < COIN_MAX)
							pSiegeWar->nDellosTax += tariffTax;
						if (pSiegeWar->nMoradonTax < COIN_MAX)
							InsertTaxUpEvent(ZONE_DELOS, tariffTax);
						break;
					default:
						nationalTax = pTable->m_iBuyPrice * 10 / 100 * pItems[i]._ICOUNT;

						if (tariffTax > nationalTax)
							tariffTax -= nationalTax;

						if (pKingSystem->m_nTerritoryTax < COIN_MAX) {
							pKingSystem->m_nTerritoryTax += (tariffTax);
							InsertTaxUpEvent(GetNation(), (tariffTax));
						}

						if (pKingSystem->m_nNationalTreasury < COIN_MAX) {
							pKingSystem->m_nNationalTreasury += (nationalTax);
							InsertTaxUpEvent((GetNation() + 10), (nationalTax));
						}
						break;
					}

					if (pTable->m_iSellPrice != SellTypeFullPrice)
						transactionPrice = ((((uint32) BuyPrice) * pItems[i]._ICOUNT) + tariffTax + nationalTax);
					else
						transactionPrice = (((uint32) pTable->m_iBuyPrice) * pItems[i]._ICOUNT);

					if (!hasCoins(transactionPrice)) {
						errorCode = 3;
						goto fail_return;
					}

					m_sItemArray[SLOT_MAX + pItems[i].IPOS].nNum = pItems[i].ITEMID;
					m_sItemArray[SLOT_MAX + pItems[i].IPOS].sDuration = pTable->m_sDuration;
					m_sItemArray[SLOT_MAX + pItems[i].IPOS].sCount += pItems[i]._ICOUNT;

					m_iGold -= transactionPrice;

					if (!pTable->m_bCountable)
						m_sItemArray[SLOT_MAX + pItems[i].IPOS].nSerialNum = g_pMain->GenerateItemSerial();

					SetUserAbility(false);
					SendItemWeight();

					string errorMessage = string_format(_T("NPC_ITEM_BUY uId-%s- nId-%s- P-%d- I-%d- Z-%d- X-%d- Y-%d- t-%d-"),
						GetName().c_str(), pNpc->GetName().c_str(), transactionPrice, pItems[i].ITEMID, GetZoneID(), uint16(GetX()), uint16(GetZ()), tariffTax);
					g_pMain->WriteTradeUserLogFile(errorMessage);
				}
			}
			real_price += transactionPrice;
		}
	}
	// Selling an item to an NPC Gold
	else if (type == 2 && pNpc->m_iSellingGroup != 249000) {
		for (int i = 0; i < purchased_item_count; i++) {
			if (isTrading()
				|| (pTable = g_pMain->GetItemPtr(pItems[i].ITEMID)) == nullptr
				|| (type == 2
					&& ((pItems[i].ITEMID >= ITEM_NO_TRADE && pItems[i].ITEMID < ITEM_NO_TRADE_MAX)
						|| pTable->m_bRace == RACE_UNTRADEABLE)))
				goto fail_return;

			_ITEM_DATA *pItem = &m_sItemArray[SLOT_MAX + pItems[i].IPOS];
			if (pItem->nNum != pItems[i].ITEMID
				|| pItem->isSealed()
				|| pItem->isRented()
				|| pItem->nExpirationTime > 0) {
				errorCode = 2;
				goto fail_return;
			}

			if (pItem->sCount < pItems[i]._ICOUNT
				|| pItems[i]._ICOUNT < 1) {
				errorCode = 3;
				goto fail_return;
			}

			if (PremiumID == 0 && pTable->m_iSellPrice == SellTypeFullPrice)
				transactionPrice = ((uint32) pTable->m_iBuyPrice * pItems[i]._ICOUNT);
			else if (PremiumID > 0 && pTable->m_iSellPrice == SellTypeFullPrice)
				transactionPrice = ((uint32) pTable->m_iBuyPrice * pItems[i]._ICOUNT);
			else if (PremiumID == 0 && pTable->m_iSellPrice != SellTypeFullPrice)
				transactionPrice = (((uint32) pTable->m_iBuyPrice / 6) * pItems[i]._ICOUNT);
			else if (PremiumID > 0 && pTable->m_iSellPrice != SellTypeFullPrice)
				transactionPrice = (((uint32) pTable->m_iBuyPrice / 4) * pItems[i]._ICOUNT);

			if (GetCoins() + transactionPrice > COIN_MAX) {
				errorCode = 3;
				goto fail_return;
			}

			GoldGain(transactionPrice, false);

			if (pTable->isStackable())
				pItem->sCount -= pItems[i]._ICOUNT;
			else
				pItem->sCount = 0;

			if (pItems[i]._ICOUNT >= pItem->sCount)
				memset(pItem, 0, sizeof(_ITEM_DATA));
			else
				pItem->sCount -= pItems[i]._ICOUNT;

			SetUserAbility(false);
			SendItemWeight();

			string errorMessage = string_format(_T("NPC_ITEM_SELL uId-%s- nId-%s- P-%d- I-%d- Z-%d- X-%d- Y-%d-"),
				GetName().c_str(), pNpc->GetName().c_str(), transactionPrice, pItems[i].ITEMID, GetZoneID(), uint16(GetX()), uint16(GetZ()));
			g_pMain->WriteTradeUserLogFile(errorMessage);
			real_price += transactionPrice;
		}
	}
	// Buying an item to an NPC NationalPoint
	else if (type == 1 && pNpc->m_iSellingGroup == 249000) {
		for (int i = 0; i < purchased_item_count; i++) {
			transactionPrice = 0;
			if (pItems[i].ITEMID != 0) {
				if (isTrading()
					|| (pTable = g_pMain->GetItemPtr(pItems[i].ITEMID)) == nullptr
					|| (type == 2
						&& ((pItems[i].ITEMID >= ITEM_NO_TRADE && pItems[i].ITEMID < ITEM_NO_TRADE_MAX)
							|| pTable->m_bRace == RACE_UNTRADEABLE)))
					goto fail_return;

				if (pTable->m_iNPBuyPrice > 200000 && pItems[i]._ICOUNT > 100) {
					errorCode = 4;
					goto fail_return;
				}

				if (pTable->m_bSellingGroup == 0
					|| pTable->m_iNPBuyPrice == 0) {
					errorCode = 2;
					goto fail_return;
				}

				if (pItems[i].IPOS >= HAVE_MAX
					|| pItems[i]._ICOUNT <= 0 || pItems[i]._ICOUNT > MAX_ITEM_COUNT) {
					errorCode = 2;
					goto fail_return;
				}

				if (i == 0) {
					_ITEM_TABLE *prTable;
					for (int j = 0; j < purchased_item_count; j++) {
						prTable = nullptr;
						prTable = g_pMain->GetItemPtr(pItems[j].ITEMID);
						if (prTable != nullptr)
							total_price += uint32(((uint32) prTable->m_iNPBuyPrice * pItems[j]._ICOUNT));
					}

					if (!hasLoyalty(total_price)) {
						errorCode = 3;
						total_price = 0;
						goto fail_return;
					}
				}

				if (pItems[i].ITEMID != 0) {
					if (m_sItemArray[SLOT_MAX + pItems[i].IPOS].nNum != 0) {
						if (m_sItemArray[SLOT_MAX + pItems[i].IPOS].nNum != pItems[i].ITEMID) {
							errorCode = 2;
							goto fail_return;
						}

						if (!pTable->m_bCountable || pItems[i]._ICOUNT <= 0) {
							errorCode = 2;
							goto fail_return;
						}

						if (pTable->m_bCountable
							&& (pItems[i]._ICOUNT + m_sItemArray[SLOT_MAX + pItems[i].IPOS].sCount) > MAX_ITEM_COUNT) {
							errorCode = 4;
							goto fail_return;
						}
					}

					uint32 BuyPrice;
					BuyPrice = pTable->m_iNPBuyPrice;

					transactionPrice = ((((uint32) BuyPrice) * pItems[i]._ICOUNT));

					if (!hasLoyalty(transactionPrice) || pTable->m_bSellingGroup == 0) {
						errorCode = 3;
						goto fail_return;
					}

					if (((pTable->m_sWeight * pItems[i]._ICOUNT) + m_sItemWeight) > m_sMaxWeight) {
						errorCode = 4;
						goto fail_return;
					}

					m_sItemArray[SLOT_MAX + pItems[i].IPOS].nNum = pItems[i].ITEMID;
					m_sItemArray[SLOT_MAX + pItems[i].IPOS].sDuration = pTable->m_sDuration;
					m_sItemArray[SLOT_MAX + pItems[i].IPOS].sCount += pItems[i]._ICOUNT;

					m_iLoyalty -= transactionPrice;

					if (!pTable->m_bCountable)
						m_sItemArray[SLOT_MAX + pItems[i].IPOS].nSerialNum = g_pMain->GenerateItemSerial();

					SetUserAbility(false);
					SendItemWeight();

					string errorMessage = string_format(_T("NPC_ITEM_BUY_NP uId-%s- nId-%s- P-%d- I-%d- Z-%d- X-%d- Y-%d-"),
						GetName().c_str(), pNpc->GetName().c_str(), transactionPrice, pItems[i].ITEMID, GetZoneID(), uint16(GetX()), uint16(GetZ()));
					g_pMain->WriteTradeUserLogFile(errorMessage);
				}
			}
			real_price += transactionPrice;
		}
	}
	// Selling an item to an NPC NationalPoint
	else
		goto fail_return;

	goto send_packet;

fail_return:
	bSuccess = false;
send_packet:
	result << bSuccess;

	if (!bSuccess)
		result << errorCode;

	else if (pNpc == nullptr) {
		bSuccess = false;
		result << uint8(0x00) << bSuccess;
	}

	if (!bSuccess)
		result << errorCode;
	else if (pNpc->m_iSellingGroup == 249000 && bSuccess)
		result << m_iLoyalty << real_price << pTable->m_bSellingGroup;
	else if (pNpc->m_iSellingGroup != 249000 && bSuccess && pTable)
		result << m_iGold << real_price << pTable->m_bSellingGroup;
	Send(&result);
}

/**
* @brief	Handles the name change response packet
* 			containing the specified new name.
*
* @param	pkt	The packet.
*/
void CUser::HandleNameChange(Packet & pkt) {
	uint8 opcode;
	pkt >> opcode;

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	switch (opcode) {
	case NameChangePlayerRequest:
		HandlePlayerNameChange(pkt);
		break;
	case ClanNameChange:
		HandleKnightsNameChange(pkt);
		break;
	}
}

void CUser::HandleKnightsNameChange(Packet & pkt) {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	ClanNameChangeOpcode response = ClanNameChangeSuccess;
	pkt.DByte();
	string strKnightsID;
	pkt >> strKnightsID;

	if (strKnightsID.empty() || strKnightsID.length() > MAX_ID_SIZE || !g_pMain->WordGuardSystem(strKnightsID, (uint8) strKnightsID.length()))
		response = ClanNameChangeInvalidName;
	else if (!isInClan() || !isClanLeader())
		response = ClanNameChangeNotInClan;
	else if (!CheckExistItem(800086000))
		response = ClanNameChangeInvalidName;

	if (response != ClanNameChangeSuccess) {
		SendKnightsNameChange(response);
		return;
	}

	Packet result(WIZ_NAME_CHANGE, uint8(ClanNameChange));
	result << strKnightsID;
	g_pMain->AddDatabaseRequest(result, this);
}

/**
* @brief	Handles the character name change response packet
* 			containing the specified new character's name.
*
* @param	pkt	The packet.
*/
void CUser::HandlePlayerNameChange(Packet & pkt) {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	NameChangeOpcode response = NameChangeSuccess;
	string strUserID;
	pkt >> strUserID;

	if (strUserID.empty() || strUserID.length() > MAX_ID_SIZE || !g_pMain->WordGuardSystem(strUserID, (uint8) strUserID.length()))
		response = NameChangeInvalidName;
	else if (isInClan())
		response = NameChangeInClan;
	else if (isKing())
		response = NameChangeKing;

	if (response != NameChangeSuccess) {
		SendNameChange(response);
		return;
	}

	// Ensure we have the scroll before handling this request.
	if (!CheckExistItem(ITEM_SCROLL_OF_IDENTITY))
		return;

	Packet result(WIZ_NAME_CHANGE, uint8(NameChangePlayerRequest));
	result << strUserID;
	g_pMain->AddDatabaseRequest(result, this);
}
/**
* @brief	Sends a name change packet.
*
* @param	opcode	Name change packet opcode.
* 					NameChangeShowDialog shows the dialog where you can set your name.
* 					NameChangeSuccess confirms the name was changed.
* 					NameChangeInvalidName throws an error reporting the name is invalid.
* 					NameChangeInClan throws an error reporting the user's still in a clan (and needs to leave).
*					NameChangeIsKing if the user is king
*/
void CUser::SendNameChange(NameChangeOpcode opcode /*= NameChangeShowDialog*/) {
	Packet result(WIZ_NAME_CHANGE, uint8(opcode));
	Send(&result);
}

void CUser::SendKnightsNameChange(ClanNameChangeOpcode opcode /*= ClanNameChangeShowDialog*/) {
	Packet result(WIZ_NAME_CHANGE, uint8(16));
	result << opcode;
	Send(&result);
}

void CUser::HandleCapeChange(Packet & pkt) {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	Packet result(WIZ_CAPE);
	CKnights *pKnights = nullptr;
	_KNIGHTS_CAPE *pCape = nullptr;
	uint32 nReqClanPoints = 0, nReqCoins = 0;
	int16 sErrorCode = 0, sCapeID;
	uint8 r, g, b;
	bool bApplyingPaint = false;

	pkt >> sCapeID >> r >> g >> b;

	// If we're not a clan leader, what are we doing changing the cape?
	if (!isClanLeader()
		|| isDead()) {
		sErrorCode = -1;
		goto fail_return;
	}

	// Does the clan exist?
	if ((pKnights = g_pMain->GetClanPtr(GetClanID())) == nullptr) {
		sErrorCode = -2;
		goto fail_return;
	}

	// Make sure we're promoted
	if (!pKnights->isPromoted()
		// and that if we're in an alliance, we're the primary clan in the alliance.
		|| (pKnights->isInAlliance() && !pKnights->isAllianceLeader())) {
		sErrorCode = -1;
		goto fail_return;
	}

	if (sCapeID >= 0) {
		// Does this cape type exist?
		if ((pCape = g_pMain->m_KnightsCapeArray.GetData(sCapeID)) == nullptr || sCapeID == 99) {
			sErrorCode = -5;
			goto fail_return;
		}

		// Is our clan allowed to use this cape?
		if ((pCape->byGrade && pKnights->m_byGrade > pCape->byGrade)
			// not sure if this should use another error, need to confirm
			|| pKnights->m_byFlag < pCape->byRanking) {
			sErrorCode = -6;
			goto fail_return;
		}

		// NOTE: Error code -8 is for nDuration
		// It applies if we do not have the required item ('nDuration', awful name).
		// Since no capes seem to use it, we'll ignore it...

		// Can we even afford this cape?
		if (!hasCoins(pCape->nReqCoins)) {
			sErrorCode = -7;
			goto fail_return;
		}

		nReqCoins = pCape->nReqCoins;
		nReqClanPoints = pCape->nReqClanPoints;
	}

	// These are 0 when not used
	if (r != 0 || g != 0 || b != 0) {
		// To use paint, the clan needs to be accredited
		if (pKnights->m_byFlag < ClanTypeAccredited5) {
			sErrorCode = -1; // need to find the error code for this
			goto fail_return;
		}

		bApplyingPaint = true;
		nReqClanPoints += 1000; // does this need tweaking per clan rank?
	}

	// If this requires clan points, does our clan have enough?
	if (pKnights->m_nClanPointFund < nReqClanPoints) {
		// this error may not be correct
		sErrorCode = -7;
		goto fail_return;
	}

	if (nReqCoins > 0)
		GoldLose(nReqCoins);

	if (nReqClanPoints) {
		pKnights->m_nClanPointFund -= nReqClanPoints;
		pKnights->UpdateClanFund();
	}

	// Are we changing the cape?
	if (sCapeID >= 0)
		pKnights->m_sCape = sCapeID;

	// Are we applying paint?
	if (bApplyingPaint) {
		pKnights->m_bCapeR = r;
		pKnights->m_bCapeG = g;
		pKnights->m_bCapeB = b;
	}

	CKnights *aKnights = g_pMain->GetClanPtr(pKnights->GetAllianceID());

	result << uint16(1) // success
		<< pKnights->GetAllianceID()
		<< pKnights->GetID()
		<< pKnights->m_sCape
		<< pKnights->m_bCapeR << pKnights->m_bCapeG << pKnights->m_bCapeB
		<< uint8(0);

	pKnights->Send(&result);

	// TO-DO:
	// When we implement alliances, this should send to the alliance
	// if the clan is part of one. Also, their capes should be updated.

	// TO-DO: Send to other servers via UDP.

	// Now tell Aujard to save (we don't particularly care whether it was able to do so or not).
	result.Initialize(WIZ_CAPE);
	result << pKnights->GetID() << pKnights->CapGetCapeID()
		<< pKnights->m_bCapeR << pKnights->m_bCapeG << pKnights->m_bCapeB;
	g_pMain->AddDatabaseRequest(result, this);
	return;

fail_return:
	result << sErrorCode;
	Send(&result);
}

void CUser::SendRepurchase(bool nReflash /*= false */) {
	g_DBAgent.LoadRepurchase(this);

	uint8 OpCode = 1;
	uint16 rCount = uint16(m_RepurchaseMap.size());
	uint32 nPrice;

	if (nReflash)
		OpCode = 4;

	Packet result(WIZ_ITEM_TRADE, uint8(5));
	result << OpCode
		<< uint8(1)
		<< rCount;

	BOOST_FOREACH(auto itr, m_RepurchaseMap) {
		_ITEM_TABLE * pItem = g_pMain->m_ItemtableArray.GetData(itr.second->nNum);

		if (pItem == nullptr)
			continue;

		nPrice = pItem->m_iBuyPrice * 300;

		if (nPrice > COIN_MAX)
			nPrice = COIN_MAX;

		result << itr.second->nNum
			<< nPrice
			<< uint16(0)
			<< rCount--
			<< uint8(0);
	}

	Send(&result);
}

void CUser::RecvRepurchase(Packet& pkt) {
	uint16 opcode, nSlot;
	uint32 nItemID, nPrice;

	pkt >> opcode >> nItemID;

	if (opcode == 0) {
		SendRepurchase(true);
		return;
	} else if (opcode != 2)
		return;

	BOOST_FOREACH(auto itr, m_RepurchaseMap) {
		if (itr.second->nNum != nItemID)
			continue;

		nSlot = itr.first;
		break;
	}

	RepurchaseMap::iterator itr = m_RepurchaseMap.find(nSlot);
	_ITEM_TABLE * pItem = g_pMain->m_ItemtableArray.GetData(nItemID);

	if (pItem == nullptr || itr == m_RepurchaseMap.end())
		return;

	nPrice = pItem->m_iBuyPrice * 300;
	if (GetCoins() < nPrice) {
		uint32 nDiference = nPrice - GetCoins();

		Packet result(WIZ_ITEM_TRADE, uint8(5));
		result << uint8(2)	// 2 success
			<< uint8(2)
			<< uint8(0) // Count
			<< uint16(0)
			<< uint16(0);

		Send(&result);
		return;
	}

	int8 pos;
	bool bNewItem = true;
	pos = FindSlotForItem(nItemID, 1);
	if (pos < 0) {
		Packet result(WIZ_ITEM_TRADE, uint8(5));
		result << uint8(2) << uint8(2) << uint16(-4); // Not enough inventory slot
		Send(&result);
		return;
	}

	_ITEM_DATA *pItemData = GetItem(pos);
	if (pItemData->nNum != 0 || pItemData == nullptr)
		bNewItem = false;

	GoldLose(nPrice);
	GiveItem(nItemID, 1);
	SendStackChange(nItemID, m_sItemArray[pos].sCount, m_sItemArray[pos].sDuration, pos - SLOT_MAX, true);
	g_DBAgent.DeleteRepurchase(itr->second->nNum, itr->second->tRepTime, this);

	Packet result(WIZ_ITEM_TRADE, uint8(5));
	result << uint8(2)	// 2 Send Success
		<< uint8(1)		// 1 Success,  2 Failed, press button again
		<< uint16(0);

	result << nItemID;

	Send(&result);
}