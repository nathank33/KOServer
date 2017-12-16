#pragma once

class CBotTableSet : public OdbcRecordset {
public:
	CBotTableSet(OdbcConnection * dbConnection, BotArray *pMap)
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("BOTDATA"); }
	virtual tstring GetColumns() {
		return _T("strUserID, Nation, Race, Class, HairRGB, [Level], Face, Knights, Fame, Zone, PX, PZ, PY, strItem, id, CoverTitle, RebLevel, strSkill, Gold, Points, Strong, Sta, Dex, Intel, Cha");
	}

	virtual bool Fetch() {
		CBot * pUser = new CBot();
		int field = 1;

		char strItem[INVENTORY_TOTAL * 8];
		memset(strItem, 0x00, sizeof(strItem));

		_dbCommand->FetchString(field++, pUser->m_strUserID);
		_dbCommand->FetchByte(field++, pUser->m_bNation);
		_dbCommand->FetchByte(field++, pUser->m_bRace);
		_dbCommand->FetchUInt16(field++, pUser->m_sClass);
		_dbCommand->FetchUInt32(field++, pUser->m_nHair);
		_dbCommand->FetchByte(field++, pUser->m_bLevel);
		_dbCommand->FetchByte(field++, pUser->m_bFace);
		_dbCommand->FetchInt16(field++, pUser->m_bKnights);
		_dbCommand->FetchByte(field++, pUser->m_bFame);
		_dbCommand->FetchByte(field++, pUser->m_bZone);
		pUser->m_curx = (float) (_dbCommand->FetchInt32(field++) / 100.0f);
		pUser->m_curz = (float) (_dbCommand->FetchInt32(field++) / 100.0f);
		pUser->m_cury = (float) (_dbCommand->FetchInt32(field++) / 100.0f);
		pUser->m_oldx = pUser->m_curx;
		pUser->m_oldy = pUser->m_cury;
		pUser->m_oldz = pUser->m_curz;

		memset(pUser->m_sItemArray, 0x00, sizeof(pUser->m_sItemArray));

		_dbCommand->FetchBinary(field++, strItem, sizeof(strItem));
		ByteBuffer itemData;
		itemData.append(strItem, sizeof(strItem));
		for (int i = 0; i < INVENTORY_TOTAL; i++) {
			uint32 nItemID;
			uint16 sDurability, sCount;
			itemData >> nItemID >> sDurability >> sCount;
			_ITEM_DATA *pItem = &pUser->m_sItemArray[i];
			pItem->nNum = nItemID;
			pItem->sDuration = sDurability;
			pItem->sCount = sCount;
		}

		_dbCommand->FetchUInt16(field++, pUser->m_sSid);
		_dbCommand->FetchUInt16(field++, pUser->m_sAchieveCoverTitle);

		_dbCommand->FetchByte(field++, pUser->m_reblvl);

		_dbCommand->FetchBinary(field++, (char *) pUser->m_bstrSkill, sizeof(pUser->m_bstrSkill));

		_dbCommand->FetchUInt32(field++, pUser->m_iGold);
		_dbCommand->FetchInt16(field++, pUser->m_sPoints);
		_dbCommand->FetchByte(field++, pUser->m_bStats[STAT_STR]);
		_dbCommand->FetchByte(field++, pUser->m_bStats[STAT_STA]);
		_dbCommand->FetchByte(field++, pUser->m_bStats[STAT_DEX]);
		_dbCommand->FetchByte(field++, pUser->m_bStats[STAT_INT]);
		_dbCommand->FetchByte(field++, pUser->m_bStats[STAT_CHA]);

		pUser->m_sSid += MAX_USER;

		if (pUser == nullptr)
			delete pUser;
		else
			m_pMap->PutData(pUser->m_sSid, pUser);

		return true;
	}

	BotArray * m_pMap;
};