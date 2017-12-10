local Ret = 0;
local NPC = 24431;
local savenum = -1;

if (EVENT == 155) then
	QuestNum = SearchQuest(UID, NPC);
	if (QuestNum == 0) then
		SelectMsg(UID, 2, -1, 8255, NPC, 10, 158);
	elseif (QuestNum > 1 and  QuestNum < 100) then
		NpcMsg(UID, 8257, NPC)
	else
		EVENT = QuestNum
	end
end

if (EVENT == 158) then
	Ret = 1;
end

savenum = 457;

if (EVENT == 200) then -- 50 Level Lamia Repeatable
	SaveEvent(UID, 2193);
end

if (EVENT == 202) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8233, NPC, 22, 203, 23, 204);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 203) then
	SaveEvent(UID, 2194);
end

if (EVENT == 204) then
	SaveEvent(UID, 2197);
end

if (EVENT == 210) then
	SaveEvent(UID, 2196);
end

if (EVENT == 207) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8263, NPC, 18, 208);
	else
		SelectMsg(UID, 4, savenum, 8264, NPC, 41, 209, 27, 158);
	end
end

if (EVENT == 208) then
	ShowMap(UID, 182);
end

if (EVENT == 209) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1900000)
		SaveEvent(UID, 2195);
	else
		ExpChange(UID, 1250000)
		SaveEvent(UID, 2195);   
	end
end

savenum = 218;

if (EVENT == 8250) then -- 50 Level Lamia
	SelectMsg(UID, 2, savenum, 8231, NPC, 28, 8251);
end

if (EVENT == 8251) then
	ShowMap(UID, 562);
	SaveEvent(UID, 9009);
end

if (EVENT == 8252) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, savenum, 8232, NPC, 10, 8260);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 8260) then
	SelectMsg(UID, 4, savenum, 8233, NPC, 22, 8253, 23, 8254);
end

if (EVENT == 8253) then
	SaveEvent(UID, 9010);
end

if (EVENT == 8254) then
	SaveEvent(UID, 9013);
end

if (EVENT == 8255) then
	SelectMsg(UID, 2, savenum, 8262, NPC, 3007, 158);
	SaveEvent(UID, 9012);
end

if (EVENT == 8257) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8263, NPC, 18, 8258);
	else
		SelectMsg(UID, 4, savenum, 8264, NPC, 41, 8259, 27, 158);
	end
end

if (EVENT == 8258) then
	ShowMap(UID, 182);
end

if (EVENT == 8259) then
	Class = CheckClass(UID);
    ExpChange(UID, 12500000)
	if (Class == 1 or Class == 5 or Class == 6) then
		GiveItem(UID, 925003595, 1)
		SaveEvent(UID, 9011);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		GiveItem(UID, 925008596, 1)
		SaveEvent(UID, 9011);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		GiveItem(UID, 926003597, 1)
		SaveEvent(UID, 9011);
		ShowEffect(UID, 300391);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		GiveItem(UID, 926008598, 1)
		SaveEvent(UID, 9011);
		ShowEffect(UID, 300391);
	end
end

savenum = 459;

if (EVENT == 300) then -- 51 Level Uruk Hai Repeatable
	SaveEvent(UID, 2205);
end

if (EVENT == 302) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8272, NPC, 22, 303, 23, 304);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 303) then
	SaveEvent(UID, 2206);
end

if (EVENT == 304) then
	SaveEvent(UID, 2209);
end

if (EVENT == 310) then
	SaveEvent(UID, 2208);
end

if (EVENT == 307) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8263, NPC, 18, 308);
	else
		SelectMsg(UID, 4, savenum, 8264, NPC, 41, 309, 27, 158);
	end
end

if (EVENT == 308) then
	ShowMap(UID, 544);
end

if (EVENT == 309) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1900000)
		SaveEvent(UID, 2207);
	else
		ExpChange(UID, 1250000)
		SaveEvent(UID, 2207);   
	end
end

savenum = 223;

if (EVENT == 8350) then -- 51 Level Uruk Hai
	SelectMsg(UID, 2, savenum, 8271, NPC, 14, 8351);
end

if (EVENT == 8351) then
	SaveEvent(UID, 9021);
end

if (EVENT == 8352) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, savenum, 8272, NPC, 10, 8360);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 8360) then
	SelectMsg(UID, 4, savenum, 8273, NPC, 22, 8353, 23, 8354);
end

