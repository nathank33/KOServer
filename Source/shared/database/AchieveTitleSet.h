#pragma once

class CAchieveTitleSet : public OdbcRecordset
{
public:
	CAchieveTitleSet(OdbcConnection * dbConnection, AchieveTitleArray * pMap) : OdbcRecordset(dbConnection), m_pMap(pMap) {}

	virtual tstring GetTableName() { return _T("ACHIEVE_TITLE"); }
	virtual tstring GetColumns() { return _T("ID, Unknow1, Strength, Health, Dexterity, Intelligence, MagicPower, Attack, Defense, Contribution, ExpBonusPercent, ShortSwordDefense, JamadarDefense, SwordDefense, BlowDefense, AxeDefense, SpearDefense, ArrowDefense, FlameBonus, IceBonus, ElectricShockBonus, FlameResistence, IceResistence, ElectrickShockResistence, MagicResistence, SpellResistence, PoisonResistence"); }

	virtual bool Fetch()
	{
		auto pData = new _ACHIEVE_TITLE;

		auto i = 1;
		_dbCommand->FetchUInt32(i++, pData->ID);
		_dbCommand->FetchUInt16(i++, pData->UnKnow1);
		_dbCommand->FetchInt16(i++, pData->Strength);
		_dbCommand->FetchInt16(i++, pData->Health);
		_dbCommand->FetchInt16(i++, pData->Dexterity);
		_dbCommand->FetchInt16(i++, pData->Intelligence);
		_dbCommand->FetchInt16(i++, pData->MagicPower);
		_dbCommand->FetchInt16(i++, pData->Attack);
		_dbCommand->FetchInt16(i++, pData->Defense);
		_dbCommand->FetchInt16(i++, pData->Contribution);
		_dbCommand->FetchInt16(i++, pData->ExpBonusPercent);
		_dbCommand->FetchInt16(i++, pData->ShortSwordDefense);
		_dbCommand->FetchInt16(i++, pData->JamadarDefense);
		_dbCommand->FetchInt16(i++, pData->SwordDefense);
		_dbCommand->FetchInt16(i++, pData->BlowDefense);
		_dbCommand->FetchInt16(i++, pData->AxeDefense);
		_dbCommand->FetchInt16(i++, pData->SpearDefense);
		_dbCommand->FetchInt16(i++, pData->ArrowDefense);
		_dbCommand->FetchInt16(i++, pData->FlameBonus);
		_dbCommand->FetchInt16(i++, pData->IceBonus);
		_dbCommand->FetchInt16(i++, pData->ElectricShockBonus);
		_dbCommand->FetchInt16(i++, pData->FlameResistence);
		_dbCommand->FetchInt16(i++, pData->IceResistence);
		_dbCommand->FetchInt16(i++, pData->ElectrickShockResistence);
		_dbCommand->FetchInt16(i++, pData->MagicResistence);
		_dbCommand->FetchInt16(i++, pData->SpellResistence);
		_dbCommand->FetchInt16(i++, pData->PoisonResistence);

		if (!m_pMap->PutData(pData->ID, pData))
			delete pData;

		return true;
	}

	AchieveTitleArray * m_pMap;
};