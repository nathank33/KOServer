#include "stdafx.h"
#include "DBAgent.h"

#pragma region Nation Transfer

void CUser::SendNationChange() {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	Packet result(WIZ_NATION_CHANGE);

	if (!CheckExistItem(ITEM_NATION_TRANSFER)) {
		result << uint8(3) << uint8(7);
		Send(&result);
		return;
	}

	if (isInClan()) {
		result << uint8(2) << uint8(2);
		Send(&result);
		return;
	}

	if (isKing()) {
		result << uint8(3) << uint8(3);
		Send(&result);
		return;
	}

	result.DByte();
	uint8 YanChar = g_DBAgent.YanChar(GetAccountName());
	uint8 YanCharClan = g_DBAgent.YanCharClan(GetAccountName());

	if (YanChar == 0) {
		result << uint8(3) << uint8(6);
		Send(&result);
		return;
	}

	if (YanCharClan == 1) {
		result << uint8(2) << uint8(2);
		Send(&result);
		return;
	}

	std::string strCharID1, strCharID2, strCharID3, strCharID4;
	uint16 Class1 = 0, Class2 = 0, Class3 = 0, Class4 = 0;

	g_DBAgent.GetAllCharID(GetAccountName(), strCharID1, strCharID2, strCharID3, strCharID4);

	result << uint8(2) << uint8(1) << YanChar;

	if (!strCharID1.empty())
		Class1 = g_DBAgent.LoadAccountNTS(strCharID1);
	if (!strCharID2.empty())
		Class2 = g_DBAgent.LoadAccountNTS(strCharID2);
	if (!strCharID3.empty())
		Class3 = g_DBAgent.LoadAccountNTS(strCharID3);
	if (!strCharID4.empty())
		Class4 = g_DBAgent.LoadAccountNTS(strCharID4);

	if (Class1 > 0)
		result << uint16(0) << strCharID1 << uint8(0) << uint8(0) << uint16(GetNation() == 1 ? (Class1 + 100) : (Class1 - 100)) << uint8(0) << uint32(0);
	if (Class2 > 0)
		result << uint16(1) << strCharID2 << uint8(0) << uint8(0) << uint16(GetNation() == 1 ? (Class2 + 100) : (Class2 - 100)) << uint8(0) << uint32(0);
	if (Class3 > 0)
		result << uint16(2) << strCharID3 << uint8(0) << uint8(0) << uint16(GetNation() == 1 ? (Class3 + 100) : (Class3 - 100)) << uint8(0) << uint32(0);
	if (Class4 > 0)
		result << uint16(3) << strCharID4 << uint8(0) << uint8(0) << uint16(GetNation() == 1 ? (Class4 + 100) : (Class4 - 100)) << uint8(0) << uint32(0);

	Send(&result);
}

void CUser::NationChangeHandle(Packet & pkt) {
	uint8 opcode = pkt.read<uint8>();

	switch (opcode) {
	case 3:
		NationChange(pkt);
		break;
	default:
		break;
	}
}

