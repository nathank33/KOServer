#pragma once

class CAchieveNormalSet : public OdbcRecordset {
public:
	CAchieveNormalSet(OdbcConnection * dbConnection, AchieveNormalArray * pMap) : OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_NORMAL"); }
	virtual tstring GetColumns() { return _T("ID, UnKnow1, UnKnow2"); }

	virtual bool Fetch() {
		auto pData = new _ACHIEVE_NORMAL;

		auto i = 1;
		_dbCommand->FetchUInt32(i++, pData->ID);
		_dbCommand->FetchUInt16(i++, pData->UnKnow1);
		_dbCommand->FetchUInt32(i++, pData->UnKnow2);

		if (!m_pMap->PutData(pData->ID, pData))
			delete pData;

		return true;
	}

	AchieveNormalArray * m_pMap;
};