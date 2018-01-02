------------------ Event Guide NPCS ----------------------
-- Create two NPCs named Event Guide and spawn them into CZ orc and human side

-- Clean up so this command can be ran multiple times
DELETE FROM K_NPC WHERE strName = '[Event Guide] Rudy' OR strName = '[Event Guide] Delmo';
DELETE FROM K_NPCPOS WHERE strName = '[Event Guide] Rudy' OR strName = '[Event Guide] Delmo';

--Human
INSERT INTO K_NPC VALUES(32752, '[Event Guide] Rudy', 11100, 130, 0, 128431369, 128431369, 2, 7, 46, 1, 0, 200, 0, 0, 300000, 0, 20000, 497, 497, 4000, 1500, 2, 7, 1000, 0, 0, 0, 250, 250, 250, 250, 250, 250, 140, 7, 14, 35, 0, 0, 0, 0, 1, 0, 0, 0, 1);
--Orc
INSERT INTO K_NPC VALUES(32753, '[Event Guide] Delmo', 21510, 100, 0, 0, 0, 1, 7, 46, 1, 0, 200, 0, 0, 300000, 0, 20000, 497, 497, 4000, 1500, 2, 7, 1000, 0, 0, 0, 250, 250, 250, 250, 250, 250, 110, 7, 14, 35, 0, 0, 0, 0, 1, 0, 0, 0, 1);

--Place these Npcs in CZ
INSERT INTO K_NPCPOS values(71, 32752, '[Event Guide] Rudy', 2, 4, 0, 0, 0, 620, 891, 1, 0, 0, 60, 180, '', 0);
INSERT INTO K_NPCPOS values(71, 32753, '[Event Guide] Delmo', 2, 4, 0, 0, 0, 1376, 1107, 1, 0, 0, 60, 90, '', 0);

------------------------- Level Exp Requirements -----------
-- Level 80-83 require 1 exp, we should set this to something
-- reasonable
UPDATE LEVEL_UP SET Exp = 2000000000 WHERE level = 80
UPDATE LEVEL_UP SET Exp = 2000000000 WHERE level = 81
UPDATE LEVEL_UP SET Exp = 2300000000 WHERE level = 82
UPDATE LEVEL_UP SET Exp = 3000000000 WHERE level = 83


-------------------------- Cleanup -------------------------
-- Cleanup queries so that other queries can be ran multiple times
DELETE FROM QUEST_HELPER WHERE sEventDataIndex >= 10000 AND sEventDataIndex <= 11000;
DELETE FROM QUEST_HELPER WHERE sNpcId = 32752 or sNPCId = 32753;
DELETE FROM QUEST_MONSTER WHERE sQuestNum >= 10000 AND sQuestNum <= 11000;
DELETE FROM ITEM_EXCHANGE WHERE nIndex >= 30000 AND nIndex <= 31000;

-------------------- Variables for QUEST_HELPER --------------
--index
DECLARE @msg tinyint = 2; -- msg is always 2 for some reason
DECLARE @level int = 1;
DECLARE @exp int = 0;
DECLARE @class tinyint = 5; -- Any class can do these events
DECLARE @nation tinyint = 3; -- Both nations can do these
DECLARE @questType tinyint = 1; -- always 1
DECLARE @zone tinyint = 71; -- Colony Zone
DECLARE @npc smallint = 32753; -- Orc event guide, change later
DECLARE @questId smallint = 10000;
--beventstatus 1 to begin tracking mobs, 2 to clear saved mob data
--trigger
--complete
DECLARE @exchange int = 30000;
DECLARE @talk int = 50000;
DECLARE @file char(40) = 'World_Events.lua';

-- Used to manage monster data
DECLARE @showInNpcMenu tinyint = 255;
DECLARE @saveMonsterData tinyint = 1;
DECLARE @clearMonsterData tinyint = 2;
DECLARE @showClientFinished tinyint = 3;
DECLARE @ignoreStatus tinyint = 0;

------------------ Variables for ITEM_EXCHANGE --------------
-- An Item named "Hunt", it lets the quest menu say "Hunt Worms"
DECLARE @huntItem int = 900005000;
DECLARE @huntFlag tinyint = 10; -- RandomFlag in item exchange for hunting?
DECLARE @unk1 int = 0; -- unsure
DECLARE @unk2 int = 0; -- unsure
DECLARE @unk3 int = 0; -- unsure
DECLARE @unk4 int = 0; -- unsure
DECLARE @unk5 int = 0; -- unsure
DECLARE @noahItem int = 900000000;
DECLARE @expItem int = 900001000;

------------------ Variables for QUEST_MONSTER ---------------
DECLARE @ashiton smallint = 6651;
DECLARE @pride smallint = 5851;
DECLARE @glutton smallint = 5951;
DECLARE @wrath smallint = 6051;
DECLARE @sloth smallint = 6151;
DECLARE @lust smallint = 6251;
DECLARE @envy smallint = 6351;
DECLARE @greed smallint = 6451;

------------------ Variables for Bifrost Events ----------------
-- These are 3 bifrost events that require
-- the user to go out and kill bifrost monsters then receive
-- fragments as a reward.
DECLARE @ashitonCount int = 5;
DECLARE @prideCount int = 3;
DECLARE @gluttonCount int = 3;
DECLARE @wrathCount int = 3;
DECLARE @slothCount int = 2;
DECLARE @lustCount int = 2;
DECLARE @envyCount int = 5;
DECLARE @greedCount int = 5;

