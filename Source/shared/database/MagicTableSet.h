#pragma once

class CMagicTableSet : public OdbcRecordset
{
public:
	CMagicTableSet(OdbcConnection * dbConnection, MagictableArray * pMap) 
		: OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("MAGIC"); }
	virtual tstring GetColumns() { return _T("MagicNum, BeforeAction, TargetAction, SelfEffect, FlyingEffect, TargetEffect, Moral, SkillLevel, Skill, Msp, HP, ItemGroup, UseItem, CastTime, ReCastTime, SuccessRate, Type1, Type2, Range, UseStanding, Etc"); }

	virtual bool Fetch()
	{
		_MAGIC_TABLE *pData = new _MAGIC_TABLE;

		int i = 1;
		_dbCommand->FetchUInt32(i++, pData->iNum);
		_dbCommand->FetchUInt32(i++, pData->nBeforeAction);
		_dbCommand->FetchByte(i++, pData->bTargetAction);
		_dbCommand->FetchByte(i++, pData->bSelfEffect);
		_dbCommand->FetchUInt16(i++, pData->bFlyingEffect);
		_dbCommand->FetchUInt16(i++, pData->iTargetEffect);
		_dbCommand->FetchByte(i++, pData->bMoral);
		_dbCommand->FetchUInt16(i++, pData->sSkillLevel);
		_dbCommand->FetchUInt16(i++, pData->sSkill);
		_dbCommand->FetchUInt16(i++, pData->sMsp);
		_dbCommand->FetchUInt16(i++, pData->sHP);
		_dbCommand->FetchByte(i++, pData->bItemGroup);
		_dbCommand->FetchUInt32(i++, pData->iUseItem);
		_dbCommand->FetchByte(i++, pData->bCastTime);
		_dbCommand->FetchUInt16(i++, pData->sReCastTime);
		_dbCommand->FetchByte(i++, pData->bSuccessRate);
		_dbCommand->FetchByte(i++, pData->bType[0]);
		_dbCommand->FetchByte(i++, pData->bType[1]);
		_dbCommand->FetchUInt16(i++, pData->sRange);
		_dbCommand->FetchByte(i++, pData->sUseStanding);
		_dbCommand->FetchUInt16(i++, pData->sEtc);

		if (!m_pMap->PutData(pData->iNum, pData))
			delete pData;

		return true;
	}

	MagictableArray *m_pMap;
};