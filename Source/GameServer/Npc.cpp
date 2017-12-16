#include "stdafx.h"
#include "Map.h"
#include "MagicInstance.h"
#include "../shared/DateTime.h"

using namespace std;

CNpc::CNpc() : Unit(UnitNPC) {
	Initialize();
}


CNpc::~CNpc() {}

/**
* @brief	Initializes this object.
*/
void CNpc::Initialize() {
	cubeisactive = 0;
	Unit::Initialize();
	m_bIsEventNpc = false;
	m_sSid = 0;
	m_sPid = 0;				// MONSTER(NPC) Picture ID
	m_sSize = 100;				// MONSTER(NPC) Size
	m_strName.clear();			// MONSTER(NPC) Name
	m_iMaxHP = 0;				// �ִ� HP
	m_iHP = 0;					// ���� HP
	m_byState = 0;			// ������ (NPC) �����̻�
	m_tNpcType = 0;				// NPC Type
	// 0 : Normal Monster
	// 1 : NPC
	// 2 : �� �Ա�,�ⱸ NPC
	// 3 : ������
	m_iSellingGroup = 0;
	//m_dwStepDelay = 0;

	m_byDirection = 0;			// npc�� ����,,
	m_iWeapon_1 = 0;
	m_iWeapon_2 = 0;
	m_NpcState = NPC_LIVE;
	m_byGateOpen = true;
	m_byObjectType = NORMAL_OBJECT;
	m_JuraidGateOpen = 1;
	m_byTrapNumber = 0;
	m_oSocketID = -1;
	m_bEventRoom = 0;

	nIsPet = false;
	strPetName = "";
	strUserName = "";
	nSerial = 0;
	UserId = -1;
	InitType3();
	InitType4(true);
}

/**
* @brief	Adds the NPC to the region.
*
* @param	new_region_x	The new region x coordinate.
* @param	new_region_z	The new region z coordinate.
*/
void CNpc::AddToRegion(int16 new_region_x, int16 new_region_z) {
	if (GetMap() == nullptr)
		return;

	if (GetMap()->GetXRegionMax() < new_region_x
		|| GetMap()->GetZRegionMax() < new_region_z)
		return;

	GetRegion()->Remove(this);
	SetRegion(new_region_x, new_region_z);

	GetRegion()->Add(this);
}

/**
* @brief	Sends the movement packet for the NPC.
*
* @param	fPosX 	The position x coordinate.
* @param	fPosY 	The position y coordinate.
* @param	fPosZ 	The position z coordinate.
* @param	fSpeed	The speed.
*/
void CNpc::MoveResult(float fPosX, float fPosY, float fPosZ, float fSpeed) {
	if (nIsPet)
		return;

	/*Packet result(WIZ_NPC_MOVE);

	SetPosition(fPosX, fPosY, fPosZ);
	RegisterRegion();
	#if __VERSION >= 1950
	result << uint8(fSpeed);
	#endif
	result << GetID() << GetSPosX() << GetSPosZ() << GetSPosY() << uint16(1);
	SendToRegion(&result);*/

	Packet result(WIZ_NPC_MOVE, uint8(SUCCESS));
	SetPosition(fPosX, fPosY, fPosZ);
	RegisterRegion();
	result << GetID() << GetSPosX() << GetSPosZ() << GetSPosY();
	if (m_bIceSpeedAmount > 0)
		result << fSpeed / float(m_bIceSpeedAmount);
	else
		result << fSpeed * float(85.99F);
	SendToRegion(&result);
}

/**
* @brief	Constructs an in/out packet for the NPC.
*
* @param	result	The packet buffer the constructed packet will be stored in.
* @param	bType 	The type (in or out).
*/
void CNpc::GetInOut(Packet & result, uint8 bType) {
	result.Initialize(WIZ_NPC_INOUT);
	result << bType << GetID();
	if (bType != INOUT_OUT)
		GetNpcInfo(result);

	if (bType == INOUT_IN)
		OnRespawn();
}

