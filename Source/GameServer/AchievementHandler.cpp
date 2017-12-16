#include "stdafx.h"

#define COVERTITLE_CHANGE_TIME 15
#define SKILLTITLE_CHANGE_TIME 15

void CUser::AchieveTimeQuest(Packet & pkt) {
	Packet result(WIZ_ACHIEVE, uint8(99));
	uint16 ErrorCode;
	uint8 QuestID, type;
	uint16 RealQuestID;
	pkt >> QuestID, type;

	RealQuestID = QuestID + 256;

	_ACHIEVE_MAIN * pAchieveMain = g_pMain->GetAchieveMainPointer(RealQuestID);

	if (pAchieveMain == nullptr) {
		ErrorCode = -2;
		goto fail_return;
	}

	if (m_AchievequestMap[RealQuestID] == 2) {
		ErrorCode = -2;
		goto fail_return;
	}

	if (m_tLastKillTime != 0) {
		ErrorCode = -1;
		goto fail_return2;
	}

	if (m_sChallangeAchieveID > 0 && m_sChallangeAchieveID != RealQuestID) {
		ErrorCode = -2;
		goto fail_return2;
	}

	// 37237 There's no ongoing achievement 7 -1
	// 37238 This does not match the achievement you are challenging. 7 -2
	// 37239 You already have another ongoing achievement. 6 -1
	// 37240 Selected Achievement Error 6 -2

	m_tLastKillTime = UNIXTIME;
	m_sChallangeAchieveID = RealQuestID;

	result << uint8(1) << uint16(RealQuestID) << uint8(1);
	Send(&result);
fail_return:
	result << uint8(6) << uint16(0) << uint16(ErrorCode);
	Send(&result);
	return;
fail_return2:
	result << uint8(7) << uint16(0) << uint16(ErrorCode);
	Send(&result);
	return;
}

void CUser::HandleAchieve(Packet & pkt) {
	auto OpCode = pkt.read<uint8>();

	switch (OpCode) {
	case 2:// quest listesindeki iteme týklayýnca geliyor. Quest bitmiþse itemi vermek için.
		AchieveGetItem(pkt);
		break;
	case 3:
		LoadAchievementData(pkt);
		break;
	case 4:
		SendAchievementDetails();
		break;
	case 6: // süreli achievement iconlarýna týklayýnc gelen packet.
		AchieveTimeQuest(pkt);
		break;
	case 16: // cover title change
		ChangeCoverTitle(pkt);
		break;
	case 17: // skill title change
		ChangeSkillTitle(pkt);
		break;
	case 18: // Cover Title Reset
		CoverTitleReset();
		break;
	case 19: // Skill Title Reset
		SkillTitleReset();
		break;
	default:
		break;
	}
}

void CUser::AchieveGetItem(Packet & pkt) {
	Packet result(WIZ_ACHIEVE, uint8(2));
	/*
	1 = Item has been issued.
	-1 = You cannot pick up the item because your item inventory is full.
	-2 = Item doesnt exist.
	*/
	int16 ReturnValue = 1;
	auto ID = pkt.read<uint16>();
	auto pAchieveMain = g_pMain->GetAchieveMainPointer(ID);

	if (!pAchieveMain)
		goto failed_return;

	// TODO: achieve item verme iþlemleri yapýlacak.

	result << ID << ReturnValue;
	Send(&result);
	//SendAchieveStatus(ID, 5);
	return;
failed_return:
	result << ID << ReturnValue;
	Send(&result);
}

