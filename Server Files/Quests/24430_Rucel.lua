local QuestNum;
local Ret = 0;
local NPC = 24430;
local savenum = -1;

if EVENT == 150 then
  QuestNum = SearchQuest(UID, NPC);
  if QuestNum == 0 then
    SelectMsg(UID, 2, -1, 8199, NPC, 10, 153);
    Ret = 1;
  elseif QuestNum > 1 and  QuestNum < 100 then
    NpcMsg(UID, 8201, NPC)
  else
    EVENT = QuestNum
  end
end

if EVENT == 153 then
  Ret = 1;
end

savenum = 423;

if (EVENT == 1100) then -- 45 Level Kongau Repeatable
	SaveEvent(UID, 2133);
end

if (EVENT == 1102) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8398, NPC, 22, 1103, 23, 1104);
	else
		SelectMsg(UID, 2, savenum, 8399, NPC, 10, 163);
	end
end

if (EVENT == 1103) then
	SaveEvent(UID, 2134);
end

if (EVENT == 1104) then
	SaveEvent(UID, 2137);
end

if (EVENT == 1110) then
	SaveEvent(UID, 2136);
end

if (EVENT == 1106) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8402, NPC, 18, 1107);
	else
		SelectMsg(UID, 4, savenum, 8403, NPC, 41, 1108, 27, 163);
	end
end

if (EVENT == 1107) then
	ShowMap(UID, 512);
end

if (EVENT == 1108) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1400000)
		SaveEvent(UID, 2135);
	else
		ExpChange(UID, 1000000)
		SaveEvent(UID, 2135);   
	end
end

-- Lv 45 Kongau
savenum = 180;

if EVENT == 8270 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 8847);
    EVENT = 8271
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 8852);
    EVENT = 8271
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 8857);
    EVENT = 8271
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 8862);
    EVENT = 8271
  end
end

if EVENT == 8271 then
  SelectMsg(UID, 1, savenum, 8397, NPC, 3013, 8279);
end

if EVENT == 8279 then
  ShowMap(UID, 560);
end

if EVENT == 8272 then
  MonsterSub = ExistMonsterQuestSub(UID);
  if MonsterSub == 0 then
    SelectMsg(UID, 2, savenum, 8398, NPC, 10, 8275);
  else
    SelectMsg(UID, 2, savenum, 8399, NPC, 10, 153);
  end
end

if EVENT == 8275 then
  SelectMsg(UID, 4, savenum, 8400, NPC, 22, 8273, 23, 8274);
end

if EVENT == 8273 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 8848);
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 8853);
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 8858);
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 8863);
  end
end

if EVENT == 8274 then
  SaveEvent(UID, 8851);
end

if EVENT == 8280 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 8850);
    EVENT = 8281
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 8855);
    EVENT = 8281
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 8860);
    EVENT = 8281
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 8865);
    EVENT = 8281
  end
end

if EVENT == 8281 then
  SelectMsg(UID, 1, savenum, 8401, NPC, 3002, 153);
end

if EVENT == 8276 then
  CountMonster = CountMonsterQuestSub(UID, savenum, 1);
  if  CountMonster < 20 then
    SelectMsg(UID, 2, savenum, 8402, NPC, 10, 8277);
  else
    SelectMsg(UID, 4, savenum, 8403, NPC, 10, 8278, 27, 153);
  end
end

if EVENT == 8277 then
  ShowMap(UID, 512);
end

if EVENT == 8278 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    Check = CheckExchange(UID, 989)
    if  Check == true then
      RunExchange(UID, 989);
      SaveEvent(UID, 8849);
      SaveEvent(UID, 8870);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 2 or Class == 7 or Class == 8 then
    Check = CheckExchange(UID, 990)
    if  Check == true then
      RunExchange(UID, 990);
      SaveEvent(UID, 8854);
      SaveEvent(UID, 8870);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 3 or Class == 9 or Class == 10 then
    Check = CheckExchange(UID, 991)
    if  Check == true then
      RunExchange(UID, 991);
      SaveEvent(UID, 8859);
      SaveEvent(UID, 8870);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 4 or Class == 11 or Class == 12 then
    Check = CheckExchange(UID, 992)
    if  Check == true then
      RunExchange(UID, 992);
      SaveEvent(UID, 8864);
      SaveEvent(UID, 8870);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  end