/**
* @brief	Constructs and sends an in/out packet for the NPC.
*
* @param	bType	The type (in or out).
* @param	fX   	The x coordinate.
* @param	fZ   	The z coordinate.
* @param	fY   	The y coordinate.
*/
void CNpc::SendInOut(uint8 bType, float fX, float fZ, float fY) {
	if (GetRegion() == nullptr) {
		SetRegion(GetNewRegionX(), GetNewRegionZ());
		if (GetRegion() == nullptr)
			return;
	}

	if (bType == INOUT_OUT) {
		GetRegion()->Remove(this);
	} else {
		GetRegion()->Add(this);
		SetPosition(fX, fY, fZ);
	}

	Packet result;
	GetInOut(result, bType);
	SendToRegion(&result);

	if (nIsPet) {
		Packet result3;
		CUser *pUser = g_pMain->GetUserPtr(UserId);
		CPet *mPet = g_pMain->GetPetPtr(nSerial);

		if (mPet == nullptr
			|| pUser == nullptr)
			return;

		if (pUser->LastWarpTime > (UNIXTIME - 2))
			return;

		result3.Initialize(WIZ_OBJECT_EVENT);
		result3 << uint8(0x0B) << uint8(0x01) << GetID() << uint32(30403);
		SendToRegion(&result3);

		g_pMain->m_LivingPetArray.insert(std::make_pair(mPet->m_Serial, mPet));

		Packet pet(WIZ_PET);
		pet.DByte();

		pet << uint8(0x01) << uint8(0x05) << uint8(0x01) << uint8(0x01) << uint8(0x00)
			<< uint32(mPet->SpecialPetID)
			<< mPet->GetName()
			<< mPet->m_sClass
			<< mPet->m_bLevel << uint16((mPet->m_iExp * 10000) / g_pMain->GetPetExpByLevel(mPet->m_bLevel))
			<< uint16(mPet->m_sMaxHp) << uint16(mPet->m_sHp)
			<< uint16(mPet->m_sMaxMp) << uint16(mPet->m_sMp)
			<< uint16(mPet->m_sSatisfaction)
			<< uint16(mPet->m_sAttack) << uint16(mPet->m_sAc)
			<< uint8(mPet->m_sFireR) << uint8(mPet->m_sColdR) << uint8(mPet->m_sLightningR)
			<< uint8(mPet->m_sMagicR) << uint8(mPet->m_sDiseaseR) << uint8(mPet->m_sPoisonR);

		for (int i = 0; i < PET_MAX; i++) // pet cıkar görürsün de ne sorcaksan sor cıkıcam valla kanka 2 minik sorum var :D 
		{
			_ITEM_DATA * pItem = &mPet->m_sItemArray[i];

			//if(pItem->nNum > 0) 
			pet << pItem->nNum << pItem->sDuration << pItem->sCount << pItem->bFlag << uint32(0) << pItem->nExpirationTime;
			//else
			//pet << uint32(0) << uint16(0) << uint16(0) << uint8(0) << uint16(0) << uint32(0) << uint32(0);
		}

		pUser->Send(&pet);
	}
}

/**
* @brief	Gets NPC information for use in various NPC packets.
*
* @param	pkt	The packet the information will be stored in.
*/
void CNpc::GetNpcInfo(Packet & pkt, uint8 myNation) {
	if (nIsPet) {
		pkt.SByte();
		CUser *pUser = g_pMain->GetUserPtr(UserId);
		if (!pUser)
			return;
		_ITEM_DATA *pItem = nullptr;

		if ((pItem = pUser->GetItem(SHOULDER)) == nullptr)
			return;

		CPet *newPet = g_pMain->GetPetPtr(pItem->nSerialNum);
		if (newPet == nullptr || newPet->pSession == nullptr)
			return;

		pkt << uint16(0) << uint8(newPet->isMonster() ? 1 : 2) << m_sPid << m_iSellingGroup
			<< uint8(15) << uint32(0)
			<< newPet->m_sSize << uint32(m_iWeapon_1) << uint32(m_iWeapon_2)
			<< pUser->GetName() << newPet->m_strPetID
			<< pUser->GetNation() << newPet->m_bLevel
			<< newPet->GetSPosX() << newPet->GetSPosZ() << newPet->GetSPosY()
			<< uint32(0) << uint8(0) << uint32(0) << int16(newPet->m_byDirection);

	}
	pkt.SByte();
#if (__VERSION >= 1950)
	pkt << GetProtoID()
		<< uint8(isMonster() ? 1 : 2) // Monster = 1, NPC = 2 (need to use a better flag)
		<< m_sPid
		<< m_iSellingGroup;
	if (nIsPet && !m_bIsEventNpc)
		pkt << uint8(64);
	else
		pkt << GetType();
	pkt << uint32(0)
		<< m_sSize
		<< uint32(m_iWeapon_1) << uint32(m_iWeapon_2); // isPet() ? string 
	if (nIsPet && m_bIsEventNpc)
		pkt << strUserName << strPetName;
	if (myNation == uint8(-1))
		pkt << uint8(isMonster() ? 0 : GetNation());
	else
		pkt << uint8(myNation);

	pkt << GetLevel()
		<< GetSPosX() << GetSPosZ() << GetSPosY();
	if (GetProtoID() == 8110)
		pkt << uint32(m_JuraidGateOpen);
	else
		pkt << uint32(m_byGateOpen);
	pkt << m_byObjectType
		<< uint16(0) << uint16(0) // unknown
		<< int16(m_byDirection);
#endif
#if (__VERSION < 1950)
	pkt << GetProtoID()
		<< uint8(isMonster() ? 1 : 2) // Monster = 1, NPC = 2 (need to use a better flag)
		<< m_sPid
		<< GetType()
		<< m_iSellingGroup
		<< m_sSize
		<< m_iWeapon_1 << m_iWeapon_2
		// Monsters require 0 regardless, otherwise they'll act as NPCs.
		<< uint8(isMonster() ? 0 : GetNation())
		<< GetLevel()
		<< GetSPosX() << GetSPosZ() << GetSPosY()
		<< uint32(isGateOpen())
		<< m_byObjectType
		<< uint16(0) << uint16(0) // unknown
		<< int16(m_byDirection);
#endif
}