void CUser::NationChange(Packet & pkt) {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	uint8 opcode;
	pkt >> opcode;

	if (opcode == 0)
		return;

	Packet result(WIZ_NATION_CHANGE);

	if (!CheckExistItem(ITEM_NATION_TRANSFER)) {
		result << uint8(3) << uint8(7);
		Send(&result);
		return;
	}

	if (isInClan()) {
		result << uint8(2) << uint8(2);
		Send(&result);
		return;
	}

	if (isKing()) {
		result << uint8(3) << uint8(3);
		Send(&result);
		return;
	}

	uint8 YanChar = g_DBAgent.YanChar(GetAccountName());
	uint8 YanCharClan = g_DBAgent.YanCharClan(GetAccountName());

	if (YanChar == 0) {
		result << uint8(3) << uint8(6);
		Send(&result);
		return;
	}

	if (YanCharClan == 1) {
		result << uint8(2) << uint8(2);
		Send(&result);
		return;
	}// hata kýsmý

	// Okuma kýsmý

	uint8 Count;
	uint16 CharNum[4];
	std::string Nick[4];
	uint8 nRace[4];
	uint8 unkNown1[4];
	uint32 unkNown2[4];
	uint8 hisRace = 0, myRace = 0;

	nRace[0] = 0;
	nRace[1] = 0;
	nRace[2] = 0;
	nRace[3] = 0;

	pkt >> Count;

	if (Count == 0) {
		result << uint8(3) << uint8(6);
		Send(&result);
		return;
	}

	for (int i = 0; i < Count; i++) {
		pkt >> CharNum[i] >> Nick[i] >> hisRace >> unkNown1[i] >> unkNown2[i];
		nRace[CharNum[i]] = hisRace;

		if (hisRace == 0
			|| (hisRace < 10 && GetNation() == 1)
			|| (hisRace > 14)
			|| (hisRace > 6 && GetNation() == 2)) {
			result << uint8(3) << uint8(6);
			Send(&result);
			return;
		}
	}

	uint8 nRet = g_DBAgent.NationTransfer(GetAccountName(), nRace[0], nRace[1], nRace[2], nRace[3]);

	myRace = g_DBAgent.LoadCharRace(GetName());

	if (myRace == 0) {
		result << uint8(2) << uint8(0);
		Send(&result);
		return;
	}

	if (nRet != 1) {
		result << uint8(3) << uint8(6);
		Send(&result);
		return;
	}

	RobItem(ITEM_NATION_TRANSFER);

	m_bRank = 0;
	m_bTitle = 0;
	m_bRace = myRace;
	m_sClass = GetNation() == KARUS ? GetClass() + 100 : GetClass() - 100;
	m_bNation = GetNation() == KARUS ? ELMORAD : KARUS;

	if (GetHealth() < (GetMaxHealth() / 2))
		HpChange(GetMaxHealth());

	SendMyInfo();

	UserInOut(INOUT_OUT);
	SetRegion(GetNewRegionX(), GetNewRegionZ());
	UserInOut(INOUT_WARP);

	g_pMain->UserInOutForMe(this);
	g_pMain->NpcInOutForMe(this);
	g_pMain->MerchantUserInOutForMe(this);

	ResetWindows();

	InitType4();
	RecastSavedMagic();

	result << uint8(3) << uint8(1);
	Send(&result);

	Disconnect();
}

uint8 CUser::GetNewRace() {
	uint8 nNewRace = 0;

	if (GetNation() == KARUS) {
		if (m_bRace == KARUS_BIG)
			nNewRace = BABARIAN;
		else if (m_bRace == KARUS_MIDDLE)
			nNewRace = ELMORAD_MAN;
		else if (m_bRace == KARUS_SMALL)
			nNewRace = ELMORAD_MAN;
		else if (m_bRace == KARUS_WOMAN)
			nNewRace = ELMORAD_WOMAN;
		else if (m_bRace == KARUS_MONSTER)
			nNewRace = ELMORAD_MONSTER;
	} else {
		if (m_bRace == BABARIAN)
			nNewRace = KARUS_BIG;
		// El Morad Male and El Morad Warriors
		else if (m_bRace == ELMORAD_MAN && isWarrior())
			nNewRace = KARUS_BIG;
		// El Morad Female and El Morad Warriors
		else if (m_bRace == ELMORAD_WOMAN && isWarrior())
			nNewRace = KARUS_BIG;
		// El Morad Male and El Morad Rogues
		else if (m_bRace == ELMORAD_MAN && isRogue())
			nNewRace = KARUS_MIDDLE;
		// El Morad Female and El Morad Rogues
		else if (m_bRace == ELMORAD_WOMAN && isRogue())
			nNewRace = KARUS_MIDDLE;
		// El Morad Male and El Morad Magicians
		else if (m_bRace == ELMORAD_MAN && isMage())
			nNewRace = KARUS_SMALL;
		// El Morad Female and El Morad Magicians
		else if (m_bRace == ELMORAD_WOMAN && isMage())
			nNewRace = KARUS_WOMAN;
		// El Morad Male and El Morad Priests
		else if (m_bRace == ELMORAD_MAN && isPriest())
			nNewRace = KARUS_MIDDLE;
		// El Morad Female and El Morad Priests
		else if (m_bRace == ELMORAD_WOMAN && isPriest())
			nNewRace = KARUS_WOMAN;
		else if (m_bRace == ELMORAD_MONSTER)
			nNewRace = KARUS_MONSTER;
	}

	return nNewRace;
}

#pragma endregion

#pragma region Gender Change

