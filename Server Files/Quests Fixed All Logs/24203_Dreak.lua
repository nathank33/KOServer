new_line: 	SelectMsg(UID, 2, 235, 1233, NPC, 28, 196);
new_line: 	SelectMsg(UID, 2, 235, 1014, NPC, 10, 201);
new_line: 	SelectMsg(UID, 4, 235, 1015, NPC, 22, 202, 23, 193);
new_line: 	SelectMsg(UID, 2, 235, 1016, NPC, 21, 193);
new_line: 		SelectMsg(UID, 2, 235, 1017, NPC, 18, 213);
new_line: 		SelectMsg(UID, 4, 235, 1018, NPC, 41, 214, 27, 193);
236 still in 	ITEM_COUNT3 = HowmuchItem(UID, 379236000);
236 still in 	RobItem(UID, 379236000, 2)
69 still in 		SelectMsg(UID, 2, 217, 507, NPC, 18, 369);
69 still in if (EVENT == 369) then
69 still in 	SaveEvent(UID, 569);
236 still in 	GiveItem(UID, 379236000, 1)
new_line: 	SelectMsg(UID, 2, 325, 520, NPC, 28, 384);
new_line: 	SelectMsg(UID, 2, 325, 521, NPC, 10, 386);
new_line: 	SelectMsg(UID, 2, 325, 522, NPC, 22, 387, 23, 193);
new_line: 	SelectMsg(UID, 2, 325, 524, NPC, 21, 193);
236 still in 	ITEM_COUNT2 = HowmuchItem(UID, 379236000);
new_line: 		SelectMsg(UID, 2, 325, 525, NPC, 18, 393);
new_line: 		SelectMsg(UID, 2, 325, 526, NPC, 18, 392);
new_line: 		SelectMsg(UID, 2, 325, 528, NPC, 41, 394, 27, 193);
236 still in 	RobItem(UID, 379236000, 3)
69 still in 	GiveItem(UID, 379069000, 1) 
236 still in 		SelectMsg(UID, 2, 336, 3236, NPC, 10, 624);
69 still in 	if (ITEM_COUNT1 > 0 and ITEM_COUNT2 > 0 and ITEM_COUNT3 > 6999999) then
236 still in 	ITEM_COUNT2 = HowmuchItem(UID, 379236000);
236 still in 	RobItem(UID, 379236000, 2)
69 still in 	SaveEvent(UID, 5169);
69 still in 		SelectMsg(UID, 2, savenum, 5169, NPC, 10, 1024);
236 still in 	ITEM_COUNT2 = HowmuchItem(UID, 379236000);
236 still in 	RobItem(UID, 379236000, 2)
236 still in 	ITEM_COUNT2 = HowmuchItem(UID, 379236000);
236 still in 	RobItem(UID, 379236000, 2)
