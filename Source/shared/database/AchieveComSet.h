#pragma once

class CAchieveComSet : public OdbcRecordset
{
public:
	CAchieveComSet(OdbcConnection * dbConnection, AchieveComArray * pMap) : OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_COM"); }
	virtual tstring GetColumns() { return _T("ID, UnKnow1, UnKnow2, UnKnow3"); }

	virtual bool Fetch()
	{
		auto pData = new _ACHIEVE_COM;

		auto i = 1;
		_dbCommand->FetchUInt32(i++, pData->ID);
		_dbCommand->FetchByte(i++, pData->UnKnow1);
		_dbCommand->FetchUInt32(i++, pData->UnKnow2);
		_dbCommand->FetchUInt32(i++, pData->UnKnow3);

		if (!m_pMap->PutData(pData->ID, pData))
			delete pData;

		return true;
	}

	AchieveComArray * m_pMap;
};