void CUser::GenderChange(Packet & pkt) {
	Packet result(WIZ_GENDER_CHANGE);

	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return;

	if (GetRace() == 0
		|| GetRace() == KARUS_BIG
		|| (GetRace() == KARUS_MIDDLE && GetClass() % 100 < 10)
		|| GetRace() == KARUS_MONSTER
		|| GetRace() == ELMORAD_MONSTER)
		goto fail_return;

	uint8 newRace, newFace, resultCode = 0, opCode;
	uint32 newHair;

	pkt >> opCode >> newRace >> newFace >> newHair;

	if (!CheckExistItem(ITEM_GENDER_CHANGE))
		goto fail_return;

	if (newRace == 0 || newFace == 0 || newHair == 0)
		goto fail_return;

	if (newRace < 10 && GetNation() != 1)
		goto fail_return;

	if (newRace > 10 && GetNation() != 2)
		goto fail_return;

	if (newRace > 13 || newRace == GetRace())
		goto fail_return;

	if (newRace > 5 && GetNation() == 1)
		goto fail_return;

	m_bRace = newRace;
	m_nHair = newHair;
	m_bFace = newFace;

	resultCode = g_DBAgent.UpdateUser(GetName(), UPDATE_PACKET_SAVE, this);

	if (resultCode < 1)
		goto fail_return;

	result << uint8(1) << uint16(GetID()) << newRace << newFace << newHair;
	SendToRegion(&result, nullptr, GetEventRoom());

	RobItem(ITEM_GENDER_CHANGE);

	if (GetHealth() < (GetMaxHealth() / 2))
		HpChange(GetMaxHealth());

	return;
fail_return:
	result << resultCode;
	Send(&result);
}

#pragma endregion

#pragma region Job Change

