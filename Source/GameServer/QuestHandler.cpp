#include "stdafx.h"
#include "KnightsManager.h"
#include <boost\foreach.hpp>	

void CUser::V3_QuestDataRequest() {
	Packet result(WIZ_QUEST, uint8(1));
	result << uint16(m_questMap.size());

	if (m_questMap.size() > 0)
		foreach(itr, m_questMap)
		result << itr->first << itr->second;

	Send(&result);
}

void CUser::SendSkillQuestFinish() {
	if (isWarrior() && isMastered()) {
		if (!V3_CheckExistEvent(334, 2))
			V3_QuestEvent(334, 2);
		if (!V3_CheckExistEvent(359, 2))
			V3_QuestEvent(359, 2);
		if (!V3_CheckExistEvent(365, 2))
			V3_QuestEvent(365, 2);
	} else if (isRogue() && isMastered()) {
		if (!V3_CheckExistEvent(335, 2))
			V3_QuestEvent(335, 2);
		if (!V3_CheckExistEvent(347, 2))
			V3_QuestEvent(347, 2);
		if (!V3_CheckExistEvent(360, 2))
			V3_QuestEvent(360, 2);
		if (!V3_CheckExistEvent(366, 2))
			V3_QuestEvent(366, 2);
	} else if (isMage() && isMastered()) {
		if (!V3_CheckExistEvent(336, 2))
			V3_QuestEvent(336, 2);
		if (!V3_CheckExistEvent(348, 2))
			V3_QuestEvent(348, 2);
		if (!V3_CheckExistEvent(361, 2))
			V3_QuestEvent(361, 2);
		if (!V3_CheckExistEvent(367, 2))
			V3_QuestEvent(367, 2);
	} else if (isPriest() && isMastered()) {
		if (!V3_CheckExistEvent(337, 2))
			V3_QuestEvent(337, 2);
		if (!V3_CheckExistEvent(349, 2))
			V3_QuestEvent(349, 2);
		if (!V3_CheckExistEvent(357, 2))
			V3_QuestEvent(357, 2);
		if (!V3_CheckExistEvent(362, 2))
			V3_QuestEvent(362, 2);
		if (!V3_CheckExistEvent(363, 2))
			V3_QuestEvent(363, 2);
		if (!V3_CheckExistEvent(364, 2))
			V3_QuestEvent(364, 2);
		if (!V3_CheckExistEvent(368, 2))
			V3_QuestEvent(368, 2);
	} else if (isKurianPortu() && isMastered()) {
		if (!V3_CheckExistEvent(1377, 2))
			V3_QuestEvent(1377, 2);
		if (!V3_CheckExistEvent(1378, 2))
			V3_QuestEvent(1378, 2);
	}
}

void CUser::V3_QuestProcess(Packet & pkt) {
	uint8 opcode = pkt.read<uint8>();
	uint32 nQuestID = pkt.read<uint32>();

	CNpc *pNpc = g_pMain->GetNpcPtr(m_sEventNid);
	_QUEST_HELPER * pQuestHelper = g_pMain->m_QuestHelperArray.GetData(nQuestID);
	// Does this quest helper exist?
	if (pQuestHelper == nullptr)
		return;

	uint16 nQuest = pQuestHelper->sEventDataIndex;

	// If we're the same min. level as the quest requires, 
	// do we have the min. required XP? Seems kind of silly, but OK..
	if (pQuestHelper->bLevel == GetLevel() && pQuestHelper->nExp > m_iExp)
		return;

	switch (opcode) {
	case 3:
	case 7:
		V3_QuestExecuteHelper(pQuestHelper);
		V3_QuestMonsterDataRequest(nQuest);
		break;

	case 4:
		V3_QuestCheckFulfill(pQuestHelper);
		break;

	case 5:
		if (!V3_CheckExistEvent(pQuestHelper->sEventDataIndex, 2))
			V3_QuestEvent(pQuestHelper->sEventDataIndex, 4);


		V3_QuestMonsterDataDeleteAll(nQuest);
		V3_QuestMonsterDataRequest(nQuest);


		// Kick the user out of the quest zone.
		// Monster suppression squad is the only zone I'm aware of that this should apply to.
		if (GetZoneID() >= 81 && GetZoneID() <= 83)
			KickOutZoneUser(true);
		break;

	case 6:
		if (!V3_CheckExistEvent(pQuestHelper->sEventDataIndex, 2))
			V3_QuestEvent(pQuestHelper->sEventDataIndex, 1);
		break;

	case 12:
		if (!V3_CheckExistEvent(pQuestHelper->sEventDataIndex, 3))
			V3_QuestEvent(pQuestHelper->sEventDataIndex, 1);
		break;
	}
}