DECLARE @fragArrogance int = 389160000;
DECLARE @fragRage int = 389162000;
DECLARE @fragSloth int = 389163000;
DECLARE @fragLechery int = 389164000;
DECLARE @fragJealousy int = 389165000;
DECLARE @fragAvarice int = 389166000;

DECLARE @fragArroganceCount int = 8;
DECLARE @fragRageCount int = 2;
DECLARE @fragSlothCount int = 3;
DECLARE @fragLecheryCount int = 2;
DECLARE @fragJealousyCount int = 3;
DECLARE @fragAvariceCount int = 2;

DECLARE @expCount1 int = 100000000;
DECLARE @noahCount1 int = 5000000;
DECLARE @expCount2 int = 125000000;
DECLARE @noahCount2 int = 10000000;
DECLARE @expCount3 int = 150000000;
DECLARE @noahCount3 int = 15000000;

-------------------- Bifrost Event 1 --------------------
SET @questId = 10000;
SET @exchange = 30000;
SET @talk = 50000;
-- Start the quest
INSERT INTO QUEST_HELPER VALUES(20000, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @saveMonsterData, 1000, 0, @exchange, @talk, @file);
INSERT INTO QUEST_HELPER VALUES(20002, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @saveMonsterData, 1010, 1015, @exchange, @talk, @file);
-- Show the quest as ready to be finished, but keep triggering 1010.
INSERT INTO QUEST_HELPER VALUES(20005, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @showClientFinished, 1010, 0, @exchange, @talk, @file);
-- Finish the event and clear monster data if 20010 is triggered
INSERT INTO QUEST_HELPER VALUES(20010, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @clearMonsterData, 190, 0, @exchange, @talk, @file);
-- Kill 3 Greeds
INSERT INTO QUEST_MONSTER(sQuestNum, sNum1a, sCount1, sNum2a, sCount2, sNum3a, sCount3, sNum4a, sCount4) VALUES(@questId, @ashiton, @ashitonCount, @pride, @prideCount, @glutton, @gluttonCount, 0, 0);
-- Grant exp, noah, fragments of Arrogance and Rage
INSERT INTO ITEM_EXCHANGE VALUES(@exchange, @huntFlag, 0, @huntItem, @questId, 0, 0, 0, 0, 0, 0, 0, 0, @expItem, @expCount1, @noahItem, @noahCount1, @fragArrogance, @fragArroganceCount, 0, 0, 0, 0, @unk2, @unk3, @unk4, @unk5);

-------------------- Bifrost Event 2 --------------------
SET @questId = 10001;
SET @exchange = 30001;
SET @talk = 50001;
INSERT INTO QUEST_HELPER VALUES(20100, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @saveMonsterData, 1100, 0, @exchange, @talk, @file);
INSERT INTO QUEST_HELPER VALUES(20102, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @saveMonsterData, 1110, 1115, @exchange, @talk, @file);
INSERT INTO QUEST_HELPER VALUES(20105, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @showClientFinished, 1110, 0, @exchange, @talk, @file);
INSERT INTO QUEST_HELPER VALUES(20110, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @clearMonsterData, 190, 0, @exchange, @talk, @file);
INSERT INTO QUEST_MONSTER(sQuestNum, sNum1a, sCount1, sNum2a, sCount2, sNum3a, sCount3, sNum4a, sCount4) VALUES(@questId, @wrath, @wrathCount, @sloth, @slothCount, @lust, @lustCount, 0, 0);
INSERT INTO ITEM_EXCHANGE VALUES(@exchange, @huntFlag, 0, @huntItem, @questId, 0, 0, 0, 0, 0, 0, 0, 0, @expItem, @expCount2, @noahItem, @noahCount2, @fragRage, @fragRageCount, @fragSloth, @fragSlothCount, @fragLechery, @fragLecheryCount, @unk2, @unk3, @unk4, @unk5);

-------------------- Bifrost Event 3 --------------------
SET @questId = 10002;
SET @exchange = 30002;
SET @talk = 50002;
INSERT INTO QUEST_HELPER VALUES(20200, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @saveMonsterData, 1200, 0, @exchange, @talk, @file);
INSERT INTO QUEST_HELPER VALUES(20202, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @saveMonsterData, 1210, 1215, @exchange, @talk, @file);
INSERT INTO QUEST_HELPER VALUES(20205, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @showClientFinished, 1210, 0, @exchange, @talk, @file);
INSERT INTO QUEST_HELPER VALUES(20210, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @clearMonsterData, 190, 0, @exchange, @talk, @file);
INSERT INTO QUEST_MONSTER(sQuestNum, sNum1a, sCount1, sNum2a, sCount2, sNum3a, sCount3, sNum4a, sCount4) VALUES(@questId, @envy, @envyCount, @greed, @greedCount, 0, 0, 0, 0);
INSERT INTO ITEM_EXCHANGE VALUES(@exchange, @huntFlag, 0, @huntItem, @questId, 0, 0, 0, 0, 0, 0, 0, 0, @expItem, @expCount3, @noahItem, @noahCount3, @fragJealousy, @fragJealousyCount, @fragAvarice, @fragAvariceCount, 0, 0, @unk2, @unk3, @unk4, @unk5);

