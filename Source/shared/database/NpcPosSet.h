#pragma once

class CNpcPosSet : public OdbcRecordset
{
public:
	CNpcPosSet(OdbcConnection * dbConnection, void * dummy) 
		: OdbcRecordset(dbConnection) {}

	virtual tstring GetTableName() { return _T("K_NPCPOS"); }
	virtual tstring GetColumns() { return _T("ZoneID, NpcID, strName, isNPC, ActType, DungeonFamily, SpecialType, TrapNumber, LeftX, TopZ, NumNPC, iRange, XiDontKnow, RegTime, byDirection, path, EventRoom"); }

	virtual bool Fetch() { return g_pMain->LoadSpawnCallback(_dbCommand); }
};