void CUser::V3_QuestEvent(uint16 sQuestID, uint8 bQuestState) {
	if (V3_CheckExistEvent(sQuestID, 2) && (bQuestState == 2 || bQuestState == 1 || bQuestState == 4)) {
		// This use to ban people for doing repeatable quests. We want repeatable quests to be allowed
	/*	Packet resultmer;
		std::string bufferpro = string_format("[Quest Message] only can do one time");
		ChatPacket::Construct(&resultmer, 7, &bufferpro);
		Send(&resultmer);
		m_bAuthority = AUTHORITY_BANNED;
		Disconnect();
		std::string BanNotice;
		BanNotice = string_format("%s is currently blocked for illegal activity. Reason: Tried to do quest bug.", GetName().c_str());
		g_pMain->SendNotice(BanNotice.c_str(), Nation::ALL);

		printf("%s is currently blocked for illegal activity. Quest BUG \n", GetName().c_str());
		return;*/
	}
	_QUEST_MONSTER * pQuestMonster = g_pMain->m_QuestMonsterArray.GetData(sQuestID);
	_QUEST_HELPER * pQuestHelper = g_pMain->m_QuestHelperArray.GetData(sQuestID);

	if (sQuestID == STARTER_SEED_QUEST && m_questMap[sQuestID] == 1 && bQuestState == 2) {
		Packet video(WIZ_SELECT_MSG);
		video << uint16(1) << uint8(15);
		Send(&video);
	}

	m_questMap[sQuestID] = bQuestState;

	if (sQuestID < 0)
		return;

	Packet result(WIZ_QUEST, uint8(2));
	result << sQuestID << bQuestState;
	Send(&result);

	if (bQuestState == 2) {
		_QUEST_HELPER * pHelper = nullptr;
		uint32 jIndex = 0;
		foreach_stlmap(itr, g_pMain->m_QuestHelperArray) {
			pHelper = itr->second;

			if (pHelper == nullptr)
				continue;

			if (pHelper->sEventDataIndex != sQuestID)
				continue;

			if (pHelper->bEventStatus != 2)
				continue;

			jIndex = pHelper->nExchangeIndex;
			break;
		}
		/* Now Closed.

					_ITEM_EXCHANGE * pItem = g_pMain->m_ItemExchangeArray.GetData(jIndex);

					if (pQuestMonster == nullptr)
					{
						for(int i = 0; i < ITEMS_IN_ORIGIN_GROUP; i++)
						{
							if(pItem != nullptr && pItem->nOriginItemNum[i] != 0)
							{
								RobItem(pItem->nOriginItemNum[i], pItem->sOriginItemCount[i]);
							}
						}
					}

					RunExchange(jIndex);*/

		V3_QuestMonsterDataDeleteAll(sQuestID);
		V3_QuestMonsterDataRequest(sQuestID);
	}

	if (bQuestState == 1 && pQuestMonster != nullptr) {
		//_QUEST_DATA * dQuest = m_QuestDataArray.GetData(sQuestID);
		Quest_MonMap::iterator itr = m_QuestMonMap.find(sQuestID);

		if (itr == m_QuestMonMap.end()) {
			_QUEST_DATA * nQuest = new _QUEST_DATA;
			//nQuest->dQuestID = sQuestID;
			nQuest->dCKills[0] = 0;
			nQuest->dCKills[1] = 0;
			nQuest->dCKills[2] = 0;
			nQuest->dCKills[3] = 0;
			m_QuestMonMap.insert(std::make_pair(sQuestID, nQuest));
		}

		V3_QuestMonsterDataRequest(sQuestID);
	}
}

