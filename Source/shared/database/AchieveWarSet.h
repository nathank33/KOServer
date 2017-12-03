#pragma once

class CAchieveWarSet : public OdbcRecordset {
public:
	CAchieveWarSet(OdbcConnection * dbConnection, AchieveWarArray * pMap) : OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_WAR"); }
	virtual tstring GetColumns() { return _T("ID, Type, UnKnow2, Data, UnKnow4"); }

	virtual bool Fetch() {
		auto pData = new _ACHIEVE_WAR;

		auto i = 1;
		_dbCommand->FetchUInt32(i++, pData->ID);
		_dbCommand->FetchByte(i++, pData->Type);
		_dbCommand->FetchByte(i++, pData->UnKnow2);
		_dbCommand->FetchUInt32(i++, pData->Data);
		_dbCommand->FetchUInt32(i++, pData->UnKnow4);

		if (!m_pMap->PutData(pData->ID, pData))
			delete pData;

		return true;
	}

	AchieveWarArray * m_pMap;
};