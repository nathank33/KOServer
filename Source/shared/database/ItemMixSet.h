#pragma once

class CItemMixSet : public OdbcRecordset {
public:
	CItemMixSet(OdbcConnection * dbConnection, ItemMixArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ITEM_MIX"); }
	virtual tstring GetColumns() {
		return _T("nIndex, nNpcID,bType,strName,bStatus,"
			"nOriginItemNum1, nOriginItemCount1, nOriginItemNum2, nOriginItemCount2, "
			"nOriginItemNum3, nOriginItemCount3, nOriginItemNum4, nOriginItemCount4, "
			"nOriginItemNum5, nOriginItemCount5,nOriginItemNum6, nOriginItemCount6,nOriginItemNum7, nOriginItemCount7,"
			"nOriginItemNum8, nOriginItemCount8,nOriginItemNum9, nOriginItemCount9,nOriginItemNum10, nOriginItemCount10,"
			"sSuccessEffect,bSuccessRate,sFailEffect,bBonusRate,"
			"nExchangeItemNum1, nExchangeItemCount1, nExchangeItemNum2, nExchangeItemCount2, nExchangeItemNum3, nExchangeItemCount3, "
			"nExchangeItemNum4, nExchangeItemCount4, nExchangeItemNum5, nExchangeItemCount5, nExchangeItemNum6, nExchangeItemCount6,"
			"nExchangeItemNum7, nExchangeItemCount7, nExchangeItemNum8, nExchangeItemCount8, nExchangeItemNum9, nExchangeItemCount9, nExchangeItemNum10, nExchangeItemCount10");
	}

	virtual bool Fetch() {
		_ITEM_MIX *pData = new _ITEM_MIX;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->nIndex);
		_dbCommand->FetchUInt16(i++, pData->sNpcNum);
		_dbCommand->FetchByte(i++, pData->bType);
		_dbCommand->FetchString(i++, pData->strName);
		_dbCommand->FetchByte(i++, pData->bStatus);

		for (int x = 0; x < ITEMS_IN_SPECIAL_ORIGIN_GROUP; x++) {
			_dbCommand->FetchUInt32(i++, pData->nOriginItemNum[x]);
			_dbCommand->FetchUInt16(i++, pData->sOriginItemCount[x]);
		}

		_dbCommand->FetchUInt16(i++, pData->sSuccessEffect);
		_dbCommand->FetchUInt16(i++, pData->bSuccessRate);
		_dbCommand->FetchUInt16(i++, pData->sFailEffect);
		_dbCommand->FetchUInt16(i++, pData->bBonusRate);

		for (int j = 0; j < ITEMS_IN_SPECIAL_EXCHANGE_GROUP; j++) {
			_dbCommand->FetchUInt32(i++, pData->nExchangeItemNum[j]);
			_dbCommand->FetchUInt16(i++, pData->sExchangeItemCount[j]);
		}

		if (!m_pMap->PutData(pData->nIndex, pData))
			delete pData;

		return true;
	}

	ItemMixArray *m_pMap;
};