/**
* @brief	Sends a gate status.
*
* @param	ObjectType  object type
* @param	bFlag  	The flag (open or shut).
* @param	bSendAI	true to update the AI server.
*/
void CNpc::SendGateFlag(uint8 bFlag /*= -1*/, bool bSendAI /*= true*/) {
	uint8 objectType = OBJECT_FLAG_LEVER;

	_OBJECT_EVENT * pObjectEvent = GetMap()->GetObjectEvent(GetProtoID());

	if (pObjectEvent)
		objectType = (uint8) pObjectEvent->sType;

	if (GetProtoID() == 8110)
		objectType = OBJECT_GATE;


	Packet result(WIZ_OBJECT_EVENT, objectType);

	if (GetProtoID() == 8110) {
		m_JuraidGateOpen = bFlag;

		result << uint8(1) << GetID() << bFlag;
		SendToRegion(&result);
	} else {

		// If there's a flag to set, set it now.
		if (bFlag >= 0)
			m_byGateOpen = (bFlag == 1);

		// Tell everyone nearby our new status.
		result << uint8(1) << GetID() << m_byGateOpen;
		SendToRegion(&result);

		// Tell the AI server our new status
		if (bSendAI) {
			result.Initialize(AG_NPC_GATE_OPEN);
			result << GetID() << GetProtoID() << m_byGateOpen;
			Send_AIServer(&result);
		}
	}
}

/**
* @brief	Changes an NPC's hitpoints.
*
* @param	amount   	The amount to adjust the HP by.
* @param	pAttacker	The attacker.
* @param	bSendToAI	true to update the AI server.
*/
void CNpc::HpChange(int amount, Unit *pAttacker /*= nullptr*/, bool bSendToAI /*= true*/) {
	uint16 tid = (pAttacker != nullptr ? pAttacker->GetID() : -1);

	if (GetType() == NPC_FOSSIL && tid < NPC_BAND && amount != -1)
		return;

	if (GetType() == NPC_TREE && tid < NPC_BAND && amount != -20)
		return;

	// Implement damage/HP cap.
	if (amount < -MAX_DAMAGE)
		amount = -MAX_DAMAGE;
	else if (amount > MAX_DAMAGE)
		amount = MAX_DAMAGE;

	// Glorious copypasta.
	if (amount < 0 && -amount > m_iHP)
		m_iHP = 0;
	else if (amount >= 0 && m_iHP + amount > m_iMaxHP)
		m_iHP = m_iMaxHP;
	else
		m_iHP += amount;

	if (nIsPet && nSerial > 0) {
		CPet * pPet = g_pMain->GetPetPtr(nSerial);

		if (pPet != nullptr)
			pPet->HpChange(amount, pAttacker, false);
	}

	// NOTE: This will handle the death notification/looting.
	if (bSendToAI)
		SendHpChangeToAI(tid, nIsPet && nSerial > 0 && isDead() ? -50000 : amount);

	if (pAttacker != nullptr
		&& pAttacker->isPlayer())
		TO_USER(pAttacker)->SendTargetHP(0, GetID(), amount);
}

void CNpc::HpChangeMagic(int amount, Unit *pAttacker /*= nullptr*/, AttributeType attributeType /*= AttributeNone*/) {
	uint16 tid = (pAttacker != nullptr ? pAttacker->GetID() : -1);

	if (GetType() == NPC_FOSSIL || GetType() == NPC_TREE)
		return;

	// Implement damage/HP cap.
	if (amount < -MAX_DAMAGE)
		amount = -MAX_DAMAGE;
	else if (amount > MAX_DAMAGE)
		amount = MAX_DAMAGE;

	HpChange(amount, pAttacker, false);
	SendHpChangeToAI(tid, amount, attributeType);
}

void CNpc::SendHpChangeToAI(uint16 sTargetID, int amount, AttributeType attributeType /*= AttributeNone*/) {
	Packet result(AG_NPC_HP_CHANGE);
	result << GetID() << sTargetID << m_iHP << amount << uint8(attributeType);
	Send_AIServer(&result);
}

/**
* @brief	Changes an NPC's mana.
*
* @param	amount	The amount to adjust the mana by.
*/
void CNpc::MSpChange(int amount) {

}

bool CNpc::CastSkill(Unit * pTarget, uint32 nSkillID) {
	if (pTarget == nullptr)
		return false;

	MagicInstance instance;

	instance.bSendFail = false;
	instance.nSkillID = nSkillID;
	instance.sCasterID = GetID();
	instance.sTargetID = pTarget->GetID();

	instance.Run();

	return (instance.bSkillSuccessful);
}

float CNpc::GetRewardModifier(uint8 byLevel) {
	int iLevelDifference = GetLevel() - byLevel;

	if (iLevelDifference <= -14)
		return 0.2f;
	else if (iLevelDifference <= -8 && iLevelDifference >= -13)
		return 0.5f;
	else if (iLevelDifference <= -2 && iLevelDifference >= -7)
		return 0.8f;

	return 1.0f;
}

float CNpc::GetPartyRewardModifier(uint32 nPartyLevel, uint32 nPartyMembers) {
	int iLevelDifference = GetLevel() - (nPartyLevel / nPartyMembers);

	if (iLevelDifference >= 8)
		return 1.3f;// was 2.0f edited byBrate
	else if (iLevelDifference >= 5)
		return 1.2f;// was 1.5f edited byBrate
	else if (iLevelDifference >= 2)
		return 1.1f;// was 1.2f edited byBrate

	return 1.0f;
}

