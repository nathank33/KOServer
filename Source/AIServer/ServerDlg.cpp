#include "stdafx.h"
#include <time.h>
#include <cstdarg>
#include "GameSocket.h"
#include "Npc.h"
#include "User.h"
#include "NpcThread.h"
#include "../GameServer/MagicProcess.h"
#include <boost\foreach.hpp>
#include "../shared/database/OdbcRecordset.h"
#include "../shared/database/MagicTableSet.h"
#include "../shared/database/MagicType1Set.h"
#include "../shared/database/MagicType2Set.h"
#include "../shared/database/MagicType4Set.h"
#include "../shared/database/ObjectPosSet.h"
#include "../shared/database/NpcPosSet.h"
#include "../shared/database/ZoneInfoSet.h"
#include "../shared/database/NpcItemSet.h"
#include "../shared/database/MakeItemGroupSet.h"
#include "../shared/database/NpcTableSet.h"
#include "../shared/database/MakeWeaponTableSet.h"
#include "../shared/database/MakeDefensiveTableSet.h"
#include "../shared/database/MakeGradeItemTableSet.h"
#include "../shared/database/MakeLareItemTableSet.h"
#include "../shared/database/ServerResourceSet.h"
#include "Region.h"
#include "../shared/Ini.h"
#include "../shared/packets.h"
#include "../shared/DateTime.h"

using namespace std;

bool g_bNpcExit = false;
ZoneArray			g_arZone;

std::vector<Thread *> g_timerThreads;

CServerDlg::CServerDlg() {
	EventNpcID = 0;
	CSWOpen = false;
	m_iYear = 0;
	m_iMonth = 0;
	m_iDate = 0;
	m_iHour = 0;
	m_iMin = 0;
	m_iWeather = 0;
	m_iAmount = 0;
	m_bIsNight = false;
	m_byBattleEvent = BATTLEZONE_CLOSE;
	m_sKillKarusNpc = 0;
	m_sKillElmoNpc = 0;
	KarusBaseMilitaryCampCount = 0;
	ElmoradBaseMilitaryCampCount = 0;
	KarusEslantMilitaryCampCount = 0;
	ElmoradEslantMilitaryCampCount = 0;
	MoradonMilitaryCampCount = 0;

}

bool CServerDlg::Startup() {
	g_timerThreads.push_back(new Thread(Timer_CheckAliveTest));
	g_timerThreads.push_back(new Thread(Timer_CheckLiveTimes));

	m_sMapEventNpc = 0;
	m_bFirstServerFlag = false;

	// Server Start
	DateTime time;
	printf("Server started on %04d-%02d-%02d at %02d:%02d\n\n", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute());

	//----------------------------------------------------------------------
	//	DB part initialize
	//----------------------------------------------------------------------
	GetServerInfoIni();

	if (!m_GameDB.Connect(m_strGameDSN, m_strGameUID, m_strGamePWD, false)) {
		OdbcError *pError = m_GameDB.GetError();
		printf("ERROR: Could not connect to the database server, received error:\n%s\n",
			pError->ErrorMessage.c_str());
		delete pError;
		return false;
	}

	//----------------------------------------------------------------------
	//	Communication Part Initialize ...
	//----------------------------------------------------------------------
	if (!m_socketMgr.Listen(m_AIServerPort, MAX_SOCKET))
		return false;

	//----------------------------------------------------------------------
	//	Load tables
	//----------------------------------------------------------------------
	if (!GetMagicTableData()
		|| !GetMagicType1Data()
		|| !GetMagicType2Data()
		|| !GetMagicType4Data()
		|| !GetNpcItemTable()
		|| !GetMakeItemGroupTable()
		|| !GetMakeWeaponItemTableData()
		|| !GetMakeDefensiveItemTableData()
		|| !GetMakeGradeItemTableData()
		|| !GetMakeLareItemTableData()
		|| !GetServerResourceTable()
		|| !GetObjectPostTableData()
		|| !GetNpcTableData(false)
		|| !GetNpcTableData(true)
		// Load maps
		|| !MapFileLoad()
		// Spawn NPC threads
		|| !CreateNpcThread())
		return false;

	//----------------------------------------------------------------------
	//	Start NPC THREAD
	//----------------------------------------------------------------------
	ResumeAI();
	return true;
}

bool CServerDlg::GetMagicTableData() {
	LOAD_TABLE(CMagicTableSet, &m_GameDB, &m_MagictableArray, false, false);
}

bool CServerDlg::GetMagicType1Data() {
	LOAD_TABLE(CMagicType1Set, &m_GameDB, &m_Magictype1Array, false, false);
}

bool CServerDlg::GetMagicType2Data() {
	LOAD_TABLE(CMagicType2Set, &m_GameDB, &m_Magictype2Array, false, false);
}

bool CServerDlg::GetMagicType4Data() {
	LOAD_TABLE(CMagicType4Set, &m_GameDB, &m_Magictype4Array, false, false);
}

