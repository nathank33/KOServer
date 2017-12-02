#include "stdafx.h"
#include "Map.h"
#include "AISocket.h"
#include "Npc.h"
#include "MagicProcess.h"

using std::string;

bool CAISocket::HandlePacket(Packet & pkt)
{
	switch (pkt.GetOpcode())
	{
	case AG_CHECK_ALIVE_REQ:
		RecvCheckAlive(pkt);
		break;
	case AI_SERVER_CONNECT:
		LoginProcess(pkt);
		break;
	case AG_SERVER_INFO:
		RecvServerInfo(pkt);
		break;
	case NPC_INFO_ALL:
		RecvNpcInfoAll(pkt);
		break;
	case MOVE_RESULT:
		RecvNpcMoveResult(pkt);
		break;
	case MOVE_END_RESULT:
		break;
	case AG_ATTACK_REQ:
		RecvNpcAttack(pkt);
		break;
		// The AI server should send magic system requests to us.
		// It shouldn't have to duplicate all the processing code.
	case AG_MAGIC_ATTACK_REQ:
		CMagicProcess::MagicPacket(pkt);
		break;
	case AG_NPC_INFO:
		RecvNpcInfo(pkt);
		break;
	case AG_NPC_REGION_UPDATE:
		RecvNpcRegionUpdate(pkt);
		break;
	case AG_USER_EXP:
		RecvUserExp(pkt);
		break;
	case AG_SYSTEM_MSG:
		RecvSystemMsg(pkt);
		break;
	case AG_NPC_GIVE_ITEM:
		RecvNpcGiveItem(pkt);
		break;
	case AG_NPC_GATE_DESTORY:
		RecvGateDestory(pkt);
		break;
	case AG_DEAD:
		RecvNpcDead(pkt);
		break;
	case AG_NPC_INOUT:
		RecvNpcInOut(pkt);
		break;
	case AG_BATTLE_EVENT:
		RecvBattleEvent(pkt);
		break;
	case AG_NPC_EVENT_ITEM:
		RecvNpcEventItem(pkt);
		break;
	case AG_NPC_GATE_OPEN:
		RecvGateOpen(pkt);
		break;
	case AG_COMPRESSED:
		RecvCompressed(pkt);
		break;
	case AG_NPC_HP_CHANGE:
		RecvNpcHpChange(pkt);
		break;
	}

	return true;
}

void CAISocket::OnConnect()
{
	KOSocket::OnConnect();

	// Set a flag to indicate whether we've ever connected before or not
	// This is used accordingly by the AI server when we tell it what our status is.
	// Kind of messy, and needs looking into further.
	m_bHasConnected = true; 
}

void CAISocket::OnDisconnect()
{
	TRACE("*** CloseProcess - socketID=%d...  ***\n", GetSocketID());
	g_pMain->DeleteAllNpcList();
	g_pMain->m_sErrorSocketCount = 3; // yup, we're definitely disconnected (magic number!)
}

void CAISocket::LoginProcess(Packet & pkt)
{
	uint8 bReconnect = pkt.read<uint8>();

	TRACE("Connected to the AI server\n");
	if (bReconnect == 1)
		TRACE("**** ReConnect - socket = %d ****\n ", GetSocketID());

	g_pMain->m_bServerCheckFlag = true;
	g_pMain->SendAllUserInfo();
}

void CAISocket::RecvServerInfo(Packet & pkt)
{
	int size = g_pMain->m_ZoneArray.GetSize();
	uint16 sTotalMonster;
	uint8 bZone;

	pkt >> bZone >> sTotalMonster;

	g_pMain->m_sZoneCount++;

	if (g_pMain->m_sZoneCount == size)
	{
		g_pMain->m_sZoneCount = 0;
		g_pMain->m_bFirstServerFlag = true;
		g_pMain->m_bPointCheckFlag = true;
		g_pMain->m_NationMonumentInformationArray.DeleteAllData();
		printf("All spawn data loaded.\n");
	}
}