void CUser::LoadAchievementData(Packet & pkt) {
	auto pCount = pkt.read<uint16>();
	uint16 AchievementID = 0;

	Packet result(WIZ_ACHIEVE, uint8(3));
	result << pCount;

	for (size_t i = 0; i < pCount; i++) {
		pkt >> AchievementID;

		auto pAchieveMain = g_pMain->GetAchieveMainPointer(AchievementID);
		_ACHIEVE_QUEST * pAchieveQuest = m_bAchieveKillCount.GetData(AchievementID);
		if (pAchieveMain) {
			switch (pAchieveMain->Type) {
			case 1:
			{
				result << AchievementID << uint8(m_AchievequestMap[AchievementID] == 2 ? 4 : 0) << uint16(pAchieveQuest == nullptr ? 0 : pAchieveQuest->KillCount[0]) << uint16(pAchieveQuest == nullptr ? 0 : pAchieveQuest->KillCount[1]);
				break;
			}
			case 2:
			{
				result << AchievementID << uint8(m_AchievequestMap[AchievementID] == 2 ? 4 : 0) << uint16(pAchieveQuest == nullptr ? 0 : pAchieveQuest->KillCount[0]) << uint16(pAchieveQuest == nullptr ? 0 : pAchieveQuest->KillCount[1]);
				break;
			}
			case 3:
			{
				result << AchievementID << uint8(m_AchievequestMap[AchievementID] == 2 ? 4 : 0) << uint16(pAchieveQuest == nullptr ? 0 : pAchieveQuest->KillCount[0]) << uint16(pAchieveQuest == nullptr ? 0 : pAchieveQuest->KillCount[1]);
				break;
			}
			case 4:
			{
				result << AchievementID << uint8(m_AchievequestMap[AchievementID] == 2 ? 4 : 0) << uint16(pAchieveQuest == nullptr ? 0 : pAchieveQuest->KillCount[0]) << uint16(pAchieveQuest == nullptr ? 0 : pAchieveQuest->KillCount[1]);
				break;
			}
			case 5:
			{
				result << AchievementID << uint8(m_AchievequestMap[AchievementID] == 2 ? 4 : 0) << uint16(pAchieveQuest == nullptr ? 0 : pAchieveQuest->KillCount[0]) << uint16(pAchieveQuest == nullptr ? 0 : pAchieveQuest->KillCount[1]);
				break;
			}
			default:
				result << AchievementID << uint8(0) << uint16(0) << uint16(0);
				break;
			}
		} else
			result << AchievementID << uint8(0) << uint16(0) << uint16(0);
	}

	Send(&result);
}

void CUser::SendAchievementDetails() {
	Packet result(WIZ_ACHIEVE, uint8(4));
	result << uint32(((UNIXTIME - m_TimeOnline) / 60) + m_dwTime) // Oyunda kalma süresi Dakika cinsinden
		<< m_iMonsterDefeatedCount
		<< m_iUserDefeatedCount
		<< m_iUserDeathCount
		<< m_iAchievementPoint
		<< AchieveLast1
		<< AchieveLast2
		<< AchieveLast3
		<< AchieveNormalCount // Normal Count
		<< AchieveQuestCount // Quest Count
		<< AchieveWarCount // War Count
		<< AchieveAdventureCount // Adventure Count
		<< AchieveChallengeCount;// Challenge Count
	Send(&result);
}

void CUser::ChangeCoverTitle(Packet & pkt) {
	Packet result(WIZ_ACHIEVE, uint8(16));

	if (ChangeCoverTitleRequest + COVERTITLE_CHANGE_TIME >= UNIXTIME)
		goto failed_return;

	ChangeCoverTitleRequest = UNIXTIME;

	uint16 UnKnow1 = 0, Title = 0;
	pkt >> UnKnow1 >> Title;

	auto pTitleData = g_pMain->GetAchieveTitlePointer(Title);
	if (!pTitleData
		|| m_AchievequestMap[pTitleData->UnKnow1] != 2)
		goto failed_return;

	result << UnKnow1 << Title << uint16(1);

	SetCoverTitle(Title);

	Send(&result);
	StateChangeServerDirect(14, Title);
	return;
failed_return:
	result << UnKnow1 << GetCoverTitle() << uint16(-5) << uint16(COVERTITLE_CHANGE_TIME - (UNIXTIME - ChangeCoverTitleRequest));
	Send(&result);
}

void CUser::ChangeSkillTitle(Packet & pkt) {
	Packet result(WIZ_ACHIEVE, uint8(17));

	if (ChangeSkillTitleRequest + SKILLTITLE_CHANGE_TIME >= UNIXTIME)
		goto failed_return;

	ChangeSkillTitleRequest = UNIXTIME;

	uint16 UnKnow1 = 0, Title = 0;
	pkt >> UnKnow1 >> Title;

	auto pTitleData = g_pMain->GetAchieveTitlePointer(Title);
	if (!pTitleData
		|| m_AchievequestMap[pTitleData->UnKnow1] != 2)
		goto failed_return;

	result << UnKnow1 << Title << uint16(1);

	SetSkillTitle(Title);

	Send(&result);
	SetUserAbility(true);
	return;
failed_return:
	result << UnKnow1 << GetSkillTitle() << uint16(-5) << uint16(SKILLTITLE_CHANGE_TIME - (UNIXTIME - ChangeSkillTitleRequest));
	Send(&result);
}

