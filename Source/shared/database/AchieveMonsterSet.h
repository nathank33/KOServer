#pragma once

class CAchieveMonsterSet : public OdbcRecordset {
public:
	CAchieveMonsterSet(OdbcConnection * dbConnection, AchieveMonsterArray * pMap) : OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_MONSTER"); }
	virtual tstring GetColumns() { return _T("ID, Unknow2, Unknow3, Monster1A, Monster1B, Monster1C, Monster1D, Count1, Monster2A, Monster2B, Monster2C, Monster2D, Count2"); }

	virtual bool Fetch() {
		auto pData = new _ACHIEVE_MONSTER;

		auto i = 1;
		_dbCommand->FetchUInt32(i++, pData->ID);
		_dbCommand->FetchByte(i++, pData->Unknow2);
		_dbCommand->FetchByte(i++, pData->Unknow3);

		for (size_t ii = 0; ii < 2; ii++) {
			for (size_t iii = 0; iii < 4; iii++)
				_dbCommand->FetchUInt32(i++, pData->MonsterID[ii][iii]);

			_dbCommand->FetchUInt32(i++, pData->MonsterCount[ii]);
		}

		if (!m_pMap->PutData(pData->ID, pData))
			delete pData;

		return true;
	}

	AchieveMonsterArray * m_pMap;
};