uint8 CUser::JobChange(uint8 NewJob /*= 0*/) {
	if (isDead()
		|| isTrading()
		|| isMerchanting()
		|| isStoreOpen()
		|| isSellingMerchant()
		|| isBuyingMerchant()
		|| isMining()
		|| m_bMerchantStatex)
		return 5;

	uint8 bNewClass = 0, bNewRace = 0;
	uint8 bResult = 0;

	if (NewJob < 1 || NewJob > 5)
		return 5; // Unknown job is selected...

	if (!CheckExistItem(ITEM_JOB_CHANGE))
		return 6; // Job Change scroll is not found...

	for (int i = 0; i < SLOT_MAX; i++) {
		if (m_sItemArray[i].nNum) {
			bResult = 7;
			break;
		}
	}

	if (bResult == 7) {
		Packet result(WIZ_CLASS_CHANGE, uint8(ALL_POINT_CHANGE));
		result << uint8(4) << int(0);
		Send(&result);
		return bResult; // While there are items equipped on you.
	}

	// If bResult between 1 and 5 character already selected job...

	// If selected a new job Warrior
	if (NewJob == 1) {
		if (GetNation() == KARUS) {
			// Beginner Karus Rogue, Magician, Priest
			if (isBeginnerRogue() || isBeginnerMage() || isBeginnerPriest() || isBeginnerPorutu()) {
				bNewClass = KARUWARRIOR;
				bNewRace = KARUS_BIG;
			}
			// Skilled Karus Rogue, Magician, Priest
			else if (isNoviceRogue() || isNoviceMage() || isNovicePriest() || isNovicePorutu()) {
				bNewClass = BERSERKER;
				bNewRace = KARUS_BIG;
			}
			// Mastered Karus Rogue, Magician, Priest
			else if (isMasteredRogue() || isMasteredMage() || isMasteredPriest() || isMasteredPorutu()) {
				bNewClass = GUARDIAN;
				bNewRace = KARUS_BIG;
			}
		} else {
			// Beginner El Morad Rogue, Magician, Priest
			if (isBeginnerRogue() || isBeginnerMage() || isBeginnerPriest() || isBeginnerPorutu()) {
				bNewClass = ELMORWARRRIOR;
				if (GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Skilled El Morad Rogue, Magician, Priest
			else if (isNoviceRogue() || isNoviceMage() || isNovicePriest() || isNovicePorutu()) {
				bNewClass = BLADE;
				if (GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Mastered El Morad Rogue, Magician, Priest
			else if (isMasteredRogue() || isMasteredMage() || isMasteredPriest() || isMasteredPorutu()) {
				bNewClass = PROTECTOR;
				if (GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
		}

		// Character already Warrior.
		if (bNewClass == 0 || bNewRace == 0)
			bResult = NewJob;
	}

	// If selected a new job Rogue
	if (NewJob == 2) {
		if (GetNation() == KARUS) {
			// Beginner Karus Warrior, Magician, Priest
			if (isBeginnerWarrior() || isBeginnerMage() || isBeginnerPriest() || isBeginnerPorutu()) {
				bNewClass = KARUROGUE;
				bNewRace = KARUS_MIDDLE;
			}
			// Skilled Karus Warrior, Magician, Priest
			else if (isNoviceWarrior() || isNoviceMage() || isNovicePriest() || isNovicePorutu()) {
				bNewClass = HUNTER;
				bNewRace = KARUS_MIDDLE;
			}
			// Mastered Karus Warrior, Magician, Priest
			else if (isMasteredWarrior() || isMasteredMage() || isMasteredPriest() || isMasteredPorutu()) {
				bNewClass = PENETRATOR;
				bNewRace = KARUS_MIDDLE;
			}
		} else {
			// Beginner El Morad Warrior, Magician, Priest
			if (isBeginnerWarrior() || isBeginnerMage() || isBeginnerPriest() || isBeginnerPorutu()) {
				bNewClass = ELMOROGUE;

				if (GetRace() == BABARIAN || GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Skilled El Morad Warrior, Magician, Priest
			else if (isNoviceWarrior() || isNoviceMage() || isNovicePriest() || isNovicePorutu()) {
				bNewClass = RANGER;

				if (GetRace() == BABARIAN || GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Mastered El Morad Warrior, Magician, Priest
			else if (isMasteredWarrior() || isMasteredMage() || isMasteredPriest() || isMasteredPorutu()) {
				bNewClass = ASSASSIN;

				if (GetRace() == BABARIAN || GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
		}

		// Character already Rogue.
		if (bNewClass == 0 || bNewRace == 0)
			bResult = NewJob;
	}

	// If selected a new job Magician
	if (NewJob == 3) {
		if (GetNation() == KARUS) {
			// Beginner Karus Warrior, Rogue, Priest
			if (isBeginnerWarrior() || isBeginnerRogue() || isBeginnerPriest() || isBeginnerPorutu()) {
				bNewClass = KARUWIZARD;

				if (GetRace() == KARUS_BIG || GetRace() == KARUS_MIDDLE || GetRace() == KARUS_MONSTER)
					bNewRace = KARUS_SMALL;
				else
					bNewRace = GetRace();
			}
			// Skilled Karus Warrior, Rogue, Priest
			else if (isNoviceWarrior() || isNoviceRogue() || isNovicePriest() || isNovicePorutu()) {
				bNewClass = SORSERER;

				if (GetRace() == KARUS_BIG || GetRace() == KARUS_MIDDLE || GetRace() == KARUS_MONSTER)
					bNewRace = KARUS_SMALL;
				else
					bNewRace = GetRace();
			}
			// Mastered Karus Warrior, Rogue, Priest
			else if (isMasteredWarrior() || isMasteredRogue() || isMasteredPriest() || isMasteredPorutu()) {
				bNewClass = NECROMANCER;

				if (GetRace() == KARUS_BIG || GetRace() == KARUS_MIDDLE || GetRace() == KARUS_MONSTER)
					bNewRace = KARUS_SMALL;
				else
					bNewRace = GetRace();
			}
		} else {
			// Beginner El Morad Warrior, Rogue, Priest
			if (isBeginnerWarrior() || isBeginnerRogue() || isBeginnerPriest() || isBeginnerPorutu()) {
				bNewClass = ELMOWIZARD;

				if (GetRace() == BABARIAN || GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Skilled El Morad Warrior, Rogue, Priest
			else if (isNoviceWarrior() || isNoviceRogue() || isNovicePriest() || isNovicePorutu()) {
				bNewClass = MAGE;

				if (GetRace() == BABARIAN || GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Mastered El Morad Warrior, Rogue, Priest
			else if (isMasteredWarrior() || isMasteredRogue() || isMasteredPriest() || isMasteredPorutu()) {
				bNewClass = ENCHANTER;

				if (GetRace() == BABARIAN || GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
		}

		// Character already Magician.
		if (bNewClass == 0 || bNewRace == 0)
			bResult = NewJob;
	}

	// If selected a new job Priest
	if (NewJob == 4) {
		if (GetNation() == KARUS) {
			// Beginner Karus Warrior, Rogue, Magician
			if (isBeginnerWarrior() || isBeginnerRogue() || isBeginnerMage() || isBeginnerPorutu()) {
				bNewClass = KARUPRIEST;

				if (GetRace() == KARUS_BIG || GetRace() == KARUS_SMALL || GetRace() == KARUS_MONSTER)
					bNewRace = KARUS_MIDDLE;
				else
					bNewRace = GetRace();
			}
			// Skilled Karus Warrior, Rogue, Magician
			else if (isNoviceWarrior() || isNoviceRogue() || isNoviceMage() || isNovicePorutu()) {
				bNewClass = SHAMAN;

				if (GetRace() == KARUS_BIG || GetRace() == KARUS_SMALL || GetRace() == KARUS_MONSTER)
					bNewRace = KARUS_MIDDLE;
				else
					bNewRace = GetRace();
			}
			// Mastered Karus Warrior, Rogue, Magician
			else if (isMasteredWarrior() || isMasteredRogue() || isMasteredMage() || isMasteredPorutu()) {
				bNewClass = DARKPRIEST;

				if (GetRace() == KARUS_BIG || GetRace() == KARUS_SMALL || GetRace() == KARUS_MONSTER)
					bNewRace = KARUS_MIDDLE;
				else
					bNewRace = GetRace();
			}
		} else {
			// Beginner El Morad Warrior, Rogue, Magician
			if (isBeginnerWarrior() || isBeginnerRogue() || isBeginnerMage() || isBeginnerPorutu()) {
				bNewClass = ELMOPRIEST;

				if (GetRace() == BABARIAN || GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Skilled El Morad Warrior, Rogue, Magician
			else if (isNoviceWarrior() || isNoviceRogue() || isNoviceMage() || isNovicePorutu()) {
				bNewClass = CLERIC;

				if (GetRace() == BABARIAN || GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
			// Mastered El Morad Warrior, Rogue, Magician
			else if (isMasteredWarrior() || isMasteredRogue() || isMasteredMage() || isMasteredPorutu()) {
				bNewClass = DRUID;

				if (GetRace() == BABARIAN || GetRace() == ELMORAD_MONSTER)
					bNewRace = ELMORAD_MAN;
				else
					bNewRace = GetRace();
			}
		}

		// Character already Priest.
		if (bNewClass == 0 || bNewRace == 0)
			bResult = NewJob;
	}

	// If selected a new job Kurian
	if (NewJob == 5 || NewJob == 6) {
		if (GetNation() == KARUS) {
			// Beginner Karus Warrior, Rogue, Magician
			if (isBeginnerRogue() || isBeginnerMage() || isBeginnerPriest() || isBeginnerWarrior()) {
				bNewClass = PORUTU;
				bNewRace = KARUS_MONSTER;
			}
			// Skilled Karus Warrior, Rogue, Magician
			else if (isNoviceRogue() || isNoviceMage() || isNovicePriest() || isNoviceWarrior()) {
				bNewClass = PORUTUSKILLED;
				bNewRace = KARUS_MONSTER;
			}
			// Mastered Karus Warrior, Rogue, Magician
			else if (isMasteredRogue() || isMasteredMage() || isMasteredPriest() || isMasteredWarrior()) {
				bNewClass = PORUTUMASTER;
				bNewRace = KARUS_MONSTER;
			}
		} else {
			// Beginner El Morad Warrior, Rogue, Magician
			if (isBeginnerRogue() || isBeginnerMage() || isBeginnerPriest() || isBeginnerWarrior()) {
				bNewClass = KURIAN;
				bNewRace = ELMORAD_MONSTER;
			}
			// Skilled El Morad Warrior, Rogue, Magician
			else if (isNoviceRogue() || isNoviceMage() || isNovicePriest() || isNoviceWarrior()) {
				bNewClass = KURIANSKILLED;
				bNewRace = ELMORAD_MONSTER;
			}
			// Mastered El Morad Warrior, Rogue, Magician
			else if (isMasteredRogue() || isMasteredMage() || isMasteredPriest() || isMasteredWarrior()) {
				bNewClass = KURIANMASTER;
				bNewRace = ELMORAD_MONSTER;
			}
		}

		// Character already Kurian.
		if (bNewClass == 0 || bNewRace == 0)
			bResult = NewJob;
	}

	if (bResult == 0) {
		RobItem(ITEM_JOB_CHANGE, 1, true);

		m_sClass = bNewClass;
		m_bRace = bNewRace;

		// Reset stat and skill points...
		AllPointChange(true);
		AllSkillPointChange(true);

		g_DBAgent.UpdateUser(GetName(), UPDATE_PACKET_SAVE, this);;

		if (GetHealth() < (GetMaxHealth() / 2))
			HpChange(GetMaxHealth());

		SendMyInfo();

		UserInOut(INOUT_OUT);
		SetRegion(GetNewRegionX(), GetNewRegionZ());
		UserInOut(INOUT_WARP);

		g_pMain->UserInOutForMe(this);
		g_pMain->NpcInOutForMe(this);
		g_pMain->MerchantUserInOutForMe(this);

		ResetWindows();

		InitType4();
		RecastSavedMagic();
	}

	return bResult;
}

#pragma endregion