#include "stdafx.h"
#include "DBAgent.h"

using std::string;
using std::vector;

void CGameServerDlg::SendEventRemainingTime(bool bSendAll, CUser *pUser, uint8 ZoneID) {
	Packet result(WIZ_BIFROST, uint8(BIFROST_EVENT));
	uint16 nRemainingTime = 0;

	if (ZoneID == ZONE_BATTLE4)
		nRemainingTime = m_byBattleRemainingTime / 2;
	else if (ZoneID == ZONE_BIFROST || ZoneID == ZONE_RONARK_LAND)
		nRemainingTime = m_sBifrostRemainingTime;

	result << nRemainingTime;

	if (pUser)
		pUser->Send(&result);
	else if (bSendAll) {
		if (ZoneID == ZONE_BATTLE4)
			Send_All(&result, nullptr, 0, ZONE_BATTLE4);
		else {
			Send_All(&result, nullptr, 0, ZONE_RONARK_LAND);
			Send_All(&result, nullptr, 0, ZONE_BIFROST);
		}
	}
}
void CUser::CastleSiegeWarProcess(CUser * pUser) {
	if (pUser == nullptr || GetClanID() <= 0)
		return;

	_KNIGHTS_SIEGE_WARFARE *pKnightSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(1);
	CKnights * pKnights = g_pMain->GetClanPtr(pUser->m_bKnights);

	pKnightSiegeWar->sMasterKnights = pKnights->m_sIndex;

	g_pMain->UpdateSiege(pKnightSiegeWar->sCastleIndex, pKnightSiegeWar->sMasterKnights, pKnightSiegeWar->bySiegeType, pKnightSiegeWar->byWarDay, pKnightSiegeWar->byWarTime, pKnightSiegeWar->byWarMinute);
	g_pMain->m_KnightsSiegeWarfareArray.GetData(pKnightSiegeWar->sMasterKnights);

	g_pMain->m_SiegeWarWinKnightsNotice = pKnights->GetName();
	g_pMain->Announcement(IDS_NPC_GUIDON_DESTORY);
	g_pMain->m_byBattleSiegeWarMomument = true;
	g_pMain->m_byBattleSiegeWarOccupy = g_pMain->m_sBattleTimeDelay > 2100 ? (3000 - g_pMain->m_sBattleTimeDelay) : 900;
	//g_pMain->KickOutZoneUsers(ZONE_DELOS, ZONE_DELOS);
	if (pKnightSiegeWar != nullptr) {
		Packet result(WIZ_SIEGE, uint8(2));
		result.SByte();
		result << uint8(g_pMain->m_byBattleSiegeWarOpen + 1) << pKnights->GetID() << pKnights->m_sMarkVersion << uint16(0)
			<< uint32(g_pMain->m_byBattleSiegeWarOpen ? g_pMain->m_byBattleSiegeWarOccupy : 0) << uint16(g_pMain->m_byBattleSiegeWarOpen ? ((50 * MINUTE) - g_pMain->m_sBattleTimeDelay) : 0)
			<< pKnights->GetName();
		g_pMain->Send_Zone(&result, ZONE_DELOS);
	}
}
void CUser::BifrostProcess(CUser * pUser) {
	if (pUser == nullptr)
		return;

	if (g_pMain->m_BifrostVictory == 0 && g_pMain->m_bAttackBifrostMonument) {
		g_pMain->m_sBifrostTime = g_pMain->m_xBifrostTime;
		g_pMain->m_sBifrostRemainingTime = g_pMain->m_sBifrostTime;
		g_pMain->m_BifrostVictory = pUser->GetNation();
		g_pMain->SendFormattedResource(pUser->GetNation() == ELMORAD ? IDS_BEEF_ROAST_VICTORY_ELMORAD : IDS_BEEF_ROAST_VICTORY_KARUS, Nation::ALL, false);
		g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);

		if (g_pMain->m_bAttackBifrostMonument)
			g_pMain->m_bAttackBifrostMonument = false;
	} else if (g_pMain->m_BifrostVictory == 1 || g_pMain->m_BifrostVictory == 2) {
		if (pUser->GetNation() != g_pMain->m_BifrostVictory && g_pMain->m_bAttackBifrostMonument) {
			g_pMain->m_BifrostVictory = 3;
			g_pMain->SendFormattedResource(pUser->GetNation() == ELMORAD ? IDS_BEEF_ROAST_VICTORY_ELMORAD : IDS_BEEF_ROAST_VICTORY_KARUS, Nation::ALL, false);

			if (g_pMain->m_bAttackBifrostMonument)
				g_pMain->m_bAttackBifrostMonument = false;
		}
	}
}