void CUser::CoverTitleReset() {
	Packet result(WIZ_ACHIEVE, uint8(18));

	if (ResetCoverTitleRequest + COVERTITLE_CHANGE_TIME >= UNIXTIME)
		goto failed_return;

	ResetCoverTitleRequest = UNIXTIME;

	result << int16(1);

	StateChangeServerDirect(14, 0);

	Send(&result);
	return;
failed_return:
	result << int16(-1) << uint16(COVERTITLE_CHANGE_TIME - (UNIXTIME - ResetCoverTitleRequest));
	Send(&result);
}

void CUser::SkillTitleReset() {
	Packet result(WIZ_ACHIEVE, uint8(19));

	if (ResetSkillTitleRequest + SKILLTITLE_CHANGE_TIME >= UNIXTIME)
		goto failed_return;

	ResetSkillTitleRequest = UNIXTIME;

	result << uint16(1);

	SetSkillTitle(0);

	Send(&result);
	SetUserAbility(true);
	return;
failed_return:
	result << int16(-1) << uint16(SKILLTITLE_CHANGE_TIME - (UNIXTIME - ResetSkillTitleRequest));
	Send(&result);
}

/// ACHIEVE_WAR
void CUser::AchieveType1(uint8 pType) {
	foreach_stlmap_nolock(itr, g_pMain->ACHIEVE_WAR) {
		auto pAchieveWarData = itr->second;

		if (pAchieveWarData->Type != pType)
			continue;

		if (m_AchievequestMap[pAchieveWarData->ID] == 2)
			continue;

		auto pAchieveMain = g_pMain->GetAchieveMainPointer(pAchieveWarData->ID);
		_ACHIEVE_QUEST * pAchieveQuest = m_bAchieveKillCount.GetData(pAchieveWarData->ID);

		if (pAchieveQuest == nullptr) {
			pAchieveQuest = new _ACHIEVE_QUEST;
			pAchieveQuest->ID = pAchieveWarData->ID;
			pAchieveQuest->KillCount[0] = 0;
			pAchieveQuest->KillCount[1] = 0;
			m_bAchieveKillCount.PutData(pAchieveQuest->ID, pAchieveQuest);
		}

		if ((pAchieveMain->ZoneID == ZONE_KARUS && (GetZoneID() != GetNation()))
			|| (pAchieveMain->ZoneID == ZONE_KARUS_ESLANT && (GetZoneID() != GetNation() + 10))
			|| (pAchieveMain->ZoneID != 0 && pAchieveMain->ZoneID != GetZoneID()))
			continue;

		if (pType == 20
			&& (pAchieveWarData->ID == 94
				|| pAchieveWarData->ID == 95
				|| pAchieveWarData->ID == 96
				|| pAchieveWarData->ID == 97
				|| pAchieveWarData->ID == 98)
			&& m_bResHpType == USER_DEAD) {
			pAchieveQuest->KillCount[0] = 0;
			continue;
		}

		if (pAchieveMain->Time > 0 && (m_AchievequestMap[pAchieveWarData->ID] != 1 || m_sChallangeAchieveID != pAchieveWarData->ID))
			continue;

		if (pAchieveQuest->KillCount[0] == 0
			&& pAchieveMain->Time == 0)
			m_AchievequestMap[pAchieveWarData->ID] = 1;
		else if (pAchieveQuest->KillCount[0] > 0
			&& ((m_tLastKillTime - UNIXTIME) > pAchieveMain->Time) && pAchieveMain->Time > 0) {
			pAchieveQuest->KillCount[0] = 0;
			m_AchievequestMap[pAchieveWarData->ID] = 0;
			continue;
		}

		pAchieveQuest->KillCount[0]++;

		if (pAchieveWarData->Data == pAchieveQuest->KillCount[0]) // Görevi bitirme iþlemleri
		{
			SendAchieveStatus(pAchieveWarData->ID, 2);
			m_AchievequestMap[pAchieveWarData->ID] = 2;
			m_iAchievementPoint += pAchieveMain->Point;

			switch (pAchieveMain->TabIndex) {
			case 0:
				AchieveNormalCount++; // Normal Count
				break;
			case 1:
				AchieveQuestCount++; // Quest Count
				break;
			case 2:
				AchieveWarCount++; // War Count
				break;
			case 3:
				AchieveAdventureCount++; // Adventure Count
				break;
			case 4:
				AchieveChallengeCount++;// Challenge Count
				break;
			default:
				break;
			}

			if (AchieveLast3 == 0
				&& AchieveLast2 == 0
				&& AchieveLast1 == 0)
				AchieveLast1 = pAchieveMain->ID;
			else {
				AchieveLast3 = AchieveLast2;
				AchieveLast2 = AchieveLast1;
				AchieveLast1 = pAchieveMain->ID;
			}
		}
	}
}

