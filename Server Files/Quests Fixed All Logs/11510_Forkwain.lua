9 still in if (EVENT == 190) then
9 still in 	SelectMsg(UID, 3, savenum, 147, NPC, 10,6092);
9 still in if EVENT == 193 then
9 still in if EVENT == 195 then
8 still in    SelectMsg(UID, 1, 8, 666, NPC, 28, 196);
9 still in    SelectMsg(UID, 1, 8, 666, NPC, 28, 196);
9 still in if EVENT == 196 then
9 still in    ShowMap(UID, 39);
9 still in    SaveEvent(UID, 439);
new_line:    SelectMsg(UID, 2, 177, 667, NPC, 10, 201);
new_line:    SelectMsg(UID, 4, 177, 668, NPC, 22, 202, 23, 193);
8 still in    SelectMsg(UID, 4, 177, 668, NPC, 22, 202, 23, 193);
9 still in      ITEM_COUNT3 = HowmuchItem(UID, 910044000); 
9 still in    Weight = CheckGiveWeight(UID, 910044000, 1) ;
9 still in    GiveItem(UID, 910044000, 1);
new_line:       SelectMsg(UID, 2, 177, 671, NPC, 18, 213);
8 still in       SelectMsg(UID, 2, 177, 671, NPC, 18, 213);
8 still in       SelectMsg(UID, 1, 8, 669, NPC, 32, 208);
9 still in       SelectMsg(UID, 1, 8, 669, NPC, 32, 208);
8 still in       SelectMsg(UID, 1, 8, 670, NPC, 21, 208);
9 still in      ITEM_COUNT1 = HowmuchItem(UID, 910040000); 
9 still in      ITEM_COUNT2 = HowmuchItem(UID, 910041000); 
new_line:         SelectMsg(UID, 2, 177, 671, NPC, 18, 213);
8 still in         SelectMsg(UID, 2, 177, 671, NPC, 18, 213);
new_line:         SelectMsg(UID, 2, 177, 672, NPC, 18, 213);
8 still in         SelectMsg(UID, 2, 177, 672, NPC, 18, 213);
new_line:         SelectMsg(UID, 4, 177, 673, NPC, 41, 214, 27, 193);
8 still in         Check = CheckExchange(UID, 88)
8 still in         RunExchange(UID, 88);	   
new_line: local savenum = 617;
9 still in -- 9¹øÀÇ ¼Ó¼º 0¹ø 4¹øÀÏ °æ¿ì
9 still in     SelectMsg(UID, 4, savenum, 4296, NPC, 22, 533, 23, 534);
9 still in    GiveItem(UID, 910050000, 1);
9 still in -- 9¹øÀÇ ¼Ó¼º 1¹ø 3¹øÀÏ °æ¿ì
9 still in ItemA = HowmuchItem(UID, 910057000); --7¹øÂ° ¿­¼è
9 still in ItemB = HowmuchItem(UID, 910050000); -- ¸í·É¼­
9 still in        GiveItem(UID, 910050000, 1);
8 still in        SelectMsg(UID, 2, savenum, 4298, NPC, 10, 193);
9 still in        SelectMsg(UID, 2, savenum, 4298, NPC, 10, 193);
9 still in        SelectMsg(UID, 2, savenum, 4299, NPC, 10, 193);
9 still in     SelectMsg(UID, 2, savenum, 4297, NPC, 4172, 537, 4173, 193);
new_line: local savenum = 618;
9 still in if EVENT == 6090 then
9 still in        SaveEvent(UID, 6039);
603 still in        SaveEvent(UID, 6039);
9 still in        EVENT = 7009
8 still in        elseif Class == 2 or Class == 7 or Class == 8 then
9 still in        EVENT = 7009
9 still in        elseif Class == 3 or Class == 9 or Class == 10 then
9 still in        EVENT = 7009
9 still in        EVENT = 7009
9 still in if EVENT == 7009 then
8 still in    SelectMsg(UID, 1, savenum, 6039, NPC, 28, 6091);
9 still in    SelectMsg(UID, 1, savenum, 6039, NPC, 28, 6091);
603 still in    SelectMsg(UID, 1, savenum, 6039, NPC, 28, 6091);
8 still in    SelectMsg(UID, 1, savenum, 6040, NPC, 28, 6091);
9 still in    SelectMsg(UID, 1, savenum, 6040, NPC, 28, 6091);
9 still in if EVENT == 6091 then
9 still in if EVENT == 6092 then
8 still in    	SelectMsg(UID, 2, savenum, 6041, NPC, 6007, 6093, 6008, 7002);
9 still in    	SelectMsg(UID, 2, savenum, 6041, NPC, 6007, 6093, 6008, 7002);
9 still in 	SelectMsg(UID, 2, savenum, 6042, NPC, 10, 193);
9 still in if EVENT == 6093 then
9 still in     SelectMsg(UID, 4, savenum, 6043, NPC, 22, 6094, 23, 193);
9 still in if EVENT == 6094 then
8 still in 		ZoneChangeParty(UID, 82, 202, 199);
9 still in 		ZoneChangeParty(UID, 82, 202, 199);
8 still in 		CheckWaiting(UID, 82, 600);
9 still in 		EVENT = 6097
9 still in 		SelectMsg(UID, 2, savenum, 6044, NPC, 10, 193);
9 still in if EVENT == 6096 then   -- A region passed  UID is contained zonenumber... broadcasting packets
8 still in   SelectMsg(UID, 6, 8201, 6059, NPC, 10, 193);
9 still in   SelectMsg(UID, 6, 8201, 6059, NPC, 10, 193);
9 still in if EVENT == 6097 then   -- B region passed
8 still in   SelectMsg(UID, 6, 8202, 6060, NPC, 10, 193);
9 still in   SelectMsg(UID, 6, 8202, 6060, NPC, 10, 193);
8 still in if EVENT == 6098 then   -- C region passed
9 still in if EVENT == 6098 then   -- C region passed
8 still in    SelectMsg(UID, 6, 8203, 6061, NPC, 10, 193);
9 still in    SelectMsg(UID, 6, 8203, 6061, NPC, 10, 193);
9 still in if EVENT == 6099 then   -- Quest Completed  WARNINIG!!! UID is contained zonenumber... broadcasting packets
9 still in      GiveItem(UID, 910135000, 1);		
9 still in if EVENT == 6099 then   -- Quest Completed  WARNINIG!!! UID is contained zonenumber... broadcasting packets
9 still in      GiveItem(UID, 910135000, 1);		
8 still in 	 SaveEvent(UID, 6080);
9 still in    quest_ITEM_COUNT = HowmuchItem(UID, 910135000);
8 still in    SelectMsg(UID, 1, savenum1, 6046, NPC, 28, 7001);
8 still in    SelectMsg(UID, 1, savenum1, 6047, NPC, 28, 7001);
9 still in    ITEM_COUNT = HowmuchItem(UID, 910135000);   
8 still in    ITEM_COUNT1 = HowmuchItem(UID, 910138000);
9 still in    ITEM_COUNT1 = HowmuchItem(UID, 910138000);
8 still in       SelectMsg(UID, 2, savenum1, 6048, NPC, 18, 7003);
8 still in       SelectMsg(UID, 2, savenum1, 6048, NPC, 18, 7003);
9 still in       SelectMsg(UID, 5, savenum1, 6049, NPC, 4006, 7004, 27, 193);
8 still in        elseif Class == 2 or Class == 7 or Class == 8 then
9 still in        elseif Class == 3 or Class == 9 or Class == 10 then
8 still in        EVENT = 7008
9 still in         Check = CheckExchange(UID, 94)
9 still in         RunExchange(UID, 94)
9 still in         Check1 = CheckExchange(UID, 95)
9 still in         RunExchange(UID, 95)
9 still in         Check2 = CheckExchange(UID, 96)
9 still in         RunExchange(UID, 96)
8 still in if EVENT == 7008 then
9 still in         Check3 = CheckExchange(UID, 97)
9 still in         RunExchange(UID, 97)
8 still in local savenum1 = 608;
8 still in    SelectMsg(UID, 1, savenum, 6080, NPC, 28, 7011);
8 still in    SelectMsg(UID, 1, savenum, 6081, NPC, 28, 7011);
9 still in    ShowMap(UID,39);
8 still in    if Level >= 60 and Level <= 80 then
8 still in    	SelectMsg(UID, 2, savenum, 6051, NPC, 6009, 7013, 6008, 7022);
9 still in    	SelectMsg(UID, 2, savenum, 6051, NPC, 6009, 7013, 6008, 7022);
9 still in 	SelectMsg(UID, 2, savenum, 6052, NPC, 10, 193);
9 still in    SelectMsg(UID, 4, savenum, 6053, NPC, 22, 7014, 23, 193);
8 still in 		ZoneChangeParty(UID, 83, 202, 199);
9 still in 		ZoneChangeParty(UID, 83, 202, 199);
8 still in 		CheckWaiting(UID, 83, 600);
8 still in 		EVENT = 7018;
9 still in 		SelectMsg(UID, 2, savenum, 6044, NPC, 10, 193);
8 still in    SelectMsg(UID, 6, 8304, 6084, NPC, 10, 193);
9 still in    SelectMsg(UID, 6, 8304, 6084, NPC, 10, 193);
8 still in    SelectMsg(UID, 6, 8301, 6062, NPC, 10, 193);
9 still in    SelectMsg(UID, 6, 8301, 6062, NPC, 10, 193);
8 still in   SelectMsg(UID, 6, 8302, 6063, NPC, 10, 193);
9 still in   SelectMsg(UID, 6, 8302, 6063, NPC, 10, 193);
8 still in if EVENT == 7018 then   -- C region passed
8 still in    SelectMsg(UID, 6, 8303, 6064, NPC, 10, 193);
9 still in    SelectMsg(UID, 6, 8303, 6064, NPC, 10, 193);
9 still in if EVENT == 7019 then   -- Quest Completed  WARNINIG!!! UID is contained zonenumber... broadcasting packets
9 still in      GiveItem(UID, 910136000, 1);		
8 still in         RunExchange(UID, 98)
9 still in         RunExchange(UID, 98)
9 still in    quest_ITEM_COUNT = HowmuchItem(UID, 910136000);
8 still in    SelectMsg(UID, 1, savenum1, 6082, NPC, 28, 7021);
8 still in    SelectMsg(UID, 1, savenum1, 6083, NPC, 28, 7021);
9 still in    ITEM_COUNT1 = HowmuchItem(UID, 910137000);
9 still in    ITEM_COUNT = HowmuchItem(UID, 910136000);
8 still in       SelectMsg(UID, 2, savenum1, 6056, NPC, 18, 7023);
8 still in       SelectMsg(UID, 2, savenum1, 6056, NPC, 18, 7023);
9 still in    ShowMap(UID, 39);
8 still in         Check = CheckExchange(UID, 98)
9 still in         Check = CheckExchange(UID, 98)
8 still in         RunExchange(UID, 98)
9 still in         RunExchange(UID, 98)