if (EVENT == 8353) then
	SaveEvent(UID, 9022);
end

if (EVENT == 8354) then
	SaveEvent(UID, 9025);
end

if (EVENT == 8355) then
	SelectMsg(UID, 2, savenum, 8262, NPC, 3007, 158);
	SaveEvent(UID, 9024);
end

if (EVENT == 8357) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8263, NPC, 18, 8358);
	else
		SelectMsg(UID, 4, savenum, 8264, NPC, 41, 8359, 27, 158);
	end
end

if (EVENT == 8358) then
	ShowMap(UID, 544);
end

if (EVENT == 8359) then
	Class = CheckClass(UID);
    ExpChange(UID, 13750000)
	if (Class == 1 or Class == 5 or Class == 6) then
		GiveItem(UID, 925004595, 1)
		SaveEvent(UID, 9023);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		GiveItem(UID, 925009596, 1)
		SaveEvent(UID, 9023);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		GiveItem(UID, 926004597, 1)
		SaveEvent(UID, 9023);
		ShowEffect(UID, 300391);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		GiveItem(UID, 926009598, 1)
		SaveEvent(UID, 9023);
		ShowEffect(UID, 300391);
	end	 
end

savenum = 465;

if (EVENT == 400) then -- 53 Level Treant Repeatable
	SaveEvent(UID, 2241);
end

if (EVENT == 410) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8285, NPC, 22, 411, 23, 412);
	else
		SelectMsg(UID, 2, savenum, 8287, NPC, 10, 158);
	end
end

if (EVENT == 411) then
	SaveEvent(UID, 2242);
end

if (EVENT == 412) then
	SaveEvent(UID, 2245);
end

if (EVENT == 413) then
	SaveEvent(UID, 2244);
end

if (EVENT == 415) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8289, NPC, 18, 416);
	else
		SelectMsg(UID, 4, savenum, 8290, NPC, 41, 417, 27, 158);
	end
end

if (EVENT == 416) then
	ShowMap(UID, 506);
end

if (EVENT == 417) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1900000)
		SaveEvent(UID, 2243);
	else
		ExpChange(UID, 1250000)
		SaveEvent(UID, 2243);   
	end
end

savenum = 233;

if (EVENT == 8650) then -- 53 Level Treant
	SelectMsg(UID, 2, savenum, 8277, NPC, 3008, 8651);
end

if (EVENT == 8651) then
	SelectMsg(UID, 2, savenum, 8278, NPC, 4080, 158);
	SaveEvent(UID, 9057);
end

if (EVENT == 8660) then
	SelectMsg(UID, 2, savenum, 8284, NPC, 3002, 8661);
end

if (EVENT == 8661) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8285, NPC, 3018, 8662, 3019, 8668);
	else
		SelectMsg(UID, 2, savenum, 8287, NPC, 10, 158);
	end
end

if (EVENT == 8662) then
	SaveEvent(UID, 9058);
end

if (EVENT == 8668) then
	SaveEvent(UID, 9061);
end

if (EVENT == 8663) then
	SelectMsg(UID, 2, savenum, 8288, NPC, 4080, 158);
	SaveEvent(UID, 9060);
end

if (EVENT == 8665) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8289, NPC, 18, 8669);
	else
		SelectMsg(UID, 4, savenum, 8290, NPC, 41, 8667, 27, 158);
	end
end

if (EVENT == 8669) then
	ShowMap(UID, 506);
end

if (EVENT == 8667) then
	GiveItem(UID, 389155000, 5)
	GiveItem(UID, 389156000, 5)
	ExpChange(UID, 15000000)
	SaveEvent(UID, 9059);
end

savenum = 467;

if (EVENT == 500) then -- 54 Level Ancient Repeatable
	SaveEvent(UID, 2253);
end

if (EVENT == 502) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8303, NPC, 22, 503, 23, 504);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 503) then
	SaveEvent(UID, 2254);
end

if (EVENT == 504) then
	SaveEvent(UID, 2257);
end

if (EVENT == 510) then
	SaveEvent(UID, 2256);
end

if (EVENT == 507) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8263, NPC, 18, 508);
	else
		SelectMsg(UID, 4, savenum, 8264, NPC, 41, 509, 27, 158);
	end
end

if (EVENT == 508) then
	ShowMap(UID, 547);
end

if (EVENT == 509) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1900000)
		SaveEvent(UID, 2255);
	else
		ExpChange(UID, 1250000)
		SaveEvent(UID, 2255);   
	end
