local Ret = 0;
local NPC = 24431;

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

if (EVENT == 8250) then -- 50 Level Lamia
	SelectMsg(UID, 2, 218, 8231, NPC, 28, 8251);
end

if (EVENT == 8251) then
	ShowMap(UID, 562);
	SaveEvent(UID, 9009);
end

if (EVENT == 8252) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, 218, 8232, NPC, 10, 8260);
	else
		SelectMsg(UID, 2, 218, 8261, NPC, 10, 158);
	end
end

if (EVENT == 8260) then
	SelectMsg(UID, 4, 218, 8233, NPC, 22, 8253, 23, 8254);
end

if (EVENT == 8253) then
	SaveEvent(UID, 9010);
end

if (EVENT == 8254) then
	SaveEvent(UID, 9013);
end

if (EVENT == 8255) then
	SelectMsg(UID, 2, 218, 8262, NPC, 3007, 158);
	SaveEvent(UID, 9012);
end

if (EVENT == 8257) then
	MonsterCount = CountMonsterQuestSub(UID, 218, 1);
	if (MonsterCount < 40) then
		SelectMsg(UID, 2, 218, 8263, NPC, 18, 8258);
	else
		SelectMsg(UID, 4, 218, 8264, NPC, 41, 8259, 27, 158);
	end
end

if (EVENT == 8258) then
	ShowMap(UID, 182);
end

if (EVENT == 8259) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		ExpChange(UID, 5000000)
		GiveItem(UID, 925003595, 1)
		SaveEvent(UID, 9011);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		ExpChange(UID, 5000000)
		GiveItem(UID, 925008596, 1)
		SaveEvent(UID, 9011);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		ExpChange(UID, 5000000)
		GiveItem(UID, 926003597, 1)
		SaveEvent(UID, 9011);
		ShowEffect(UID, 300391);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		ExpChange(UID, 5000000)
		GiveItem(UID, 926008598, 1)
		SaveEvent(UID, 9011);
		ShowEffect(UID, 300391);
	end
end

if (EVENT == 8350) then -- 50 Level Uruk Hai
	SelectMsg(UID, 2, 223, 8271, NPC, 14, 8351);
end

if (EVENT == 8351) then
	SaveEvent(UID, 9021);
end

if (EVENT == 8352) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, 223, 8272, NPC, 10, 8360);
	else
		SelectMsg(UID, 2, 223, 8261, NPC, 10, 158);
	end
end

if (EVENT == 8360) then
	SelectMsg(UID, 4, 223, 8273, NPC, 22, 8353, 23, 8354);
end

if (EVENT == 8353) then
	SaveEvent(UID, 9022);
end

if (EVENT == 8354) then
	SaveEvent(UID, 9025);
end

if (EVENT == 8355) then
	SelectMsg(UID, 2, 223, 8262, NPC, 3007, 158);
	SaveEvent(UID, 9024);
end

if (EVENT == 8357) then
	MonsterCount = CountMonsterQuestSub(UID, 223, 1);
	if (MonsterCount < 40) then
		SelectMsg(UID, 2, 223, 8263, NPC, 18, 8358);
	else
		SelectMsg(UID, 4, 223, 8264, NPC, 41, 8359, 27, 158);
	end
end

if (EVENT == 8358) then
	ShowMap(UID, 544);
end

if (EVENT == 8359) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		ExpChange(UID, 5500000)
		GiveItem(UID, 925004595, 1)
		SaveEvent(UID, 9023);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		ExpChange(UID, 5500000)
		GiveItem(UID, 925009596, 1)
		SaveEvent(UID, 9023);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		ExpChange(UID, 5500000)
		GiveItem(UID, 926004597, 1)
		SaveEvent(UID, 9023);
		ShowEffect(UID, 300391);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		ExpChange(UID, 5500000)
		GiveItem(UID, 926009598, 1)
		SaveEvent(UID, 9023);
		ShowEffect(UID, 300391);
	end	 
end

if (EVENT == 8650) then -- 53 Level Treant
	SelectMsg(UID, 2, 233, 8277, NPC, 3008, 8651);
end

if (EVENT == 8651) then
	SelectMsg(UID, 2, 233, 8278, NPC, 4080, 158);
	SaveEvent(UID, 9057);
end

if (EVENT == 8660) then
	SelectMsg(UID, 2, 233, 8284, NPC, 3002, 8661);
end

if (EVENT == 8661) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, 233, 8285, NPC, 3018, 8662, 3019, 8668);
	else
		SelectMsg(UID, 2, 233, 8287, NPC, 10, 158);
	end