/// ACHIEVE_MONSTER
void CUser::AchieveType2(CNpc * pNpc) {
	if (!pNpc
		|| pNpc == nullptr)
		return;

	AchieveType1(20);

	m_iMonsterDefeatedCount++;

	foreach_stlmap_nolock(itr, g_pMain->ACHIEVE_MAIN) {
		auto pMainData = itr->second;
		if (pMainData->Type != 2)
			continue;

		if (pMainData->ZoneID != 0
			&& pMainData->ZoneID != GetZoneID())
			continue;

		if (m_AchievequestMap[pMainData->ID] == 2)
			continue;

		_ACHIEVE_MONSTER * pMonsterData = g_pMain->GetAchieveMonsterPointer(pMainData->ID);

		if (pMonsterData == nullptr)
			continue;

		_ACHIEVE_QUEST * pAchieveQuest = m_bAchieveKillCount.GetData(pMainData->ID);

		if (pAchieveQuest == nullptr) {
			pAchieveQuest = new _ACHIEVE_QUEST;
			pAchieveQuest->ID = pMainData->ID;
			pAchieveQuest->KillCount[0] = 0;
			pAchieveQuest->KillCount[1] = 0;
			m_bAchieveKillCount.PutData(pAchieveQuest->ID, pAchieveQuest);
		}

		for (size_t i = 0; i < 2; i++) {
			if (pAchieveQuest->KillCount[i] == pMonsterData->MonsterCount[i])
				continue;

			for (size_t ii = 0; ii < 4; ii++) {
				if (pMonsterData->MonsterID[i][ii] != pNpc->GetProtoID())
					continue;
				else if (pAchieveQuest->KillCount[i] == 0) {
					m_AchievequestMap[pMonsterData->ID] = 1;
					pAchieveQuest->KillCount[i]++;
				} else
					pAchieveQuest->KillCount[i]++;
			}
		}

		if (pAchieveQuest->KillCount[0] == pMonsterData->MonsterCount[0]
			&& pAchieveQuest->KillCount[1] == pMonsterData->MonsterCount[1]
			&& m_AchievequestMap[pMonsterData->ID] == 1) {
			m_AchievequestMap[pMonsterData->ID] = 2;// görevi bitir
			SendAchieveStatus(pMonsterData->ID, 2);
			m_iAchievementPoint += pMainData->Point;

			switch (pMainData->TabIndex) {
			case 0:
				AchieveNormalCount++; // Normal Count
				break;
			case 1:
				AchieveQuestCount++; // Quest Count
				break;
			case 2:
				AchieveWarCount++; // War Count
				break;
			case 3:
				AchieveAdventureCount++; // Adventure Count
				break;
			case 4:
				AchieveChallengeCount++;// Challenge Count
				break;
			default:
				break;
			}

			if (AchieveLast3 == 0
				&& AchieveLast2 == 0
				&& AchieveLast1 == 0)
				AchieveLast1 = pMainData->ID;
			else {
				AchieveLast3 = AchieveLast2;
				AchieveLast2 = AchieveLast1;
				AchieveLast1 = pMainData->ID;
			}
		}
	}
}

/// ACHIEVE_COM
void CUser::AchieveType3() {
}