bool CServerDlg::GetMakeWeaponItemTableData() {
	LOAD_TABLE(CMakeWeaponTableSet, &m_GameDB, &m_MakeWeaponItemArray, true, false);
}

bool CServerDlg::GetMakeDefensiveItemTableData() {
	LOAD_TABLE(CMakeDefensiveTableSet, &m_GameDB, &m_MakeDefensiveItemArray, true, false);
}

bool CServerDlg::GetMakeGradeItemTableData() {
	LOAD_TABLE(CMakeGradeItemTableSet, &m_GameDB, &m_MakeGradeItemArray, false, false);
}

bool CServerDlg::GetMakeLareItemTableData() {
	LOAD_TABLE(CMakeLareItemTableSet, &m_GameDB, &m_MakeLareItemArray, false, false);
}

bool CServerDlg::GetServerResourceTable() {
	LOAD_TABLE(CServerResourceSet, &m_GameDB, &m_ServerResourceArray, false, false);
}

bool CServerDlg::GetNpcItemTable() {
	LOAD_TABLE(CNpcItemSet, &m_GameDB, &m_NpcItemArray, false, false);
}

bool CServerDlg::GetMakeItemGroupTable() {
	LOAD_TABLE(CMakeItemGroupSet, &m_GameDB, &m_MakeItemGroupArray, false, false);
}

bool CServerDlg::GetObjectPostTableData() {
	LOAD_TABLE(CObjectPosSet, &m_GameDB, &m_ObjectEventArray, false, false);
}

bool CServerDlg::GetNpcTableData(bool bNpcData /*= true*/) {
	if (bNpcData) { LOAD_TABLE(CNpcTableSet, &m_GameDB, &m_arNpcTable, false, false); } else { LOAD_TABLE(CMonTableSet, &m_GameDB, &m_arMonTable, false, false); }
}

bool CServerDlg::CreateNpcThread() {
	m_TotalNPC = m_sMapEventNpc;
	MaxMonsterID = m_sMapEventNpc;
	m_CurrentNPC = 0;

	LOAD_TABLE_ERROR_ONLY(CNpcPosSet, &m_GameDB, nullptr, false, false);

	Guard lock(m_npcThreadLock);
	foreach_stlmap_nolock(itr, g_arZone) {
		CNpcThread * pNpcThread = new CNpcThread();
		m_arNpcThread.insert(make_pair(itr->first, pNpcThread));

		foreach_stlmap(npcItr, m_arNpc) {
			if (npcItr->second->GetZoneID() != itr->first)
				continue;

			CNpc * pNpc = npcItr->second;
			if (pNpc != nullptr) {
				pNpc->Init();
				pNpcThread->m_pNpcs.insert(pNpc);
			}
		}
	}

	printf("Monster Init - %d, threads = %lld\n", (uint16) m_TotalNPC, (long long) m_arNpcThread.size());
	return true;
}


