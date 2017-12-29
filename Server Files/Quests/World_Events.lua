local RET = 0;
local NPC = 32753;
local EXIT = 1
local QuestId = -1;
local DIALOG = 2;
local MENU = 4;

---------------------- Bifrost Event 1 --------------------------
-- Requires the users to kill Ego, Glutton, and Avarice
QuestId = 10000
ExchangeId = 30000
ExpReward = 10000000

-- Should auto accept the quest and track bifrost monsters
if (EVENT == 1000) then
    print("In Lua 1000");
    SaveEvent(UID, 20000);
end

-- Check if they killed enough monsters
if (EVENT == 1010) then
    print("In Lua 1010");
	MonsterCount1 = CountMonsterQuestSub(UID, QuestId, 1);
    MonsterCount2 = CountMonsterQuestSub(UID, QuestId, 2);
    MonsterCount3 = CountMonsterQuestSub(UID, QuestId, 3);
	if MonsterCount1 >= 3 then
        SelectMsg(UID, DIALOG, QuestId, 50001, NPC, 41, 1020, 27, EXIT);
	end
end

-- Tell the client that the quest is ready to be submitted, then go back to event 1010.
if (EVENT == 1015) then
    print("In Lua 1015");
	SaveEvent(UID, 20005);
end

-- Grant them their reward
if (EVENT == 1020) then
    print("In Lua 1020");
    CanExchange = CheckExchange(UID, ExchangeId);
    if CanExchange == true then
      RunExchange(UID, ExchangeId);
      ExpChange(UID, ExpReward);
      ShowEffect(UID, 300391);
      SaveEvent(UID, 20010);
    else
      RET = 1;
    end
end
-----------------------------------------------------------------



-- Event to exit safely
if (EVENT == EXIT) then
	RET = 1;
end