#pragma once

#include "stdafx.h"
#include "Unit.h"

class CGameServerDlg;
class CPet : public Unit {
public:
	CPet();
	std::string m_strPetID;
	uint16	m_sNid;
	uint64	m_iExp;
	typedef CSTLMap <_PET_MOVE> PetMovingMap;
	PetMovingMap m_MovingMap;
	uint32 LastStep;
	_ITEM_DATA m_sItemArray[PET_MAX];
	uint32	SpecialPetID;
	uint8	m_bLevel;
	uint32 LastMovedTime;
	uint16	m_sSatisfaction;
	uint64	m_Serial;
	bool isMoving;
	bool isDropping;
	uint8	mode;
	uint8	m_sClass;
	int		m_sMaxHp, m_sHp, m_sMaxMp, m_sMp;
	uint16 m_sAttack, m_sAc;
	uint8	m_byState;			// ¸ó½ºÅÍ (NPC) »óÅÂ
	int   m_iSellingGroup;		// ItemGroup
	int16	m_byDirection;
	uint8	m_NpcState;			// NPCÀÇ »óÅÂ - »ì¾Ò´Ù, Á×¾ú´Ù, ¼­ÀÖ´Ù µîµî...

	short	m_sPid;				// MONSTER(NPC) Picture ID
	short	m_sSize;			// MONSTER(NPC) Size
	int		m_iWeapon_1;
	int		m_iWeapon_2;

	uint8	m_bNation;
	uint8	m_bZone;
	float	m_curx, m_curz, m_cury;

	INLINE float GetX() { return m_curx; }
	INLINE float GetY() { return m_cury; }
	INLINE float GetZ() { return m_curz; }

	INLINE uint16 GetSPosX() { return uint16(GetX() * 10); };
	INLINE uint16 GetSPosY() { return uint16(GetY() * 10); };
	INLINE uint16 GetSPosZ() { return uint16(GetZ() * 10); };

	bool	m_bMonster; // are we a monster or an NPC?

	CNpc * m_pNpc;
	CUser* pSession;

	virtual void Initialize();
	virtual uint16 GetID() { return m_sNid; };
	virtual std::string & GetName() { return m_strPetID; };
	virtual int32 GetHealth() { return m_pNpc->GetHealth(); };
	virtual int32 GetMaxHealth() { return m_pNpc->GetMaxHealth(); };
	virtual int32 GetMana() { return m_pNpc->GetMana(); };
	virtual int32 GetMaxMana() { return m_pNpc->GetMaxMana(); };
	virtual bool isBlinking() { return m_pNpc->isBlinking(); };
	virtual bool isDead() { return m_pNpc->isDead(); };
	// 90 asas ve captain hýzý 67 swli hýz
	virtual void Moving(float toX, float toY, float toZ, uint8 Speed);
	void SendMoveResult(float fX, float fY, float fZ, float fSpeed /*= 0.0f*/);
	virtual void AddtoMovingMap(float toX, float toY, float toZ, uint8 speed, bool IsDrop, uint32 BundleID);
	uint16	m_sSid; // prototype ID
	INLINE short GetProtoID() { return m_sSid; };
	INLINE bool isMonster() { return m_bMonster; }

	void GetNpcInfos(Packet & pkt, CUser* pUser = nullptr);

	virtual bool isAlive() { return m_pNpc->isAlive(); };
	virtual void GetInOut(Packet& result, uint8 bType);
	virtual void AddToRegion(int16 new_region_x, int16 new_region_z);
	virtual short GetDamage(Unit* pTarget, _MAGIC_TABLE* pSkill, bool bPreviewOnly) { return m_pNpc->GetDamage(pTarget, pSkill, bPreviewOnly); };
	virtual void OnAttack(Unit* pTarget, AttackType attackType);
	virtual void OnDefend(Unit* pAttacker, AttackType attackType);
	virtual void InsertSavedMagic(uint32 nSkillID, uint16 sDuration);
	virtual bool HasSavedMagic(uint32 nSkillID) { return m_pNpc->HasSavedMagic(nSkillID); };
	virtual int16 GetSavedMagicDuration(uint32 nSkillID) { return m_pNpc->GetSavedMagicDuration(nSkillID); };
	virtual void HpChange(int amount, Unit* pAttacker, bool bSendToAI);
	virtual void HpChangeMagic(int amount, Unit* pAttacker, AttributeType attributeType);
	virtual void MSpChange(int amount);
	virtual void ExpChange(int amount);
	virtual void SatisfactionChange(int amount);
	virtual void Dead();
	virtual void StateChangeServerDirect(uint8 bType, uint32 nBuff);
	virtual bool isHostileTo(Unit* pTarget) { return m_pNpc->isHostileTo(pTarget); };
	virtual bool CanAttack(Unit* pTarget) { return m_pNpc->CanAttack(pTarget); };
	virtual bool isAttackable(Unit* pTarget) { return m_pNpc->isAttackable(pTarget); };
	virtual bool CanCastRHit(uint16 m_SocketID) { return m_pNpc->CanCastRHit(m_SocketID); };
	virtual bool isSameEventRoom(Unit* pTarget) { return m_pNpc->isSameEventRoom(pTarget); };
	virtual void SummonPet(CUser * pSessionz);
	virtual void UpdatePetItem();

	virtual ~CPet();
};