end

savenum = 419;

if (EVENT == 1000) then -- 44 Ape Repeatable
	SaveEvent(UID, 2109);
end

if (EVENT == 1010) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8181, NPC, 22, 1011, 23, 1012);
	else
		SelectMsg(UID, 2, savenum, 8182, NPC, 10, 153);
	end
end

if (EVENT == 1011) then
	SaveEvent(UID, 2110);
end

if (EVENT == 1012) then
	SaveEvent(UID, 2113);
end

if (EVENT == 1013) then
	SaveEvent(UID, 2112);
end

if (EVENT == 1015) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8182, NPC, 18, 1016);
	else
		SelectMsg(UID, 4, savenum, 8411, NPC, 41, 1017, 27, 153);
	end
end

if (EVENT == 1016) then
	ShowMap(UID, 542);
end

if (EVENT == 1017) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1400000)
		SaveEvent(UID, 2111);
	else
		ExpChange(UID, 1000000)
		SaveEvent(UID, 2111);   
	end
end

-- Lv 44 Ape
savenum = 152;

if EVENT == 8600 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 8889);
    EVENT = 8601
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 8894);
    EVENT = 8601
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 8899);
    EVENT = 8601
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 8904);
    EVENT = 8601
  end
end

if EVENT == 8601 then
  SelectMsg(UID, 1, savenum, 8175, NPC, 3002, 8602);
end

if EVENT == 8602 then
  SelectMsg(UID, 1, savenum, 8176, NPC, 4241, 8603);
end

if EVENT == 8603 then
  MonsterSub = ExistMonsterQuestSub(UID);
  if MonsterSub == 0 then
    SelectMsg(UID, 1, savenum, 8177, NPC, 10, 8604);
  else
    SelectMsg(UID, 1, savenum, 8178, NPC, 10, 153);
  end
end

if EVENT == 8604 then
  SelectMsg(UID, 1, savenum, 8179, NPC, 22, 8605, 23, 8606);
end

if EVENT == 8605 then
  ShowMap(UID, 542);
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 8890);
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 8895);
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 8900);
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 8905);
  end
end

if EVENT == 8606 then
  SelectMsg(UID, 1, savenum, 8492, NPC, 10, -1);
  SaveEvent(UID, 8893);
end

if EVENT == 8610 then
  SelectMsg(UID, 2, savenum, 8201, NPC, 3002, 8611);
end

if EVENT == 8611 then
  MonsterSub = ExistMonsterQuestSub(UID);
  if MonsterSub == 0 then
    SelectMsg(UID, 4, savenum, 8181, NPC, 3018, 8612, 3019, 8619);
  else
    SelectMsg(UID, 2, savenum, 8182, NPC, 4242, 153);
  end
end

if EVENT == 8612 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 8890);
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 8895);
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 8900);
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 8905);
  end
end

if EVENT == 8619 then
  SaveEvent(UID, 8893);
end

if EVENT == 8613 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 8892);
    EVENT = 8614
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 8897);
    EVENT = 8614
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 8902);
    EVENT = 8614
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 8907);
    EVENT = 8614
  end
end

if EVENT == 8614 then
  SelectMsg(UID, 1, savenum, 8401, NPC, 4080, 153);
end

if EVENT == 8615 then
  CountMonster = CountMonsterQuestSub(UID, savenum, 1);
  if CountMonster < 20 then
    SelectMsg(UID, 2, savenum, 8182, NPC, 10, 8617);
  else
    SelectMsg(UID, 4, savenum, 8411, NPC, 10, 8618, 27, 153);
  end
end

if EVENT == 8617 then
  ShowMap(UID, 542);
end

if EVENT == 8618 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    Check = CheckExchange(UID, 928)
    if  Check == true then
      RunExchange(UID, 928);
      SaveEvent(UID, 8891);
      SaveEvent(UID, 8912);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 2 or Class == 7 or Class == 8 then
    Check = CheckExchange(UID, 929)
    if  Check == true then
      RunExchange(UID, 929);
      SaveEvent(UID, 8896);
      SaveEvent(UID, 8912);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 3 or Class == 9 or Class == 10 then
    Check = CheckExchange(UID, 930)
    if  Check == true then
      RunExchange(UID, 930);
      SaveEvent(UID, 8901);
      SaveEvent(UID, 8912);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 4 or Class == 11 or Class == 12 then
    Check = CheckExchange(UID, 931)
    if  Check == true then
      RunExchange(UID, 931);
      SaveEvent(UID, 8906);
      SaveEvent(UID, 8912);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  end