void CAISocket::RecvNpcInfoAll(Packet & pkt)
{
	uint8 bCount = pkt.read<uint8>(); // max of 20

	pkt.SByte();
	for (int i = 0; i < bCount; i++)
	{
		int16 bDirection;
		std::string strName;

		CNpc* pNpc = new CNpc();
		pNpc->Initialize();
		uint16 EventRoom;

		pkt >> pNpc->m_NpcState >> pNpc->m_sNid >> pNpc->m_sSid >> pNpc->m_sPid >> pNpc->m_sSize >> pNpc->m_iWeapon_1 >> pNpc->m_iWeapon_2
			>> pNpc->m_bZone >> strName >> pNpc->m_bNation >> pNpc->m_bLevel
			>> pNpc->m_curx >> pNpc->m_curz >> pNpc->m_cury >> bDirection
			>> pNpc->m_tNpcType >> pNpc->m_iSellingGroup >> pNpc->m_iMaxHP >> pNpc->m_iHP >> pNpc->m_byGateOpen
			>> pNpc->m_fTotalHitrate >> pNpc->m_fTotalEvasionrate 
			>> pNpc->m_sTotalAc >> pNpc->m_sTotalHit 
			>> pNpc->m_byObjectType
			>> pNpc->m_byTrapNumber >> pNpc->m_bMonster >> pNpc->m_oSocketID >> EventRoom
			>> pNpc->m_sFireR >> pNpc->m_sColdR >> pNpc->m_sLightningR
			>> pNpc->m_sMagicR >> pNpc->m_sDiseaseR >> pNpc->m_sPoisonR >> pNpc->m_bIsEventNpc >> pNpc->nIsPet >> pNpc->strPetName >> pNpc->strUserName >> pNpc->nSerial >> pNpc->UserId;


		pNpc->SetNPCEventRoom(EventRoom);
		pNpc->SetUnitEventRoom(EventRoom);

		if (strName.empty())
			strName = "<the spawn with no name>";

		if (strName.length() > MAX_NPC_SIZE)
		{
			pNpc->DecRef();
			continue;
		}

		pNpc->m_pMap = g_pMain->GetZoneByID(pNpc->GetZoneID());
		if (pNpc->GetMap() == nullptr)
		{
			pNpc->DecRef();
			continue;
		}

		//TRACE("Recv --> NpcUserInfo : uid = %d, x=%f, z=%f.. \n", nid, fPosX, fPosZ);
		pNpc->m_strName = strName;

		pNpc->m_byDirection = bDirection;
		pNpc->SetRegion(pNpc->GetNewRegionX(), pNpc->GetNewRegionZ());

		if (pNpc->m_byObjectType == SPECIAL_OBJECT)
		{
			_OBJECT_EVENT* pEvent = pNpc->GetMap()->GetObjectEvent(pNpc->m_sSid);
			if (pEvent != nullptr)
				pEvent->byLife = 1;
		}

		//	TRACE("Recv --> NpcUserInfoAll : uid=%d, sid=%d, name=%s, x=%f, z=%f. gate=%d, objecttype=%d \n", nid, sPid, szName, fPosX, fPosZ, byGateOpen, byObjectType);

		if (!g_pMain->m_arNpcArray.PutData(pNpc->GetID(), pNpc))
		{
			TRACE("Npc PutData Fail - %d\n", pNpc->GetID());
			pNpc->DecRef();
			continue;
		}

		if (pNpc->m_NpcState == NPC_DEAD)
		{
			TRACE("Recv --> NpcUserInfoAll : nid=%d, sid=%d, name=%s\n", pNpc->GetID(), pNpc->m_sSid, strName.c_str());
			continue;
		}

		pNpc->SendInOut(INOUT_IN, pNpc->GetX(), pNpc->GetZ(), pNpc->GetY());
	}
}

void CAISocket::RecvNpcMoveResult(Packet & pkt)
{
	uint8 flag;			// 01(INFO_MODIFY), 02(INFO_DELETE)	
	uint16 sNid;
	float fX, fY, fZ, fSecForMetor;
	pkt >> flag >> sNid >> fX >> fZ >> fY >> fSecForMetor;

	CNpc * pNpc = g_pMain->GetNpcPtr(sNid);
	if (pNpc == nullptr)
		return;

	if (pNpc->isDead())
	{
		Packet result(AG_NPC_HP_REQ);
		result << sNid << pNpc->m_iHP;
		Send(&result);
	}


	pNpc->MoveResult(fX, fY, fZ, (float)1000 / 1000);
}

void CAISocket::RecvNpcAttack(Packet & pkt)
{
	CNpc * pAttacker;
	Unit * pTarget;
	uint16 sAttackerID, sTargetID;
	int16 sDamage;
	uint8 bResult = ATTACK_FAIL;

	pkt >> sAttackerID >> sTargetID;

	pAttacker = g_pMain->GetNpcPtr(sAttackerID);
	pTarget = g_pMain->GetUnitPtr(sTargetID);

	if (pAttacker == nullptr
		|| pAttacker->isPlayer()
		|| pTarget == nullptr
		|| pTarget->isBlinking()
		|| pAttacker->isDead()
		|| pTarget->isDead()
		|| !pTarget->isAttackable()
		|| pAttacker->GetDamage(pTarget) < 1)
		return;

	if (pAttacker->GetEventRoom() != pTarget->GetEventRoom())
		return;

	// TODO: Wrap this up into its own virtual method
	sDamage = pAttacker->GetDamage(pTarget);
	if (sDamage > 0)
	{
		pTarget->HpChange(-(sDamage), pAttacker);
		if (pTarget->isDead())
			bResult = ATTACK_TARGET_DEAD;
		else
			bResult = ATTACK_SUCCESS;

		// Every hit takes a little of the defender's armour durability.
		if (pTarget->isPlayer())
			TO_USER(pTarget)->ItemWoreOut(DEFENCE, sDamage);
	}

	Packet result(WIZ_ATTACK, uint8(LONG_ATTACK));
	result << bResult << sAttackerID << sTargetID;
	pAttacker->SendToRegion(&result);
}