/**
* @brief	Executes the death action.
*
* @param	pKiller	The killer.
*/
void CNpc::OnDeath(Unit *pKiller) {
	if (m_NpcState == NPC_DEAD || pKiller == nullptr)
		return;

	ASSERT(GetMap() != nullptr);
	ASSERT(GetRegion() != nullptr);

	m_NpcState = NPC_DEAD;
	m_sACPercent = 100;

	if (m_byObjectType == SPECIAL_OBJECT) {
		_OBJECT_EVENT *pEvent = GetMap()->GetObjectEvent(GetProtoID());
		if (pEvent != nullptr)
			pEvent->byLife = 0;
	}
	if (GetProtoID() != 31527
		|| GetProtoID() != 31528
		|| GetProtoID() != 31529
		|| GetProtoID() != 31530)
		Unit::OnDeath(pKiller);
	OnDeathProcess(pKiller);

	GetRegion()->Remove(TO_NPC(this));
	SetRegion();

	if (nIsPet && nSerial > 0) {
		CPet * pPet = g_pMain->GetPetPtr(nSerial);

		if (pPet != nullptr)
			pPet->Dead();
	}
}

/**
* @brief	Executes the death process.
*
* @param	pKiller	The killer.
*/
void CNpc::OnDeathProcess(Unit *pKiller) {
	if ((TO_NPC(this) == nullptr
		|| pKiller == nullptr))
		return;

	if (pKiller->GetID() > NPC_BAND)
		return;

	CUser * pUser = TO_USER(pKiller);

	if (pUser == nullptr)
		return;

	if (!m_bMonster) {
		switch (m_tNpcType) {
		case NPC_BIFROST_MONUMENT:
			pUser->BifrostProcess(pUser);
			break;
		case NPC_CZ_MONUMENT:
			CZMonumentProcess(pUser);
			break;
		case NPC_PVP_MONUMENT:
			PVPMonumentProcess(pUser);
			break;
		case NPC_BATTLE_MONUMENT:
			BattleMonumentProcess(pUser);
			break;
		case NPC_HUMAN_MONUMENT:
			NationMonumentProcess(pUser);
			break;
		case NPC_KARUS_MONUMENT:
			NationMonumentProcess(pUser);
			break;
		case NPC_DESTROYED_ARTIFACT:
			pUser->CastleSiegeWarProcess(pUser);
			break;
		}
	} else if (m_bMonster) {
		if (GetProtoID() == 700 || GetProtoID() == 750 || GetProtoID() == 701 || GetProtoID() == 751) {
			if (pUser->V3_CheckExistEvent(STARTER_SEED_QUEST, 1)) {
				_QUEST_HELPER * pQuestHelper;
				if (pUser->GetNation() == ELMORAD)
					pQuestHelper = g_pMain->m_QuestHelperArray.GetData(5005);
				else
					pQuestHelper = g_pMain->m_QuestHelperArray.GetData(5002);

				pUser->V3_QuestRunEvent(pQuestHelper, pQuestHelper->nEventTriggerIndex);
			}
		}

		else if (g_pMain->m_MonsterRespawnListArray.GetData(GetProtoID()) != nullptr && pUser->isInPKZone()) {
			g_pMain->SpawnEventNpc(g_pMain->m_MonsterRespawnListArray.GetData(GetProtoID())->sSid, true, GetZoneID(), GetX(), GetY(), GetZ(), g_pMain->m_MonsterRespawnListArray.GetData(GetProtoID())->sCount);

		} else if (g_pMain->pTempleEvent.ActiveEvent == TEMPLE_EVENT_JURAD_MOUNTAIN) {
			if (JuraidTempleEventZone()) {
				if (g_pMain->m_MonsterRespawnListArray.GetData(GetProtoID()) != nullptr)
					g_pMain->SpawnEventNpc(g_pMain->m_MonsterRespawnListArray.GetData(GetProtoID())->sSid, true, GetZoneID(), GetX(), GetY(), GetZ(), g_pMain->m_MonsterRespawnListArray.GetData(GetProtoID())->sCount, 5, 60 * 60, 0, GetNation(), pUser->GetSocketID(), pUser->GetEventRoom());


				if (GetProtoID() == 8100 || GetProtoID() == 8102) {
					pUser->GetNation() == KARUS ? g_pMain->pTempleEvent.KarusDeathRoom1[pUser->GetEventRoom()]++ : g_pMain->pTempleEvent.ElmoDeathRoom1[pUser->GetEventRoom()]++;

					if ((g_pMain->pTempleEvent.KarusDeathRoom1[pUser->GetEventRoom()] > 3 && pUser->GetNation() == KARUS)
						|| (g_pMain->pTempleEvent.ElmoDeathRoom1[pUser->GetEventRoom()] > 3 && pUser->GetNation() == ELMORAD)) {
						if (pUser->GetNation() == KARUS) {
							g_pMain->pTempleEvent.KarusDeathRoom2[pUser->GetEventRoom()] = 0;
							CNpc * pNpc = g_pMain->GetNpcPtr(g_pMain->pTempleEvent.JuraidKarusGateID1[pUser->GetEventRoom()]);

							if (pNpc == nullptr)
								printf("Bulunamadi... K 1\n");
							return;
							pNpc->SendGateFlag(2);
						} else {
							g_pMain->pTempleEvent.ElmoDeathRoom2[pUser->GetEventRoom()] = 0;
							CNpc * pNpc = g_pMain->GetNpcPtr(g_pMain->pTempleEvent.JuraidElmoGateID1[pUser->GetEventRoom()]);

							if (pNpc == nullptr)
								printf("Bulunamadi... E 1\n");
							return;
							pNpc->SendGateFlag(2);
						}
					}
				} else if (GetProtoID() == 8101 || GetProtoID() == 8104) {
					pUser->GetNation() == KARUS ? g_pMain->pTempleEvent.KarusDeathRoom2[pUser->GetEventRoom()]++ : g_pMain->pTempleEvent.ElmoDeathRoom2[pUser->GetEventRoom()]++;

					if ((g_pMain->pTempleEvent.KarusDeathRoom2[pUser->GetEventRoom()] > 3 && pUser->GetNation() == KARUS)
						|| (g_pMain->pTempleEvent.ElmoDeathRoom2[pUser->GetEventRoom()] > 3 && pUser->GetNation() == ELMORAD)) {
						if (pUser->GetNation() == KARUS) {
							g_pMain->pTempleEvent.KarusDeathRoom3[pUser->GetEventRoom()] = 0;
							CNpc * pNpc = g_pMain->GetNpcPtr(g_pMain->pTempleEvent.JuraidKarusGateID2[pUser->GetEventRoom()]);

							if (pNpc == nullptr)
								printf("Bulunamadi... K 2\n");
							return;
							pNpc->SendGateFlag(2);
						} else {
							g_pMain->pTempleEvent.ElmoDeathRoom3[pUser->GetEventRoom()] = 0;
							CNpc * pNpc = g_pMain->GetNpcPtr(g_pMain->pTempleEvent.JuraidElmoGateID2[pUser->GetEventRoom()]);

							if (pNpc == nullptr)
								printf("Bulunamadi... E 2\n");
							return;
							pNpc->SendGateFlag(2);
						}
					}
				} else if (GetProtoID() == 8103 || GetProtoID() == 8105) {
					pUser->GetNation() == KARUS ? g_pMain->pTempleEvent.KarusDeathRoom3[pUser->GetEventRoom()]++ : g_pMain->pTempleEvent.ElmoDeathRoom3[pUser->GetEventRoom()]++;

					if ((g_pMain->pTempleEvent.KarusDeathRoom3[pUser->GetEventRoom()] > 3 && pUser->GetNation() == KARUS)
						|| (g_pMain->pTempleEvent.ElmoDeathRoom3[pUser->GetEventRoom()] > 3 && pUser->GetNation() == ELMORAD)) {
						if (pUser->GetNation() == KARUS) {
							CNpc * pNpc = g_pMain->GetNpcPtr(g_pMain->pTempleEvent.JuraidKarusGateID3[pUser->GetEventRoom()]);

							if (pNpc == nullptr)
								printf("Bulunamadi... K 3\n");
							return;
							pNpc->SendGateFlag(2);
						} else {
							CNpc * pNpc = g_pMain->GetNpcPtr(g_pMain->pTempleEvent.JuraidElmoGateID3[pUser->GetEventRoom()]);

							if (pNpc == nullptr)
								printf("Bulunamadi... E 3\n");
							return;
							pNpc->SendGateFlag(2);
						}
					}
				} else if (GetProtoID() == 8106)// Deva
				{
					g_pMain->pTempleEvent.isDevaControl[GetEventRoom()] = true;
					g_pMain->pTempleEvent.isDevaFlag[GetEventRoom()] = true;
					g_pMain->pTempleEvent.DevaNation[GetEventRoom()] = pUser->GetNation();
				}

			}
		} else if (m_tNpcType == NPC_CHAOS_STONE && pUser->isInPKZone()) {
			ChaosStoneProcess(pUser, 5);
		} else if ((m_sSid == g_pMain->Zone1Boss[GetEventRoom()] || m_sSid == g_pMain->Zone2Boss[GetEventRoom()] || m_sSid == g_pMain->Zone3Boss[GetEventRoom()]) && (GetZoneID() == ZONE_STONE1 || GetZoneID() == ZONE_STONE2 || GetZoneID() == ZONE_STONE3)) {
			Packet dresult(WIZ_EVENT);
			dresult << uint8(TEMPLE_EVENT_FINISH)
				<< uint8(0) << uint8(0)
				<< uint8(1) << uint8(20) << uint32(0);
			TO_USER(pKiller)->Send(&dresult);
			TO_USER(pKiller)->m_TimeMonsterStone = UNIXTIME + 20;
		}

		if (g_pMain->m_bForgettenTempleIsActive && GetZoneID() == ZONE_FORGOTTEN_TEMPLE)
			g_pMain->m_ForgettenTempleMonsterList.erase(m_sNid);

		if (pUser->isInParty()) {
			_PARTY_GROUP *pParty = g_pMain->GetPartyPtr(pUser->GetPartyID());
			if (pParty != nullptr) {
				for (int i = 0; i < 8; i++) {
					if (pParty->uid[i] >= 0) {
						CUser * pUserRange = g_pMain->GetUserPtr(pParty->uid[i]);
						if (!isInRangeSlow(pUserRange, 50.0f) || pUserRange == nullptr)
							continue;

						CUser * pUserParty = g_pMain->GetUserPtr(pParty->uid[i]);
						pUserParty->V3_MonsterCount(GetProtoID());
						pUserParty->AchieveType2(this);
					}
				}
			}
		} else {
			pUser->V3_MonsterCount(GetProtoID());
			pUser->AchieveType2(this);
		}
	}

	DateTime time;
	string pKillerPartyUsers;

	if (pUser->isInParty()) {
		CUser *pPartyUser;
		_PARTY_GROUP *pParty = g_pMain->GetPartyPtr(pUser->GetPartyID());
		if (pParty) {
			for (int i = 0; i < MAX_PARTY_USERS; i++) {
				pPartyUser = g_pMain->GetUserPtr(pParty->uid[i]);
				if (pPartyUser)
					pKillerPartyUsers += string_format("%s,", pPartyUser->GetName().c_str());
			}
		}

		if (!pKillerPartyUsers.empty())
			pKillerPartyUsers = pKillerPartyUsers.substr(0, pKillerPartyUsers.length() - 1);
	}

	if (pKillerPartyUsers.empty())
		g_pMain->WriteDeathNpcLogFile(string_format("[ %s - %d:%d:%d ] Killer=%s,SID=%d,Target=%s,Zone=%d,X=%d,Z=%d\n", m_bMonster ? "MONSTER" : "NPC", time.GetHour(), time.GetMinute(), time.GetSecond(), pKiller->GetName().c_str(), GetProtoID(), GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));
	else
		g_pMain->WriteDeathNpcLogFile(string_format("[ %s - %d:%d:%d ] Killer=%s,KillerParty=%s,SID=%d,Target=%s,Zone=%d,X=%d,Z=%d\n", m_bMonster ? "MONSTER" : "NPC", time.GetHour(), time.GetMinute(), time.GetSecond(), pKiller->GetName().c_str(), pKillerPartyUsers.c_str(), GetProtoID(), GetName().c_str(), GetZoneID(), uint16(GetX()), uint16(GetZ())));

	InitType3();
	InitType4(false);
}