void CServerDlg::UserEventRoomUpdate(uint16 uid, uint16 RoomEvent) {
	CUser *TargetUser = g_pMain->GetUserPtr(uid);

	if (TargetUser == nullptr)
		return;

	TargetUser->SetUserEventRoom(RoomEvent);
	TargetUser->SetUnitEventRoom(RoomEvent);

}
bool CServerDlg::LoadSpawnCallback(OdbcCommand *dbCommand) {
	// Avoid allocating stack space for these.
	// This method will only ever run in the same thread.
	static int nRandom = 0;
	static double dbSpeed = 0;
	static CNpcTable * pNpcTable = nullptr;
	static CRoomEvent* pRoom = nullptr;
	static char szPath[500];
	static float fRandom_X = 0.0f, fRandom_Z = 0.0f;
	static uint8 rand = myrand(1, 4);
	// Unfortunately we cannot simply read what we need directly
	// into the CNpc instance. We have to resort to creating
	// copies of the data to allow for the way they handle multiple spawns...
	// Best we can do, I think, is to avoid allocating it on the stack.
	static uint8	bNumNpc, bZoneID, bActType, bRegenType, bDungeonFamily, bSpecialType,
		bTrapNumber, bDotCnt;
	static uint8 isNPC;
	static uint16	sSid, sRegTime, iRange, xiDontKnow, EveTRoom;
	static uint32	nServerNum;
	static int16	bDirection;
	static int32	iLeftX, iTopZ, iRightX, iBottomZ,
		iLimitMinX, iLimitMinZ, iLimitMaxX, iLimitMaxZ;


	string strName;
	dbCommand->FetchByte(1, bZoneID);
	dbCommand->FetchUInt16(2, sSid);
	dbCommand->FetchString(3, strName);
	dbCommand->FetchByte(4, isNPC);
	dbCommand->FetchByte(5, bActType);
	dbCommand->FetchByte(6, bDungeonFamily);
	dbCommand->FetchByte(7, bSpecialType);
	dbCommand->FetchByte(8, bTrapNumber);
	dbCommand->FetchInt32(9, iLeftX);
	dbCommand->FetchInt32(10, iTopZ);
	dbCommand->FetchByte(11, bNumNpc);
	dbCommand->FetchUInt16(12, iRange);
	dbCommand->FetchUInt16(13, xiDontKnow);// iRange
	dbCommand->FetchUInt16(14, sRegTime);
	dbCommand->FetchInt16(15, bDirection);
	dbCommand->FetchString(16, szPath, sizeof(szPath));
	dbCommand->FetchUInt16(17, EveTRoom);

	isNPC--;
	uint8 bPathSerial = 1;

	if ((bSpecialType == 7 && bTrapNumber == rand) || bSpecialType != 7)
		for (uint8 j = 0; j < bNumNpc; j++) {



			CNpc * pNpc = new CNpc();

			pNpc->m_byMoveType = bActType;// bActType;
			pNpc->m_byInitMoveType = bActType;

			bool isMonster = isNPC == 1 ? false : true;
			if (isMonster) {
				pNpcTable = m_arMonTable.GetData(sSid);
			} else {
				pNpc->m_byMoveType = bActType;
				pNpcTable = m_arNpcTable.GetData(sSid);
			}

			if (pNpcTable == nullptr) {
				printf("NPC %d not found in %s table.\n", sSid, isNPC ? "K_MONSTER" : "K_NPC");
				delete pNpc;
				return false;
			}
			uint16 myID = ++m_TotalNPC;
			++MaxMonsterID;
			pNpc->Load(myID, pNpcTable, !isNPC);

			pNpc->m_byBattlePos = 0;

			if (pNpc->m_byMoveType >= 2) {
				pNpc->m_byBattlePos = myrand(1, 3);

				if (pNpc->m_byMoveType == 5 || pNpc->m_byMoveType == 4)
					pNpc->m_byBattlePos = 0;

				pNpc->m_byPathCount = bPathSerial++;
			}

			pNpc->InitPos();

			pNpc->m_bZone = bZoneID;

			nRandom = iRange;
			if (nRandom <= 0)
				fRandom_X = (float) iLeftX;
			else
				fRandom_X = (float) (myrand((iLeftX - nRandom) * 10, (iLeftX + nRandom) * 10) / 10);

			nRandom = iRange;
			if (nRandom <= 0)
				fRandom_Z = (float) iTopZ;
			else
				fRandom_Z = (float) (myrand((iTopZ - nRandom) * 10, (iTopZ + nRandom) * 10) / 10);

			pNpc->SetPosition(fRandom_X, 0.0f, fRandom_Z);

			pNpc->m_sRegenTime = sRegTime * SECOND;
			pNpc->m_byDirection = bDirection;
			pNpc->m_sMaxPathCount = strlen(szPath) / 8;

			if (pNpc->m_byMoveType == 2 && pNpc->m_sMaxPathCount == 0) {
				pNpc->m_byMoveType = 1;
				TRACE("##### ServerDlg:CreateNpcThread - Path type Error :  nid=%d, sid=%d, name=%s, acttype=%d, path=%d #####\n",
					pNpc->GetID(), pNpc->GetProtoID(), pNpc->GetName().c_str(), pNpc->m_byMoveType, pNpc->m_sMaxPathCount);
			}

			if (pNpc->m_sMaxPathCount > 0) {
				int index = 0;
				__Vector3 vStart;
				vStart.Set(pNpc->GetX(), 0, pNpc->GetZ());
				for (int l = 0; l < pNpc->m_sMaxPathCount; l++) {

					if (l > 5000)
						break;

					static char szX[5], szZ[5];

					memset(szX, 0, sizeof(szX));
					memset(szZ, 0, sizeof(szZ));

					memcpy(szX, szPath + index, 4);
					index += 4;

					memcpy(szZ, szPath + index, 4);
					index += 4;

					float myX = 0.0f, myZ = 0.0f;
					__Vector3 vEnd, vNewPos;
					float fDis = 0.0f;
					myX = (float) atoi(szX);
					myZ = (float) atoi(szZ);

					if (myX < 1 || myZ < 1)
						continue;

					if (l > 0)
						vStart.Set(pNpc->m_PathList.pPattenPos[l - 1].x, 0, pNpc->m_PathList.pPattenPos[l - 1].z);

					vEnd.Set(myX, 0, myZ);

					fDis = pNpc->GetDistance(vStart, vEnd);


					if (fDis > pNpc->GetProto()->m_bySpeed_1) {
						for (;;) {
							pNpc->GetVectorPosition(vStart, vEnd, pNpc->GetProto()->m_bySpeed_1, &vNewPos);
							//printf("%d,%d-%d,%d/(%d,%d)\n",uint16(vStart.x),uint16(vStart.z),uint16(myX),uint16(myZ),uint16(vNewPos.x),uint16(vNewPos.z));

							pNpc->m_PathList.pPattenPos[l].x = (short) vNewPos.x;
							pNpc->m_PathList.pPattenPos[l].z = (short) vNewPos.z;
							vStart.Set(vNewPos.x, 0, vNewPos.z);
							fDis = pNpc->GetDistance(vStart, vEnd);
							if (fDis > pNpc->GetProto()->m_bySpeed_1) {
								l++;
								pNpc->m_sMaxPathCount++;
							} else {
								l++;
								pNpc->m_sMaxPathCount++;
								pNpc->m_PathList.pPattenPos[l].x = (short) vEnd.x;
								pNpc->m_PathList.pPattenPos[l].z = (short) vEnd.z;
								//printf("%d,%d-%d,%d/(%d,%d)\n",uint16(vStart.x),uint16(vStart.z),uint16(myX),uint16(myZ),uint16(vEnd.x),uint16(vEnd.z));
								break;
							}
						}
					} else {
						pNpc->m_PathList.pPattenPos[l].x = (short) myX;
						pNpc->m_PathList.pPattenPos[l].z = (short) myZ;
					}


				}
			}

			pNpc->m_nInitMinX = pNpc->m_nLimitMinX = iLeftX - nRandom;
			pNpc->m_nInitMinY = pNpc->m_nLimitMinZ = iTopZ - nRandom;
			pNpc->m_nInitMaxX = pNpc->m_nLimitMaxX = iLeftX + nRandom;
			pNpc->m_nInitMaxY = pNpc->m_nLimitMaxZ = iTopZ + nRandom;

			// dungeon work
			pNpc->m_byDungeonFamily = bDungeonFamily;
			pNpc->m_bySpecialType = (NpcSpecialType) bSpecialType;
			pNpc->m_byRegenType = bRegenType;
			pNpc->m_byTrapNumber = bTrapNumber;

			pNpc->m_oSocketID = -1;
			pNpc->m_bEventRoom = 0;
			pNpc->SetNPCEventRoom(0);
			pNpc->SetUnitEventRoom(0);

			pNpc->m_pMap = GetZoneByID(pNpc->GetZoneID());
			if (pNpc->GetMap() == nullptr) {
				printf(_T("ERROR: NPC %d in zone %d that does not exist."), sSid, bZoneID);
				delete pNpc;
				return false;
			}

			if (!m_arNpc.PutData(pNpc->GetID(), pNpc)) {
				--m_TotalNPC;
				TRACE("Npc PutData Fail - %d\n", pNpc->GetID());
				delete pNpc;
				continue;
			}

			if (pNpc->GetMap()->m_byRoomEvent > 0 && pNpc->m_byDungeonFamily > 0) {
				pRoom = pNpc->GetMap()->m_arRoomEventArray.GetData(pNpc->m_byDungeonFamily);
				if (pRoom == nullptr) {
					printf("ERROR: Map Room Npc Fail!!\n");
					printf("### Map - Room Array MonsterNid Fail : nid=%d, sid=%d zid= %d###\n",
						pNpc->GetID(), pNpc->GetProtoID(), pNpc->GetZoneID());
					delete pNpc;
					return false;
				}

				// this is why their CSTLMap class sucks.
				int *pInt = new int;
				*pInt = pNpc->GetID();
				if (!pRoom->m_mapRoomNpcArray.PutData(*pInt, pInt)) {
					delete pInt;
					TRACE("### Map - Room Array MonsterNid Fail : nid=%d, sid=%d ###\n",
						pNpc->GetID(), pNpc->GetProtoID());
				}
			}

		}
	return true;
}
void CServerDlg::ResumeAI() {
	Guard lock(m_npcThreadLock);
	foreach(itr, m_arNpcThread)
		itr->second->m_thread.start(NpcThreadProc, itr->second);

	m_zoneEventThread.start(ZoneEventThreadProc, this);
}

