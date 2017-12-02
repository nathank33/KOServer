id_map: {400: 159, 410: 160, 411: 161, 412: 162}
new_line: local Savenum = 159;
400 still in if (EVENT == 4000) then
400 still in 			SelectMsg(UID, 3, -1, 4001, NPC, 4000, 4007, 4001, 4010, 4002, 4013, 4003, 4016, 47, 4002);
400 still in 			SelectMsg(UID, 3, -1, 4001, NPC, 4000, 4007, 4001, 4010, 4002, 4013, 47, 4002);
400 still in 			SelectMsg(UID, 3, -1, 4001, NPC, 4000, 4007, 4001, 4010, 47, 4002);
400 still in 			SelectMsg(UID, 2, -1, 4001, NPC, 4000, 4007, 47, 4002);
400 still in 			SelectMsg(UID, 2, -1, 4000, NPC, 10, 4002);
400 still in 		SelectMsg(UID, 2, -1, 4000, NPC, 10, 4002);
400 still in if (EVENT == 4002) then
400 still in if (EVENT == 4007) then
400 still in 	Check = CheckExchange(UID, 400)
400 still in 		min_count = GetMaxExchange(UID, 400);
400 still in 				SelectMsg(UID, 2, Savenum, 4010, NPC, 10, 4002);
400 still in 				SelectMsg(UID, 2, Savenum, 4011, NPC, 10, 4002);
400 still in 				SelectMsg(UID, 4, Savenum, 4006, NPC, 4004, 4008, 4005, 4002);
400 still in 			SelectMsg(UID, 2, Savenum, 4007, NPC, 10, 4002);
400 still in if (EVENT == 4008) then
400 still in 	min_count = GetMaxExchange(UID, 400);
400 still in 	RunCountExchange(UID, 400, min_count);	
new_line: local Savenum = 160;
400 still in 				SelectMsg(UID, 2, Savenum, 4010, NPC, 10, 4002);
400 still in 				SelectMsg(UID, 2, Savenum, 4011, NPC, 10, 4002);
400 still in 				SelectMsg(UID, 4, Savenum, 4006, NPC, 4004, 4011, 4005, 4002);
400 still in 			SelectMsg(UID, 2, Savenum, 4007, NPC, 10, 4002);
new_line: local Savenum = 161;
400 still in 				SelectMsg(UID, 2, Savenum, 4010, NPC, 10, 4002);
400 still in 				SelectMsg(UID, 2, Savenum, 4011, NPC, 10, 4002);
400 still in 				SelectMsg(UID, 4, Savenum, 4006, NPC, 4004, 4014, 4005, 4002);
400 still in 			SelectMsg(UID, 2, Savenum, 4007, NPC, 10, 4002);
new_line: local Savenum = 162;
400 still in 			ItemB = HowmuchItem(UID, 379004000); 
400 still in 				SelectMsg(UID, 2, Savenum, 4010, NPC, 10, 4002);
400 still in 				SelectMsg(UID, 2, Savenum, 4011, NPC, 10, 4002);
400 still in 				SelectMsg(UID, 4, Savenum, 4006, NPC, 4004, 4017, 4005, 4002);
400 still in 			SelectMsg(UID, 2, Savenum, 4007, NPC, 10, 4002);