end

savenum = 425;

if (EVENT == 1200) then -- 46 Level Burning Skeleton Repeatable
	SaveEvent(UID, 2145);
end

if (EVENT == 1202) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8192, NPC, 22, 1211, 23, 1212);
	else
		SelectMsg(UID, 2, savenum, 8399, NPC, 10, 153);
	end
end

if (EVENT == 1211) then
	SaveEvent(UID, 2146);
end

if (EVENT == 1212) then
	SaveEvent(UID, 2149);
end

if (EVENT == 1213) then
	SaveEvent(UID, 2148);
end

if (EVENT == 1206) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8402, NPC, 18, 1216);
	else
		SelectMsg(UID, 4, savenum, 8194, NPC, 41, 1217, 27, 153);
	end
end

if (EVENT == 1216) then
	ShowMap(UID, 510);
end

if (EVENT == 1217) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1400000)
		SaveEvent(UID, 2147);
	else
		ExpChange(UID, 1000000)
		SaveEvent(UID, 2147);   
	end
end

-- Lv 46 Burning skeleton
savenum = 197;

if EVENT == 8500 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 8931);
    EVENT = 8501
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 8936);
    EVENT = 8501
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 8941);
    EVENT = 8501
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 8946);
    EVENT = 8501
  end
end

if EVENT == 8501 then
  SelectMsg(UID, 1, savenum, 8191, NPC, 56, 153);
end

if EVENT == 8502 then
  MonsterSub = ExistMonsterQuestSub(UID);
  if MonsterSub == 0 then
    SelectMsg(UID, 2, savenum, 8192, NPC, 10, 8505);
  else
    SelectMsg(UID, 2, savenum, 8399, NPC, 10, 153);
  end
end

if EVENT == 8505 then
  SelectMsg(UID, 4, savenum, 8193, NPC, 22, 8503, 23, 8504);
end

if EVENT == 8503 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 8932);
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 8937);
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 8942);
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 8947);
  end
end

if EVENT == 8504 then
  SaveEvent(UID, 8935);
end

if EVENT == 8510 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 8934);
    EVENT = 8511
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 8939);
    EVENT = 8511
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 8944);
    EVENT = 8511
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 8949);
    EVENT = 8511
  end
end

if EVENT == 8511 then
  SelectMsg(UID, 1, savenum, 8180, NPC, 3007, 153);
end

if EVENT == 8506 then
  CountMonster = CountMonsterQuestSub(UID, savenum, 1);
  if CountMonster < 20 then
    SelectMsg(UID, 2, savenum, 8402, NPC, 10, 8507);
  else
    Class = CheckClass(UID);
    if Class == 1 or Class == 5 or Class == 6 or Class == 3 or Class == 9 or Class == 10 or Class == 4 or Class == 11 or Class == 12 then
      SelectMsg(UID, 4, savenum, 8194, NPC, 10, 8508, 27, 153);
    elseif Class == 2 or Class == 7 or Class == 8 then
      SelectMsg(UID, 5, savenum, 8171, NPC, 10, 8508, 27, 153);
    end
  end
end

if EVENT == 8507 then
  ShowMap(UID, 510);
end

if EVENT == 8508 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    Check = CheckExchange(UID, 920)
    if  Check == true then
      RunExchange(UID, 920);
      SaveEvent(UID, 8933);
      SaveEvent(UID, 8954);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 2 or Class == 7 or Class == 8 then
    Check = CheckExchange(UID, 921)
    if  Check == true then
      RunExchange(UID, 921);
      SaveEvent(UID, 8938);
      SaveEvent(UID, 8954);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 3 or Class == 9 or Class == 10 then
    Check = CheckExchange(UID, 922)
    if  Check == true then
      RunExchange(UID, 922);
      SaveEvent(UID, 8943);
      SaveEvent(UID, 8954);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 4 or Class == 11 or Class == 12 then
    Check = CheckExchange(UID, 923)
    if  Check == true then
      RunExchange(UID, 923);
      SaveEvent(UID, 8948);
      SaveEvent(UID, 8954);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  end