void CAISocket::RecvNpcInfo(Packet & pkt)
{
	std::string strName;
	uint8 Mode;
	uint16 sNid;
	int16 byDirection;
	bool bCreated = false;
	uint16 EventRoom = 0;
	pkt.SByte();
	pkt >> Mode >> sNid;

	CNpc *pNpc = g_pMain->GetNpcPtr(sNid);
	if (pNpc == nullptr)
	{
		pNpc = new CNpc();
		pNpc->m_sNid = sNid;
		bCreated = true;
	}

	pkt >> pNpc->m_sSid >> pNpc->m_sPid >> pNpc->m_sSize >> pNpc->m_iWeapon_1 >> pNpc->m_iWeapon_2
		>> pNpc->m_bZone >> strName >> pNpc->m_bNation >> pNpc->m_bLevel
		>> pNpc->m_curx >> pNpc->m_curz >> pNpc->m_cury >> byDirection
		>> pNpc->m_tNpcType >> pNpc->m_iSellingGroup >> pNpc->m_iMaxHP >> pNpc->m_iHP >> pNpc->m_byGateOpen
		>> pNpc->m_fTotalHitrate >> pNpc->m_fTotalEvasionrate 
		>> pNpc->m_sTotalAc >> pNpc->m_sTotalHit 
		>> pNpc->m_byObjectType
		>> pNpc->m_byTrapNumber >> pNpc->m_bMonster >> pNpc->m_oSocketID >> EventRoom
		>> pNpc->m_sFireR >> pNpc->m_sColdR >> pNpc->m_sLightningR
		>> pNpc->m_sMagicR >> pNpc->m_sDiseaseR >> pNpc->m_sPoisonR >> pNpc->m_bIsEventNpc >> pNpc->nIsPet >> pNpc->strPetName >> pNpc->strUserName >> pNpc->nSerial >> pNpc->UserId;



	if(pNpc->GetProtoID() == 8110)
		pNpc->m_JuraidGateOpen = 1;

	if(pNpc->GetZoneID() == ZONE_JURAD_MOUNTAIN)
	{
		if((pNpc->GetSPosX() / 10) == 512 && (pNpc->GetSPosZ() / 10) == 256 && pNpc->GetProtoID() == 8110)
		{
			g_pMain->pTempleEvent.JuraidElmoGateID3[EventRoom] = sNid;
			byDirection = 90;
		}

		if((pNpc->GetSPosX() / 10) == 512 && (pNpc->GetSPosZ() / 10) == 767 && pNpc->GetProtoID() == 8110)
			g_pMain->pTempleEvent.JuraidKarusGateID3[EventRoom] = sNid;

		if((pNpc->GetSPosX() / 10) == 308 && pNpc->GetProtoID() == 8110)
		{
			g_pMain->pTempleEvent.JuraidKarusGateID2[EventRoom] = sNid;
			byDirection = 135;
		}

		if((pNpc->GetSPosX() / 10) == 715 && pNpc->GetProtoID() == 8110)
		{
			g_pMain->pTempleEvent.JuraidElmoGateID2[EventRoom] = sNid;
			byDirection = 225;
		}

		if((pNpc->GetSPosX() / 10) == 224 && pNpc->GetProtoID() == 8110)
		{
			g_pMain->pTempleEvent.JuraidKarusGateID1[EventRoom] = sNid;
			byDirection = 90;
		}

		if((pNpc->GetSPosX() / 10) == 799 && pNpc->GetProtoID() == 8110)
			g_pMain->pTempleEvent.JuraidElmoGateID1[EventRoom] = sNid;
	}

	if(pNpc->GetZoneID() == ZONE_STONE1)
	{
		if(pNpc->GetProtoID() == 7032)
			byDirection = 225;
		else if(pNpc->GetProtoID() == 7000
			|| pNpc->GetProtoID() == 7001
			|| pNpc->GetProtoID() == 7003)
		{
			uint16 RandomMap[MAX_MONSTER_STONE_EVENT_ROOM];
			memset(RandomMap, 0, sizeof(RandomMap));
			int i = 0;
			foreach_stlmap_nolock (itr, g_pMain->m_MonsterRespawnListRandomArray)
			{
				_MONSTER_RESPAWN_LIST_RANDOM * pRandom = itr->second;

				if(pRandom == nullptr)
					continue;

				if(pRandom->ZoneID != ZONE_STONE1 
					|| pRandom->Family != g_pMain->Zone1Family[EventRoom])
					continue;

				if((pRandom->isBoss == 0
					&& pNpc->GetProtoID() == 7003)
					|| (pRandom->isBoss == 1 
					&& pNpc->GetProtoID() != 7003))
					continue;

				if(pRandom->isBoss == 1 
					&& pNpc->GetProtoID() == 7003)
				{
					g_pMain->Zone1Boss[EventRoom] = pRandom->sSid;
					pNpc->m_sSid = pRandom->sSid;
					pNpc->m_sPid = pRandom->sPid;
					pNpc->m_strName = pRandom->strName;

				}else if(pNpc->GetProtoID() != 7003 
					&& pRandom->isBoss == 0)
				{
					RandomMap[i] = pRandom->sIndex;
					i++;
				}
			}

			if(pNpc->GetProtoID() != 7003 && i > 0)
			{
				uint16 Randomized = myrand(0,i-1);
				_MONSTER_RESPAWN_LIST_RANDOM * kRandom = g_pMain->m_MonsterRespawnListRandomArray.GetData(RandomMap[Randomized]);
				if(kRandom != nullptr)
				{
					pNpc->m_sSid = kRandom->sSid;
					pNpc->m_sPid = kRandom->sPid;
					pNpc->m_strName = kRandom->strName;
				}

			}
		}
	}

	if(pNpc->GetZoneID() == ZONE_STONE2)
	{
		if(pNpc->GetProtoID() == 7033)
			byDirection = 225;
		else if(pNpc->GetProtoID() == 7005
			|| pNpc->GetProtoID() == 7006
			|| pNpc->GetProtoID() == 7008
			|| pNpc->GetProtoID() == 7040)
		{
			uint16 RandomMap[MAX_MONSTER_STONE_EVENT_ROOM];
			memset(RandomMap, 0, sizeof(RandomMap));
			int i = 0;
			foreach_stlmap_nolock (itr, g_pMain->m_MonsterRespawnListRandomArray)
			{
				_MONSTER_RESPAWN_LIST_RANDOM * pRandom = itr->second;

				if(pRandom == nullptr)
					continue;

				if(pRandom->ZoneID != ZONE_STONE2
					|| pRandom->Family != g_pMain->Zone2Family[EventRoom])
					continue;

				if((pRandom->isBoss == 0
					&& pNpc->GetProtoID() == 7008)
					|| (pRandom->isBoss == 1 
					&& pNpc->GetProtoID() != 7008))
					continue;

				if(pRandom->isBoss == 1 
					&& pNpc->GetProtoID() == 7008)
				{
					g_pMain->Zone2Boss[EventRoom] = pRandom->sSid;
					pNpc->m_sSid = pRandom->sSid;
					pNpc->m_sPid = pRandom->sPid;
					pNpc->m_strName = pRandom->strName;

				}else if(pNpc->GetProtoID() != 7008
					&& pRandom->isBoss == 0)
				{
					RandomMap[i] = pRandom->sIndex;
					i++;
				}
			}

			if(pNpc->GetProtoID() != 7008 && i > 0)
			{
				uint16 Randomized = myrand(0,i-1);
				_MONSTER_RESPAWN_LIST_RANDOM * kRandom = g_pMain->m_MonsterRespawnListRandomArray.GetData(RandomMap[Randomized]);
				if(kRandom != nullptr)
				{
					pNpc->m_sSid = kRandom->sSid;
					pNpc->m_sPid = kRandom->sPid;
					pNpc->m_strName = kRandom->strName;
				}

			}
		}
	}

	if(pNpc->GetZoneID() == ZONE_STONE3)
	{
		if(pNpc->GetProtoID() == 7034)
			byDirection = 225;
		else if(pNpc->GetProtoID() == 7011
			|| pNpc->GetProtoID() == 7010
			|| pNpc->GetProtoID() == 7013)
		{
			uint16 RandomMap[MAX_MONSTER_STONE_EVENT_ROOM];
			memset(RandomMap, 0, sizeof(RandomMap));
			int i = 0;
			foreach_stlmap_nolock (itr, g_pMain->m_MonsterRespawnListRandomArray)
			{
				_MONSTER_RESPAWN_LIST_RANDOM * pRandom = itr->second;

				if(pRandom == nullptr)
					continue;

				if(pRandom->ZoneID != ZONE_STONE3
					|| pRandom->Family != g_pMain->Zone3Family[EventRoom])
					continue;

				if((pRandom->isBoss == 0
					&& pNpc->GetProtoID() == 7013)
					|| (pRandom->isBoss == 1 
					&& pNpc->GetProtoID() != 7013))
					continue;

				if(pRandom->isBoss == 1 
					&& pNpc->GetProtoID() == 7013)
				{
					g_pMain->Zone3Boss[EventRoom] = pRandom->sSid;
					pNpc->m_sSid = pRandom->sSid;
					pNpc->m_sPid = pRandom->sPid;
					pNpc->m_strName = pRandom->strName;

				}else if(pNpc->GetProtoID() != 7013
					&& pRandom->isBoss == 0)
				{
					RandomMap[i] = pRandom->sIndex;
					i++;
				}
			}

			if(pNpc->GetProtoID() != 7013 && i > 0)
			{
				uint16 Randomized = myrand(0,i-1);
				_MONSTER_RESPAWN_LIST_RANDOM * kRandom = g_pMain->m_MonsterRespawnListRandomArray.GetData(RandomMap[Randomized]);
				if(kRandom != nullptr)
				{
					pNpc->m_sSid = kRandom->sSid;
					pNpc->m_sPid = kRandom->sPid;
					pNpc->m_strName = kRandom->strName;
				}

			}
		}
	}

	if (pNpc->nIsPet)

	{
		CPet * mPet = g_pMain->GetPetPtr(pNpc->nSerial);
		if (mPet != nullptr)
		{
			mPet->m_pNpc = pNpc;
			mPet->m_pNpc->m_sNid = mPet->m_sNid;
			mPet->mode = 4;
			pNpc->m_iMaxHP = mPet->m_sMaxHp;
			pNpc->m_iHP = mPet->m_sHp;

		}
		else
			return;
	}

	if (strName.empty() || strName.length() > MAX_NPC_SIZE)
	{
		pNpc->DecRef();
		return;
	}
	pNpc->SetUnitEventRoom(EventRoom);
	pNpc->SetNPCEventRoom(EventRoom);
	pNpc->m_NpcState = Mode;
	pNpc->m_byDirection = byDirection;
	pNpc->m_strName = strName;

	pNpc->m_pMap = g_pMain->GetZoneByID(pNpc->GetZoneID());
	if (pNpc->GetMap() == nullptr)
	{
		pNpc->DecRef();
		return;
	}

	pNpc->RegisterRegion();

	if (pNpc->m_byObjectType == SPECIAL_OBJECT)
	{
		_OBJECT_EVENT *pEvent = pNpc->GetMap()->GetObjectEvent( pNpc->m_sSid );
		if (pEvent != nullptr)
			pEvent->byLife = 1;
	}

	if (bCreated)
		g_pMain->m_arNpcArray.PutData(pNpc->GetID(), pNpc);

	if (pNpc->m_NpcState == NPC_DEAD)
	{
		TRACE("RecvNpcInfo - dead monster nid=%d, name=%s\n", pNpc->GetID(), pNpc->GetName().c_str());
		return;
	}

	pNpc->SendInOut(INOUT_IN, pNpc->GetX(), pNpc->GetZ(), pNpc->GetY());

	if(g_pMain->m_byBattleSiegeWarOpen && pNpc->m_sSid == 541)
		g_pMain->KickOutZoneUsers(ZONE_DELOS,ZONE_DELOS);

}

