local Ret = 0;
local NPC = 24432;

if (EVENT == 160) then
	QuestNum = SearchQuest(UID, NPC);
	if (QuestNum == 0) then
		SelectMsg(UID, 2, -1, 8255, NPC, 10, 163);
	elseif (QuestNum > 1 and QuestNum < 100) then
		NpcMsg(UID, 8257, NPC)
	else
		EVENT = QuestNum
	end
end

if (EVENT == 163) then
	Ret = 1;
end

local savenum = 413;

if (EVENT == 1000) then
	SaveEvent(UID, 2073);
end

if (EVENT == 1002) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 798, NPC, 22, 1003, 23, 1004);
	else
		SelectMsg(UID, 2, savenum, 798, NPC, 10, 163);
	end
end

if (EVENT == 1003) then
	SaveEvent(UID, 2074);
end

if (EVENT == 1004) then
	SaveEvent(UID, 2077);
end

if (EVENT == 1010) then
	SaveEvent(UID, 2076);
end

if (EVENT == 1006) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterSub < 20) then
		SelectMsg(UID, 2, savenum, 798, NPC, 18, 1007);
	else
		SelectMsg(UID, 4, savenum, 798, NPC, 41, 1008, 27, 163);
	end
end

if (EVENT == 1007) then
	ShowMap(UID, 107);
end

if (EVENT == 1008) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 560000)
		SaveEvent(UID, 2075);
	else
		ExpChange(UID, 400000)
		SaveEvent(UID, 2075);   
	end
end

local savenum = 413;

if (EVENT == 1000) then -- 41 Lard Orc Repeatable
	SaveEvent(UID, 2073);
end

if (EVENT == 1002) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8152, NPC, 22, 1003, 23, 1004);
	else
		SelectMsg(UID, 2, savenum, 8152, NPC, 10, 163);
	end
end

if (EVENT == 1003) then
	SaveEvent(UID, 2074);
end

if (EVENT == 1004) then
	SaveEvent(UID, 2077);
end

if (EVENT == 1010) then
	SaveEvent(UID, 2076);
end

if (EVENT == 1006) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8152, NPC, 18, 1007);
	else
		SelectMsg(UID, 4, savenum, 8152, NPC, 41, 1008, 27, 163);
	end
end

if (EVENT == 1007) then
	ShowMap(UID, 107);
end

if (EVENT == 1008) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1400000)
		SaveEvent(UID, 2075);
	else
		ExpChange(UID, 1000000)
		SaveEvent(UID, 2075);   
	end
end

savenum = 143;

if (EVENT == 8750) then -- 41 Level Lard Orc
	SelectMsg(UID, 2, savenum, 8151, NPC, 10, 8751);
end

if (EVENT == 8751) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 8679);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 8684);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 8689);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 8694);
	end
end

if (EVENT == 8752) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8151, NPC, 22, 8753, 23, 8754);
	else
		SelectMsg(UID, 2, savenum, 8151, NPC, 10, 163);
	end
end

if (EVENT == 8753) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 8680);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 8685);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 8690);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 8695);
	end
end

if (EVENT == 8754) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 8683);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 8688);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 8693);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 8698);
	end
end

if (EVENT == 8760) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 8682);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 8687);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 8692);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 8697);
	end
end

if (EVENT == 8756) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8151, NPC, 18, 8757);
	else
		SelectMsg(UID, 4, savenum, 8151, NPC, 41, 8758, 23, 163);
	end
end

if (EVENT == 8757) then
	ShowMap(UID, 107);
end

if (EVENT == 8758) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		ExpChange(UID, 850000)
		GoldGain(UID, 100000)
		GiveItem(UID, 972100791, 1)
		GiveItem(UID, 972140434, 1)
		GiveItem(UID, 972170822, 1)
		GiveItem(UID, 972180736, 1)
		SaveEvent(UID, 8681);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		ExpChange(UID, 850000)
		GoldGain(UID, 100000)
		GiveItem(UID, 972110496, 1)
		GiveItem(UID, 972110496, 1)
		GiveItem(UID, 972150923, 1)
		SaveEvent(UID, 8686);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		ExpChange(UID, 850000)
		GoldGain(UID, 100000)
		GiveItem(UID, 972120358, 1)
		GiveItem(UID, 972120366, 1)
		GiveItem(UID, 972120374, 1)
		SaveEvent(UID, 8691);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		ExpChange(UID, 850000)
		GoldGain(UID, 100000)
		GiveItem(UID, 972130766, 1)
		GiveItem(UID, 972160763, 1)
		SaveEvent(UID, 8696);
	end