end

savenum = 429;

if (EVENT == 1300) then -- 48 Level Ash Knights Repeatable
	SaveEvent(UID, 2169);
end

if (EVENT == 1302) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8155, NPC, 22, 1311, 23, 1312);
	else
		SelectMsg(UID, 2, savenum, 8399, NPC, 10, 153);
	end
end

if (EVENT == 1311) then
	SaveEvent(UID, 2170);
end

if (EVENT == 1312) then
	SaveEvent(UID, 2173);
end

if (EVENT == 1313) then
	SaveEvent(UID, 2172);
end

if (EVENT == 1307) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8402, NPC, 18, 1316);
	else
		SelectMsg(UID, 4, savenum, 8214, NPC, 41, 1317, 27, 153);
	end
end

if (EVENT == 1316) then
	ShowMap(UID, 504);
end

if (EVENT == 1317) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1400000)
		SaveEvent(UID, 2171);
	else
		ExpChange(UID, 1000000)
		SaveEvent(UID, 2171);   
	end
end

-- Lv 48 Ash knights
savenum = 208;

if EVENT == 8070 then
  SelectMsg(UID, 1, savenum, 8147, NPC, 14, 8351);
end

if EVENT == 8071 then
  SaveEvent(UID, 8985);
end

if EVENT == 8072 then
  MonsterSub = ExistMonsterQuestSub(UID);
  if MonsterSub == 0 then
    SelectMsg(UID, 2, savenum, 8155, NPC, 10, 8080);
  else
    SelectMsg(UID, 2, savenum, 8399, NPC, 10, 153);
  end
end

if EVENT == 8080 then
  SelectMsg(UID, 4, savenum, 8173, NPC, 22, 8073, 23, 8074);
end

if EVENT == 8073 then
  SaveEvent(UID, 8986);
end

if EVENT == 8074 then
  SaveEvent(UID, 8989);
end

if EVENT == 8075 then
  SelectMsg(UID, 1, 818, 8213, NPC, 3014, 153);
  SaveEvent(UID, 8988);
end

if EVENT == 8077 then
  CountMonster = CountMonsterQuestSub(UID, savenum, 1);
  if CountMonster < 20 then
    SelectMsg(UID, 2, savenum, 8402, NPC, 10, 8078);
  else
    SelectMsg(UID, 4, savenum, 8214, NPC, 10, 8079, 27, 153);
  end
end

if EVENT == 8078 then
  ShowMap(UID, 504);
end

if EVENT == 8079 then
  Check = CheckExchange(UID, 818)
  if  Check == true then
    RunExchange(UID, 818);
    SaveEvent(UID, 8987);
    SaveEvent(UID, 8993);
  else
    Ret = 1;
  end
end

savenum = 431;

if (EVENT == 1400) then -- 49 Level Haunga Repeatable
	SaveEvent(UID, 2181);
end

if (EVENT == 1402) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8218, NPC, 22, 1411, 23, 1412);
	else
		SelectMsg(UID, 2, savenum, 8399, NPC, 10, 153);
	end
end

if (EVENT == 1411) then
	SaveEvent(UID, 2182);
end

if (EVENT == 1412) then
	SaveEvent(UID, 2185);
end

if (EVENT == 1413) then
	SaveEvent(UID, 2184);
end

if (EVENT == 1407) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8402, NPC, 18, 1416);
	else
		SelectMsg(UID, 4, savenum, 8220, NPC, 41, 1417, 27, 153);
	end
end

if (EVENT == 1416) then
	ShowMap(UID, 516);
end

if (EVENT == 1417) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1400000)
		SaveEvent(UID, 2183);
	else
		ExpChange(UID, 1000000)
		SaveEvent(UID, 2183);   
	end
end

-- lv 49 Haunga
savenum = 212;

if EVENT == 8150 then
  SelectMsg(UID, 1, savenum, 8217, NPC, 3002, 8151);
end

if EVENT == 8151 then
  SaveEvent(UID, 8997);
end

if EVENT == 8152 then
  MonsterSub = ExistMonsterQuestSub(UID);
  if MonsterSub == 0 then
    SelectMsg(UID, 2, savenum, 8218, NPC, 10, 8160);
  else
    SelectMsg(UID, 2, savenum, 8399, NPC, 10, 153);
  end
