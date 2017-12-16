#include "stdafx.h"
#include "Map.h"
#include "DBAgent.h"

CPet::CPet() : Unit(UnitType::UnitPet) {
	CPet::Initialize();
}

void CPet::Initialize() {
	m_MovingMap.DeleteAllData();
	memset(m_sItemArray, 0x00, sizeof(m_sItemArray));
	SpecialPetID = 0;
	isMoving = false;
	isDropping = false;

	m_sSid = 0;
	m_sPid = 0;
	m_sSize = 100;
	m_strPetID.clear();
	m_byState = 0;
	m_iSellingGroup = 0;
	m_byDirection = 0;
	m_iWeapon_1 = 0;
	m_iWeapon_2 = 0;
	m_oSocketID = -1;
	m_bEventRoom = 0;

	m_NpcState = NPC_LIVE;
	m_iExp = 0;
	m_bLevel = 0;
	m_sClass = 0;
	m_bNation = 0;
	m_sSatisfaction = 0;
	m_Serial = 0;
	m_sMaxHp = 0;
	m_sMaxMp = 0;
	m_sHp = 0;
	m_sMp = 0;
	m_sAttack = 0;
	m_sAc = 0;

	m_bResistanceBonus = 0;
	m_sFireR = m_sColdR = m_sLightningR = m_sMagicR = m_sDiseaseR = m_sPoisonR = 0;

	m_pNpc = nullptr;
	mode = 0;
	LastStep = 1;
	LastMovedTime = 0;
	Unit::Initialize();
}

CPet::~CPet() {
}

void CPet::AddtoMovingMap(float toX, float toY, float toZ, uint8 speed, bool IsDrop, uint32 BundleID) {
	if (m_pNpc == nullptr)
		return;

	_PET_MOVE * pMove = new _PET_MOVE;

	pMove->X = toX;
	pMove->Y = toY;
	pMove->Z = toZ;
	pMove->Speed = speed;
	pMove->Time = uint32(UNIXTIME);
	pMove->Drop = IsDrop;
	pMove->BundleID = BundleID;

	Moving(toX, toY, toZ, speed);
	m_MovingMap.PutData(LastStep, pMove);
	LastStep++;
}

void CPet::SendMoveResult(float fX, float fY, float fZ, float fSpeed /*= 0.0f*/) {
	Packet result(MOVE_RESULT, uint8(SUCCESS));
	result << m_pNpc->GetID() << fX << fZ << fY << fSpeed;
	g_pMain->Send_AIServer(&result);
}

void CPet::Moving(float toX, float toY, float toZ, uint8 Speed) {
	if (this == nullptr || m_pNpc == nullptr)
		return;

	m_pNpc->SetPosition(toX, toY, toZ);
	m_pNpc->RegisterRegion();
	SendMoveResult(toX, toY, toZ, Speed);
	Packet result2(WIZ_NPC_MOVE);
	result2 << uint8(Speed) << m_pNpc->GetID() << m_pNpc->GetSPosX() << m_pNpc->GetSPosZ() << m_pNpc->GetSPosY() << uint16(1);
	m_pNpc->SendToRegion(&result2);
	LastMovedTime = uint32(UNIXTIME);
}

void CPet::SummonPet(CUser * pSessionz) {
	pSession = pSessionz;
	pSession->SummonPetID = SpecialPetID;

	if (pSession->SummonPetID < 1)
		return;

	m_NpcState = NPC_LIVE;
	m_sNid = pSession->GetSocketID() + INVALID_BAND;
	m_sSid = 19019;
	m_sPid = 25500;
	m_sSize = 80;
	m_iWeapon_1 = m_iWeapon_2 = 0;
	m_bZone = pSession->GetZoneID();
	m_bNation = pSession->GetNation();
	m_bLevel = 1;
	m_curx = pSession->m_curx;
	m_curz = pSession->m_curz;
	m_cury = pSession->m_cury;
	m_iSellingGroup = 0;
	m_fTotalHitrate = 0;
	m_fTotalEvasionrate = 0;
	m_sTotalAc = 0;
	m_sTotalHit = 5;
	m_byDirection = 45;
	m_oSocketID = pSession->GetSocketID() + INVALID_BAND;
	m_bEventRoom = pSession->m_bEventRoom;
	m_sFireR = m_sColdR = m_sLightningR = 0;
	m_sMagicR = m_sDiseaseR = m_sPoisonR = 0;
	m_sAttack = 20;
	m_sAc = 5;
	m_sMaxHp = 100;
	m_sMaxMp = 100;
	m_sHp = 100;
	m_sMp = 100;
	m_bMonster = false;

	g_pMain->SpawnEventNpc(m_sSid, false, m_bZone, m_curx, m_cury, m_curz, 1, 2, 0, 0, m_bNation, m_oSocketID, m_bEventRoom, true, GetName(), pSession->GetName(), m_Serial, pSession->GetID());
}

void CPet::GetInOut(Packet& result, uint8 bType) {
}

