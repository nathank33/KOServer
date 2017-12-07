local Ret = 0;
local NPC = 14435;

if (EVENT == 100) then
	QuestNum = SearchQuest(UID, NPC);
	if (QuestNum == 0) then
		SelectMsg(UID, 2, -1, 4937, NPC, 10, 158);
	elseif (QuestNum > 1 and  QuestNum < 100) then
		NpcMsg(UID, 4937, NPC)
	else
		EVENT = QuestNum
	end
end

if (EVENT == 158) then
	Ret = 1;
end

local savenum = 127;

if (EVENT == 232) then -- 35 Level
	SelectMsg(UID, 4, savenum, 4931, NPC, 22, 233, 23, 158);
end

if (EVENT == 233) then
	SaveEvent(UID, 10013);
end

if (EVENT == 235) then
	SaveEvent(UID, 10015);
end

if (EVENT == 236) then
	ANIMAL = HowmuchItem(UID, 379273000);
	if (ANIMAL < 3) then
		SelectMsg(UID, 2, savenum, 4991, NPC, 19, 238);
	else
		SelectMsg(UID, 4, savenum, 4998, NPC, 22, 237, 23, 158);
	end
end

if (EVENT == 238) then
	ShowMap(UID, 96);
end

if (EVENT == 237) then
	RobItem(UID, 379273000, 3)
	ExpChange(UID, 270000)
	SaveEvent(UID, 10014);
end

local savenum1 = 130;

if (EVENT == 242) then -- 36 Level
	SelectMsg(UID, 4, savenum1, 575, NPC, 22, 243, 23, 158);
end

if (EVENT == 243) then
	SaveEvent(UID, 10025);
end

if (EVENT == 245) then
	SaveEvent(UID, 10027);
end

if (EVENT == 246) then
	ROTTEN = HowmuchItem(UID, 379274000);
	if (ROTTEN < 3) then
		SelectMsg(UID, 2, savenum1, 788, NPC, 19, 248);
	else
		SelectMsg(UID, 4, savenum1, 6106, NPC, 22, 247, 23, 158);
	end
end

if (EVENT == 248) then
	ShowMap(UID, 98);
end

if (EVENT == 247) then
	RobItem(UID, 379274000, 3)
	ExpChange(UID, 300000)
	SaveEvent(UID, 10026);
end

local savenum2 = 133;

if (EVENT == 252) then -- 37 Level
	SelectMsg(UID, 4, savenum2, 576, NPC, 22, 253, 23, 158);
end

if (EVENT == 253) then
	SaveEvent(UID, 9867);
end

if (EVENT == 255) then
	SaveEvent(UID, 9869);
end

if (EVENT == 256) then
	FEATHER = HowmuchItem(UID, 379272000);
	if (FEATHER < 3) then
		SelectMsg(UID, 2, savenum2, 6187, NPC, 19, 258);
	else
		SelectMsg(UID, 4, savenum2, 6196, NPC, 22, 257, 23, 158);
	end
end

if (EVENT == 258) then
	ShowMap(UID, 100);
end

if (EVENT == 257) then
	RobItem(UID, 379272000, 3)
	ExpChange(UID, 330000)
	SaveEvent(UID, 9868);
end

local savenum3 = 136;

if (EVENT == 262) then -- 38 Level
	SelectMsg(UID, 4, savenum3, 586, NPC, 22, 263, 23, 158);
end

if (EVENT == 263) then
	SaveEvent(UID, 9879);
end

if (EVENT == 265) then
	SaveEvent(UID, 9881);
end

if (EVENT == 266) then
	SKULL = HowmuchItem(UID, 379253000);
	if (SKULL < 3) then
		SelectMsg(UID, 2, savenum3, 6127, NPC, 19, 268);
	else
		SelectMsg(UID, 4, savenum3, 6128, NPC, 22, 267, 23, 158);
	end
end

if (EVENT == 268) then
	ShowMap(UID, 102);
end

if (EVENT == 267) then
	RobItem(UID, 379253000, 3)
	ExpChange(UID, 370000)
	SaveEvent(UID, 9880);
end

local savenum4 = 139;

if (EVENT == 272) then -- 39 Level
	SelectMsg(UID, 4, savenum4, 6139, NPC, 22, 273, 23, 158);
end

if (EVENT == 273) then
	SaveEvent(UID, 9891);
end

if (EVENT == 275) then
	SaveEvent(UID, 9893);
end

if (EVENT == 276) then
	COARSE = HowmuchItem(UID, 379275000);
	if (COARSE < 3) then
		SelectMsg(UID, 2, savenum4, 6141, NPC, 19, 278);
	else
		SelectMsg(UID, 4, savenum4, 6142, NPC, 22, 277, 23, 158);
	end
end

if (EVENT == 278) then
	ShowMap(UID, 104);
end

if (EVENT == 277) then
	RobItem(UID, 379275000, 3)
	ExpChange(UID, 400000)
	SaveEvent(UID, 9892);
end

local savenum5 = 142;

if (EVENT == 282) then -- 40 Level
	SelectMsg(UID, 4, savenum5, 590, NPC, 22, 283, 23, 158);
end

if (EVENT == 283) then
	SaveEvent(UID, 9903);
end

if (EVENT == 285) then
	SaveEvent(UID, 9905);
end

if (EVENT == 286) then
	SKULL1 = HowmuchItem(UID, 379253000);
	COARSE1 = HowmuchItem(UID, 379275000);
	if (SKULL1 < 5 or COARSE1 < 5) then
		SelectMsg(UID, 2, savenum5, 6141, NPC, 19, 288);
	elseif (SKULL1 > 4 and COARSE1 > 4) then
		SelectMsg(UID, 4, savenum5, 6142, NPC, 22, 287, 23, 158);
	end
