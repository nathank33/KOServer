#pragma once

class CItemCrashSet : public OdbcRecordset
{
public:
	CItemCrashSet(OdbcConnection * dbConnection, ItemCrashArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ITEM_CRASH"); }
	virtual tstring GetColumns() { return _T("nIndex,Flag,ItemID,ItemCount,SuccessRate"); }
	virtual tstring GetOrderBy() { return _T("DropRate"); }

	virtual bool Fetch()
	{
		_ITEM_CRASH * pData = new _ITEM_CRASH;

		int index = 1;
		_dbCommand->FetchUInt32(index++,pData->Index);
		_dbCommand->FetchByte(index++, pData->Flag);
		_dbCommand->FetchUInt32(index++, pData->ItemID);
		_dbCommand->FetchUInt16(index++, pData->ItemCount);
		_dbCommand->FetchUInt16(index++, pData->SuccessRate);

		if(!m_pMap->PutData(pData->Index,pData))
			delete pData;
		return true;
	}

	ItemCrashArray *m_pMap;
};