bool CServerDlg::MapFileLoad() {
	ZoneInfoMap zoneMap;

	m_sTotalMap = 0;
	LOAD_TABLE_ERROR_ONLY(CZoneInfoSet, &m_GameDB, &zoneMap, false, false);

	foreach(itr, zoneMap) {
		_ZONE_INFO *pZone = itr->second;

		MAP *pMap = new MAP();
		if (!pMap->Initialize(pZone)) {
			printf("ERROR: Unable to load SMD - %s\n", pZone->m_MapName.c_str());
			delete pZone;
			delete pMap;
			g_arZone.DeleteAllData();
			m_sTotalMap = 0;
			return false;
		}

		delete pZone;
		g_arZone.PutData(pMap->m_nZoneNumber, pMap);
		m_sTotalMap++;
	}

	return true;
}

/**
* @brief	Gets & formats a cached server resource (_SERVER_RESOURCE entry).
*
* @param	nResourceID	Identifier for the resource.
* @param	result	   	The string to store the formatted result in.
*/
void CServerDlg::GetServerResource(int nResourceID, string * result, ...) {
	_SERVER_RESOURCE *pResource = m_ServerResourceArray.GetData(nResourceID);
	if (pResource == nullptr) {
		*result = nResourceID;
		return;
	}

	va_list args;
	va_start(args, result);
	_string_format(pResource->strResource, result, args);
	va_end(args);
}