void CUser::TempleProcess(Packet &pkt) {
	uint8 opcode = pkt.read<uint8>();

	switch (opcode) {
	case MONSTER_STONE:
		MonsterStoneProcess();
		break;
	case TEMPLE_EVENT_JOIN:
		TempleOperations(opcode);
		break;
	case TEMPLE_EVENT_DISBAND:
		TempleOperations(opcode);
		break;
	}
}

void CUser::MonsterStoneProcess() {
	if (GetZoneID() == ZONE_STONE1 ||
		GetZoneID() == ZONE_STONE3 ||
		GetZoneID() == ZONE_STONE3)
		return;

	if (CheckExistItem(ITEM_MONSTER_STONE, 1)) {
		uint8 EventRoom = 0;
		uint8 ZoneNumber = 0;
		uint32 ZoneTime = 0;

		if (GetLevel() < 40)
			ZoneNumber = ZONE_STONE1;
		else if (GetLevel() < 60)
			ZoneNumber = ZONE_STONE2;
		else if (GetLevel() < 84)
			ZoneNumber = ZONE_STONE3;
		else
			return;

		bool IsFull = true;

		for (int i = 1; i < MAX_MONSTER_STONE_EVENT_ROOM; i++) {
			if (ZoneNumber == ZONE_STONE1) {
				IsFull = g_pMain->IsBusy1[i];
				ZoneTime = g_pMain->Zone1[i];
			} else if (ZoneNumber == ZONE_STONE2) {
				IsFull = g_pMain->IsBusy2[i];
				ZoneTime = g_pMain->Zone2[i];
			} else if (ZoneNumber == ZONE_STONE3) {
				IsFull = g_pMain->IsBusy3[i];
				ZoneTime = g_pMain->Zone3[i];
			} else
				continue;

			if (IsFull || ZoneTime > uint32(UNIXTIME))
				continue;

			if (ZoneNumber == ZONE_STONE1) {
				g_pMain->Zone1[i] = uint32(UNIXTIME) + 1503;
				EventRoom = i;
				g_pMain->Zone1Family[i] = myrand(1, 4);
				break;
			} else if (ZoneNumber == ZONE_STONE2) {
				g_pMain->Zone2[i] = uint32(UNIXTIME) + 1503;
				EventRoom = i;
				g_pMain->Zone2Family[i] = myrand(5, 9);
				break;
			} else if (ZoneNumber == ZONE_STONE3) {
				g_pMain->Zone3[i] = uint32(UNIXTIME) + 1503;
				EventRoom = i;
				g_pMain->Zone3Family[i] = myrand(10, 13);
				break;
			} else
				continue;
		}

		if (EventRoom == 0)
			return;

		if (ZoneNumber == ZONE_STONE1)
			g_pMain->IsBusy1[EventRoom] = true;
		else if (ZoneNumber == ZONE_STONE2)
			g_pMain->IsBusy2[EventRoom] = true;
		else if (ZoneNumber == ZONE_STONE3)
			g_pMain->IsBusy3[EventRoom] = true;
		else
			return;

		m_TimeMonsterStone = UNIXTIME + 1502;
		RobItem(ITEM_MONSTER_STONE, 1);
		g_pMain->MonsterStoneSummon(EventRoom, ZoneNumber);
		ZoneChange(ZoneNumber, 0.0f, 0.0f, EventRoom);
		if (ZoneNumber == ZONE_STONE1)
			CheckWaiting(ZONE_STONE1, 1501);
		else if (ZoneNumber == ZONE_STONE2)
			CheckWaiting(ZONE_STONE2, 1501);
		else if (ZoneNumber == ZONE_STONE3)
			CheckWaiting(ZONE_STONE3, 1501);

		printf("%s gonna go %d %d\n", GetName().c_str(), EventRoom, ZoneNumber);
	}
}