void CAISocket::RecvNpcRegionUpdate(Packet & pkt)
{
	uint16 sNpcID;
	float fX, fY, fZ;

	pkt >> sNpcID >> fX >> fY >> fZ;

	CNpc * pNpc = g_pMain->GetNpcPtr(sNpcID);
	if (pNpc == nullptr)
		return;

	pNpc->SetPosition(fX, fY, fZ);
	pNpc->RegisterRegion();
}

void CAISocket::RecvUserExp(Packet & pkt)
{
	uint16 tid;
	pkt >> tid;

	CUser* pUser = g_pMain->GetUserPtr(tid);
	if (pUser == nullptr)
		return;

	pUser->RecvUserExp(pkt);
}

void CAISocket::RecvSystemMsg(Packet & pkt)
{
	Packet result;
	std::string strSysMsg;
	uint8 bType;

	pkt >> bType >> strSysMsg;

	ChatPacket::Construct(&result, bType, &strSysMsg);
	g_pMain->Send_All(&result);
}

void CAISocket::RecvNpcGiveItem(Packet & pkt)
{
	Packet result(WIZ_ITEM_DROP);
	short sUid, sNid, regionx, regionz;
	float fX, fZ, fY;
	uint8 byCount, bZone;
	int nItemNumber[NPC_HAVE_ITEM_LIST];
	uint8 nSlotIndex[NPC_HAVE_ITEM_LIST];
	short sCount[NPC_HAVE_ITEM_LIST];
	CUser* pUser = nullptr;
	pkt >> sUid >> sNid >> bZone >> regionx >> regionz >> fX >> fZ >> fY >> byCount;

	for (int i = 0; i < byCount; i++)
	{
		pkt >> nItemNumber[i] >> sCount[i];
		nSlotIndex[i] = i;
	}

	if (sUid < 0 || sUid >= MAX_USER)
		return;

	C3DMap *pMap = g_pMain->GetZoneByID(bZone);
	if (pMap == nullptr)
		return;

	pUser = g_pMain->GetUserPtr(sUid);
	if (pUser == nullptr) 
		return;

	_LOOT_BUNDLE * pBundle = new _LOOT_BUNDLE;

	pBundle->tDropTime = uint32(UNIXTIME);
	pBundle->x = fX;
	pBundle->z = fZ;
	pBundle->y = fY;
	pBundle->LooterID = -1;
	pBundle->ItemsCount = 0;

	memset(pBundle->Items, 0, sizeof(pBundle->Items));

	for (int i = 0; i < byCount; i++)
	{
		if (g_pMain->GetItemPtr(nItemNumber[i]))
		{
			_LOOT_ITEM pItem(nItemNumber[i], sCount[i]);
			if (nItemNumber[i] == ITEM_GOLD)
			{
				// Add on any additional coins earned because of a global coin event.
				// NOTE: Officially it caps at SHRT_MAX, but that's really only for technical reasons.
				// Using the unsigned range gives us a little bit of wiggle room.
				uint32 coinAmount = sCount[i] * (100 + g_pMain->m_byCoinEventAmount) / 100;
				if (sCount[i] + coinAmount > USHRT_MAX)
					coinAmount = USHRT_MAX;

				pItem.sCount = coinAmount;
			}

			pBundle->Items[nSlotIndex[i]] = pItem; // emplace_back() would be so much more useful here, but requires C++11.
			pBundle->ItemsCount++;


		}
	}



	if (!pMap->RegionItemAdd(regionx, regionz, pBundle))
	{
		delete pBundle;
		return;
	}


	result << sNid << pBundle->nBundleID << uint8(2);

	pBundle->LooterID = pUser->GetID();

	if (!pUser->isInParty())
	{

		pUser->Send(&result);

		if(pUser->isSummonPet && pUser->SummonPetID > 0 )
		{
			_ITEM_TABLE * pItemData = nullptr;
			if((pItemData = pUser->GetItemPrototype(SHOULDER)) == nullptr
				|| !pItemData->isPet())
				return;

			_ITEM_DATA *pItem = nullptr;
			if ((pItem = pUser->GetItem(SHOULDER)) == nullptr 
				|| pItem->nNum != pItemData->Getnum())
				return;

			CPet *newPet = g_pMain->GetPetPtr(pItem->nSerialNum);
			if(newPet == nullptr || newPet->m_pNpc == nullptr)
				return;

			if(newPet->m_pNpc == nullptr)
				return;

			if(newPet->mode != 8)
				return;

			newPet->AddtoMovingMap(fX,fY,fZ,0,true,pBundle->nBundleID);

			/*if(newPet->mode == 8 )
			{
				for (int i = 0; i < byCount; i++)
				{
					pBundle->LooterID = pUser->SummonPetID;
					pUser->GiveItem(nItemNumber[i],sCount[i]);
				}
			}*/
		}

	}
	else
	{
		g_pMain->Send_PartyMember(pUser->GetPartyID(), &result);

		_PARTY_GROUP* pParty = g_pMain->GetPartyPtr(pUser->GetPartyID());
		if (pParty == nullptr)
			return;

		for (int i = 0; i < MAX_PARTY_USERS; i++)
		{
			CUser *pUsers = g_pMain->GetUserPtr(pParty->uid[i]);
			if (pUsers == nullptr)
				continue;

			if(pUser->isSummonPet && pUser->SummonPetID > 0 )
			{
				_ITEM_TABLE * pItemDatas = nullptr;
				if((pItemDatas = pUsers->GetItemPrototype(SHOULDER)) == nullptr
					|| !pItemDatas->isPet())
					return;

				_ITEM_DATA *pItems = nullptr;
				if ((pItems = pUsers->GetItem(SHOULDER)) == nullptr 
					|| pItems->nNum != pItemDatas->Getnum())
					return;

				CPet *newPets = g_pMain->GetPetPtr(pItems->nSerialNum);
				if(newPets == nullptr)
					return;

				if(newPets->m_pNpc == nullptr)
					return;

				if(newPets->mode != 8)
					return;

				if(!pUsers->isInRange(pBundle->x, pBundle->z, RANGE_50M))
					return;

				newPets->AddtoMovingMap(fX,fY,fZ,0,true,pBundle->nBundleID);

				/*if(newPets->mode == 8 )
				{
					for (int i = 0; i < byCount; i++)
					{
						pBundle->LooterID = pUser->SummonPetID;
						pUsers->GiveItem(nItemNumber[i],sCount[i]);
					}
				}*/
			}
		}
	}

}