end

savenum = 237;

if (EVENT == 9180) then -- 54 Level Ancient
	SelectMsg(UID, 2, savenum, 8271, NPC, 14, 9181);
end

if (EVENT == 9181) then
	SaveEvent(UID, 9069);
end

if (EVENT == 9182) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, savenum, 8303, NPC, 10, 9190);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9190) then
	SelectMsg(UID, 4, savenum, 8304, NPC, 22, 9183, 23, 9184);
end

if (EVENT == 9183) then
	SaveEvent(UID, 9070);
end

if (EVENT == 9184) then
	SaveEvent(UID, 9073);
end

if (EVENT == 9185) then
	SelectMsg(UID, 2, savenum, 8262, NPC, 3007, 158);
	SaveEvent(UID, 9072);
end

if (EVENT == 9187) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8263, NPC, 18, 9188);
	else
		SelectMsg(UID, 4, savenum, 8264, NPC, 41, 9189, 27, 158);
	end
end

if (EVENT == 9188) then
	ShowMap(UID, 547);
end

if (EVENT == 9189) then
	Class = CheckClass(UID);
    ExpChange(UID, 16250000)
	if (Class == 1 or Class == 5 or Class == 6) then
		GiveItem(UID, 925000595, 1)
		SaveEvent(UID, 9071);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		GiveItem(UID, 925005596, 1)
		SaveEvent(UID, 9071);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		GiveItem(UID, 926000597, 1)
		SaveEvent(UID, 9071);
		ShowEffect(UID, 300391);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		GiveItem(UID, 926005598, 1)
		SaveEvent(UID, 9071);
		ShowEffect(UID, 300391);
	end	 
end

savenum = 471;

if (EVENT == 600) then -- 55 Level Dragon Tooth Commander Repeatable (this quest doesn't exist in the db)
	SaveEvent(UID, 2277);
end

if (EVENT == 602) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8444, NPC, 22, 603, 23, 604);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 603) then
	SaveEvent(UID, 2278);
end

if (EVENT == 604) then
	SaveEvent(UID, 2281);
end

if (EVENT == 610) then
	SaveEvent(UID, 2280);
end

if (EVENT == 607) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 15) then
		SelectMsg(UID, 2, savenum, 8263, NPC, 18, 608);
	else
		SelectMsg(UID, 4, savenum, 8450, NPC, 41, 609, 27, 158);
	end
end

if (EVENT == 608) then
	ShowMap(UID, 587);
end

if (EVENT == 609) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 3750000)
		SaveEvent(UID, 2279);
	else
		ExpChange(UID, 2500000)
		SaveEvent(UID, 2279);   
	end
end

savenum = 241;

if (EVENT == 9020) then -- 55 Level Manticore (not correct in db)
	SelectMsg(UID, 2, savenum, 8271, NPC, 14, 9021);
end

if (EVENT == 9021) then
	SaveEvent(UID, 9093);
end

if (EVENT == 9022) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, savenum, 8444, NPC, 10, 9030);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9030) then
	SelectMsg(UID, 4, savenum, 8445, NPC, 22, 9023, 23, 9024);
end

if (EVENT == 9023) then
	SaveEvent(UID, 9094);
end

if (EVENT == 9024) then
	SaveEvent(UID, 9097);
end

if (EVENT == 9025) then
	SelectMsg(UID, 2, savenum, 8262, NPC, 3007, 158);
	SaveEvent(UID, 9096);
end

if (EVENT == 9027) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 30) then
		SelectMsg(UID, 2, savenum, 8263, NPC, 18, 9028);
	else
		SelectMsg(UID, 4, savenum, 8450, NPC, 41, 9029, 27, 158);
	end
end

if (EVENT == 9028) then
	ShowMap(UID, 587);
end

if (EVENT == 9029) then
	ExpChange(UID, 10000000)
	SaveEvent(UID, 9095);   
end

savenum = 473;

if (EVENT == 700) then -- 56 Level Uruk Blade Repeatable
	SaveEvent(UID, 2289);
end

if (EVENT == 702) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8454, NPC, 22, 703, 23, 704);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 703) then
	SaveEvent(UID, 2290);
end

if (EVENT == 704) then
	SaveEvent(UID, 2293);
end

if (EVENT == 710) then
	SaveEvent(UID, 2292);
end

if (EVENT == 707) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 30) then
		SelectMsg(UID, 2, savenum, 8263, NPC, 18, 708);
	else
		SelectMsg(UID, 4, savenum, 8264, NPC, 41, 709, 27, 158);
	end