void CNpc::InitType3() {
	for (int i = 0; i < MAX_TYPE3_REPEAT; i++)
		m_durationalSkills[i].Reset();

	m_bType3Flag = false;
}

void CNpc::InitType4(bool bRemoveSavedMagic /*= false*/, uint8 buffType /* = 0 */) {
	Type4BuffMap buffMap = m_buffMap; // copy the map

	for (auto itr = buffMap.begin(); itr != buffMap.end(); itr++) {
#ifdef GAMESERVER
		if (buffType > 0 && itr->second.m_bBuffType != buffType)
			continue;

		CMagicProcess::RemoveType4Buff(itr->first, this, bRemoveSavedMagic, buffType > 0 ? true : false);
#endif
	}
}

/**
* @brief	Executes the Npc respawn.
*/
void CNpc::OnRespawn() {
	if (GetProtoID() == BORDER_WAR_MONUMENT_SID
		|| (g_pMain->m_byBattleOpen == NATION_BATTLE
			&& (GetProtoID() == ELMORAD_MONUMENT_SID
				|| GetProtoID() == ASGA_VILLAGE_MONUMENT_SID
				|| GetProtoID() == RAIBA_VILLAGE_MONUMENT_SID
				|| GetProtoID() == DODO_CAMP_MONUMENT_SID
				|| GetProtoID() == LUFERSON_MONUMENT_SID
				|| GetProtoID() == LINATE_MONUMENT_SID
				|| GetProtoID() == BELLUA_MONUMENT_SID
				|| GetProtoID() == LAON_CAMP_MONUMENT_SID))) {
		_MONUMENT_INFORMATION * pData = new	_MONUMENT_INFORMATION();
		pData->sSid = GetProtoID();
		pData->sNid = m_sNid;
		pData->RepawnedTime = int32(UNIXTIME);

		if (GetProtoID() == DODO_CAMP_MONUMENT_SID || GetProtoID() == LAON_CAMP_MONUMENT_SID)
			g_pMain->m_bMiddleStatueNation = m_bNation;

		if (!g_pMain->m_NationMonumentInformationArray.PutData(pData->sSid, pData))
			delete pData;
	} else if (g_pMain->m_bForgettenTempleIsActive && GetZoneID() == ZONE_FORGOTTEN_TEMPLE)
		g_pMain->m_ForgettenTempleMonsterList.insert(std::make_pair(m_sNid, GetProtoID()));
}

