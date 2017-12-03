#pragma once

class CPetTableSet : public OdbcRecordset {
public:
	CPetTableSet(OdbcConnection * dbConnection, PetArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("PET_DATA"); }
	virtual tstring GetColumns() {
		return _T("PET_SERIAL, PET_NAME, PET_EXP, PET_LEVEL, PET_STOMACH, PET_SLOT, PET_ITEMID, PET_CLASS, PET_ID");
	}

	virtual bool Fetch() {
		CPet * pData = new CPet();
		int i = 1;
		uint32 ItemId;
		uint8 Slot;
		_dbCommand->FetchUInt64(i++, pData->m_Serial);
		_dbCommand->FetchString(i++, pData->m_strPetID);
		_dbCommand->FetchUInt64(i++, pData->m_iExp);
		_dbCommand->FetchByte(i++, pData->m_bLevel);
		_dbCommand->FetchUInt16(i++, pData->m_sSatisfaction);
		_dbCommand->FetchByte(i++, Slot);
		_dbCommand->FetchUInt32(i++, ItemId);
		_ITEM_DATA * pItem = &pData->m_sItemArray[Slot];
		pItem->nNum = ItemId;
		pItem->sCount = 1;
		pItem->sDuration = 1;
		_dbCommand->FetchByte(i++, pData->m_sClass);
		_dbCommand->FetchUInt32(i++, pData->SpecialPetID);
		if (pData->SpecialPetID > g_pMain->LastPetID)
			g_pMain->LastPetID = pData->SpecialPetID;
		pData->m_pNpc = nullptr;

		if (pData == nullptr)
			delete pData;
		else
			m_pMap->insert(std::make_pair(pData->m_Serial, pData));


		return true;
	}

	PetArray * m_pMap;
};