end

if (EVENT == 8662) then
	SaveEvent(UID, 9058);
end

if (EVENT == 8668) then
	SaveEvent(UID, 9061);
end

if (EVENT == 8663) then
	SelectMsg(UID, 2, 233, 8288, NPC, 4080, 158);
	SaveEvent(UID, 9060);
end

if (EVENT == 8665) then
	MonsterCount = CountMonsterQuestSub(UID, 233, 1);
	if (MonsterCount < 40) then
		SelectMsg(UID, 2, 233, 8289, NPC, 18, 8669);
	else
		SelectMsg(UID, 4, 233, 8290, NPC, 41, 8667, 27, 158);
	end
end

if (EVENT == 8669) then
	ShowMap(UID, 506);
end

if (EVENT == 8667) then
	GiveItem(UID, 389155000, 5)
	GiveItem(UID, 389156000, 5)
	ExpChange(UID, 6000000)
	SaveEvent(UID, 9059);
end

if (EVENT == 9180) then -- 54 Level Ancient
	SelectMsg(UID, 2, 237, 8271, NPC, 14, 9181);
end

if (EVENT == 9181) then
	SaveEvent(UID, 9069);
end

if (EVENT == 9182) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, 237, 8303, NPC, 10, 9190);
	else
		SelectMsg(UID, 2, 237, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9190) then
	SelectMsg(UID, 4, 237, 8304, NPC, 22, 9183, 23, 9184);
end

if (EVENT == 9183) then
	SaveEvent(UID, 9070);
end

if (EVENT == 9184) then
	SaveEvent(UID, 9073);
end

if (EVENT == 9185) then
	SelectMsg(UID, 2, 237, 8262, NPC, 3007, 158);
	SaveEvent(UID, 9072);
end

if (EVENT == 9187) then
	MonsterCount = CountMonsterQuestSub(UID, 237, 1);
	if (MonsterCount < 40) then
		SelectMsg(UID, 2, 237, 8263, NPC, 18, 9188);
	else
		SelectMsg(UID, 4, 237, 8264, NPC, 41, 9189, 27, 158);
	end
end

if (EVENT == 9188) then
	ShowMap(UID, 547);
end

if (EVENT == 9189) then
	Class = CheckClass(UID);
	if (Class == 1 or Class == 5 or Class == 6) then
		ExpChange(UID, 6500000)
		GiveItem(UID, 925000595, 1)
		SaveEvent(UID, 9071);
		ShowEffect(UID, 300391);
	elseif (Class == 2 or Class == 7 or Class == 8) then
		ExpChange(UID, 6500000)
		GiveItem(UID, 925005596, 1)
		SaveEvent(UID, 9071);
		ShowEffect(UID, 300391);
	elseif (Class == 3 or Class == 9 or Class == 10) then
		ExpChange(UID, 6500000)
		GiveItem(UID, 926000597, 1)
		SaveEvent(UID, 9071);
		ShowEffect(UID, 300391);
	elseif (Class == 4 or Class == 11 or Class == 12) then
		ExpChange(UID, 6500000)
		GiveItem(UID, 926005598, 1)
		SaveEvent(UID, 9071);
		ShowEffect(UID, 300391);
	end	 
end

if (EVENT == 9020) then -- 55 Level Dragon Tooth Commander
	SelectMsg(UID, 2, 263, 8271, NPC, 14, 9021);
end

if (EVENT == 9021) then
	SaveEvent(UID, 9093);
end

if (EVENT == 9022) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, 263, 8444, NPC, 10, 9030);
	else
		SelectMsg(UID, 2, 263, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9030) then
	SelectMsg(UID, 4, 263, 8445, NPC, 22, 9023, 23, 9024);
end

if (EVENT == 9023) then
	SaveEvent(UID, 9094);
end

if (EVENT == 9024) then
	SaveEvent(UID, 9097);
end

if (EVENT == 9025) then
	SelectMsg(UID, 2, 263, 8262, NPC, 3007, 158);
	SaveEvent(UID, 9096);
end

if (EVENT == 9027) then
	MonsterCount = CountMonsterQuestSub(UID, 263, 1);
	if (MonsterCount < 30) then
		SelectMsg(UID, 2, 263, 8263, NPC, 18, 9028);
	else
		SelectMsg(UID, 4, 263, 8450, NPC, 41, 9029, 27, 158);
	end
end

if (EVENT == 9028) then
	ShowMap(UID, 587);
end