/**
* @brief	Executes the death process.
*
* @param	pUser	The User.
* @param	MonsterCount The Respawn boss count.
*/
void CNpc::ChaosStoneProcess(CUser *pUser, uint16 MonsterCount) {
	if (pUser == nullptr)
		return;

	g_pMain->SendNotice<CHAOS_STONE_ENEMY_NOTICE>("", GetZoneID(), Nation::ALL);

	std::vector<uint32> MonsterSpawned;
	std::vector<uint32> MonsterSpawnedFamily;
	bool bLoopBack = true;

	for (uint8 i = 0; i < MonsterCount; i++) {
		uint32 nMonsterNum = myrand(0, g_pMain->m_MonsterSummonListZoneArray.GetSize());
		_MONSTER_SUMMON_LIST_ZONE * pMonsterSummonListZone = g_pMain->m_MonsterSummonListZoneArray.GetData(nMonsterNum);

		if (pMonsterSummonListZone != nullptr) {
			if (pMonsterSummonListZone->ZoneID == GetZoneID()) {
				if (std::find(MonsterSpawned.begin(), MonsterSpawned.end(), nMonsterNum) == MonsterSpawned.end()) {
					if (std::find(MonsterSpawnedFamily.begin(), MonsterSpawnedFamily.end(), pMonsterSummonListZone->byFamily) == MonsterSpawnedFamily.end()) {
						g_pMain->SpawnEventNpc(pMonsterSummonListZone->sSid, true, GetZoneID(), GetX(), GetY(), GetZ(), 1, CHAOS_STONE_MONSTER_RESPAWN_RADIUS, CHAOS_STONE_MONSTER_LIVE_TIME);
						MonsterSpawned.push_back(nMonsterNum);
						MonsterSpawnedFamily.push_back(pMonsterSummonListZone->byFamily);
						bLoopBack = false;
					}
				}
			}
		}

		if (bLoopBack)
			i--;
		else
			bLoopBack = true;
	}
}