/// ACHIEVE_NORMAL
void CUser::AchieveType4(uint8 pType) {
	/*
	type1 is king
	type2 Achieve Contribution x NP
	type3 Reach Level x
	type5 Achieve Knight Contribution Point of x CLAN NPSÝ
	type10 Become a member of Knight Trainees

	*/
	foreach_stlmap_nolock(itr, g_pMain->ACHIEVE_NORMAL) {
		auto pAchieveNormalData = itr->second;

		if (pAchieveNormalData->UnKnow1 != pType)
			continue;

		if (m_AchievequestMap[pAchieveNormalData->ID] == 2)
			continue;

		auto pAchieveMain = g_pMain->GetAchieveMainPointer(pAchieveNormalData->ID);
		_ACHIEVE_QUEST * pAchieveQuest = m_bAchieveKillCount.GetData(pAchieveNormalData->ID);

		if (pAchieveQuest == nullptr) {
			pAchieveQuest = new _ACHIEVE_QUEST;
			pAchieveQuest->ID = pAchieveNormalData->ID;
			pAchieveQuest->KillCount[0] = 0;
			pAchieveQuest->KillCount[1] = 0;
			m_bAchieveKillCount.PutData(pAchieveQuest->ID, pAchieveQuest);
		}

		if ((pAchieveMain->ZoneID == ZONE_KARUS && (GetZoneID() != GetNation()))
			|| (pAchieveMain->ZoneID == ZONE_KARUS_ESLANT && (GetZoneID() != GetNation() + 10))
			|| (pAchieveMain->ZoneID != 0 && pAchieveMain->ZoneID != GetZoneID()))
			continue;

		if (pAchieveQuest->KillCount[0] == 0)
			m_AchievequestMap[pAchieveNormalData->ID] = 1;

		CKnights * pKnights = g_pMain->GetClanPtr(GetClanID());
		_KNIGHTS_SIEGE_WARFARE * pSiegeKnights = g_pMain->GetSiegeMasterKnightsPtr(1);
		switch (pType) {
		case 1:
			if (isKing())
				pAchieveQuest->KillCount[0] = pAchieveNormalData->UnKnow2;
			break;
		case 2:
			if (GetLoyalty() >= pAchieveNormalData->UnKnow2)
				pAchieveQuest->KillCount[0] = pAchieveNormalData->UnKnow2;
			break;
		case 3:
			if (GetLevel() >= pAchieveNormalData->UnKnow2)
				pAchieveQuest->KillCount[0] = pAchieveNormalData->UnKnow2;
			break;
		case 5:
			if (m_pKnightsUser == nullptr)
				continue;

			if (m_pKnightsUser->nDonatedNP >= pAchieveNormalData->UnKnow2)
				pAchieveQuest->KillCount[0] = pAchieveNormalData->UnKnow2;
			break;
		case 10:
			// Become a member of Knight Trainees 419
			// Become a member of Knights 420
			// Become a member of Castle Knights 421
			if (pKnights == nullptr)
				continue;

			if (pSiegeKnights == nullptr
				&& pAchieveNormalData->ID == 421)
				continue;

			if (pSiegeKnights->sMasterKnights != GetClanID()
				&& pAchieveNormalData->ID == 421)
				continue;

			if (pAchieveNormalData->ID == 419
				&& pKnights->m_byFlag < ClanTypePromoted)
				continue;

			pAchieveQuest->KillCount[0] = pAchieveNormalData->UnKnow2;
			break;
		default:
			break;
		}

		if (pAchieveNormalData->UnKnow2 == pAchieveQuest->KillCount[0]) // Görevi bitirme iþlemleri
		{
			SendAchieveStatus(pAchieveNormalData->ID, 2);
			m_AchievequestMap[pAchieveNormalData->ID] = 2;
			m_iAchievementPoint += pAchieveMain->Point;

			switch (pAchieveMain->TabIndex) {
			case 0:
				AchieveNormalCount++; // Normal Count
				break;
			case 1:
				AchieveQuestCount++; // Quest Count
				break;
			case 2:
				AchieveWarCount++; // War Count
				break;
			case 3:
				AchieveAdventureCount++; // Adventure Count
				break;
			case 4:
				AchieveChallengeCount++;// Challenge Count
				break;
			default:
				break;
			}

			if (AchieveLast3 == 0
				&& AchieveLast2 == 0
				&& AchieveLast1 == 0)
				AchieveLast1 = pAchieveMain->ID;
			else {
				AchieveLast3 = AchieveLast2;
				AchieveLast2 = AchieveLast1;
				AchieveLast1 = pAchieveMain->ID;
			}
		}
	}
}

/// Ekranýn orta altýnda achieve i bitirdiðine dair uif çýkartmak için
void CUser::SendAchieveStatus(uint16 AchieveID, uint8 AchieveStatus) {
	Packet result(WIZ_ACHIEVE, uint8(1));
	result << AchieveID << AchieveStatus;// achieve ok
	Send(&result);
}