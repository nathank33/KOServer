#pragma once

class CItemMiningSet : public OdbcRecordset {
public:
	CItemMiningSet(OdbcConnection * dbConnection, ItemMiningArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ITEM_MINING"); }
	virtual tstring GetColumns() {
		return _T("nIndex, ExchangeItemNum, ExchangeRate, isGoldenMattock");
	}

	virtual bool Fetch() {
		_MINING_ITEM *pData = new _MINING_ITEM;

		int i = 1;

		_dbCommand->FetchUInt32(i++, pData->nMiningID);
		_dbCommand->FetchUInt32(i++, pData->nExchangeItemNum);
		_dbCommand->FetchUInt16(i++, pData->sExchangeItemRate);
		_dbCommand->FetchUInt16(i++, pData->isGoldenMattock);

		if (pData->isGoldenMattock == 0) {
			for (int ad = 0; ad < pData->sExchangeItemRate; ad++) {
				g_pMain->bRandArrayNormalMattock[g_pMain->TotalMiningExchangeRate + ad] = pData->nExchangeItemNum;
			}
			g_pMain->TotalMiningExchangeRate += pData->sExchangeItemRate;
		} else {
			for (int ad = 0; ad < pData->sExchangeItemRate; ad++) {
				g_pMain->bRandArrayGoldenMattock[g_pMain->TotalGoldenMiningExchangeRate + ad] = pData->nExchangeItemNum;
			}
			g_pMain->TotalGoldenMiningExchangeRate += pData->sExchangeItemRate;
		}

		if (!m_pMap->PutData(pData->nMiningID, pData))
			delete pData;

		return true;
	}

	ItemMiningArray * m_pMap;
};