if (EVENT == 9029) then
	ExpChange(UID, 10000000)
	SaveEvent(UID, 9095);   
end

if (EVENT == 9040) then -- 56 Level Uruk Blade
	SelectMsg(UID, 2, 263, 8271, NPC, 14, 9041);
end

if (EVENT == 9041) then
	SaveEvent(UID, 9105);
end

if (EVENT == 9042) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, 263, 8454, NPC, 10, 9050);
	else
		SelectMsg(UID, 2, 263, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9050) then
	SelectMsg(UID, 4, 263, 8455, NPC, 22, 9043, 23, 9044);
end

if (EVENT == 9043) then
	SaveEvent(UID, 9106);
end

if (EVENT == 9044) then
	SaveEvent(UID, 9109);
end

if (EVENT == 9045) then
	SelectMsg(UID, 2, 263, 8262, NPC, 3007, 158);
	SaveEvent(UID, 9108);
end

if (EVENT == 9047) then
	MonsterCount = CountMonsterQuestSub(UID, 263, 1);
	if (MonsterCount < 40) then
		SelectMsg(UID, 2, 263, 8263, NPC, 18, 9048);
	else
		SelectMsg(UID, 4, 263, 8264, NPC, 41, 9049, 27, 158);
	end
end

if (EVENT == 9048) then
	ShowMap(UID, 551);
end

if (EVENT == 9049) then
	ExpChange(UID, 10000000)
	SaveEvent(UID, 9107);	 
end

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
	SelectMsg(UID, 2, 285, 8458, NPC, 4080, 158);
end

if (EVENT == 9082) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, 285, 8459, NPC, 10, 9085);
	else
		SelectMsg(UID, 2, 285, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9085) then
	SelectMsg(UID, 4, 285, 8460, NPC, 22, 9083, 23, 9084);
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
	SelectMsg(UID, 2, 285, 8461, NPC, 3002, 158);
end

if (EVENT == 9086) then
	MonsterCount = CountMonsterQuestSub(UID, 285, 1);
	if (MonsterCount < 40) then
		SelectMsg(UID, 2, 285, 8462, NPC, 18, 9087);
	else
		SelectMsg(UID, 4, 285, 8463, NPC, 41, 9088, 27, 158);
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
	SelectMsg(UID, 2, 287, 8472, NPC, 4080, 158);
end

if (EVENT == 9102) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, 287, 8473, NPC, 10, 9105);
	else
		SelectMsg(UID, 2, 287, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9105) then
	SelectMsg(UID, 4, 287, 8474, NPC, 22, 9103, 23, 9104);
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
	SelectMsg(UID, 2, 287, 8461, NPC, 3002, 158);
end

if (EVENT == 9106) then
	MonsterCount = CountMonsterQuestSub(UID, 287, 1);
	if (MonsterCount < 40) then
		SelectMsg(UID, 2, 287, 8462, NPC, 18, 9107);
	else
		SelectMsg(UID, 4, 287, 8475, NPC, 41, 9108, 27, 158);
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
	SelectMsg(UID, 2, 289, 8472, NPC, 4080, 158);
end

local MonsterSub = 0;

if (EVENT == 9122) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, 289, 8480, NPC, 10, 9125);
	else
		SelectMsg(UID, 2, 289, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9125) then
	SelectMsg(UID, 4, 289, 8481, NPC, 22, 9123, 23, 9124);
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
	SelectMsg(UID, 2, 289, 8461, NPC, 29, 158);
end

if (EVENT == 9126) then
	MonsterCount = CountMonsterQuestSub(UID, 289, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, 289, 8462, NPC, 18, 9127);
	else
		SelectMsg(UID, 4, 289, 8482, NPC, 41, 9128, 27, 158);
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
	SelectMsg(UID, 2, 293, 8472, NPC, 4080, 158);
end

if (EVENT == 9162) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 2, 293, 8486, NPC, 10, 9165);
	else
		SelectMsg(UID, 2, 293, 8261, NPC, 10, 158);
	end
end

if (EVENT == 9165) then
	SelectMsg(UID, 4, 293, 8487, NPC, 22, 9163, 23, 9164);
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
	SelectMsg(UID, 2, 293, 8461, NPC, 29, 158);
end

if (EVENT == 9166) then
	MonsterCount = CountMonsterQuestSub(UID, 293, 1);
	if (MonsterCount < 20) then
		SelectMsg(UID, 2, 293, 8462, NPC, 18, 9167);
	else
		SelectMsg(UID, 4, 293, 8488, NPC, 41, 9168, 27, 158);
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