// game server에 모든 npc정보를 전송..
void CServerDlg::AllNpcInfo() {
	Packet result(NPC_INFO_ALL);
	result.SByte();
	foreach_stlmap_nolock(itr, g_arZone) {
		uint32 nZone = itr->first;
		uint8 bCount = 0;

		result.clear();
		result << bCount;

		foreach_stlmap(itr, m_arNpc) {
			CNpc *pNpc = itr->second;
			if (pNpc == nullptr
				|| pNpc->GetZoneID() != nZone)
				continue;

			pNpc->FillNpcInfo(result);
			if (++bCount == NPC_NUM) {
				result.put(0, bCount);
				m_socketMgr.SendAllCompressed(&result);

				// Reset packet buffer
				bCount = 0;
				result.clear();
				result << bCount;
			}
		}

		if (bCount != 0 && bCount < NPC_NUM) {
			result.put(0, bCount);
			m_socketMgr.SendAllCompressed(&result);
		}

		Packet serverInfo(AG_SERVER_INFO, uint8(nZone));
		serverInfo << uint16(m_TotalNPC);
		m_socketMgr.SendAll(&serverInfo);
	}
}

Unit * CServerDlg::GetUnitPtr(uint16 id) {
	if (id < NPC_BAND)
		return GetUserPtr(id);

	return GetNpcPtr(id);
}

CNpc * CServerDlg::GetNpcPtr(uint16 npcId) {
	return m_arNpc.GetData(npcId);
}

CUser* CServerDlg::GetUserPtr(uint16 sessionId) {
	Guard lock(m_userLock);
	auto itr = m_pUser.find(sessionId);
	if (itr == m_pUser.end())
		return nullptr;

	return itr->second;
}

bool CServerDlg::SetUserPtr(uint16 sessionId, CUser * pUser) {
	if (sessionId >= MAX_USER)
		return false;

	Guard lock(m_userLock);
	auto itr = m_pUser.find(sessionId);
	if (itr != m_pUser.end()) {
		TRACE("Warning: User %u has not been removed from the session map.\n", sessionId);
		return false;
	}

	m_pUser[sessionId] = pUser;
	return true;
}

void CServerDlg::DeleteUserPtr(uint16 sessionId) {
	Guard lock(m_userLock);
	auto itr = m_pUser.find(sessionId);
	if (itr != m_pUser.end()) {
		delete itr->second;
		m_pUser.erase(itr);
	}
}

void CServerDlg::CheckAliveTest() {
	Packet result(AG_CHECK_ALIVE_REQ);
	SessionMap sessMap = m_socketMgr.GetActiveSessionMap();
	uint32 count = 0, sessCount = sessMap.size();
	BOOST_FOREACH(auto itr, sessMap) {
		if (itr.second->Send(&result))
			count++;
	}

	if (sessCount > 0 && count == 0)
		DeleteAllUserList();
	//printf("Delete all user list???");
}

uint32 THREADCALL CServerDlg::Timer_CheckAliveTest(void * lpParam) {
	while (g_bRunning) {
		g_pMain->CheckAliveTest();
		sleep(10 * SECOND);
	}
	return 0;
}

uint32 THREADCALL CServerDlg::Timer_CheckLiveTimes(void * lpParam) {
	while (g_bRunning) {
		g_pMain->CheckLiveTimes();
		sleep(1 * SECOND);
	}
	return 0;
}

void CServerDlg::CheckLiveTimes() {
	std::vector<uint16> deleted;

	foreach_stlmap_nolock(itr, m_NpcLiveTimeArray) {
		if (int32(UNIXTIME) - itr->second->SpawnedTime > itr->second->Duration) {
			CNpc *pNpc = GetNpcPtr(itr->second->Nid);

			if (pNpc != nullptr)
				pNpc->Dead();

			deleted.push_back(itr->second->nIndex);
		}
	}

	foreach(itr, deleted)
		m_NpcLiveTimeArray.DeleteData(*itr);
}

void CServerDlg::DeleteAllUserList(CGameSocket *pSock) {
	// If a cconnected, show it...
	if (pSock != nullptr) {
		printf("Game Server disconnected - %s\n", pSock->GetRemoteIP().c_str());
		return;
	}

	// Server didn't disconnect? 
	if (!m_bFirstServerFlag)
		return;

	// If there's no servers even connected, cleanup.
	TRACE("*** DeleteAllUserList - Start *** \n");
	foreach_stlmap_nolock(itr, g_arZone) {
		MAP * pMap = itr->second;
		if (pMap == nullptr)
			continue;
		for (int i = 0; i < pMap->GetXRegionMax(); i++) {
			for (int j = 0; j < pMap->GetZRegionMax(); j++)
				pMap->m_ppRegion[i][j].m_RegionUserArray.DeleteAllData();
		}
	}

	Guard lock(m_userLock);
	foreach(itr, m_pUser) {
		if (itr->second == nullptr)
			continue;

		delete itr->second;
	}
	m_pUser.clear();

	// Party Array Delete 
	m_arParty.DeleteAllData();

	m_bFirstServerFlag = false;
	TRACE("*** DeleteAllUserList - End *** \n");

	printf("[ DELETE All User List ]\n");
}