void CAISocket::RecvCheckAlive(Packet & pkt)
{
	Packet result(AG_CHECK_ALIVE_REQ);
	g_pMain->m_sErrorSocketCount = 0;
	Send(&result);

	foreach_stlmap(itr,g_pMain->m_arBotArray)
	{
		CBot * pBot = itr->second;
		if(pBot != nullptr)
		{	
			if(pBot->isInGame())
			{

				if((pBot->m_bResHpType == USER_MINING || pBot->m_bResHpType == USER_FLASHING) && pBot->m_iGold + 15 < uint32(UNIXTIME))
				{
					Packet result(WIZ_MINING, uint8(MiningAttempt));
					uint16 resultCode = MiningResultSuccess, Random = myrand(0,10000);
					uint16 sEffect = 0;

					if(Random > 4000 || pBot->m_bResHpType == USER_SITDOWN) // EXP
						sEffect = 13082; // "XP" effect
					else
						sEffect = 13081; // "Item" effect

					result << resultCode << pBot->GetID() << sEffect;
					pBot->SendToRegion(&result);
					pBot->m_iGold = uint32(UNIXTIME);
				}
				else if(pBot->isMerchanting() && !pBot->MerchantChat.empty() && pBot->m_iLoyalty < uint32(UNIXTIME) - 99)
				{

					Packet result(WIZ_CHAT);


					ChatPacket::Construct(&result, MERCHANT_CHAT, &pBot->MerchantChat, &pBot->GetName(), pBot->m_bNation, pBot->GetID());
					pBot->SendToRegion(&result);
					pBot->m_iLoyalty = uint32(UNIXTIME);


				}
			}
		}
	}
}