void CUser::V3_QuestExecuteHelper(_QUEST_HELPER * pQuestHelper) {
	if (pQuestHelper == nullptr && pQuestHelper->bQuestType != 3)
		return;

	V3_QuestRunEvent(pQuestHelper, pQuestHelper->nEventTriggerIndex); // NOTE: Fulfill will use nEventCompleteIndex
}

void CUser::V3_QuestCheckFulfill(_QUEST_HELPER * pQuestHelper) {
	if (pQuestHelper == nullptr || !V3_CheckExistEvent(pQuestHelper->sEventDataIndex, 1))
		return;

	V3_QuestRunEvent(pQuestHelper, pQuestHelper->nEventCompleteIndex);
}

void CUser::V3_RequestStart() {
	_QUEST_DATA * dQuest = nullptr;
	foreach(itr, m_QuestMonMap) {
		V3_QuestMonsterDataRequest(itr->first);
	}
}

void CUser::V3_QuestMonsterDataRequest(uint16 uQuestID) {
	Packet result(WIZ_QUEST, uint8(9));
	Quest_MonMap::iterator itr = m_QuestMonMap.find(uQuestID);

	if (itr == m_QuestMonMap.end())
		return;



	result << uint8(1)
		<< itr->first
		<< itr->second->dCKills[0] << itr->second->dCKills[1]
		<< itr->second->dCKills[2] << itr->second->dCKills[3];

	Send(&result);
}

void CUser::V3_QuestMonsterDataDeleteAll(uint16 nQuestID) {
	m_QuestMonMap.erase(nQuestID);
}

bool CUser::V3_CheckExistEvent(uint16 sQuestID, uint8 bQuestState) {
	// Attempt to find a quest with that ID in the map
	QuestMap::iterator itr = m_questMap.find(sQuestID);

	// If it doesn't exist, it doesn't exist. 
	// Unless of course, we wanted it to not exist, in which case we're right!
	// (this does seem silly, but this behaviour is STILL expected, so do not remove it.)
	if (itr == m_questMap.end())
		return bQuestState == 0;


	return itr->second == bQuestState;
}

void CUser::V3_MonsterCount(uint16 sNpcID) {
	foreach(itr, m_QuestMonMap) {
		V3_QuestKillCount(itr->first, sNpcID);
	}
}

void CUser::V3_QuestKillCount(uint16 nQuestID, uint16 sNpcID) {
	_QUEST_MONSTER *pQuestMonster = g_pMain->m_QuestMonsterArray.GetData(nQuestID);
	Quest_MonMap::iterator itr = m_QuestMonMap.find(nQuestID);

	if (pQuestMonster == nullptr || itr == m_QuestMonMap.end())
		return;

	// TODO: Implement obscure zone ID logic
	for (int Count = 0; Count < QUEST_MOB_GROUPS; Count++) {
		for (int i = 0; i < QUEST_MOBS_PER_GROUP; i++) {
			if (pQuestMonster->sNum[Count][i] != sNpcID)
				continue;

			if (itr->second->dCKills[Count] + 1 > pQuestMonster->sCount[Count])
				return;

			itr->second->dCKills[Count]++;

			Packet result(WIZ_QUEST, uint8(9));
			result << uint8(2) << uint16(nQuestID) << uint8(Count + 1) << itr->second->dCKills[Count];
			Send(&result);

			if (itr->second->dCKills[Count] >= pQuestMonster->sCount[Count]) {
				V3_QuestEvent(nQuestID, 3);
			}
			return;
		}
	}
}

bool CUser::V3_QuestRunEvent(_QUEST_HELPER * pQuestHelper, uint32 nEventID, int8 bSelectedReward) {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return false;

	// Lookup the corresponding NPC.
	if (pQuestHelper->strLuaFilename == "01_main.lua") {
		m_sEventNid = 10000;
	}

	CNpc * pNpc = g_pMain->GetNpcPtr(m_sEventNid);
	bool result = false;

	// Make sure the NPC exists and is not dead (we should also check if it's in range)
	if (pNpc == nullptr || pNpc->isDead())
		return result;

	// Increase the NPC's reference count to ensure it doesn't get freed while executing a script
	pNpc->IncRef();

	m_nQuestHelperID = pQuestHelper->nIndex;
	result = g_pMain->GetLuaEngine()->ExecuteScript(this, pNpc, nEventID, bSelectedReward,
		pQuestHelper->strLuaFilename.c_str());

	// Decrease it now that we've finished with it + free if necessary
	pNpc->DecRef();

	return result;
}