void CServerDlg::Send(Packet * pkt) {
	m_socketMgr.SendAll(pkt);
}

void CServerDlg::GameServerAcceptThread() {
	m_socketMgr.RunServer();
}

bool CServerDlg::AddObjectEventNpc(_OBJECT_EVENT* pEvent, MAP * pMap) {
	int sSid = 0;

	if (pEvent->sType == OBJECT_GATE
		|| pEvent->sType == OBJECT_GATE2
		|| pEvent->sType == OBJECT_GATE_LEVER
		|| pEvent->sType == OBJECT_ANVIL
		|| pEvent->sType == OBJECT_ARTIFACT)
		sSid = pEvent->sIndex;
	else
		sSid = pEvent->sControlNpcID;

	if (sSid <= 0)
		return false;

	CNpcTable * pNpcTable = m_arNpcTable.GetData(sSid);
	if (pNpcTable == nullptr) {
		//printf("#### AddObjectEventNpc Fail : [sid = %d], zone=%d #####\n", pEvent->sIndex, zone_number);
		return false;
	}

	CNpc *pNpc = new CNpc();

	pNpc->m_byMoveType = 0;
	pNpc->m_byInitMoveType = 0;

	pNpc->m_byBattlePos = 0;

	pNpc->m_byObjectType = SPECIAL_OBJECT;
	pNpc->m_byGateOpen = (pEvent->sStatus == 1);

	pNpc->m_bZone = pMap->m_nZoneNumber;
	pNpc->SetPosition(pEvent->fPosX, pEvent->fPosY, pEvent->fPosZ);

	pNpc->m_nInitMinX = (int) pEvent->fPosX - 1;
	pNpc->m_nInitMinY = (int) pEvent->fPosZ - 1;
	pNpc->m_nInitMaxX = (int) pEvent->fPosX + 1;
	pNpc->m_nInitMaxY = (int) pEvent->fPosZ + 1;

	pNpc->Load(m_sMapEventNpc++, pNpcTable, false);
	pNpc->m_pMap = pMap;
	pNpc->SetUnitEventRoom(pEvent->RoomEvent);
	pNpc->SetNPCEventRoom(pEvent->RoomEvent);

	if (pNpc->GetMap() == nullptr
		|| !m_arNpc.PutData(pNpc->GetID(), pNpc)) {
		m_sMapEventNpc--;
		printf("Npc PutData Fail - %d\n", pNpc->GetProtoID());
		delete pNpc;
		return false;
	}

	m_TotalNPC = m_sMapEventNpc;
	return true;
}