void CAISocket::RecvGateDestory(Packet & pkt)
{
	uint16 nid, sCurZone, rX, rZ;
	bool bGateStatus;
	pkt >> nid >> bGateStatus >> sCurZone >> rX >> rZ;

	CNpc* pNpc = g_pMain->GetNpcPtr(nid);
	if (pNpc == nullptr)
		return;

	pNpc->m_byGateOpen = bGateStatus;
	TRACE("RecvGateDestory - (%d,%s), gate_status=%d\n", pNpc->GetID(), pNpc->GetName().c_str(), pNpc->m_byGateOpen);
}

// TODO: Remove this. NPCs don't just randomly die, it would make sense to do this as a result of the cause, not just because.
void CAISocket::RecvNpcDead(Packet & pkt)
{
	CNpc * pNpc;
	Unit * pAttacker;
	uint16 nid, attackerID;
	pkt >> nid >> attackerID;

	pNpc = g_pMain->GetNpcPtr(nid);
	if (pNpc == nullptr
		|| pNpc->GetMap() == nullptr)
		return;

	pAttacker = g_pMain->GetUnitPtr(attackerID);
	pNpc->OnDeath(pAttacker);

	if(pNpc->m_bIsEventNpc)
		g_pMain->m_arNpcArray.DeleteData(pNpc->GetID());
}

