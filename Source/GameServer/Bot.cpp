#include "stdafx.h"
#include "Map.h"
#include "../shared/DateTime.h"

CBot::CBot()
{
	Initialize();
}

void CBot::Initialize()
{
	MerchantChat.clear();
	LastWarpTime = 0;
	m_tLastKillTime = 0;
	m_sChallangeAchieveID = 0;
	m_pktcount = 0;
	m_strUserID.clear();
	m_strMemo.clear();
	m_strAccountID.clear();
	for (int i = 0; i < STAT_COUNT; i++)
		m_bRebStats[i] = 0;
	m_sAchieveCoverTitle = 0;
	
	memset(&m_arSellMerchantItems, 0, sizeof(m_arSellMerchantItems));
	memset(&m_arBuyMerchantItems, 0, sizeof(m_arBuyMerchantItems));
	m_bMerchantState = MERCHANT_STATE_NONE;
	m_bAuthority = 1;
	m_sBind = -1;
	m_state = GAME_STATE_CONNECTED;
	m_bPartyLeader = false;
	m_bIsChicken = false;
	m_bIsHidingHelmet = false;
	m_bIsHidingCospre = false;
	
	m_bPremiumMerchant = false;
	m_bInParty = false;

	m_bInvisibilityType = INVIS_NONE;

	m_sDirection = 0;

	memset(&m_bStats, 0, sizeof(m_bStats));

	m_bAuthority = AUTHORITY_PLAYER;
	m_bLevel = 1;
	m_iExp = 0;
	m_iBank = m_iGold = 0;
	m_iLoyalty = 100;
	m_iLoyaltyMonthly = 0;
	m_iMannerPoint = 0;
	m_sHp = m_sMp = m_sSp = 0;

	m_iMaxHp = 0;
	m_iMaxMp = 1;
	m_iMaxSp = 120;
	m_sMerchantsSocketID = -1;
	m_bResHpType = USER_STANDING;
	m_bBlockPrivateChat = false;
	m_sPrivateChatUser = -1;
	m_bNeedParty = 0x01;


	m_bAbnormalType = ABNORMAL_NORMAL;	// User starts out in normal size.
	m_nOldAbnormalType = m_bAbnormalType;


	m_teamColour = TeamColourNone;

}

bool CBot::RegisterRegion()
{
	uint16 
		new_region_x = GetNewRegionX(), new_region_z = GetNewRegionZ(), 
		old_region_x = GetRegionX(),	old_region_z = GetRegionZ();

	if (GetRegion() == nullptr || (old_region_x == new_region_x && old_region_z == new_region_z))
		return false;

	AddToRegion(new_region_x, new_region_z);

	RemoveRegion(old_region_x - new_region_x, old_region_z - new_region_z);
	InsertRegion(new_region_x - old_region_x, new_region_z - old_region_z);	

	return true;
}

void CBot::AddToRegion(int16 new_region_x, int16 new_region_z)
{
	GetRegion()->Remove(this);
	SetRegion(new_region_x, new_region_z);
	GetRegion()->Add(this);
}

void CBot::RemoveRegion(int16 del_x, int16 del_z)
{
	ASSERT(GetMap() != nullptr);

	Packet result;
	GetInOut(result, INOUT_OUT);
	g_pMain->Send_OldRegions(&result, del_x, del_z, GetMap(), GetRegionX(), GetRegionZ(),nullptr,0);
}

void CBot::InsertRegion(int16 insert_x, int16 insert_z)
{
	ASSERT(GetMap() != nullptr);

	Packet result;
	GetInOut(result, INOUT_IN);
	g_pMain->Send_NewRegions(&result, insert_x, insert_z, GetMap(), GetRegionX(), GetRegionZ(),nullptr,0);
}

void CBot::SetRegion(uint16 x /*= -1*/, uint16 z /*= -1*/) 
{
	m_sRegionX = x; m_sRegionZ = z; 
	m_pRegion = m_pMap->GetRegion(x, z); // TODO: Clean this up
}

void CBot::StateChangeServerDirect(uint8 bType, uint32 nBuff)
{
	uint8 buff = *(uint8 *)&nBuff; // don't ask
	switch (bType)
	{
	case 1:
		m_bResHpType = buff;
		break;

	case 2:
		m_bNeedParty = buff;
		break;

	case 3:
		m_nOldAbnormalType = m_bAbnormalType;


		m_bAbnormalType = nBuff;
		break;

	case 5:
		m_bAbnormalType = nBuff;
		break;

	case 6:
		nBuff = m_bPartyLeader; // we don't set this here.
		break;

	case 7:
		break;

	case 8: // beginner quest
		break;

	case 14:
		break;
	}

	Packet result(WIZ_STATE_CHANGE);
	result << GetID() << bType << nBuff; 
	SendToRegion(&result);
}