CNpc * CServerDlg::SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ, uint16 Radius, uint16 sDuration, uint8 nation, int16 socketID, uint16 nEventRoom, bool nIsPet, std::string strPetName, std::string strUserName, uint64 nSerial, uint16 UserId /* = -1*/) {
	static float fRandom_X = 0.0f, fRandom_Z = 0.0f;
	CNpcTable * proto = nullptr;
	MAP * pZone = GetZoneByID(byZone);

	if (pZone == nullptr)
		return nullptr;

	if (bIsMonster)
		proto = m_arMonTable.GetData(sSid);
	else
		proto = m_arNpcTable.GetData(sSid);

	if (proto == nullptr)
		return nullptr;

	Guard lock(m_npcThreadLock);
	auto itr = m_arNpcThread.find(byZone);
	if (itr == m_arNpcThread.end())
		return false;

	CNpc * pNpc = new CNpc();

	pNpc->nIsPet = nIsPet;
	pNpc->strUserName = strUserName;
	pNpc->strPetName = strPetName;
	pNpc->nSerial = nSerial;
	pNpc->UserId = UserId;
	pNpc->m_bIsEventNpc = true;
	pNpc->m_byMoveType = (bIsMonster ? 1 : 0);
	pNpc->m_byInitMoveType = pNpc->m_byMoveType;



	pNpc->m_bZone = byZone;



	uint16 nRandom = Radius;
	if (nRandom <= 0)
		fRandom_X = (float) fX;
	else
		fRandom_X = (float) (myrand((fX - nRandom) * 10, (fX + nRandom) * 10) / 10);

	nRandom = Radius;
	if (nRandom <= 0)
		fRandom_Z = (float) fZ;
	else
		fRandom_Z = (float) (myrand((fZ - nRandom) * 10, (fZ + nRandom) * 10) / 10);

	pNpc->SetPosition(fRandom_X, 0.0f, fRandom_Z);

	pNpc->m_pMap = pZone;
	pNpc->m_oSocketID = socketID;
	pNpc->m_bEventRoom = nEventRoom;
	pNpc->SetNPCEventRoom(nEventRoom);
	pNpc->SetUnitEventRoom(nEventRoom);

	uint16 myID = ++m_TotalNPC;
	Guard lock2(m_freeIdsLock);
	if (freeIDs.size() > 0) {
		myID = freeIDs.back();
		freeIDs.pop_back();

		myID -= NPC_BAND;
	} else
		myID = ++MaxMonsterID;

	pNpc->Load(myID, proto, bIsMonster, nation);
	pNpc->m_byBattlePos = 0;

	if (pNpc->GetProtoID() != 7032
		&& pNpc->GetProtoID() != 7033
		&& pNpc->GetProtoID() != 7034
		&& pNpc->GetProtoID() != 8110)
		pNpc->InitPos();


	if (pNpc->GetZoneID() == ZONE_STONE1) {
		if ((pNpc->GetSPosX() / 10) == 126 && pNpc->GetProtoID() == 7032) {
			pNpc->SetPosition(126.2194f, -0.34175f, 209.0543f);
			pNpc->m_byMoveType = 4;
			pNpc->m_sMaxPathCount = 0;
		}

		if ((pNpc->GetSPosX() / 10) == 97 && pNpc->GetProtoID() == 7032) {
			pNpc->SetPosition(97.34771f, -0.13837f, 128.5445f);
			pNpc->m_byMoveType = 4;
			pNpc->m_sMaxPathCount = 0;
		}

	}

	if (pNpc->GetZoneID() == ZONE_STONE2) {
		if ((pNpc->GetSPosX() / 10) == 128 && pNpc->GetProtoID() == 7033) {
			pNpc->SetPosition(128.124f, -0.468628f, 208.7566f);
			pNpc->m_byMoveType = 4;
			pNpc->m_sMaxPathCount = 0;
		}

		if ((pNpc->GetSPosX() / 10) == 152 && pNpc->GetProtoID() == 7033) {
			pNpc->SetPosition(152.245f, 0.216793f, 123.7196f);
			pNpc->m_byMoveType = 4;
			pNpc->m_sMaxPathCount = 0;
		}

		if ((pNpc->GetSPosX() / 10) == 99 && pNpc->GetProtoID() == 7033) {
			pNpc->SetPosition(99.81824f, 7.528255f, 46.26767f);
			pNpc->m_byMoveType = 4;
			pNpc->m_sMaxPathCount = 0;
		}
	}


	if (pNpc->GetZoneID() == ZONE_STONE3) {
		if ((pNpc->GetSPosX() / 10) == 131 && pNpc->GetProtoID() == 7034) {
			pNpc->SetPosition(131.124f, -0.440191f, 208.4271f);
			pNpc->m_byMoveType = 4;
			pNpc->m_sMaxPathCount = 0;
		}

		if ((pNpc->GetSPosX() / 10) == 144 && pNpc->GetProtoID() == 7034) {
			pNpc->SetPosition(144.4414f, 0.366179f, 121.3319f);
			pNpc->m_byMoveType = 4;
			pNpc->m_sMaxPathCount = 0;
		}

		if ((pNpc->GetSPosX() / 10) == 100 && pNpc->GetProtoID() == 7034) {
			pNpc->SetPosition(100.9698f, 1.411695f, 46.68652f);
			pNpc->m_byMoveType = 4;
			pNpc->m_sMaxPathCount = 0;
		}

	}


	if (pNpc->GetZoneID() == ZONE_JURAD_MOUNTAIN) {
		if (pNpc->GetProtoID() == 8110) {
			pNpc->m_byMoveType = 4;
			pNpc->m_sMaxPathCount = 0;

		}
		if ((pNpc->GetSPosX() / 10) == 512 && (pNpc->GetSPosZ() / 10) == 256 && pNpc->GetProtoID() == 8110)
			pNpc->SetPosition(512.10f, 18.80f, 256.40f);

		if ((pNpc->GetSPosX() / 10) == 512 && (pNpc->GetSPosZ() / 10) == 767 && pNpc->GetProtoID() == 8110)
			pNpc->SetPosition(512.10f, 18.80f, 767.10f);

		if ((pNpc->GetSPosX() / 10) == 715 && pNpc->GetProtoID() == 8110)
			pNpc->SetPosition(715.30f, 18.80f, 172.00f);

		if ((pNpc->GetSPosX() / 10) == 799 && pNpc->GetProtoID() == 8110)
			pNpc->SetPosition(799.80f, 18.80f, 375.60f);

		if ((pNpc->GetSPosX() / 10) == 308 && pNpc->GetProtoID() == 8110)
			pNpc->SetPosition(308.50f, 18.80f, 847.80f);

		if ((pNpc->GetSPosX() / 10) == 224 && pNpc->GetProtoID() == 8110)
			pNpc->SetPosition(224.00f, 18.80f, 644.90f);
	}

	pNpc->Init();
	m_arNpc.PutData(pNpc->GetID(), pNpc);
	itr->second->AddNPC(pNpc);

	pNpc->m_nInitMinX = pNpc->m_nLimitMinX = (int) fX - nRandom;
	pNpc->m_nInitMinY = pNpc->m_nLimitMinZ = (int) fZ - nRandom;
	pNpc->m_nInitMaxX = pNpc->m_nLimitMaxX = (int) fX + nRandom;
	pNpc->m_nInitMaxY = pNpc->m_nLimitMaxZ = (int) fZ + nRandom;


	if (sDuration > 0) // Duration npc or monsters
	{
		_NPC_LIVE_TIME * pData = new _NPC_LIVE_TIME();
		pData->nIndex = EventNpcID++;
		pData->SocketID = socketID;
		pData->Nid = pNpc->m_sNid;
		pData->Duration = sDuration;
		pData->SpawnedTime = int32(UNIXTIME);
		if (!m_NpcLiveTimeArray.PutData(pData->nIndex, pData))
			delete pData;
	}

	return pNpc;
}


