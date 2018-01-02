local RET = 0;
local NPC = 32753;
local EXIT = 1
local QuestId = -1;
local DIALOG = 2;
local MENU = 4;

---------------------- Bifrost Event 1 --------------------------
-- Kill Ashiton, Pride, and Glutton
QuestId = 10000
ExchangeId = 30000

-- Should auto accept the quest and track bifrost monsters
if (EVENT == 1000) then
    SaveEvent(UID, 20000);
end

-- Check if they killed enough monsters
if (EVENT == 1010) then
	MonsterCount1 = CountMonsterQuestSub(UID, QuestId, 1);
    MonsterCount2 = CountMonsterQuestSub(UID, QuestId, 2);
    MonsterCount3 = CountMonsterQuestSub(UID, QuestId, 3);
	if MonsterCount1 >= 5 and MonsterCount2 >= 3 and MonsterCount3 >= 3 then
        SelectMsg(UID, DIALOG, QuestId, 50099, NPC, 41, 1020, 27, EXIT);
	end
end

-- Tell the client that the quest is ready to be submitted, then go back to event 1010.
if (EVENT == 1015) then
	SaveEvent(UID, 20005);
end

-- Grant them their reward
if (EVENT == 1020) then
    CanExchange = CheckExchange(UID, ExchangeId);
    if CanExchange == true then
      RunExchange(UID, ExchangeId);
      ShowEffect(UID, 300391);
      SaveEvent(UID, 20010);
    else
      RET = 1;
    end
end

---------------------- Bifrost Event 2 --------------------------
-- Kill Wrath, Sloth, Lust
QuestId = 10001
ExchangeId = 30001

if (EVENT == 1100) then
    SaveEvent(UID, 20100);
end

if (EVENT == 1110) then
	MonsterCount1 = CountMonsterQuestSub(UID, QuestId, 1);
    MonsterCount2 = CountMonsterQuestSub(UID, QuestId, 2);
    MonsterCount3 = CountMonsterQuestSub(UID, QuestId, 3);
	if MonsterCount1 >= 3 and MonsterCount2 >= 2 and MonsterCount3 >= 2 then
        SelectMsg(UID, DIALOG, QuestId, 50099, NPC, 41, 1120, 27, EXIT);
	end
end

if (EVENT == 1115) then
	SaveEvent(UID, 20105);
end

if (EVENT == 1120) then
    CanExchange = CheckExchange(UID, ExchangeId);
    if CanExchange == true then
      RunExchange(UID, ExchangeId);
      ShowEffect(UID, 300391);
      SaveEvent(UID, 20110);
    else
      RET = 1;
    end
end

---------------------- Bifrost Event 3 --------------------------
-- Kill Envy, Greed
QuestId = 10002
ExchangeId = 30002

if (EVENT == 1200) then
    SaveEvent(UID, 20200);
end

if (EVENT == 1210) then
	MonsterCount1 = CountMonsterQuestSub(UID, QuestId, 1);
    MonsterCount2 = CountMonsterQuestSub(UID, QuestId, 2);
    MonsterCount3 = CountMonsterQuestSub(UID, QuestId, 3);
	if MonsterCount1 >= 3 and MonsterCount2 >= 2 and MonsterCount3 >= 2 then
        SelectMsg(UID, DIALOG, QuestId, 50099, NPC, 41, 1220, 27, EXIT);
	end
end

if (EVENT == 1215) then
	SaveEvent(UID, 20105);
end

if (EVENT == 1220) then
    CanExchange = CheckExchange(UID, ExchangeId);
    if CanExchange == true then
      RunExchange(UID, ExchangeId);
      ShowEffect(UID, 300391);
      SaveEvent(UID, 20210);
    else
      RET = 1;
    end
end


-----------------------------------------------------------
-- Event to exit safely
if (EVENT == EXIT) then
	RET = 1;
end