end

if EVENT == 8160 then
  SelectMsg(UID, 4, savenum, 8219, NPC, 22, 8153, 23, 8154);
end

if EVENT == 8153 then
  SaveEvent(UID, 8998);
end

if EVENT == 8154 then
  SaveEvent(UID, 9001);
end

if EVENT == 8155 then
  SelectMsg(UID, 1, savenum, 8213, NPC, 3014, 153);
  SaveEvent(UID, 9000);
end

if EVENT == 8157 then
  CountMonster = CountMonsterQuestSub(UID, savenum, 1);
  if CountMonster < 20 then
    SelectMsg(UID, 2, savenum, 8402, NPC, 10, 8158);
  else
    SelectMsg(UID, 4, savenum, 8220, NPC, 10, 8159, 27, 153);
  end
end

if EVENT == 8158 then
  ShowMap(UID, 516);
end

if EVENT == 8159 then
  Check = CheckExchange(UID, 948)
  if  Check == true then
    RunExchange(UID, 948);
    SaveEvent(UID, 8999);
    SaveEvent(UID, 9005);
  else
    Ret = 1;
  end
end

savenum = 463;

if (EVENT == 300) then -- 53 Level Dragon Tooth Soldier Repeatable
	SaveEvent(UID, 2229);
end

if (EVENT == 302) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8001, NPC, 22, 311, 23, 312);
	else
		SelectMsg(UID, 2, savenum, 8399, NPC, 10, 153);
	end
end

if (EVENT == 311) then
	SaveEvent(UID, 2230);
end

if (EVENT == 312) then
	SaveEvent(UID, 2233);
end

if (EVENT == 313) then
	SaveEvent(UID, 2232);
end

if (EVENT == 307) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8402, NPC, 18, 316);
	else
		SelectMsg(UID, 4, savenum, 8004, NPC, 41, 317, 27, 153);
	end
end

if (EVENT == 316) then
	ShowMap(UID, 585);
end

if (EVENT == 317) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1900000)
		SaveEvent(UID, 2231);
	else
		ExpChange(UID, 1250000)
		SaveEvent(UID, 2231);   
	end
end

-- lv 53 Dragon Tooth Soldier
savenum = 229;

if EVENT == 8550 then
  SelectMsg(UID, 1, savenum, 8000, NPC, 14, 8551);
end

if EVENT == 8551 then
  SaveEvent(UID, 9045);
end

if EVENT == 8552 then
  MonsterSub = ExistMonsterQuestSub(UID);
  if MonsterSub == 0 then
    SelectMsg(UID, 2, savenum, 8001, NPC, 10, 8560);
  else
    SelectMsg(UID, 2, savenum, 8399, NPC, 10, 153);
  end
end

if EVENT == 8560 then
  SelectMsg(UID, 4, savenum, 8002, NPC, 22, 8553, 23, 8554);
end

if EVENT == 8553 then
  SaveEvent(UID, 9046);
end

if EVENT == 8554 then
  SaveEvent(UID, 9049);
end

if EVENT == 8555 then
  SelectMsg(UID, 1, savenum, 8003, NPC, 3014, 153);
  SaveEvent(UID, 9048);
end

if EVENT == 8557 then
  CountMonster = CountMonsterQuestSub(UID, savenum, 1);
  if CountMonster < 20 then
    SelectMsg(UID, 2, savenum, 8402, NPC, 10, 8558);
  else
    SelectMsg(UID, 4, savenum, 8004, NPC, 10, 8559, 27, 153);
  end
end

if EVENT == 8558 then
  ShowMap(UID, 585);
end

if EVENT == 8559 then
  Check = CheckExchange(UID, 1001)
  if  Check == true then
    RunExchange(UID, 1001);
    SaveEvent(UID, 9047);
    SaveEvent(UID, 9053);
  else
    Ret = 1;
  end
end

savenum = 469;

if (EVENT == 400) then -- 54 Level Dragon Tooth Skeleton Repeatable
	SaveEvent(UID, 2265);
end

if (EVENT == 402) then
	MonsterSub = ExistMonsterQuestSub(UID);
	if (MonsterSub == 0) then
		SelectMsg(UID, 4, savenum, 8013, NPC, 22, 411, 23, 412);
	else
		SelectMsg(UID, 2, savenum, 8399, NPC, 10, 153);
	end