end

if (EVENT == 708) then
	ShowMap(UID, 551);
end

if (EVENT == 709) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 3750000)
		SaveEvent(UID, 2291);
	else
		ExpChange(UID, 2500000)
		SaveEvent(UID, 2291);   
	end
end

savenum = 263;

if (EVENT == 9040) then -- 56 Level Uruk Blade
	SelectMsg(UID, 2, savenum, 8271, NPC, 14, 9041);
end

if (EVENT == 9041) then
	SaveEvent(UID, 9105);
end

if (EVENT == 9042) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, savenum, 8454, NPC, 10, 9050);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9050) then
	SelectMsg(UID, 4, savenum, 8455, NPC, 22, 9043, 23, 9044);
end

if (EVENT == 9043) then
	SaveEvent(UID, 9106);
end

if (EVENT == 9044) then
	SaveEvent(UID, 9109);
end

if (EVENT == 9045) then
	SelectMsg(UID, 2, savenum, 8262, NPC, 3007, 158);
	SaveEvent(UID, 9108);
end

if (EVENT == 9047) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8263, NPC, 18, 9048);
	else
		SelectMsg(UID, 4, savenum, 8264, NPC, 41, 9049, 27, 158);
	end
end

if (EVENT == 9048) then
	ShowMap(UID, 551);
end

if (EVENT == 9049) then
	ExpChange(UID, 25000000)
	SaveEvent(UID, 9107);	 
end

savenum = 285;

if (EVENT == 9080) then -- 60 Level Deruvish
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9129);
		EVENT = 9081
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9134);
		EVENT = 9081
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9139);
		EVENT = 9081
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9144);
		EVENT = 9081
	end
end

if (EVENT == 9081) then
	SelectMsg(UID, 2, savenum, 8458, NPC, 4080, 158);
end

if (EVENT == 9082) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, savenum, 8459, NPC, 10, 9085);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9085) then
	SelectMsg(UID, 4, savenum, 8460, NPC, 22, 9083, 23, 9084);
end

if (EVENT == 9083) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9130);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9135);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9140);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9145);
	end
end

if (EVENT == 9084) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9133);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9138);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9143);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9148);
	end
end

if (EVENT == 9090) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9132);
		EVENT = 9091
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9137);
		EVENT = 9091
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9142);
		EVENT = 9091
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9147);
		EVENT = 9091
	end
end

if (EVENT == 9091) then
	SelectMsg(UID, 2, savenum, 8461, NPC, 3002, 158);
end

if (EVENT == 9086) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 40) then
		SelectMsg(UID, 2, savenum, 8462, NPC, 18, 9087);
	else
		SelectMsg(UID, 4, savenum, 8463, NPC, 41, 9088, 27, 158);
	end
end

if (EVENT == 9087) then
	ShowMap(UID, 518);
end

if (EVENT == 9088) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		GiveItem(UID, 206004003, 1)
		SaveEvent(UID, 9131);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		GiveItem(UID, 246004003, 1)
		SaveEvent(UID, 9136);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		GiveItem(UID, 266004003, 1)
		SaveEvent(UID, 9141);
		ShowEffect(UID, 300391);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		GiveItem(UID, 286004003, 1)
		SaveEvent(UID, 9146);
		ShowEffect(UID, 300391);
	end 
end

savenum = 287;

if (EVENT == 9100) then -- 60 Level Apostle
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9171);
		EVENT = 9101
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9176);
		EVENT = 9101
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9181);
		EVENT = 9101
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9186);
		EVENT = 9101
	end
end

if (EVENT == 9101) then
	SelectMsg(UID, 2, savenum, 8472, NPC, 4080, 158);
end

if (EVENT == 9102) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, savenum, 8473, NPC, 10, 9105);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9105) then
	SelectMsg(UID, 4, savenum, 8474, NPC, 22, 9103, 23, 9104);
end

if (EVENT == 9103) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9172);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9177);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9182);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9187);
	end
end

if (EVENT == 9104) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9175);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9180);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9185);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9190);
	end
end

if (EVENT == 9110) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9174);
		EVENT = 9111
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9179);
		EVENT = 9111
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9184);
		EVENT = 9111
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9189);
		EVENT = 9111
	end
end

if (EVENT == 9111) then
	SelectMsg(UID, 2, savenum, 8461, NPC, 3002, 158);
