#include "stdafx.h"
#include "../shared/Condition.h"
#include "KnightsManager.h"
#include "KingSystem.h"
#include "User.h"

static std::queue<Packet> _queue;
static uint16 _queuerId;
static bool _running = true;
static std::recursive_mutex _locker;
static std::vector<Thread *> s_threads;

static Condition s_hEvent;
static Thread * s_thread;

void CUser::Startup(uint32 dwThreads)
{
	for (unsigned long i = 0; i < dwThreads; i++)
		s_threads.push_back(new Thread(CUser::UserThreadProc, (void *)i));
}
bool CUser::HandlePacket(Packet & pktx)
{
	
	Packet pkt(uint8(1));
	uint8 command = pktx.GetOpcode();
	BYTE tmpbuf[4096];
	ZeroMemory(tmpbuf, 4096);

	if(pktx.size() < 4)
		return false;
	
	uint32 crc  = *(uint32*)(pktx.contents() + pktx.size() - 4);
	uint32 packetcrc = 0;

	for (uint16 i = 0; i < pktx.size() - 4; i++)
	{
		tmpbuf[i] = ((~(pktx.GetByte(i) ^ CRYPT_KEY / 8) ^ CRYPT_KEY * 5) ^ CRYPT_KEY * 22) ^ CRYPT_KEY;
		tmpbuf[i] ^= ~((i + 1) ^ ((i + 1) * CRYPT_KEY / 244));
		tmpbuf[i] ^= (m_pktcount * CRYPT_KEY);
		pkt << uint8(tmpbuf[i]);
		packetcrc += ((tmpbuf[i] ^ CRYPT_KEY) * 8);
	}

	command = ((~(command ^ CRYPT_KEY / 8) ^ CRYPT_KEY * 5) ^ CRYPT_KEY * 22) ^ CRYPT_KEY;
	command ^= (m_pktcount * CRYPT_KEY);

	pktx.SetOpcode(command);
	pkt.SetOpcode(command);

	if (crc != packetcrc && command == WIZ_LOGIN)
	{
		printf("crc1 : %p -- crc2 : %p opcode : %02X", crc, packetcrc, command);
		return false;
	}
	m_pktcount++;
	// crc çözümlüyor burda sonra threada yonlendiriyor ayýklayýp normal halde
	
	printf("pktsize: %d, pktcount:%d\n",pkt.size(),m_pktcount);
	Packet pks;
	pks << uint16(GetSocketID()) << uint8(command);
	
	printf("pkssize: %d, pktcount:%d\n",pks.size(),m_pktcount);
	
	if (pkt.size())
		pks.append(pkt.contents(), pkt.size());
	printf("pkssize: %d, pktcount:%d\n",pks.size(),m_pktcount);
	
	_lock.lock();
	_queue.push(pks);
	_lock.unlock();
	s_hEvent.Signal();
	return true;
}