void CUser::TempleOperations(uint8 bType) {
	uint16 nActiveEvent = (uint16) g_pMain->pTempleEvent.ActiveEvent;

	uint8 bResult = 1;
	Packet result(WIZ_EVENT);
	Packet result2(WIZ_SELECT_MSG);

	if (bType == TEMPLE_EVENT_JOIN && !isEventUser()) {
		if (nActiveEvent == TEMPLE_EVENT_CHAOS) {
			if (CheckExistItem(CHAOS_MAP, 1))
				bResult = 1;
			else if (m_sItemArray[RIGHTHAND].nNum == MATTOCK || m_sItemArray[RIGHTHAND].nNum == GOLDEN_MATTOCK || isMining())
				bResult = 4;
			else
				bResult = 3;
		}

		else if (nActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR) {
			if (GetLevel() < g_pMain->pTempleEvent.m_nBorderDefenseWarMINLEVEL)
				bResult = WarpListMinLevel;
		}

		result << bType << bResult << nActiveEvent;
		Send(&result);

		if (bResult == 1) {
			GetNation() == KARUS ? g_pMain->pTempleEvent.KarusUserCount++ : g_pMain->pTempleEvent.ElMoradUserCount++;
			g_pMain->pTempleEvent.AllUserCount = (g_pMain->pTempleEvent.KarusUserCount + g_pMain->pTempleEvent.ElMoradUserCount);
			g_pMain->AddEventUser(this);
			TempleOperations(TEMPLE_EVENT_COUNTER);
		}
	} else if (bType == TEMPLE_EVENT_DISBAND && isEventUser()) {
		GetNation() == KARUS ? g_pMain->pTempleEvent.KarusUserCount-- : g_pMain->pTempleEvent.ElMoradUserCount--;
		g_pMain->pTempleEvent.AllUserCount = g_pMain->pTempleEvent.KarusUserCount + g_pMain->pTempleEvent.ElMoradUserCount;
		g_pMain->RemoveEventUser(this);
		result << bType << bResult << nActiveEvent;
		Send(&result);
		TempleOperations(TEMPLE_EVENT_COUNTER);
	} else if (bType == TEMPLE_EVENT_COUNTER) {
		result << bType << nActiveEvent;

		if (nActiveEvent == TEMPLE_EVENT_CHAOS)
			result << g_pMain->pTempleEvent.AllUserCount;
		else if (nActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR)
			result << g_pMain->pTempleEvent.KarusUserCount << g_pMain->pTempleEvent.ElMoradUserCount;
		else if (nActiveEvent == TEMPLE_EVENT_JURAD_MOUNTAIN)
			result2 << uint16(0x00) << uint8(0x07) << uint64(0x00) << uint32(0x06) << g_pMain->pTempleEvent.KarusUserCount << uint16(0x00) << g_pMain->pTempleEvent.ElMoradUserCount << uint16(0x00) << g_pMain->m_nTempleEventRemainSeconds << uint16(0x00);

		if (nActiveEvent == TEMPLE_EVENT_JURAD_MOUNTAIN)
			g_pMain->Send_All(&result2, nullptr, Nation::ALL, 0, true, 0);
		else
			g_pMain->Send_All(&result, nullptr, Nation::ALL, 0, true, 0);
	}
}

void CGameServerDlg::AddEventUser(CUser *pUser) {
	if (pUser == nullptr) {
		TRACE("#### AddEventUser : pUser == nullptr ####\n");
		return;
	}

	_TEMPLE_EVENT_USER * pEventUser = new _TEMPLE_EVENT_USER;

	pEventUser->m_socketID = pUser->GetSocketID();
	pEventUser->m_bEventRoom = pUser->GetEventRoom();

	if (!g_pMain->m_TempleEventUserArray.PutData(pEventUser->m_socketID, pEventUser))
		delete pEventUser;
}

void CGameServerDlg::RemoveEventUser(CUser *pUser) {
	if (pUser == nullptr) {
		TRACE("#### RemoveEventUser : pUser == nullptr ####\n");
		return;
	}

	if (g_pMain->m_TempleEventUserArray.GetData(pUser->GetSocketID()) != nullptr)
		g_pMain->m_TempleEventUserArray.DeleteData(pUser->GetSocketID());
}

void CGameServerDlg::UpdateEventUser(CUser *pUser, uint16 nEventRoom) {
	if (pUser == nullptr) {
		TRACE("#### UpdateEventUser : pUser == nullptr ####\n");
		return;
	}

	_TEMPLE_EVENT_USER * pEventUser = g_pMain->m_TempleEventUserArray.GetData(pUser->GetSocketID());

	if (pEventUser) {
		Packet result2(AG_USER_EVENTROOM);
		result2 << pUser->GetID() << uint16(nEventRoom);
		g_pMain->Send_AIServer(&result2);

		pEventUser->m_bEventRoom = nEventRoom;
		pUser->m_bEventRoom = nEventRoom;
		pUser->SetUserEventRoom(nEventRoom);
		pUser->SetUnitEventRoom(nEventRoom);
	}
}