end

if (EVENT == 9106) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 40) then
		SelectMsg(UID, 2, savenum, 8462, NPC, 18, 9107);
	else
		SelectMsg(UID, 4, savenum, 8475, NPC, 41, 9108, 27, 158);
	end
end

if (EVENT == 9107) then
	ShowMap(UID, 553);
end

if (EVENT == 9108) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		GiveItem(UID, 206005003, 1)
		SaveEvent(UID, 9173);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		GiveItem(UID, 246005003, 1)
		SaveEvent(UID, 9178);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		GiveItem(UID, 266005003, 1)
		SaveEvent(UID, 9183);
		ShowEffect(UID, 300391);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		GiveItem(UID, 286005003, 1)
		SaveEvent(UID, 9188);
		ShowEffect(UID, 300391);
	end
end

savenum = 289;

if (EVENT == 9120) then -- 60 Level Troll
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9213);
		EVENT = 9121
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9218);
		EVENT = 9121
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9223);
		EVENT = 9121
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9228);
		EVENT = 9121
	end
end

if (EVENT == 9121) then
	SelectMsg(UID, 2, savenum, 8472, NPC, 4080, 158);
end

if (EVENT == 9122) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, savenum, 8480, NPC, 10, 9125);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9125) then
	SelectMsg(UID, 4, savenum, 8481, NPC, 22, 9123, 23, 9124);
end

if (EVENT == 9123) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9214);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9219);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9224);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9229);
	end
end

if (EVENT == 9124) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9217);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9222);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9227);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9232);
	end
end

if (EVENT == 9130) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9216);
		EVENT = 9131
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9221);
		EVENT = 9131
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9226);
		EVENT = 9131
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9231);
		EVENT = 9131
	end
end

if (EVENT == 9131) then
	SelectMsg(UID, 2, savenum, 8461, NPC, 29, 158);
end

if (EVENT == 9126) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8462, NPC, 18, 9127);
	else
		SelectMsg(UID, 4, savenum, 8482, NPC, 41, 9128, 27, 158);
	end
end

if (EVENT == 9127) then
	ShowMap(UID, 555);
end

if (EVENT == 9128) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		GiveItem(UID, 206003003, 1)
		SaveEvent(UID, 9215);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		GiveItem(UID, 246003003, 1)
		SaveEvent(UID, 9220);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		GiveItem(UID, 266003003, 1)
		SaveEvent(UID, 9225);
		ShowEffect(UID, 300391);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		GiveItem(UID, 286003003, 1)
		SaveEvent(UID, 9230);
		ShowEffect(UID, 300391);
	end
end

savenum = 293;

if (EVENT == 9160) then -- 60 Level Stone Golem
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9297);
		EVENT = 9161
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9302);
		EVENT = 9161
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9307);
		EVENT = 9161
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9312);
		EVENT = 9161
	end
end

if (EVENT == 9161) then
	SelectMsg(UID, 2, savenum, 8472, NPC, 4080, 158);
end

if (EVENT == 9162) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, savenum, 8486, NPC, 10, 9165);
	else
		SelectMsg(UID, 2, savenum, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9165) then
	SelectMsg(UID, 4, savenum, 8487, NPC, 22, 9163, 23, 9164);
end

if (EVENT == 9163) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9298);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9303);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9308);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9313);
	end
end

if (EVENT == 9164) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9301);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9306);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9311);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9316);
	end
end

if (EVENT == 9170) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9300);
		EVENT = 9171
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9305);
		EVENT = 9171
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9310);
		EVENT = 9171
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9315);
		EVENT = 9171
	end
end

if (EVENT == 9171) then
	SelectMsg(UID, 2, savenum, 8461, NPC, 29, 158);
end

if (EVENT == 9166) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8462, NPC, 18, 9167);
	else
		SelectMsg(UID, 4, savenum, 8488, NPC, 41, 9168, 27, 158);
	end
end

if (EVENT == 9167) then
	ShowMap(UID, 557);
end

if (EVENT == 9168) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		GiveItem(UID, 206001003, 1)
		SaveEvent(UID, 9299);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		GiveItem(UID, 246001003, 1)
		SaveEvent(UID, 9304);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		GiveItem(UID, 266001003, 1)
		SaveEvent(UID, 9309);
		ShowEffect(UID, 300391);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		GiveItem(UID, 286001003, 1)
		SaveEvent(UID, 9314);
		ShowEffect(UID, 300391);
	end
end