void CServerDlg::NpcUpdate(uint16 sSid, bool bIsMonster, uint8 byGroup, uint16 sPid) {
	CNpcTable * proto = nullptr;

	if (bIsMonster)
		proto = m_arMonTable.GetData(sSid);
	else
		proto = m_arNpcTable.GetData(sSid);

	if (proto == nullptr)
		return;

	if (byGroup > 0)
		proto->m_byGroupSpecial = byGroup;

	if (sPid > 0)
		proto->m_sPid = sPid;
}

MAP * CServerDlg::GetZoneByID(int zonenumber) {
	return g_arZone.GetData(zonenumber);
}

void CServerDlg::GetServerInfoIni() {
	CIni ini("./AIServer.ini");
	ini.GetString("ODBC", "GAME_DSN", "KO_GAME", m_strGameDSN, false);
	ini.GetString("ODBC", "GAME_UID", "username", m_strGameUID, false);
	ini.GetString("ODBC", "GAME_PWD", "password", m_strGamePWD, false);

	CIni ini2(CONF_GAME_SERVER);
	KarusBaseMilitaryCampCount = ini2.GetInt("MILITARY_CAMP", "ZONE_KARUS_BASE", 0);
	if (KarusBaseMilitaryCampCount > 3)
		KarusBaseMilitaryCampCount = 3;

	ElmoradBaseMilitaryCampCount = ini2.GetInt("MILITARY_CAMP", "ZONE_ELMORAD_BASE", 0);
	if (ElmoradBaseMilitaryCampCount > 3)
		ElmoradBaseMilitaryCampCount = 3;

	KarusEslantMilitaryCampCount = ini2.GetInt("MILITARY_CAMP", "ZONE_KARUS_ESLANT", 0);
	if (KarusEslantMilitaryCampCount > 3)
		KarusEslantMilitaryCampCount = 3;

	ElmoradEslantMilitaryCampCount = ini2.GetInt("MILITARY_CAMP", "ZONE_ELMORAD_ESLANT", 0);
	if (ElmoradEslantMilitaryCampCount > 3)
		ElmoradEslantMilitaryCampCount = 3;

	MoradonMilitaryCampCount = ini2.GetInt("MILITARY_CAMP", "ZONE_MORADON", 0);
	if (MoradonMilitaryCampCount > 5)
		MoradonMilitaryCampCount = 5;

	m_AIServerPort = ini.GetInt("SETTINGS", "PORT", 10020);
}

void CServerDlg::SendSystemMsg(std::string & pMsg, int type) {
	Packet result(AG_SYSTEM_MSG, uint8(type));
	result << pMsg;
	Send(&result);
}

void CServerDlg::ResetBattleZone() {
	TRACE("ServerDlg - ResetBattleZone() : start \n");
	foreach_stlmap_nolock(itr, g_arZone) {
		MAP *pMap = itr->second;
		if (pMap == nullptr || pMap->m_byRoomEvent == 0)
			continue;
		//if( pMap->IsRoomStatusCheck() == true )	continue;	// 전체방이 클리어 되었다면
		pMap->InitializeRoom();
	}
	TRACE("ServerDlg - ResetBattleZone() : end \n");
}

CServerDlg::~CServerDlg() {
	g_bNpcExit = true;

	printf("Waiting for NPC threads to exit...");

	Guard lock(m_npcThreadLock);
	foreach(itr, m_arNpcThread) {
		CNpcThread * pThread = itr->second;
		pThread->m_thread.waitForExit();
		delete pThread;
	}
	m_arNpcThread.clear();

	printf(" exited.\n");

	printf("Waiting for zone event thread to exit...");
	m_zoneEventThread.waitForExit();
	printf(" exited.\n");

	printf("Waiting for timer threads to exit...");
	foreach(itr, g_timerThreads) {
		(*itr)->waitForExit();
		delete (*itr);
	}
	printf(" exited.\n");

	printf("Freeing user sessions...");
	for (int i = 0; i < MAX_USER; i++) {
		if (m_pUser[i] != nullptr) {
			delete m_pUser[i];
			m_pUser[i] = nullptr;
		}
	}
	printf(" done.\n");

	m_ZoneNpcList.clear();

	printf("Shutting down socket system...");
	m_socketMgr.Shutdown();
	printf(" done.\n");
}