uint32 THREADCALL CUser::UserThreadProc(void * lpParam)
{
	while (true)
	{
		Packet p;
		Packet *l = nullptr;
		uint16 uid = -1;
		uint8 command;
		// Pull the next packet from the shared queue
		_locker.lock();
		if (_queue.size())
		{
			p = _queue.front();
			_queue.pop();
			l = &p;
		}
		_locker.unlock();

		

		if (l == nullptr)
		{
			// If we're shutting down, don't bother waiting for more (there are no more).
			if (!_running)
				break;

			s_hEvent.Wait();
			continue;
		}

		

		
		//
		// References are fun =p
		Packet pkt = p;
		// Attempt to lookup the user if necessary
		CUser *pUser = nullptr;

		pkt >> uid >> command;
		if (uid >= 0)
		{
			pUser = g_pMain->GetUserPtr(uid);

			// Check to make sure they're still connected.
			if (pUser == nullptr)
					continue;
		}
		else
			continue;
		
	// If crypto's not been enabled yet, force the version packet to be sent.
	if (!pUser->isCryptoEnabled())
	{
		if (command == WIZ_VERSION_CHECK)
			pUser->VersionCheck(pkt);

	}
	// If we're not authed yet, forced us to before we can do anything else.
	// NOTE: We're checking the account ID store here because it's only set on successful auth,
	// at which time the other account ID will be cleared out (yes, it's messy -- need to clean it up).
	else if (pUser->m_strAccountID.empty())
	{
		if (command == WIZ_LOGIN)
			pUser->LoginProcess(pkt);

	}
	// If we haven't logged in yet, don't let us hit in-game packets.
	// TODO: Make sure we support all packets in the loading stage (and rewrite this logic considerably better).
	else if (!pUser->m_bSelectedCharacter)
	{
		switch (command)
		{
		case WIZ_SEL_NATION:
			pUser->SelNationToAgent(pkt);
			break;
		case WIZ_BRATE:
			pUser->BRATE(pkt);
			break;
		case WIZ_ALLCHAR_INFO_REQ:
			pUser->AllCharInfoToAgent();
			break;
		case WIZ_CHANGE_HAIR:
			pUser->ChangeHair(pkt);
			break;
		case WIZ_NEW_CHAR:
			pUser->NewCharToAgent(pkt);
			break;
		case WIZ_DEL_CHAR:
			pUser->DelCharToAgent(pkt);
			break;
		case WIZ_SEL_CHAR:
			pUser->SelCharToAgent(pkt);
			break;
		case WIZ_SPEEDHACK_CHECK:
			pUser->SpeedHackTime(pkt);
			break;

		default:
			printf("[SID=%d] Unhandled packet (%X) prior to selecting character\n", pUser->GetSocketID(), command);
			break;
		}
	}
	else if (pUser->m_bSelectedCharacter)
	{
	// Otherwise, assume we're authed & in-game.
	switch (command)
	{
	case WIZ_GAMESTART:
		pUser->GameStart(pkt);
		break;
	case WIZ_SERVER_INDEX:
		pUser->SendServerIndex();
		break;
	case WIZ_CHANGE_HAIR:
		pUser->ChangeHair(pkt);
		break;
	case WIZ_RENTAL:
		pUser->RentalSystem(pkt);
		break;
	case WIZ_SKILLDATA:
		pUser->SkillDataProcess(pkt);
		break;
	case WIZ_MOVE:
		pUser->MoveProcess(pkt);
		break;
	case WIZ_ROTATE:
		pUser->Rotate(pkt);
		break;
	case WIZ_ATTACK:
		pUser->Attack(pkt);
		break;
	case WIZ_CHAT:
		pUser->Chat(pkt);
		break;
	case WIZ_CHAT_TARGET:
		pUser->ChatTargetSelect(pkt);
		break;
	case WIZ_REGENE:	
		pUser->Regene(pkt.read<uint8>()); // respawn type
		break;
	case WIZ_REQ_USERIN:
		pUser->RequestUserIn(pkt);
		break;
	case WIZ_REQ_NPCIN:
		pUser->RequestNpcIn(pkt);
		break;
	case WIZ_WARP:
		if (pUser->isGM())
			pUser->RecvWarp(pkt);
		break;
	case WIZ_ITEM_MOVE:
		pUser->ItemMove(pkt);
		break;
	case WIZ_NPC_EVENT:
		pUser->NpcEvent(pkt);
		break;
	case WIZ_ITEM_TRADE:
		pUser->ItemTrade(pkt);
		break;
	case WIZ_TARGET_HP:
		{
			uint16 uid = pkt.read<uint16>();
			uint8 echo = pkt.read<uint8>();
			pUser->SetTargetID(uid);
			pUser->SendTargetHP(echo, uid);
		}
		break;
	case WIZ_BUNDLE_OPEN_REQ:
		pUser->BundleOpenReq(pkt);
		break;
	case WIZ_ITEM_GET:
		pUser->ItemGet(pkt);
		break;
	case WIZ_ZONE_CHANGE:
		pUser->RecvZoneChange(pkt);
		break;
	case WIZ_POINT_CHANGE:
		pUser->PointChange(pkt);
		break;
	case WIZ_STATE_CHANGE:
		pUser->StateChange(pkt);
		break;
	case WIZ_PARTY:
		pUser->PartyProcess(pkt);
		break;
	case WIZ_EXCHANGE:
		pUser->ExchangeProcess(pkt);
		break;
	case WIZ_QUEST:
		pUser->QuestV2PacketProcess(pkt);
		break;
	case WIZ_MERCHANT:
		pUser->MerchantProcess(pkt);
		break;
	case WIZ_MAGIC_PROCESS:
		CMagicProcess::MagicPacket(pkt, pUser);
		break;
	case WIZ_SKILLPT_CHANGE:
		pUser->SkillPointChange(pkt);
		break;
	case WIZ_OBJECT_EVENT:
		pUser->ObjectEvent(pkt);
		break;
	case WIZ_WEATHER:
	case WIZ_TIME:
		pUser->UpdateGameWeather(pkt);
		break;
	case WIZ_CLASS_CHANGE:
		pUser->ClassChange(pkt);
		break;
	case WIZ_CONCURRENTUSER:
		pUser->CountConcurrentUser();
		break;
	case WIZ_DATASAVE:
		pUser->UserDataSaveToAgent();
		break;
	case WIZ_ITEM_REPAIR:
		pUser->ItemRepair(pkt);
		break;
	case WIZ_KNIGHTS_PROCESS:
		CKnightsManager::PacketProcess(pUser, pkt);
		break;
	case WIZ_ITEM_REMOVE:
		pUser->ItemRemove(pkt);
		break;
	case WIZ_OPERATOR:
		pUser->OperatorCommand(pkt);
		break;
	case WIZ_SPEEDHACK_CHECK:
		pUser->SpeedHackTime(pkt);
		break;
	case WIZ_WAREHOUSE:
		pUser->WarehouseProcess(pkt);
		break;
	case WIZ_HOME:
		pUser->Home();
		break; 
	case WIZ_FRIEND_PROCESS:
		pUser->FriendProcess(pkt);
		break;
	case WIZ_WARP_LIST:
		pUser->SelectWarpList(pkt);
		break;
	case WIZ_VIRTUAL_SERVER:
		pUser->ServerChangeOk(pkt);
		break;
	case WIZ_PARTY_BBS:
		pUser->PartyBBS(pkt);
		break;
	case WIZ_MAP_EVENT:
		break;
	case WIZ_CLIENT_EVENT:
		pUser->ClientEvent(pkt.read<uint16>());
		break;
	case WIZ_SELECT_MSG:
		pUser->RecvSelectMsg(pkt);
		break;
	case WIZ_ITEM_UPGRADE:
		pUser->ItemUpgradeProcess(pkt);
		break;
	case WIZ_EVENT:
		pUser->TempleProcess(pkt);
		break;
	case WIZ_SHOPPING_MALL: // letter system's used in here too
		pUser->ShoppingMall(pkt);
		break;
	case WIZ_NAME_CHANGE:
		pUser->HandleNameChange(pkt);
		break;
	case WIZ_PACKET3:
		pUser->HandleCNameChange(pkt);
		break;
	case WIZ_KING:
		CKingSystem::PacketProcess(pUser, pkt);
		break;
	case WIZ_HELMET:
		pUser->HandleHelmet(pkt);
		break;
	case WIZ_CAPE:
		pUser->HandleCapeChange(pkt);
		break;
	case WIZ_CHALLENGE:
		pUser->HandleChallenge(pkt);
		break;
	case WIZ_RANK:
		pUser->HandlePlayerRankings(pkt);
		break;
	case WIZ_MINING:
		pUser->HandleMiningSystem(pkt);
		break;
	case WIZ_USER_INFO:
		pUser->HandleUserInfo(pkt);
		break;
	case WIZ_ACHIEVE:
		pUser->Achieve(pkt);
		break;
	case WIZ_SIEGE:
		pUser->SiegeWarFareNpc(pkt);
		break;
	case WIZ_LOGOSSHOUT:
		pUser->LogosShout(pkt);
		break;
	case WIZ_GENIE:
		pUser->HandleGenie(pkt);
		break;
	case WIZ_CAPTURE:
		pUser->HandleCapture(pkt);
		break;
	case WIZ_NATION_CHAT:
		pUser->ChatRoomHandle(pkt);
		break;
	default:
		TRACE("[SID=%d] Unknown packet %X\n", pUser->GetSocketID(), command);
		break;
	}
		pUser->Update();
	}
	

	pkt.clear();
	p.clear();
	uid = -1;
	
 }

	printf("[User Thread %d] Exiting...\n", lpParam);
	return 0;
}


void CUser::Shutdown()
{
	_running = false;

	// Wake them up in case they're sleeping.
	if (!s_threads.empty())
	{
		// Wake them up in case they're sleeping.
		s_hEvent.Broadcast();

		foreach (itr, s_threads)
		{
			(*itr)->waitForExit();
			delete (*itr);
		}

		s_threads.clear();
	}

	_locker.lock();
	while (_queue.size())
	{
		Packet p = _queue.front();
		_queue.pop();
		p.clear();
	}
	_locker.unlock();

}