void CUser::V3_QuestUpdateEvent(uint16 sQuestID) {
	_QUEST_HELPER * pQuestHelper = g_pMain->m_QuestHelperArray.GetData(sQuestID);

	if (pQuestHelper == nullptr)
		return;

	V3_QuestEvent(pQuestHelper->sEventDataIndex, pQuestHelper->bEventStatus);
}

void CUser::V3_QuestSendNpcMsg(uint32 nQuestID, uint16 sNpcID) {
	Packet result(WIZ_QUEST, uint8(7));
	result << nQuestID << sNpcID;
	Send(&result);
}

void CUser::V3_QuestShowGiveItem(uint32 nUnk1, uint32 sUnk1,
	uint32 nUnk2, uint32 sUnk2,
	uint32 nUnk3, uint32 sUnk3,
	uint32 nUnk4, uint32 sUnk4,
	uint32 nUnk5, uint32 sUnk5) {
	Packet result(WIZ_QUEST, uint8(10));
	result << nUnk1 << sUnk1
		<< nUnk2 << sUnk2
		<< nUnk3 << sUnk3
		<< nUnk4 << sUnk4
		<< nUnk5 << sUnk5;
	Send(&result);
}

void CUser::V3_QuestShowMap(uint32 nQuestHelperID) {
	Packet result(WIZ_QUEST, uint8(11));
	result << nQuestHelperID;
	Send(&result);
}

uint16 CUser::V3_QuestCheckMonsterCount(uint16 sQuestID, uint8 Slot) {
	Quest_MonMap::iterator itr = m_QuestMonMap.find(sQuestID);

	if (itr == m_QuestMonMap.end())
		return 0;

	return itr->second->dCKills[Slot - 1];
}

uint16 CUser::V3_QuestSearchEligibleQuest(uint16 sNpcID) {
	Guard lock(g_pMain->m_questNpcLock);
	QuestNpcList::iterator itr = g_pMain->m_QuestNpcList.find(sNpcID);

	if (itr == g_pMain->m_QuestNpcList.end() || itr->second.empty())
		return 0;

	// Loop through all the QuestHelper instances attached to that NPC.
	foreach(itr2, itr->second) {
		_QUEST_HELPER * pHelper = (*itr2);
		if (pHelper->bLevel > GetLevel()
			|| (pHelper->bLevel == GetLevel() && pHelper->nExp > m_iExp)
			|| (pHelper->bClass != 5 && !JobGroupCheck(pHelper->bClass))
			|| (pHelper->bNation != 3 && pHelper->bNation != GetNation())
			|| (pHelper->sEventDataIndex == 0)
			|| (pHelper->bEventStatus < 0 || V3_CheckExistEvent(pHelper->sEventDataIndex, 2))
			|| !V3_CheckExistEvent(pHelper->sEventDataIndex, pHelper->bEventStatus))
			continue;

		return 2;
	}
	return 0;
}

uint16 CUser::V3_CheckMonsterCount(uint16 bQuest, uint8 bGroup) {
	_QUEST_MONSTER * pQuestMonster = g_pMain->m_QuestMonsterArray.GetData(bQuest);
	Quest_MonMap::iterator itr = m_QuestMonMap.find(bQuest);

	if (pQuestMonster == nullptr || itr == m_QuestMonMap.end() || bGroup == 0 || bGroup >= QUEST_MOB_GROUPS)
		return 0;

	return itr->second->dCKills[bGroup - 1];
}