end

savenum = 417;

if (EVENT == 1100) then -- 43 Level Megantalion Repeatable
	SaveEvent(UID, 2097);
end

if (EVENT == 1102) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8152, NPC, 22, 1103, 23, 1104);
	else
		SelectMsg(UID, 2, savenum, 8152, NPC, 10, 163);
	end
end

if (EVENT == 1103) then
	SaveEvent(UID, 2098);
end

if (EVENT == 1104) then
	SaveEvent(UID, 2101);
end

if (EVENT == 1110) then
	SaveEvent(UID, 2100);
end

if (EVENT == 1106) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8152, NPC, 18, 1107);
	else
		SelectMsg(UID, 4, savenum, 8152, NPC, 41, 1108, 27, 163);
	end
end

if (EVENT == 1107) then
	ShowMap(UID, 107);
end

if (EVENT == 1108) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1400000)
		SaveEvent(UID, 2099);
	else
		ExpChange(UID, 1000000)
		SaveEvent(UID, 2099);   
	end
end

savenum = 149;

if (EVENT == 8950) then -- 43 Level Megantalion
	SelectMsg(UID, 2, savenum, 8151, NPC, 10, 8951);
end

if (EVENT == 8951) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 8763);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 8768);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 8773);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 8778);
	end
end

if (EVENT == 8952) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8151, NPC, 22, 8953, 23, 8954);
	else
		SelectMsg(UID, 2, savenum, 8151, NPC, 10, 163);
	end
end

if (EVENT == 8953) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 8764);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 8769);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 8774);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 8779);
	end
end

if (EVENT == 8954) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 8767);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 8772);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 8777);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 8782);
	end
end

if (EVENT == 8960) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 8766);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 8771);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 8776);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 8781);
	end
end

if (EVENT == 8956) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8151, NPC, 18, 8957);
	else
		SelectMsg(UID, 4, savenum, 8151, NPC, 41, 8958, 23, 163);
	end
end

if (EVENT == 8957) then
	ShowMap(UID, 107);
end

if (EVENT == 8958) then
	Class = CheckClass(UID);
    ExpChange(UID, 2375000)
	GoldGain(UID, 100000)
	if (Class == 1 or Class == 5 or Class == 6) then
		GiveItem(UID, 205005005, 1)
		SaveEvent(UID, 8765);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		GiveItem(UID, 245005005, 1)
		SaveEvent(UID, 8770);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		GiveItem(UID, 265005005, 1)
		SaveEvent(UID, 8775);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		GiveItem(UID, 285005005, 1)
		SaveEvent(UID, 8780);
	end
end

savenum = 421;

if (EVENT == 1200) then -- 45 Level Scolar Repeatable
	SaveEvent(UID, 2121);
end

if (EVENT == 1202) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8152, NPC, 22, 1203, 23, 1204);
	else
		SelectMsg(UID, 2, savenum, 8152, NPC, 10, 163);
	end
end

if (EVENT == 1203) then
	SaveEvent(UID, 2122);
end

if (EVENT == 1204) then
	SaveEvent(UID, 2125);
end

if (EVENT == 1210) then
	SaveEvent(UID, 2124);
end

if (EVENT == 1206) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 10) then
		SelectMsg(UID, 2, savenum, 8152, NPC, 18, 1207);
	else
		SelectMsg(UID, 4, savenum, 8152, NPC, 41, 1208, 27, 163);
	end
end

if (EVENT == 1207) then
	ShowMap(UID, 540);
end

if (EVENT == 1208) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1400000)
		SaveEvent(UID, 2123);
	else
		ExpChange(UID, 1000000)
		SaveEvent(UID, 2123);   
	end
end

savenum = 175

if (EVENT == 8170) then -- 45 Level Scolar
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 8805);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 8810);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 8815);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 8820);
	end
end

if (EVENT == 8172) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, savenum, 8390, NPC, 10, 8175);
	else
		SelectMsg(UID, 2, savenum, 8259, NPC, 10, 163);
	end
end

if (EVENT == 8175) then
	SelectMsg(UID, 4, savenum, 8391, NPC, 22, 8173, 23, 163);
end

if (EVENT == 8173) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 8806);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 8811);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 8816);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 8821);
	end
end