void CAISocket::RecvNpcInOut(Packet & pkt)
{
	uint8 bType;
	uint16 sNid;
	float fX, fZ, fY;

	pkt >> bType >> sNid >> fX >> fZ >> fY;
	CNpc * pNpc = g_pMain->GetNpcPtr(sNid);
	if (pNpc)
		pNpc->SendInOut(bType, fX, fZ, fY);
}

void CAISocket::RecvBattleEvent(Packet & pkt)
{
	string chatstr, strMaxUserName, strKnightsName;
	CUser* pUser = nullptr;
	CKnights* pKnights = nullptr;

	uint8 bType, bResult;
	pkt >> bType >> bResult;

	if (bType == BATTLE_EVENT_OPEN)
	{
	}
	else if (bType == BATTLE_MAP_EVENT_RESULT)
	{
		if (!g_pMain->isWarOpen())
		{
			TRACE("#### RecvBattleEvent Fail : battleopen = %d, type = %d\n", g_pMain->m_byBattleOpen, bType);
			return;
		}

		if (bResult == KARUS)
			g_pMain->m_byKarusOpenFlag = true;	
		else if (bResult == ELMORAD)
			g_pMain->m_byElmoradOpenFlag = true;
	}
	else if (bType == BATTLE_EVENT_RESULT)
	{
		if (!g_pMain->isWarOpen())
		{
			TRACE("#### RecvBattleEvent Fail : battleopen = %d, type=%d\n", g_pMain->m_byBattleOpen, bType);
			return;
		}

		pkt.SByte();
		pkt >> strMaxUserName;

		if (!strMaxUserName.empty()
			&& !g_pMain->m_byBattleSave)
		{
			Packet result(WIZ_BATTLE_EVENT, bType);
			result.SByte();
			result << bResult << strMaxUserName;

			g_pMain->AddDatabaseRequest(result);
			g_pMain->m_byBattleSave = true;
		}
	}
	else if (bType == BATTLE_EVENT_MAX_USER)
	{
		pkt.SByte();
		pkt >> strMaxUserName;

		if (!strMaxUserName.empty())
		{
			pUser = g_pMain->GetUserPtr(strMaxUserName, TYPE_CHARACTER);
			if (pUser != nullptr)
			{
				pKnights = g_pMain->GetClanPtr(pUser->GetClanID());
				if (pKnights)
					strKnightsName = pKnights->m_strName;

				if (g_pMain->m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE
					|| g_pMain->m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE2
					|| g_pMain->m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE6
					|| g_pMain->m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE3
					|| g_pMain->m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE4)
				{
					if (pUser->GetNation() == KARUS)
						g_pMain->m_sKilledElmoNpc++;
					else
						g_pMain->m_sKilledKarusNpc++;

					if (g_pMain->m_sKilledKarusNpc == 3 || g_pMain->m_sKilledElmoNpc == 3)
					{
						g_pMain->m_bResultDelay = true;
						g_pMain->m_bResultDelayVictory = pUser->GetNation();
						if(g_pMain->m_byBattleZone + ZONE_BATTLE_BASE != ZONE_BATTLE4)
							g_pMain->BattleZoneResult(pUser->GetNation());

						if(g_pMain->m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE4
							&& (pUser->GetNation() == 1 && g_pMain->m_sKarusMonuments >= 7)
							&& (pUser->GetNation() == 2 && g_pMain->m_sElmoMonuments >= 7))
							g_pMain->BattleZoneResult(pUser->GetNation());
					}
				}
			}
		}

		int nResourceID = 0;
		switch (bResult)
		{
		case 1: // captain
			nResourceID = IDS_KILL_CAPTAIN;
			break;
		case 3: // Karus warder 1
			nResourceID = IDS_KILL_KARUS_GUARD1;
			break;
		case 4: // Karus warder 2
			nResourceID = IDS_KILL_KARUS_GUARD2;
			break;
		case 5: // El Morad warder 1
			nResourceID = IDS_KILL_ELMO_GUARD1;
			break;
		case 6: // El Morad warder 2
			nResourceID = IDS_KILL_ELMO_GUARD2;
			break;
		case 7: // Karus Keeper
			nResourceID = IDS_KILL_GATEKEEPER;
			break; 
		case 8: // El Morad Keeper
			nResourceID = IDS_KILL_GATEKEEPER;
			break;
		}

		if (nResourceID == 0)
		{
			TRACE("RecvBattleEvent: could not establish resource for result %d", bResult);
			return;
		}

		g_pMain->GetServerResource(nResourceID, &chatstr, strKnightsName.c_str(), strMaxUserName.c_str());

		Packet result;
		string finalstr;

		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &finalstr, chatstr.c_str());
		ChatPacket::Construct(&result, WAR_SYSTEM_CHAT, &finalstr);
		g_pMain->Send_All(&result);

		ChatPacket::Construct(&result, PUBLIC_CHAT, &finalstr);
		g_pMain->Send_All(&result);
	}
}

