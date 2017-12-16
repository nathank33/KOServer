local Ret = 0;
 -- This ID replaces 31102_Casket02. I chose that file
 -- since it's pretty much empty.
local NPC = 31102;
local EXIT = 1
local QuestId = -1;

index
DECLARE @msg tinyint = 2;
DECLARE @level int = 1;
DECLARE @exp int = 0;
DECLARE @class tinyint = 5;
DECLARE @nation tinyint = 3;
DECLARE @questType tinyint = 1;
DECLARE @zone tinyint = cz;
DECLARE @npc smallint = 31102;
DECLARE @questId smallint = 10000;
beventstatus
trigger
complete
DECLARE @talk int = 20000;
DECLARE @file char(40) = "World_Events.lua";


INSERT INTO QUEST_HELPER VALUES(20000, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, 1, 99999, 0, @talk, @file

-- Bifrost Event 1
-- Requires the users to kill Ego, Glutton, and Avarice
QuestId = 10000
ExchangeId = 10000
ExpReward = 10000000

if (EVENT == 1000) then
    -- Should auto accept the quest and track bifrost monsters
    SaveEvent(UID, 20000);
end

if (EVENT == 1010) then
	MonsterCount1 = CountMonsterQuestSub(UID, savenum, 1);
    MonsterCount2 = CountMonsterQuestSub(UID, savenum, 2);
    MonsterCount3 = CountMonsterQuestSub(UID, savenum, 3);
	if MonsterCount1 < 3 or MonsterCount2 < 3 or MonsterCount3 < 3 then
		SelectMsg(UID, 2, savenum, 8152, NPC, 18, EXIT);
	else
		SelectMsg(UID, 4, savenum, 8152, NPC, 41, 1008, 27, EXIT);
	end
end

if (EVENT == 1020) then
    CanExchange = CheckExchange(UID, ExchangeId)
    if CanExchange == true then
      RunExchange(UID, ExchangeId);
      ExpChange(UID, ExpReward);
      ShowEffect(UID, 300391);
      SaveEvent(UID, 20010);
    else
      Ret = 1;
    end
end


-- Event to exit safely
if (EVENT == EXIT) then
	Ret = 1;
end