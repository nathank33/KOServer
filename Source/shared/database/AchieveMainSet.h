#pragma once

class CAchieveMainSet : public OdbcRecordset
{
public:
	CAchieveMainSet(OdbcConnection * dbConnection, AchieveMainArray * pMap) : OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_MAIN"); }
	virtual tstring GetColumns() { return _T("ID, Type, TitleID, Point, ItemID, Count, ZoneID, UnKnow7, TabIndex, Time, Name, Description, PropertiesType, SubTabIndex"); }

	virtual bool Fetch()
	{
		auto pData = new _ACHIEVE_MAIN;

		auto i = 1;
		_dbCommand->FetchUInt32(i++, pData->ID);
		_dbCommand->FetchByte(i++, pData->Type);
		_dbCommand->FetchUInt16(i++, pData->TitleID);
		_dbCommand->FetchUInt16(i++, pData->Point);
		_dbCommand->FetchUInt32(i++, pData->ItemID);
		_dbCommand->FetchUInt32(i++, pData->Count);
		_dbCommand->FetchByte(i++, pData->ZoneID);
		_dbCommand->FetchByte(i++, pData->UnKnow7);
		_dbCommand->FetchByte(i++, pData->TabIndex);
		_dbCommand->FetchUInt16(i++, pData->Time);
		_dbCommand->FetchString(i++, pData->Name);
		_dbCommand->FetchString(i++, pData->Description);
		_dbCommand->FetchByte(i++, pData->PropertiesType);
		_dbCommand->FetchByte(i++, pData->SubTabIndex);
				
		if (!m_pMap->PutData(pData->ID, pData))
			delete pData;


		return true;
	}

	AchieveMainArray * m_pMap;
};