void CAISocket::RecvNpcEventItem(Packet & pkt)
{
	uint16 sUid, sNid;
	uint32 nItemID, nCount;

	pkt >> sUid >> sNid >> nItemID >> nCount;

	CUser *pUser = g_pMain->GetUserPtr(sUid);
	if (pUser == nullptr)
		return;

	pUser->GiveItem(nItemID, nCount);
}

void CAISocket::RecvGateOpen(Packet & pkt)
{
	uint16 sNid, sEventID; 
	bool bFlag;

	pkt >> sNid >> sEventID >> bFlag;

	CNpc *pNpc = g_pMain->GetNpcPtr(sNid);
	if (pNpc == nullptr)	
	{
		TRACE("#### RecvGateOpen Npc Pointer null : nid=%d ####\n", sNid);
		return;
	}

	pNpc->m_byGateOpen = bFlag; // possibly not needed (we'll do it below), but need to make sure.

	_OBJECT_EVENT *pEvent = pNpc->GetMap()->GetObjectEvent(sEventID);
	if (pEvent == nullptr)	
	{
		TRACE("#### RecvGateOpen Npc Object fail : nid=%d, sid=%d ####\n", sNid, sEventID);
		return;
	}

	if (pNpc->isGate())
		pNpc->SendGateFlag(bFlag, false);
}

void CAISocket::RecvCompressed(Packet & pkt)
{
	uint32 compressedLength, originalLength;
	uint32 crc;
	pkt >> compressedLength >> originalLength >> crc;

	char *decompressedBuffer = new char[originalLength];

	// Does the length match what it's supposed to be
	uint32 result = lzf_decompress(pkt.contents() + pkt.rpos(), compressedLength, decompressedBuffer, originalLength);
	if (result
		!= originalLength)
	{
		delete [] decompressedBuffer;
		return;
	}

	pkt.Initialize(*decompressedBuffer);
	if (originalLength > 1)
		pkt.append(decompressedBuffer + 1, originalLength - 1);

	delete [] decompressedBuffer;

	HandlePacket(pkt);
}

void CAISocket::RecvNpcHpChange(Packet & pkt)
{
	Unit * pAttacker = nullptr;
	int16 nid, sAttackerID;
	int32 nHP, nAmount;

	pkt >> nid >> sAttackerID >> nHP >> nAmount;

	CNpc * pNpc = g_pMain->GetNpcPtr(nid);
	if (pNpc == nullptr)
		return;

	pAttacker = g_pMain->GetUnitPtr(sAttackerID);
	pNpc->HpChange(nAmount, pAttacker, false); 
}