if (EVENT == 8180) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 8808);
		EVENT = 8181
    elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 8813);
		EVENT = 8181
    elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 8818);
		EVENT = 8181
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 8823);
		EVENT = 8181
	end
end

if (EVENT == 8181) then
	SelectMsg(UID, 2, savenum, 8375, NPC, 3007, 163);
end

if (EVENT == 8176) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 10) then
		SelectMsg(UID, 2, savenum, 8376, NPC, 18, 8177);
	else
		SelectMsg(UID, 4, savenum, 8392, NPC, 41, 8178, 27, 163);
	end
end

if (EVENT == 8177) then
	ShowMap(UID, 540);
end

if (EVENT == 8178) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		GiveItem(UID, 205001005, 1)
		GoldGain(UID, 100000)
		ExpChange(UID, 1500000)
		SaveEvent(UID, 8807);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then      
		GiveItem(UID, 245001005, 1)
		GoldGain(UID, 100000)
		ExpChange(UID, 1500000)
		SaveEvent(UID, 8812);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then    
		GiveItem(UID, 265001005, 1)
		GoldGain(UID, 100000)
		ExpChange(UID, 1500000)
		SaveEvent(UID, 8817);
		ShowEffect(UID, 300391);
    elseif (Class == 4 or Class == 11 or Class == 12) then     
		GiveItem(UID, 285001005, 1)
		GoldGain(UID, 100000)
		ExpChange(UID, 1500000)
		SaveEvent(UID, 8822);
		ShowEffect(UID, 300391);
	end	 
end

savenum = 206

if (EVENT == 9430) then -- 47 Level Macairodus
	SelectMsg(UID, 2, savenum, 8760, NPC, 10, 9431);
end

if (EVENT == 9431) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9471);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9476);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9481);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9486);
	end 
end

if (EVENT == 9432) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8760, NPC, 22, 9433, 23, 9434);
	else
		SelectMsg(UID, 2, savenum, 8760, NPC, 10, 163);
	end
end

if (EVENT == 9433) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9472);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9477);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9482);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9487);
	end
end

if (EVENT == 9434) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9475);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9480);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9485);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9490);
	end
end

if (EVENT == 9440) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9474);
		EVENT = 9441
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9479);
		EVENT = 9441
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9484);
		EVENT = 9441
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9489);
		EVENT = 9441
	end
end

if (EVENT == 9441) then
	SelectMsg(UID, 2, savenum, 8758, NPC, 3007, 163);
end

if (EVENT == 9436) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 40) then
		SelectMsg(UID, 2, savenum, 8376, NPC, 18, 9437);
	else
		SelectMsg(UID, 4, savenum, 8392, NPC, 41, 9438, 27, 163);
	end
end

if (EVENT == 9437) then
	ShowMap(UID, 619);
end

if (EVENT == 9438) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
        GiveItem(UID, 910162272, 1)
		GoldGain(UID, 100000)
		ExpChange(UID, 1900000)
		SaveEvent(UID, 9473);
	elseif (Class == 2 or Class == 7 or Class == 8) then      
        GiveItem(UID, 910163273, 1)
		GoldGain(UID, 100000)
		ExpChange(UID, 1900000)
		SaveEvent(UID, 9478);
	elseif (Class == 3 or Class == 9 or Class == 10) then
        GiveItem(UID, 910161271, 1)
		GoldGain(UID, 100000)
		ExpChange(UID, 1900000)
		SaveEvent(UID, 9483);
	elseif (Class == 4 or Class == 11 or Class == 12) then     
        GiveItem(UID, 910164274, 1)
        GiveItem(UID, 910162272, 1)
		GoldGain(UID, 100000)
		ExpChange(UID, 1900000)
		SaveEvent(UID, 9488);
	end	 
end

savenum = 227

if (EVENT == 9450) then -- 51 Level Blood Don
	SelectMsg(UID, 2, savenum, 8762, NPC, 10, 9451);
end

if (EVENT == 9451) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9513);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9518);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9523);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9528);
	end
end

if (EVENT == 9452) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8762, NPC, 22, 9453, 23, 163);
	else
		SelectMsg(UID, 2, savenum, 8762, NPC, 10, 163);
	end
end

if (EVENT == 9453) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9514);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9519);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9524);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9529);
	end
end

if (EVENT == 9460) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9516);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9521);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9526);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9531);
	end
end

if (EVENT == 9456) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8376, NPC, 18, 9457);
	else
		SelectMsg(UID, 4, savenum, 8392, NPC, 41, 9458, 27, 163);
	end
