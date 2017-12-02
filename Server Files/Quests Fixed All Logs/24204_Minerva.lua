new_line: 	SelectMsg(UID, 2, 273, 3026, NPC, 10, 196);
new_line: 	SelectMsg(UID, 2, 273, 3027, NPC, 28, 232);
new_line: 	SelectMsg(UID, 2, 273, 3010, NPC, 10, 201);
new_line: 	SelectMsg(UID, 4, 273, 3017, NPC, 22, 202, 23, 193);
new_line: 	SelectMsg(UID, 2, 273, 3020, NPC, 3016, 193);
new_line: 		SelectMsg(UID, 2, 273, 3019, NPC, 10, 213);
new_line: 		SelectMsg(UID, 4, 273, 3022, NPC, 41, 214, 27, 193);
309 still in 	SaveEvent(UID, 3092);
309 still in 	SaveEvent(UID, 3093);
309 still in 	SaveEvent(UID, 3095);
70 still in 	ITEM_COUNT = HowmuchItem(UID, 900017000);
70 still in 	RobItem(UID, 900017000, 7)
309 still in 	SaveEvent(UID, 3094);
70 still in 	ITEM_COUNTA = HowmuchItem(UID, 379047000);
309 still in 			SelectMsg(UID, 2, 327, 3052, NPC, 18, 309);
309 still in if (EVENT == 309) then
70 still in 	RobItem(UID, 379047000, 5)
new_line: 	SelectMsg(UID, 2, 327, 3001, NPC, 28, 401);
new_line: 	SelectMsg(UID, 2, 327, 3002, NPC, 28, 401);
new_line: 	SelectMsg(UID, 2, 327, 3056, NPC, 10, 404);
new_line: 	SelectMsg(UID, 4, 327, 3057, NPC, 22, 405, 23, 193);
new_line: 	SelectMsg(UID, 2, 327, 3062, NPC, 3006, 193);
70 still in 	ITEM_COUNTC = HowmuchItem(UID, 379067000);
new_line: 		SelectMsg(UID, 4, 327, 3063, NPC, 41, 410, 27, 193);
new_line: 			SelectMsg(UID, 2, 327, 3060, NPC, 18, 409);
new_line: 			SelectMsg(UID, 2, 327, 3060, NPC, 18, 411);
new_line: 			SelectMsg(UID, 2, 327, 3060, NPC, 18, 412);
309 still in 	ShowMap(UID, 309);
70 still in 	RobItem(UID, 379067000, 1)
70 still in if (EVENT == 623) then -- 70 Level Skill
70 still in 		elseif (ITEM_COUNTC < 7000000) then
70 still in 	GoldLose(UID, 7000000)
70 still in 	GiveItem(UID, 900007000, 1)
70 still in 	GiveItem(UID, 900007000, 1)
70 still in 	GiveItem(UID, 900007000, 1)
70 still in 	GiveItem(UID, 900007000, 1)
70 still in 	GiveItem(UID, 900007000, 1)
70 still in 	GiveItem(UID, 900007000, 1)
70 still in 	GiveItem(UID, 900007000, 1)