/*
* @brief	Executes the pvp monument process.
*
* @param	pUser	The User.
*/
void CNpc::PVPMonumentProcess(CUser *pUser) {
	if (pUser == nullptr)
		return;

	std::string sKillMonuPvP;
	std::string sKillMonuEvent;
	std::string sMonuNation;

	Packet result(WIZ_CHAT, uint8(MONUMENT_NOTICE));
	result << uint8(FORCE_CHAT) << pUser->GetNation() << pUser->GetName().c_str();
	g_pMain->Send_Zone(&result, GetZoneID(), nullptr, Nation::ALL);

	if (pUser->GetNation() == KARUS)
		sMonuNation = "Karus";
	else
		sMonuNation = "Human";

	sKillMonuPvP = string_format("%s has destroyed the %s. %s nation get +5 NPs per kill!", pUser->GetName().c_str(), GetName().c_str(), sMonuNation.c_str());
	sKillMonuEvent = string_format("%s has destroyed the %s. %s nation get +10 NPs per kill!", pUser->GetName().c_str(), GetName().c_str(), sMonuNation.c_str());

	if (GetZoneID() == ZONE_RONARK_LAND) {
		g_pMain->m_nPVPMonumentNation[GetZoneID()] = pUser->GetNation();
		g_pMain->SendAnnouncement(sKillMonuPvP.c_str());
	} else {
		g_pMain->m_nEventMonumentNation[GetZoneID()] = pUser->GetNation();
		g_pMain->SendAnnouncement(sKillMonuEvent.c_str());
	}

	g_pMain->NpcUpdate(GetProtoID(), m_bMonster, pUser->GetNation(), pUser->GetNation() == KARUS ? MONUMENT_KARUS_SPID : MONUMENT_ELMORAD_SPID);
	pUser->GiveItem(BLUE_TREASURE_CHEST, 1);
}

/*
* @brief	Executes the battle monument process.
*
* @param	pUser	The User.
*/
void CNpc::BattleMonumentProcess(CUser *pUser) {
	if (pUser && g_pMain->m_byBattleOpen == NATION_BATTLE) {
		g_pMain->NpcUpdate(GetProtoID(), m_bMonster, pUser->GetNation(), pUser->GetNation() == KARUS ? MONUMENT_KARUS_SPID : MONUMENT_ELMORAD_SPID);
		g_pMain->Announcement(DECLARE_BATTLE_MONUMENT_STATUS, Nation::ALL, m_byTrapNumber, pUser);

		if (pUser->GetNation() == KARUS) {
			g_pMain->m_sKarusMonumentPoint += 2;
			g_pMain->m_sKarusMonuments++;

			if (g_pMain->m_sElmoMonuments != 0)
				g_pMain->m_sElmoMonuments--;

			if (g_pMain->m_sKarusMonuments >= 7) {
				g_pMain->m_sKarusMonumentPoint += 10;
				Packet result(WIZ_MAP_EVENT);
				result << uint8(3) << uint8(1) << short(15);
				g_pMain->Send_Zone(&result, ZONE_BATTLE4);
			}

			if (g_pMain->m_sKilledElmoNpc == 3 && g_pMain->m_sKarusMonuments >= 7)
				g_pMain->BattleZoneResult(pUser->GetNation());

		} else {
			g_pMain->m_sElmoMonumentPoint += 2;
			g_pMain->m_sElmoMonuments++;

			if (g_pMain->m_sKarusMonuments != 0)
				g_pMain->m_sKarusMonuments--;

			if (g_pMain->m_sElmoMonuments >= 7) {
				g_pMain->m_sElmoMonumentPoint += 10;
				Packet result(WIZ_MAP_EVENT);
				result << uint8(3) << uint8(2) << short(15);
				g_pMain->Send_Zone(&result, ZONE_BATTLE4);
			}

			if (g_pMain->m_sKilledKarusNpc == 3 && g_pMain->m_sElmoMonuments >= 7)
				g_pMain->BattleZoneResult(pUser->GetNation());
		}

		g_pMain->NereidsMonumentEvent(m_byTrapNumber, pUser->GetNation(), nullptr);

	}
}

