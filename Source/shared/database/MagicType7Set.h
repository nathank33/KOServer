#pragma once

class CMagicType7Set : public OdbcRecordset {
public:
	CMagicType7Set(OdbcConnection * dbConnection, Magictype7Array * pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MAGIC_TYPE7"); }
	virtual tstring GetColumns() { return _T("nIndex, byValidGroup, byNatoinChange, shMonsterNum, byTargetChange, byStateChange, byRadius, shHitrate, shDuration, shDamage, nNeedItem"); }

	virtual bool Fetch() {
		_MAGIC_TYPE7 *pData = new _MAGIC_TYPE7;

		int index = 1;
		_dbCommand->FetchUInt32(index++, pData->iNum);
		_dbCommand->FetchByte(index++, pData->bValidGroup);
		_dbCommand->FetchByte(index++, pData->bNationChange);
		_dbCommand->FetchUInt16(index++, pData->sMonsterNum);
		_dbCommand->FetchByte(index++, pData->bTargetChange);
		_dbCommand->FetchByte(index++, pData->bStateChange);
		_dbCommand->FetchByte(index++, pData->bRadius);
		_dbCommand->FetchUInt16(index++, pData->sHitRate);
		_dbCommand->FetchUInt16(index++, pData->sDuration);
		_dbCommand->FetchUInt16(index++, pData->sDamage);
		_dbCommand->FetchUInt32(index++, pData->nNeedItem);

		if (!m_pMap->PutData(pData->iNum, pData))
			delete pData;

		return true;
	}

	Magictype7Array *m_pMap;
};