#pragma once

class CMonsterRespawnListRandomSet : public OdbcRecordset
{
public:
	CMonsterRespawnListRandomSet(OdbcConnection * dbConnection, MonsterRespawnListRandomArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MONSTER_RESPAWN_LIST_RANDOM"); }
	virtual tstring GetColumns() { return _T("sIndex, sSid, strName, sPid, ZoneID, isBoss, Family"); }

	virtual bool Fetch()
	{
		_MONSTER_RESPAWN_LIST_RANDOM * pData = new _MONSTER_RESPAWN_LIST_RANDOM;

		int i = 1;
		_dbCommand->FetchUInt16(i++, pData->sIndex);
		_dbCommand->FetchUInt16(i++, pData->sSid);
		_dbCommand->FetchString(i++, pData->strName);
		_dbCommand->FetchUInt16(i++, pData->sPid);
		_dbCommand->FetchByte(i++, pData->ZoneID);
		_dbCommand->FetchByte(i++, pData->isBoss);
		_dbCommand->FetchByte(i++, pData->Family);
		
		if (!m_pMap->PutData(pData->sIndex, pData))
			delete pData;

		return true;
	}

	MonsterRespawnListRandomArray *m_pMap;
};