void CGameServerDlg::SetEventUser(CUser *pUser) {
	if (pUser == nullptr) {
		TRACE("#### SetEventUser : pUser == nullptr ####\n");
		return;
	}

	uint8 nMaxUserCount = 0;

	switch (g_pMain->pTempleEvent.ActiveEvent) {
	case TEMPLE_EVENT_BORDER_DEFENCE_WAR:
		nMaxUserCount = 8;
		break;
	case TEMPLE_EVENT_CHAOS:
		nMaxUserCount = 18;
		break;
	case TEMPLE_EVENT_JURAD_MOUNTAIN:
		nMaxUserCount = 8;
		break;
	}

	if (g_pMain->TempleEventGetRoomUsers(g_pMain->pTempleEvent.LastEventRoom, pUser->GetNation()) >= nMaxUserCount)
		g_pMain->pTempleEvent.LastEventRoom++;

	if (g_pMain->TempleEventGetRoomUsers(g_pMain->pTempleEvent.LastEventRoom, pUser->GetNation()) <= nMaxUserCount)
		g_pMain->UpdateEventUser(pUser, g_pMain->pTempleEvent.LastEventRoom);
}

bool CUser::isEventUser() {
	_TEMPLE_EVENT_USER * pEventUser = g_pMain->m_TempleEventUserArray.GetData(GetSocketID());

	if (pEventUser != nullptr)
		return true;

	return false;
}

uint8 CUser::GetMonsterChallengeTime() {
	if (g_pMain->m_bForgettenTempleIsActive
		&& g_pMain->m_nForgettenTempleLevelMin != 0
		&& g_pMain->m_nForgettenTempleLevelMax != 0
		&& GetLevel() >= g_pMain->m_nForgettenTempleLevelMin
		&& GetLevel() <= g_pMain->m_nForgettenTempleLevelMax
		&& !g_pMain->m_bForgettenTempleSummonMonsters)
		return g_pMain->m_nForgettenTempleChallengeTime;

	return 0;
}

uint8 CUser::GetEventZoneTime() {
	if (g_pMain->m_bEventZoneIsActive
		&& g_pMain->m_nEventZoneLevelMin != 0
		&& g_pMain->m_nEventZoneLevelMax != 0
		&& GetLevel() >= g_pMain->m_nEventZoneLevelMin
		&& GetLevel() <= g_pMain->m_nEventZoneLevelMax)
		return g_pMain->m_nEventZoneTime;

	return 0;
}
void CUser::HandleCapture(Packet & pkt) {
	if ((UNIXTIME - m_tBorderCapure) < 10)
		return;

	if (GetZoneID() != ZONE_BORDER_DEFENSE_WAR || g_pMain->pTempleEvent.m_sMiniTimerNation[GetEventRoom()] == GetNation())
		return;

	uint16 KO = 360;

	g_pMain->pTempleEvent.m_sBdwMiniTimer[GetEventRoom()] = KO;
	g_pMain->pTempleEvent.m_sMiniTimerNation[GetEventRoom()] = GetNation();

	GetNation() == KARUS ? g_pMain->pTempleEvent.ElmoDeathCount[GetEventRoom()] = g_pMain->pTempleEvent.ElmoDeathCount[GetEventRoom()] + 2 : g_pMain->pTempleEvent.KarusDeathCount[GetEventRoom()] = g_pMain->pTempleEvent.KarusDeathCount[GetEventRoom()] + 2;

	Packet result1(WIZ_CAPTURE, uint8(0x05));
	result1 << GetNation() << GetName().c_str();
	Packet result(WIZ_CAPTURE, uint8(0x04));
	result << GetNation() << uint16(KO);

	g_pMain->Send_Zone(&result1, GetZoneID(), nullptr, Nation::ALL, GetEventRoom());
	g_pMain->Send_Zone(&result, GetZoneID(), nullptr, Nation::ALL, GetEventRoom());

	result.Initialize(WIZ_EVENT);
	result << uint8(TEMPLE_EVENT_BORDER_COUNTER);
	m_iLoyaltyDaily += 2;
	UpdatePlayerRank();
	result << g_pMain->pTempleEvent.ElmoDeathCount[GetEventRoom()] << uint16(0x00) << g_pMain->pTempleEvent.KarusDeathCount[GetEventRoom()] << uint16(0x00);
	g_pMain->Send_Zone(&result, GetZoneID(), nullptr, Nation::ALL, GetEventRoom());
}
uint8 CUser::GetMonsterChallengeUserCount() { return (uint8) (uint8) g_pMain->m_nForgettenTempleUsers.size(); }
uint8 CUser::GetEventZoneUserCount() { return (uint8) g_pMain->m_nEventZoneUsers.size(); }