end

if (EVENT == 288) then
	ShowMap(UID, 106);
end

if (EVENT == 287) then
	RobItem(UID, 379253000, 5)
	RobItem(UID, 379275000, 5)
	ExpChange(UID, 800000)
	GoldGain(UID, 100000)
	SaveEvent(UID, 9904);
end

local savenum6 = 145;

if (EVENT == 292) then -- 41 Level
	SelectMsg(UID, 4, savenum6, 6163, NPC, 22, 293, 23, 158);
end

if (EVENT == 293) then
	SaveEvent(UID, 9915);
end

if (EVENT == 295) then
	SaveEvent(UID, 9917);
end

if (EVENT == 296) then
	ORK = HowmuchItem(UID, 379277000);
	if (ORK < 7) then
		SelectMsg(UID, 2, savenum6, 6163, NPC, 19, 298);
	else
		SelectMsg(UID, 4, savenum6, 6142, NPC, 22, 297, 23, 158);
	end
end

if (EVENT == 298) then
	ShowMap(UID, 108);
end

if (EVENT == 297) then
	RobItem(UID, 379277000, 7)
	ExpChange(UID, 900000)
	GoldGain(UID, 100000)
	SaveEvent(UID, 9916);
end

local savenum7 = 148; --sEventDataIndex of Oath of Darkness Quest

if (EVENT == 302) then -- 42 Level Oath of Darkness Quest --bEventStatus 0
	SelectMsg(UID, 4, savenum7, 4932, NPC, 22, 303, 23, 158);
end

if (EVENT == 303) then 
	SaveEvent(UID, 9927); -- 9927 = nIndex for next event, nEventTriggerIndex = 306 nEventCompleteIndex = 305
end -- bEventStatus 1

if (EVENT == 305) then --nEventCompleteIndex (when you have all the items) it runs this event
	SaveEvent(UID, 9929); --bEventStatus 3
end

if (EVENT == 306) then
	OATH = HowmuchItem(UID, 379276000);
	if (OATH < 3) then
		SelectMsg(UID, 2, savenum7, 4932, --QUEST TALK iNum 4932 --
		NPC, 19, 308);
	else
		SelectMsg(UID, 4, savenum7, 4934, NPC, 22 -- 22 Accepted
		, 307, 23 -- 23 Rejected
		, 158);
	end
end

if (EVENT == 308) then
	ShowMap(UID, 110);
end

if (EVENT == 307) then
	RobItem(UID, 379276000, 3)
	ExpChange(UID, 900000)
	GoldGain(UID, 100000)
	SaveEvent(UID, 9928); -- bEventStatus 2
end

local savenum8 = 151;

if (EVENT == 312) then -- 43 Level
	SelectMsg(UID, 4, savenum8, 576, NPC, 22, 313, 23, 158);
end

if (EVENT == 313) then
	SaveEvent(UID, 9939);
end

if (EVENT == 315) then
	SaveEvent(UID, 9941);
end

if (EVENT == 316) then
	FEATHER = HowmuchItem(UID, 379272000);
	if (FEATHER < 7) then
		SelectMsg(UID, 2, savenum8, 6187, NPC, 19, 318);
	else
		SelectMsg(UID, 4, savenum8, 6196, NPC, 22, 317, 23, 158);
	end
end

if (EVENT == 318) then
	ShowMap(UID, 112);
end

if (EVENT == 317) then
	RobItem(UID, 379272000, 7)
	ExpChange(UID, 950000)
	GoldGain(UID, 100000)
	SaveEvent(UID, 9940);
end


	--SelectMsg(UID, 4, savenum7(aka 148)(sEventDataIndex), 4932(nEventTalkIndex), NPC, (22(1st Button), which leads to 303(Next Event)), 23(2nd Button), 158(To Exit));


local savenum9 = 1373;
if (EVENT == 500) then -- Collection of magic materials
	SelectMsg(UID, 2, savenum9, 44200, NPC, 4440, 503);
end

if (EVENT == 503) then
	SelectMsg(UID, 4, savenum9, 44201, NPC, 22, 505, 23, 158);
end

if (EVENT == 505) then
	SaveEvent(UID, 3964);
end

if (EVENT == 506) then
	SaveEvent(UID, 3966);
end

if (EVENT == 507) then
		Skull = HowmuchItem(UID, 810494000);
	if (Skull < 5) then
		SelectMsg(UID, 2, savenum9, 6002, NPC, 19, 508);
	else
		SelectMsg(UID, 4, savenum9, 6196, NPC, 22, 509, 23, 158);
	end
end

if (EVENT == 508) then
	ShowMap(UID, 112); --not proper id
end

if (EVENT == 509) then
	RobItem(UID, 810494000, 5) -- take 5 of item id 810494000
	ExpChange(UID, 25000000) -- Give Exp
	--GoldGain(UID, 100000) --Give Gold N/A for this quest
	--SaveEvent(UID, 3968);
end

if (EVENT == 510) then -- Collection of weapon materials
	SelectMsg(UID, 4, savenum7, 4932, NPC, 22, 303, 23, 158);
end

if (EVENT == 520) then -- Ecosystem Research 1
	SelectMsg(UID, 4, savenum7, 4932, NPC, 22, 303, 23, 158);
end

if (EVENT == 530) then -- Ecosystem Research 2
	SelectMsg(UID, 4, savenum7, 4932, NPC, 22, 303, 23, 158);
end
