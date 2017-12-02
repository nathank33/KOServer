#pragma once

class CCypherTableSet : public OdbcRecordset
{
public:
	CCypherTableSet(OdbcConnection * dbConnection, CypherRingArray *pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("USERDATA_CYPHER"); }
	virtual tstring GetColumns() 
	{
		return _T("id, strUserId, strSerial, sClass, bLevel, iExp, bRace");
	}

	virtual bool Fetch()
	{
		int i = 1;
		_CYPHERRING_DATA * pData = new _CYPHERRING_DATA;
		_dbCommand->FetchUInt32(i++, pData->ID);
		_dbCommand->FetchString(i++, pData->UserName);
		_dbCommand->FetchUInt64(i++, pData->m_Serial);
		_dbCommand->FetchByte(i++, pData->sClass);
		_dbCommand->FetchByte(i++, pData->bLevel);
		_dbCommand->FetchUInt64(i++, pData->iExp);
		_dbCommand->FetchUInt16(i++, pData->bRace);

		if (pData->ID > g_pMain->LastPetID)
			g_pMain->LastPetID = pData->ID;
		
		if (pData == nullptr)
			delete pData;
		else
			m_pMap->insert(std::make_pair(pData->m_Serial, pData));


		return true;
	}

	CypherRingArray * m_pMap;
};