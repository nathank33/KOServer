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
DECLARE @greed smallint = 6451;

------------------ Variables for Bifrost Event ----------------
DECLARE @greedCount int = 3;

DECLARE @fragArrogance int = 389160000;
DECLARE @fragRage int = 389162000;
DECLARE @fragSloth int = 389163000;
DECLARE @fragLechery int = 389164000;
DECLARE @fragJealousy int = 389165000;
DECLARE @fragAvarice int = 389166000;

DECLARE @fragArroganceCount int = 2;
DECLARE @fragRageCount int = 1;
DECLARE @fragSlothCount int = 2;
DECLARE @fragLecheryCount int = 1;
DECLARE @fragJealousyCount int = 2;
DECLARE @fragAvariceCount int = 1;

DECLARE @expCount int = 50000000;
DECLARE @noahCount int = 3000000;

-- Allow the NPC to display quests in the menu
--INSERT INTO QUEST_HELPER VALUES(19999, @msg, 1, @exp, @class, @nation, @questType, @zone, @npc, 0, @ignoreStatus, 160, 0, 0, 0, @file);

-- Show the quest in the NPC menu
-- INSERT INTO QUEST_HELPER VALUES(20000, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @showInNpcMenu, 1000, 0, @exchange, @talk, @file);
-- Tell the user that the world event is not in progress

-- Start the quest
INSERT INTO QUEST_HELPER VALUES(20000, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @saveMonsterData, 1000, 0, @exchange, @talk, @file);
INSERT INTO QUEST_HELPER VALUES(20002, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @saveMonsterData, 1010, 1015, @exchange, @talk, @file);
-- Show the quest as ready to be finished, but keep triggering 1010.
INSERT INTO QUEST_HELPER VALUES(20005, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @showClientFinished, 1010, 0, @exchange, @talk, @file);
-- Finish the event and clear monster data if 20010 is triggered
INSERT INTO QUEST_HELPER VALUES(20010, @msg, @level, @exp, @class, @nation, @questType, @zone, @npc, @questId, @clearMonsterData, 190, 0, @exchange, @talk, @file);
-- Kill 3 Greeds
INSERT INTO QUEST_MONSTER(sQuestNum, sNum1a, sCount1, sNum2a, sCount2, sNum3a, sCount3, sNum4a, sCount4) VALUES(@questId, @greed, @greedCount, 0, 0, 0, 0, 0, 0);
-- Grant exp, noah, fragments of Arrogance and Rage
INSERT INTO ITEM_EXCHANGE VALUES(@exchange, @huntFlag, 0, @huntItem, @questId, 0, 0, 0, 0, 0, 0, 0, 0, @expItem, @expCount, @noahItem, @noahCount, @fragArrogance, @fragArroganceCount, @fragRage, @fragRageCount, 0, 0, @unk2, @unk3, @unk4, @unk5);