void CBot::GetInOut(Packet & result, uint8 bType)
{
	result.Initialize(WIZ_USER_INOUT);
	result << uint16(bType) << GetID();
	if (bType != INOUT_OUT)
		GetUserInfo(result);
}

void CBot::UserInOut(uint8 bType)
{
	Packet result;
	GetInOut(result, bType);
	
		Guard lock(g_pMain->m_BotcharacterNameLock);
	if (bType == INOUT_OUT)
	{
		GetRegion()->Remove(this);
		m_state = GAME_STATE_CONNECTED;
		
		std::string upperName = GetName();
		STRTOUPPER(upperName);

		g_pMain->m_BotcharacterNameMap.erase(upperName);
	}
	else
	{
		GetRegion()->Add(this);
		m_state = GAME_STATE_INGAME;

	std::string upperName = GetName();
	STRTOUPPER(upperName);
	g_pMain->m_BotcharacterNameMap[upperName] = this;

	}
	SendToRegion(&result);
}

void CBot::SendToRegion(Packet *pkt)
{
	g_pMain->Send_Region(pkt, GetMap(), GetRegionX(), GetRegionZ(), nullptr, 0);
}

void CBot::GetUserInfo(Packet & pkt)
{

	pkt.SByte();
	pkt		<< GetName()
		<< uint16(m_bNation) << GetClanID() << m_bFame;

	CKnights * pKnights = g_pMain->GetClanPtr(GetClanID());
	if (pKnights == nullptr)
	{
		pkt	<< uint32(0) << uint16(0) << uint8(0) << uint16(-1) << uint32(0) << uint8(0);
	}
	else
	{
		CKnights *aKnights = g_pMain->GetClanPtr(pKnights->GetAllianceID());

		pkt	<< pKnights->GetAllianceID()
			<< pKnights->m_strName
			<< pKnights->m_byGrade << pKnights->m_byRanking
			<< uint16(pKnights->m_sMarkVersion) // symbol/mark version
			<< pKnights->GetCapeID(aKnights) // cape ID 
			<< pKnights->m_bCapeR << pKnights->m_bCapeG << pKnights->m_bCapeB << uint8(0) // this is stored in 4 bytes after all.
			// not sure what this is, but it (just?) enables the clan symbol on the cape 
			// value in dump was 9, but everything tested seems to behave as equally well...
			// we'll probably have to implement logic to respect requirements.
			<< uint8(1); 
	}
	uint8 bInvisibilityType = m_bInvisibilityType;
	if (bInvisibilityType != INVIS_NONE)
		bInvisibilityType = INVIS_DISPEL_ON_MOVE;

	pkt	<< m_bLevel << m_bRace << m_sClass
		<< GetSPosX() << GetSPosZ() << GetSPosY()
		<< m_bFace << m_nHair
		<< m_bResHpType << uint32(m_bAbnormalType)
		<< m_bNeedParty
		<< m_bAuthority
		<< m_bPartyLeader // is party leader (bool)
		<< bInvisibilityType // visibility state
		<< uint8(m_teamColour) // team colour (i.e. in soccer, 0=none, 1=blue, 2=red)
		<< m_bIsHidingHelmet << m_bIsHidingCospre // either this is correct and items are super buggy, or it causes baldness. You choose.
		<< m_sDirection // direction 
		<< m_bIsChicken // chicken/beginner flag
		<< m_bRank // king flag
		<< uint16(0)
		<< int8(-1) << int8(-1); // NP ranks (total, monthly)


	uint8 equippedItems[] = 
	{
		BREAST, LEG, HEAD, GLOVE, FOOT, SHOULDER, RIGHTHAND, LEFTHAND, CWING, CHELMET, CLEFT, CRIGHT, CTOP, FAIRY
	};

	foreach_array (i, equippedItems) 
	{
		_ITEM_DATA * pItem = &m_sItemArray[equippedItems[i]];

		if(pItem == nullptr)
			continue; 

		pkt << pItem->nNum << pItem->sDuration << pItem->bFlag;
	}
	


		pkt << m_bZone << uint8(-1) << uint8(-1) << uint16(0) << uint16(0) << uint16(0) << m_bIsHidingCospre << uint8(0) << uint8(GetRebLevel()) << uint16(m_sAchieveCoverTitle) << uint16(0) << uint8(0) << uint8(0);
	
} 