end

if (EVENT == 411) then
	SaveEvent(UID, 2266);
end

if (EVENT == 412) then
	SaveEvent(UID, 2269);
end

if (EVENT == 413) then
	SaveEvent(UID, 2268);
end

if (EVENT == 407) then
	CountMonster = CountMonsterQuestSub(UID, savenum, 1);
	if (CountMonster < 20) then
		SelectMsg(UID, 2, savenum, 8402, NPC, 18, 416);
	else
		SelectMsg(UID, 4, savenum, 8220, NPC, 41, 417, 27, 153);
	end
end

if (EVENT == 416) then
	ShowMap(UID, 519);
end

if (EVENT == 417) then
	Prem = GetPremium(UID);
	if (Prem > 0) then
		ExpChange(UID, 1900000)
		SaveEvent(UID, 2267);
	else
		ExpChange(UID, 1000000)
		SaveEvent(UID, 2267);   
	end
end

-- Lv 54 Dragon Tooth Skeleton
savenum = 239;

if EVENT == 9000 then
  SelectMsg(UID, 1, savenum, 8000, NPC, 14, 9001);
end

if EVENT == 9001 then
  SaveEvent(UID, 9081);
end

local MonsterSub = 0;

if EVENT == 9002 then
  MonsterSub = ExistMonsterQuestSub(UID);
  if MonsterSub == 0 then
    SelectMsg(UID, 2, savenum, 8013, NPC, 10, 9010);
  else
    SelectMsg(UID, 2, savenum, 8399, NPC, 10, 153);
  end
end

if EVENT == 9010 then
  SelectMsg(UID, 4, savenum, 8014, NPC, 22, 9003, 23, 9004);
end

if EVENT == 9003 then
  SaveEvent(UID, 9082);
end

if EVENT == 9004 then
  SaveEvent(UID, 9085);
end

if EVENT == 9005 then
  SelectMsg(UID, 1, savenum, 8003, NPC, 3014, 153);
  SaveEvent(UID, 9084);
end

if EVENT == 9007 then
  CountMonster = CountMonsterQuestSub(UID, savenum, 1);
  if CountMonster < 20 then
    SelectMsg(UID, 2, savenum, 8402, NPC, 10, 9008);
  else
    SelectMsg(UID, 4, savenum, 8220, NPC, 10, 9009, 27, 153);
  end
end

if EVENT == 9008 then
  ShowMap(UID, 519);
end

if EVENT == 9009 then
  Check = CheckExchange(UID, 942)
  if  Check == true then
    RunExchange(UID, 942);
    SaveEvent(UID, 9083);
    SaveEvent(UID, 9089);
  else
    Ret = 1;
  end
end

-- Lv 50 Sheriff
savenum = 221;

if EVENT == 9470 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 9555);
    EVENT = 9471
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 9560);
    EVENT = 9471
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 9565);
    EVENT = 9471
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 9570);
    EVENT = 9471
  end
end
if EVENT == 9471 then
  SelectMsg(UID, 1, savenum, 8764, 24430, 3014, 193);
end

local MonsterSub = 0;

if EVENT == 9472 then
  MonsterSub = ExistMonsterQuestSub(UID);
  if MonsterSub == 0 then
    SelectMsg(UID, 2, savenum, 8764, 24430, 10, 9475);
  else
    SelectMsg(UID, 2, savenum, 8764, 24430, 10, 193);
  end
end

if EVENT == 9475 then
  SelectMsg(UID, 4, savenum, 8764, 24430, 22, 9473, 23, 9474);
end

if EVENT == 9473 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 9556);
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 9561);
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 9566);
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 9571);
  end
end

if EVENT == 9474 then
  SaveEvent(UID, 9559);
end

if EVENT == 9480 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 9558);
    EVENT = 9482
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 9563);
    EVENT = 9482
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 9568);
    EVENT = 9482
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 9573);
    EVENT = 9482
  end
end

if EVENT == 9482 then
  SelectMsg(UID, 1, savenum, 8764, 24430, 3002, 193);
end

if EVENT == 9476 then
  CountMonster = CountMonsterQuestSub(UID, savenum, 1);
  if CountMonster < 30 then
    SelectMsg(UID, 2, savenum, 8764, 24430, 10, 9477);
  else
    SelectMsg(UID, 4, savenum, 8764, 24430, 10, 9478, 27, 193);
  end