void CPet::AddToRegion(int16 new_region_x, int16 new_region_z) {
}

void CPet::OnAttack(Unit* pTarget, AttackType attackType) {
}

void CPet::OnDefend(Unit* pAttacker, AttackType attackType) {
}

void CPet::InsertSavedMagic(uint32 nSkillID, uint16 sDuration) {
}

void CPet::HpChange(int amount, Unit* pAttacker, bool bSendToAI) {
	if (m_pNpc == nullptr)
		return;

	CUser * pUser = g_pMain->GetUserPtr(m_pNpc->UserId);

	if (pUser == nullptr)
		return;

	Packet result(WIZ_PET);
	result << uint8(1) << uint8(7) << uint16(m_pNpc->GetMaxHealth()) << uint16(m_pNpc->GetHealth());
	pUser->Send(&result);

	if (m_pNpc->isDead()) {
		Guard lock(pUser->_unitlock);
		Type9BuffMap & buffMap = pUser->m_type9BuffMap;
		g_DBAgent.UpdatePetData(m_Serial, this);
		// Ensure this type of skill isn't already in use.

		Packet result2(WIZ_PET);
		result2 << uint8(1) << uint8(5) << uint8(2) << uint16(1);
		pUser->SendToRegion(&result2);
		g_pMain->m_LivingPetArray.erase(m_Serial);
		if (buffMap.find(8) != buffMap.end())
			return;

		buffMap.insert(std::make_pair(8, _BUFF_TYPE9_INFO(500117, UNIXTIME - 1)));
	}
	// NOTE: This will handle the death notification/looting.
	if (bSendToAI)
		m_pNpc->SendHpChangeToAI(m_pNpc->m_sNid, m_pNpc->nIsPet && m_pNpc->nSerial > 0 && isDead() ? -50000 : amount);
}

void CPet::Dead() {
	if (m_pNpc == nullptr)
		return;

	if (m_pNpc->GetHealth() > 0)
		m_pNpc->HpChange(-m_pNpc->GetHealth(), nullptr, true);
}

void CPet::HpChangeMagic(int amount, Unit* pAttacker, AttributeType attributeType) {
}

void CPet::ExpChange(int amount) {
	CUser * pUser = g_pMain->GetUserPtr(m_pNpc->UserId);

	if (pUser == nullptr)
		return;

	uint64 MaxEXP = g_pMain->GetPetExpByLevel(m_bLevel);

	m_iExp += amount;
	if (m_iExp < 0)
		m_iExp = 0;
	else if (m_iExp > MaxEXP && m_bLevel < MAX_LEVEL_PET) {
		m_iExp -= MaxEXP;
		m_bLevel++;
	} else if (m_iExp > MaxEXP && m_bLevel == MAX_LEVEL_PET)
		m_iExp = MaxEXP;

	Packet result(WIZ_PET);
	result << uint8(1) << uint8(10) << uint64(amount) << uint16((m_iExp * 10000) / g_pMain->GetPetExpByLevel(m_bLevel)) << m_bLevel << m_sSatisfaction;
	pUser->Send(&result);
}

void CPet::MSpChange(int amount) {
	CUser * pUser = g_pMain->GetUserPtr(m_pNpc->UserId);

	if (pUser == nullptr)
		return;

	m_sMp += amount;
	if (m_sMp < 0)
		m_sMp = 0;
	else if (m_sMp > m_sMaxMp)
		m_sMp = m_sMaxMp;

	Packet result(WIZ_PET);
	result << uint8(1) << uint8(13) << uint16(m_sMaxMp) << uint16(m_sMp);
	pUser->Send(&result);
}

void CPet::UpdatePetItem() {
	CUser * pUser = g_pMain->GetUserPtr(m_pNpc->UserId);

	if (pUser == nullptr)
		return;

	Packet result(WIZ_PET);
	result.DByte();
	result << uint8(1) << uint8(14) << uint32(SpecialPetID) << m_strPetID << m_sClass << m_bLevel << uint16((m_iExp * 10000) / g_pMain->GetPetExpByLevel(m_bLevel)) << uint16(m_sSatisfaction);
	pUser->Send(&result);
}

void CPet::SatisfactionChange(int amount) {
	if (m_pNpc == nullptr)
		return;

	CUser * pUser = g_pMain->GetUserPtr(m_pNpc->UserId);

	if (pUser == nullptr)
		return;

	m_sSatisfaction += amount;
	if (m_sSatisfaction < 0)
		m_sSatisfaction = 0;
	else if (m_sSatisfaction > 10000)
		m_sSatisfaction = 10000;

	if (amount > 0) {
		Packet result(WIZ_PET);
		result << uint8(1) << uint8(16) << uint16(m_sSatisfaction);
		pUser->Send(&result);
	}

	Packet result(WIZ_PET);
	result << uint8(1) << uint8(15) << uint16(m_sSatisfaction);
	pUser->Send(&result);
	//UpdatePetItem();
}

void CPet::StateChangeServerDirect(uint8 bType, uint32 nBuff) {
}