id_map: {8: 177, 9: 220, 202: 494, 603: 199}
9 still in if (EVENT == 190) then
9 still in 		SelectMsg(UID, 2, -1, 664, NPC, 10, 193);
9 still in if (EVENT == 193) then
9 still in if (EVENT == 195) then -- 44 Level Recons
new_line: 	SelectMsg(UID, 2, 177, 1173, NPC, 10, 201);
new_line: 	SelectMsg(UID, 4, 177, 1174, NPC, 22, 202, 23, 193);
202 still in if (EVENT == 202) then
9 still in 	Check = isRoomForItem(UID, 910044000);
8 still in 		SelectMsg(UID, 2, -1, 832, NPC, 27, 193);
9 still in 		SelectMsg(UID, 2, -1, 832, NPC, 27, 193);
9 still in 		GiveItem(UID, 910044000, 1)
8 still in 		SaveEvent(UID, 468);
new_line: 	SelectMsg(UID, 2, 177, 1175, NPC, 10, 193);
9 still in 	ITEMA = HowmuchItem(UID, 910040000);
9 still in 	ITEMB = HowmuchItem(UID, 910041000);
new_line: 		SelectMsg(UID, 2, 177, 1177, NPC, 18, 213);
8 still in 		SelectMsg(UID, 2, 177, 1177, NPC, 18, 213);
new_line: 		SelectMsg(UID, 2, 177, 1178, NPC, 18, 213);
8 still in 		SelectMsg(UID, 2, 177, 1178, NPC, 18, 213);
new_line: 		SelectMsg(UID, 4, 177, 1179, NPC, 41, 214, 27, 193);
9 still in 	RobItem(UID, 910040000, 3)
9 still in 	RobItem(UID, 910041000, 1)
9 still in 	SaveEvent(UID, 469);
new_line: local savenum = 199;
9 still in if (EVENT == 6092) then -- 46 Level Quest Area
9 still in 	SelectMsg(UID, 2, savenum, 6065, NPC, 6007, 6093, 4005, 193);
9 still in if (EVENT == 6093) then
9 still in 	ITEM_COUNT = HowmuchItem(UID, 910135000);   
8 still in 	ITEM_COUNT1 = HowmuchItem(UID, 910138000);
9 still in 	ITEM_COUNT1 = HowmuchItem(UID, 910138000);
9 still in 		SelectMsg(UID, 4, savenum, 6067, NPC, 4543, 6095, 4191, 193);
9 still in 		SelectMsg(UID, 4, savenum, 6070, NPC, 4006, 7004, 4005, 193);
9 still in if (EVENT == 6095) then
8 still in 		ZoneChangeParty(UID, 81, 202, 199);
9 still in 		ZoneChangeParty(UID, 81, 202, 199);
202 still in 		ZoneChangeParty(UID, 81, 202, 199);
9 still in 		EVENT = 6096
8 still in 		SelectMsg(UID, 2, savenum, 6068, NPC, 10, 193);
9 still in 		SelectMsg(UID, 2, savenum, 6068, NPC, 10, 193);
9 still in if (EVENT == 6096) then
8 still in 	elseif (Class == 2 or Class == 7 or Class == 8) then
9 still in 	elseif (Class == 3 or Class == 9 or Class == 10) then
8 still in 		SaveEvent(UID, 6058);
9 still in if (EVENT == 6094) then
9 still in 	ITEM_COUNT = HowmuchItem(UID, 910135000);   
8 still in 	ITEM_COUNT1 = HowmuchItem(UID, 910138000);
9 still in 	ITEM_COUNT1 = HowmuchItem(UID, 910138000);
9 still in 		SelectMsg(UID, 4, savenum, 6043, NPC, 4543, 6095, 4191, 193);
9 still in 		SelectMsg(UID, 4, savenum, 6043, NPC, 4006, 7004, 4005, 193);
8 still in 	elseif (Class == 2 or Class == 7 or Class == 8) then
8 still in 		SaveEvent(UID, 6048);
9 still in 	elseif (Class == 3 or Class == 9 or Class == 10) then
8 still in 	elseif (Class == 2 or Class == 7 or Class == 8) then
9 still in 	elseif (Class == 3 or Class == 9 or Class == 10) then
8 still in 		EVENT = 7008
9 still in 	RobItem(UID, 910135000, 1)
8 still in 	RobItem(UID , 910138000, 3)
9 still in 	RobItem(UID , 910138000, 3)
9 still in 	GiveItem(UID, 972190733, 1)
8 still in 	GiveItem(UID, 972230788, 1)
9 still in 	GiveItem(UID, 972230788, 1)
8 still in 	GiveItem(UID, 972260823, 1)
9 still in 	GiveItem(UID, 972260823, 1)
9 still in 	GiveItem(UID, 972270431, 1)
9 still in 	RobItem(UID, 910135000, 1)
8 still in 	RobItem(UID , 910138000, 3)
9 still in 	RobItem(UID , 910138000, 3)
9 still in 	GiveItem(UID, 972200491, 1)
9 still in 	GiveItem(UID, 972200491, 1)
9 still in 	GiveItem(UID, 972240919, 1)
9 still in 	RobItem(UID, 910135000, 1)
8 still in 	RobItem(UID , 910138000, 3)
9 still in 	RobItem(UID , 910138000, 3)
9 still in 	GiveItem(UID, 972210359, 1)
9 still in 	GiveItem(UID, 972210367, 1)
9 still in 	GiveItem(UID, 972210375, 1)
8 still in if (EVENT == 7008) then
9 still in 	RobItem(UID, 910135000, 1)
8 still in 	RobItem(UID, 910138000, 3)
9 still in 	RobItem(UID, 910138000, 3)
9 still in 	GiveItem(UID, 972220761, 1)
9 still in 	GiveItem(UID, 972250765, 1)
9 still in 	SaveEvent(UID, 6059); 
new_line: 	SelectMsg(UID, 4, 220, 4196, NPC, 22, 533, 23, 534);
9 still in 	SelectMsg(UID, 4, 220, 4196, NPC, 22, 533, 23, 534);
9 still in 	Check = isRoomForItem(UID, 910050000);
8 still in 		SelectMsg(UID, 2, -1, 832, NPC, 27, 193);
9 still in 		SelectMsg(UID, 2, -1, 832, NPC, 27, 193);
9 still in 		GiveItem(UID, 910050000, 1)
9 still in 	SaveEvent(UID, 4209);
8 still in if (EVENT == 538) then
8 still in 	SaveEvent(UID, 4208);
9 still in 	ITEM7 = HowmuchItem(UID, 910057000);
new_line: 		SelectMsg(UID, 4, 220, 4197, NPC, 4172, 537, 4173, 193);
9 still in 		SelectMsg(UID, 4, 220, 4197, NPC, 4172, 537, 4173, 193);
8 still in 		SelectMsg(UID, 2, 9, 4198, NPC, 18, 192);
new_line: 		SelectMsg(UID, 2, 220, 4198, NPC, 18, 192);
9 still in 		SelectMsg(UID, 2, 220, 4198, NPC, 18, 192);
9 still in if (EVENT == 192) then
9 still in 	RobItem(UID, 910057000, 1)
8 still in 	ExpChange(UID, 15218948)
9 still in 	ExpChange(UID, 15218948)
8 still in 	SelectMsg(UID, 4, 202, 9238, NPC, 22, 1002, 23, 1003);
9 still in 	SelectMsg(UID, 4, 202, 9238, NPC, 22, 1002, 23, 1003);
new_line: 	SelectMsg(UID, 4, 494, 9238, NPC, 22, 1002, 23, 1003);
9 still in 	ITEMBDW = HowmuchItem(UID, 900143000);
8 still in 		SelectMsg(UID, 2, 202, 9238, NPC, 18, 191);
9 still in 		SelectMsg(UID, 2, 202, 9238, NPC, 18, 191);
new_line: 		SelectMsg(UID, 2, 494, 9238, NPC, 18, 191);
8 still in 		SelectMsg(UID, 4, 202, 9238, NPC, 4006, 1008, 4005, 193);
9 still in 		SelectMsg(UID, 4, 202, 9238, NPC, 4006, 1008, 4005, 193);
new_line: 		SelectMsg(UID, 4, 494, 9238, NPC, 4006, 1008, 4005, 193);
9 still in if (EVENT == 191) then
8 still in if (EVENT == 1008) then
9 still in 	RobItem(UID, 900143000, 1)