end

if (EVENT == 9457) then
	ShowMap(UID, 623);
end

if (EVENT == 9458) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
        GiveItem(UID, 910160118, 1)
		ExpChange(UID, 4000000)
		SaveEvent(UID, 9515);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then      
		GiveItem(UID, 910160118, 1)
		ExpChange(UID, 4000000)
		SaveEvent(UID, 9520);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then    
		GiveItem(UID, 910160118, 1)
		ExpChange(UID, 4000000)
		SaveEvent(UID, 9525);
		ShowEffect(UID, 300391);
	elseif (Class == 4 or Class == 11 or Class == 12) then     
		GiveItem(UID, 910160118, 1)
		ExpChange(UID, 4000000)
		SaveEvent(UID, 9530);
		ShowEffect(UID, 300391);
	end	 
end

savenum = 475;

if (EVENT == 200) then -- 56 Level Grell Repeatable
	SaveEvent(UID, 2301);
end

if (EVENT == 202) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8152, NPC, 22, 203, 23, 204);
	else
		SelectMsg(UID, 2, savenum, 8152, NPC, 10, 163);
	end
end

if (EVENT == 203) then
	SaveEvent(UID, 2302);
end

if (EVENT == 204) then
	SaveEvent(UID, 2305);
end

if (EVENT == 210) then
	SaveEvent(UID, 2304);
end

if (EVENT == 207) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 30) then
		SelectMsg(UID, 2, savenum, 8152, NPC, 18, 210);
	else
		SelectMsg(UID, 4, savenum, 8152, NPC, 41, 208, 27, 163);
	end
end

if (EVENT == 210) then
	ShowMap(UID, 330);
end

if (EVENT == 208) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 3750000)
		SaveEvent(UID, 2303);
	else
		ExpChange(UID, 2500000)
		SaveEvent(UID, 2303);   
	end
end

savenum = 265

if (EVENT == 9060) then -- 56 Level Grell
	SelectMsg(UID, 2, savenum, 8225, NPC, 3003, 9061);
end

if (EVENT == 9061) then
	SaveEvent(UID, 9117);
end

if (EVENT == 9062) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
        SelectMsg(UID, 4, savenum, 8421, NPC, 22, 9063, 23, 9064);
	else
		SelectMsg(UID, 2, savenum, 8422, NPC, 10, 163);
	end
end

if (EVENT == 9063) then
	SaveEvent(UID, 9118);
end

if (EVENT == 9064) then
	SaveEvent(UID, 9121);
end

if (EVENT == 9065) then
	SelectMsg(UID, 2, savenum, 8423, NPC, 3014, 163);
	SaveEvent(UID, 9120);
end

if (EVENT == 9067) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8424, NPC, 18, 9068);
	else
		SelectMsg(UID, 4, savenum, 8425, NPC, 41, 9069, 27, 163);
	end
end

if (EVENT == 9068) then
	ShowMap(UID, 330);
end

if (EVENT == 9069) then
	GiveItem(UID, 379155000, 1)
	ExpChange(UID, 30000000)
	SaveEvent(UID, 9119);
end

savenum = 478;

if (EVENT == 300) then -- 57 Level Hell Hound Repeatable
	SaveEvent(UID, 2325);
end

if (EVENT == 302) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8152, NPC, 22, 303, 23, 304);
	else
		SelectMsg(UID, 2, savenum, 8152, NPC, 10, 163);
	end
end

if (EVENT == 303) then
	SaveEvent(UID, 2326);
end

if (EVENT == 304) then
	SaveEvent(UID, 2329);
end

if (EVENT == 210) then
	SaveEvent(UID, 2328);
end

if (EVENT == 307) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 30) then
		SelectMsg(UID, 2, savenum, 8152, NPC, 18, 210);
	else
		SelectMsg(UID, 4, savenum, 8152, NPC, 41, 308, 27, 163);
	end
end

if (EVENT == 310) then
	ShowMap(UID, 37);
end

if (EVENT == 308) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 3750000)
		SaveEvent(UID, 2327);
	else
		ExpChange(UID, 2500000)
		SaveEvent(UID, 2327);   
	end
end

savenum = 267

if (EVENT == 9320) then -- 57 Level Hell Hound
	SelectMsg(UID, 2, savenum, 8676, NPC, 10, 9321);
end

if (EVENT == 9321) then
	SaveEvent(UID, 9339);
end