end

if EVENT == 9477 then
  ShowMap(UID, 621);
end

if EVENT == 9478 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    Check = CheckExchange(UID, 1033)
    if Check == true then
      RunExchange(UID, 1122);
      ExpChange(UID, 4000000);
      SaveEvent(UID, 9557);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 2 or Class == 7 or Class == 8 then
    Check = CheckExchange(UID, 1034)
    if  Check == true then
      RunExchange(UID, 1123);
      ExpChange(UID, 4000000);
      SaveEvent(UID, 9562);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 3 or Class == 9 or Class == 10 then
    Check = CheckExchange(UID, 1035)
    if  Check == true then
      RunExchange(UID, 1124);
      ExpChange(UID, 4000000);
      SaveEvent(UID, 9567);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 4 or Class == 11 or Class == 12 then
    Check = CheckExchange(UID, 1036)
    if  Check == true then
      RunExchange(UID, 1125);
      ExpChange(UID, 4000000);
      SaveEvent(UID, 9572);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  end
end

-- Lv 52 Garuna
savenum = 231;

if EVENT == 9490 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 9597);
    EVENT = 9491
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 9602);
    EVENT = 9491
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 9607);
    EVENT = 9491
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 9612);
    EVENT = 9491
  end
end
if EVENT == 9491 then
  SelectMsg(UID, 1, savenum, 8766, 24430, 3014, 193);
end

if EVENT == 9492 then
  MonsterSub = ExistMonsterQuestSub(UID);
  if MonsterSub == 0 then
    SelectMsg(UID, 2, savenum, 8766, 24430, 10, 9495);
  else
    SelectMsg(UID, 2, savenum, 8766, 24430, 10, 193);
  end
end

if EVENT == 9495 then
  SelectMsg(UID, 4, savenum, 8766, 24430, 22, 9493, 23, 9494);
end

if EVENT == 9493 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 9598);
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 9603);
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 9608);
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 9613);
  end
end

if EVENT == 9494 then
  SaveEvent(UID, 9601);
end

if EVENT == 9500 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    SaveEvent(UID, 9600);
    EVENT = 9502
  elseif Class == 2 or Class == 7 or Class == 8 then
    SaveEvent(UID, 9605);
    EVENT = 9502
  elseif Class == 3 or Class == 9 or Class == 10 then
    SaveEvent(UID, 9610);
    EVENT = 9502
  elseif Class == 4 or Class == 11 or Class == 12 then
    SaveEvent(UID, 9615);
    EVENT = 9502
  end
end

if EVENT == 9502 then
  SelectMsg(UID, 1, savenum, 8766, 24430, 3002, 193);
end

if EVENT == 9496 then
  CountMonster = CountMonsterQuestSub(UID, savenum, 1);
  if CountMonster < 30 then
    SelectMsg(UID, 2, savenum, 8766, 24430, 10, 9497);
  else
    SelectMsg(UID, 4, savenum, 8766, 24430, 10, 9498, 27, 193);
  end
end

if EVENT == 9497 then
  ShowMap(UID, 625);
end

if EVENT == 9498 then
  Class = CheckClass(UID);
  if Class == 1 or Class == 5 or Class == 6 then
    Check = CheckExchange(UID, 1033)
    if  Check == true then
      RunExchange(UID, 1130);
      ExpChange(UID, 4500000);
      SaveEvent(UID, 9599);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 2 or Class == 7 or Class == 8 then
    Check = CheckExchange(UID, 1034)
    if  Check == true then
      RunExchange(UID, 1131);
      ExpChange(UID, 4500000);
      SaveEvent(UID, 9604);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 3 or Class == 9 or Class == 10 then
    Check = CheckExchange(UID, 1035)
    if  Check == true then
      RunExchange(UID, 1132);
      ExpChange(UID, 4500000);
      SaveEvent(UID, 9609);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  elseif Class == 4 or Class == 11 or Class == 12 then
    Check = CheckExchange(UID, 1036)
    if  Check == true then
      RunExchange(UID, 1133);
      ExpChange(UID, 4500000);
      SaveEvent(UID, 9614);
      ShowEffect(UID, 300391)
    else
      Ret = 1;
    end
  end
end
return Ret;