/*
* @brief  Executes the nation monument process.
*
* @param  pUser  The User.
*/
void CNpc::NationMonumentProcess(CUser *pUser) {
	if (!pUser)
		return;

	if (g_pMain->m_byBattleOpen == NATION_BATTLE) {
		g_pMain->NpcUpdate(GetProtoID(), m_bMonster, pUser->GetNation());
		g_pMain->Announcement(DECLARE_NATION_MONUMENT_STATUS, Nation::ALL, GetProtoID(), pUser);

		uint16 sSid = 0;

		foreach_stlmap(itr, g_pMain->m_NationMonumentInformationArray)
			if (itr->second->sSid == (pUser->GetNation() == KARUS ? GetProtoID() + 10000 : GetProtoID() - 10000))
				sSid = itr->second->sSid;

		if (sSid != 0)
			g_pMain->m_NationMonumentInformationArray.DeleteData(sSid);
	} else {
		g_pMain->NpcUpdate(GetProtoID(), m_bMonster, pUser->GetNation());

		uint16 sSid = 0;

		foreach_stlmap(itr, g_pMain->m_NationMonumentInformationArray)
			if (itr->second->sSid == (pUser->GetNation() == KARUS ? GetProtoID() + 10000 : GetProtoID() - 10000))
				sSid = itr->second->sSid;

		if (sSid != 0)
			g_pMain->m_NationMonumentInformationArray.DeleteData(sSid);
	}
}

/*
* @brief  Executes the Cz monument process.
*
* @param  pUser  The User.
*/
void CNpc::CZMonumentProcess(CUser *pUser) {
	if (pUser == nullptr)
		return;

	std::string sKillMonu;
	std::string sRecoMonu;

	sKillMonu = string_format("%s has destroyed the %s at Ronark Land!", pUser->GetName().c_str(), GetName().c_str());
	sRecoMonu = string_format("%s has recovered the %s at Ronark Land!", pUser->GetName().c_str(), GetName().c_str());

	g_pMain->NpcUpdate(GetProtoID(), m_bMonster, pUser->GetNation());

	if (pUser->GetNation() == KARUS) {
		if (GetSpid() == 30092) {
			g_pMain->SendAnnouncement(sKillMonu.c_str());
			pUser->GiveItem(BLUE_TREASURE_CHEST, 1);
			pUser->SendLoyaltyChange(150);
		} else {
			g_pMain->SendAnnouncement(sRecoMonu.c_str());
		}
	} else {
		if (GetSpid() == 30091) {
			g_pMain->SendAnnouncement(sKillMonu.c_str());
			pUser->GiveItem(BLUE_TREASURE_CHEST, 1);
			pUser->SendLoyaltyChange(150);
		} else {
			g_pMain->SendAnnouncement(sRecoMonu.c_str());
		}
	}
}


void CNpc::HPTimeChangeType3() {
	if (isDead()
		|| !m_bType3Flag || isPlayer())
		return;

	uint16	totalActiveDurationalSkills = 0,
		totalActiveDOTSkills = 0;

	bool bIsDOT = false;
	for (int i = 0; i < MAX_TYPE3_REPEAT; i++) {
		MagicType3 * pEffect = &m_durationalSkills[i];
		if (!pEffect->m_byUsed)
			continue;

		// Has the required interval elapsed before using this skill?
		if ((UNIXTIME - pEffect->m_tHPLastTime) >= pEffect->m_bHPInterval) {
			Unit * pUnit = g_pMain->GetUnitPtr(pEffect->m_sSourceID);

			if (pUnit == nullptr)
				continue;

			if (!isInRangeSlow(pUnit, 50.0f))
				m_bType3Flag = false;

			// Reduce the HP 
			HpChange(pEffect->m_sHPAmount, pUnit); // do we need to specify the source of the DOT?
			pEffect->m_tHPLastTime = UNIXTIME;

			if (pEffect->m_sHPAmount < 0)
				bIsDOT = true;

			// Has the skill expired yet?
			if (++pEffect->m_bTickCount == pEffect->m_bTickLimit) {
				Packet result(WIZ_MAGIC_PROCESS, uint8(MAGIC_DURATION_EXPIRED));

				// Healing-over-time skills require the type 100
				if (pEffect->m_sHPAmount > 0)
					result << uint8(100);
				else // Damage-over-time requires 200.
					result << uint8(200);

				Send_AIServer(&result);

				pEffect->Reset();
			}
		}

		if (pEffect->m_byUsed) {
			totalActiveDurationalSkills++;
			if (pEffect->m_sHPAmount < 0)
				totalActiveDOTSkills++;
		}
	}

	// Have all the skills expired?
	if (totalActiveDurationalSkills == 0)
		m_bType3Flag = false;
}

void CNpc::Type4Duration() {
	Guard lock(m_buffLock);
	if (m_buffMap.empty())
		return;

	foreach(itr, m_buffMap) {
		if (itr->second.m_tEndTime > UNIXTIME)
			continue;

		CMagicProcess::RemoveType4Buff(itr->first, this, true, false);
		break; // only ever handle one at a time with the current logic
	}
}