// First job change; you're a [novice], Harry!
bool CUser::PromoteUserNovice() {
	uint8 bNewClasses[] = {ClassWarriorNovice, ClassRogueNovice, ClassMageNovice, ClassPriestNovice, ClassPorutuSkilled};
	uint8 bOldClass = GetClassType() - 1; // convert base class 1,2,3,4 to 0,1,2,3 to align with bNewClasses
	if (GetClassType() == 13)
		bOldClass = 4;

	// Make sure it's a beginner class.
	if (!isBeginner())
		return false;

	Packet result(WIZ_CLASS_CHANGE, uint8(6));

	// Build the new class.
	uint16 sNewClass = (GetNation() * 100) + bNewClasses[bOldClass];
	result << sNewClass << GetID();
	SendToRegion(&result, nullptr, GetEventRoom());

	// Change the class & update party.
	result.clear();
	result << uint8(2) << sNewClass;
	ClassChange(result, false); // TODO: Clean this up. Shouldn't need to build a packet for this.

	// Update the clan.
	result.clear();
	result << uint16(0);
	CKnightsManager::CurrentKnightsMember(this, result); // TODO: Clean this up too.
	return true;
}

// From novice to master.
bool CUser::PromoteUser() {
	/* unlike the official, the checks & item removal should be handled in the script, not here */
	uint8 bOldClass = GetClassType();

	// We must be a novice before we can be promoted to master.
	if (!isNovice())
		return false;

	Packet result(WIZ_CLASS_CHANGE, uint8(6));

	// Build the new class.
	uint16 sNewClass = (GetNation() * 100) + bOldClass + 1;
	result << sNewClass << GetID();
	SendToRegion(&result, nullptr, GetEventRoom());

	// Change the class & update party.
	result.clear();
	result << uint8(2) << sNewClass;
	ClassChange(result, false); // TODO: Clean this up. Shouldn't need to build a packet for this.

	// use integer division to get from 5/7/9/11 (novice classes) to 1/2/3/4 (base classes)
	uint8 bBaseClass = (bOldClass / 2) - 1;

	// this should probably be moved to the script
	V3_QuestEvent(bBaseClass, 2);

	// Update the clan.
	result.clear();
	result << uint16(0);
	CKnightsManager::CurrentKnightsMember(this, result); // TODO: Clean this up too.
	return true;
}

void CUser::PromoteClan(ClanTypeFlag byFlag) {
	if (!isInClan())
		return;

	CKnightsManager::UpdateKnightsGrade(GetClanID(), byFlag);
}

void CUser::SendClanPointChange(int32 nChangeAmount) {
	if (!isInClan())
		return;

	CKnightsManager::UpdateClanPoint(GetClanID(), nChangeAmount);
}

uint8 CUser::GetClanGrade() {
	if (!isInClan())
		return 0;

	CKnights * pClan = g_pMain->GetClanPtr(GetClanID());
	if (pClan == nullptr)
		return 0;

	return pClan->m_byGrade;
}

uint32 CUser::GetClanPoint() {
	if (!isInClan())
		return 0;

	CKnights * pClan = g_pMain->GetClanPtr(GetClanID());
	if (pClan == nullptr)
		return 0;

	return pClan->m_nClanPointFund;
}

uint8 CUser::GetClanRank() {
	if (!isInClan())
		return ClanTypeNone;

	CKnights * pClan = g_pMain->GetClanPtr(GetClanID());
	if (pClan == nullptr)
		return ClanTypeNone;

	return pClan->m_byFlag;
}

uint8 CUser::GetBeefRoastVictory() {
	if (g_pMain->m_sBifrostTime <= 90 * MINUTE && g_pMain->m_BifrostVictory != 0)
		return g_pMain->m_sBifrostVictoryAll;
	else
		return g_pMain->m_BifrostVictory;
}

uint8 CUser::GetWarVictory() { return g_pMain->m_bVictory; }

uint8 CUser::CheckMiddleStatueCapture() { return g_pMain->m_bMiddleStatueNation == GetNation() ? 1 : 0; }

void CUser::MoveMiddleStatue() { Warp((GetNation() == KARUS ? DODO_CAMP_WARP_X : LAON_CAMP_WARP_X) + myrand(0, DODO_LAON_WARP_RADIUS), (GetNation() == KARUS ? DODO_CAMP_WARP_Z : LAON_CAMP_WARP_Z) + myrand(0, DODO_LAON_WARP_RADIUS)); }

uint8 CUser::GetPVPMonumentNation() { return g_pMain->m_nPVPMonumentNation[GetZoneID()]; }

uint8 CUser::GetEventMonumentNation() { return g_pMain->m_nEventMonumentNation[GetZoneID()]; }