if (EVENT == 9322) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8676, NPC, 22, 9323, 23, 9324);
	else
		SelectMsg(UID, 2, savenum, 8676, NPC, 10, 163);
	end
end

if (EVENT == 9323) then
	SaveEvent(UID, 9340);
end

if (EVENT == 9324) then
	SaveEvent(UID, 9343);
end

if (EVENT == 9325) then
	SaveEvent(UID, 9342);
end

if (EVENT == 9327) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, savenum, 8551, NPC, 18, 9328);
	else
		SelectMsg(UID, 4, savenum, 8425, NPC, 41, 9329, 27, 163);
	end
end

if (EVENT == 9328) then
	ShowMap(UID, 37);
end

if (EVENT == 9329) then
	ExpChange(UID, 12000000)
	SaveEvent(UID, 9341); 
end

savenum = 897

if (EVENT == 9340) then -- 59 Level Manticore
	SelectMsg(UID, 2, savenum, 8680, NPC, 10, 9341);
end

if (EVENT == 9341) then
	SaveEvent(UID, 9363);
end

if (EVENT == 9342) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8680, NPC, 22, 9343, 23, 9344);
	else
		SelectMsg(UID, 2, savenum, 8680, NPC, 10, 163);
	end
end

if (EVENT == 9343) then
	SaveEvent(UID, 9364);
end

if (EVENT == 9344) then
	SaveEvent(UID, 9367);
end

if (EVENT == 9345) then
	SaveEvent(UID, 9366);
end

if (EVENT == 9347) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 40) then
		SelectMsg(UID, 2, savenum, 8565, NPC, 18, 9348);
	else
		SelectMsg(UID, 4, savenum, 8425, NPC, 41, 9349, 27, 163);
	end
end

if (EVENT == 9348) then
	ShowMap(UID, 704);
end

if (EVENT == 9349) then
	ExpChange(UID, 13000000)
	SaveEvent(UID, 9365); 
end

savenum = 291

if (EVENT == 9140) then -- 60 Level Lich
	SelectMsg(UID, 2, savenum, 8207, NPC, 4244, 9141);
end

if (EVENT == 9141) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9255);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9260);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9265);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9270);
	end
end

if (EVENT == 9150) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8435, NPC, 3018, 9152, 3019, 9159);
	else
		SelectMsg(UID, 2, savenum, 8436, NPC, 4242, 163);
	end
end

if (EVENT == 9152) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9256);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9261);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9266);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9271);
	end
end

if (EVENT == 9159) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9259);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9264);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9269);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9274);
	end
end

if (EVENT == 9153) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		SaveEvent(UID, 9258);
		EVENT = 9154
	elseif (Class == 2 or Class == 7 or Class == 8) then
		SaveEvent(UID, 9263);
		EVENT = 9154
	elseif (Class == 3 or Class == 9 or Class == 10) then
		SaveEvent(UID, 9268);
		EVENT = 9154
	elseif (Class == 4 or Class == 11 or Class == 12) then
		SaveEvent(UID, 9273);
		EVENT = 9154
	end
end

if (EVENT == 9154) then
	SelectMsg(UID, 2, savenum, 8434, NPC, 57, 163);
end

if (EVENT == 9155) then
	MonsterCount = CountMonsterQuestSub(UID, savenum, 1);
	if (MonsterCount < 10) then
		SelectMsg(UID, 2, savenum, 8437, NPC, 18, 9157);
	else
		SelectMsg(UID, 4, savenum, 8438, NPC, 41, 9158, 27, 163);
	end
end

if (EVENT == 9157) then
	ShowMap(UID, 17);
end

if (EVENT == 9158) then
    Class = CheckClass(UID);
    if (Class == 1 or Class == 5 or Class == 6) then
		GiveItem(UID, 206002003, 1)
		SaveEvent(UID, 9257);
		ShowEffect(UID, 300391)
    elseif (Class == 2 or Class == 7 or Class == 8) then
		GiveItem(UID, 246002003, 1)
		SaveEvent(UID, 9262);
		ShowEffect(UID, 300391)
    elseif (Class == 3 or Class == 9 or Class == 10) then
		GiveItem(UID, 266002003, 1)
		SaveEvent(UID, 9267);
		ShowEffect(UID, 300391)
    elseif (Class == 4 or Class == 11 or Class == 12) then
		GiveItem(UID, 286002003, 1)
		SaveEvent(UID, 9272);
		ShowEffect(UID, 300391)
	end	 
end