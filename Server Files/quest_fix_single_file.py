import os
import re

OLD_QUEST_DIR = "./Quests/"
NEW_QUEST_DIR = "./Quests Fixed/"
NEW_QUEST_LOG_DIR = "./Quests Fixed Logs/"
SHOW_IDS = False 
# 14432_Cheina, 14431_Bess, Fix 14435_Moradon


FILE_NAME = ".lua"
ID_MAP = {
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
11111: 11111,
}

REGEX_MAP = {
    lambda id: "SelectMsg\(UID, 2, %s," % id:
        lambda id: "SelectMsg(UID, 2, %s," % id,
    lambda id: "SelectMsg\(UID, 4, %s," % id:
        lambda id: "SelectMsg(UID, 4, %s," % id,
    lambda id: "CountMonsterQuestSub\(UID, %s," % id:
        lambda id: "CountMonsterQuestSub(UID, %s," % id,
    lambda id: " = %s;" % id: # savenum = 123;
        lambda id: " = %s;" % id,
    lambda id: " = %s ;" % id: # savenum = 123 ;
        lambda id: " = %s ;" % id
}

def main():
    if not os.path.exists(NEW_QUEST_DIR):
        os.makedirs(NEW_QUEST_DIR)
    if not os.path.exists(NEW_QUEST_LOG_DIR):
        os.makedirs(NEW_QUEST_LOG_DIR)
    
    file = open(os.path.join(OLD_QUEST_DIR, FILE_NAME), "r")
    if SHOW_IDS:
        print(str(sorted(get_quest_ids(file))))
        file.close()
        return
    
    new_file = open(os.path.join(NEW_QUEST_DIR, FILE_NAME), "w")
    new_file_log = open(os.path.join(NEW_QUEST_LOG_DIR, FILE_NAME + ".log.lua"), "w")
    new_file_log.write("id_map: %s\n" % str(ID_MAP))
    

    for line in file:
        new_line = line
        for old_id, new_id in ID_MAP.items():
            for regex_old_func, regex_new_func in REGEX_MAP.items():
                regex = re.compile(regex_old_func(old_id), re.IGNORECASE)
                new_line = regex.sub(regex_new_func(new_id), line)
                if new_line != line:
                    print_and_log(new_file_log, "new_line: %s" % new_line)
                    break
            if str(old_id) in new_line:
                print_and_log(new_file_log, "%d still in %s" % (old_id, new_line))

            # Break anytime the line changes
            # It's possible to have cycles if the ids point
            # to each other
            if new_line != line:
                break
        new_file.write(new_line)
    
    file.close()
    new_file.close()
    new_file_log.close()
 
def print_and_log(file, str):
    print(str)
    file.write(str)

def get_quest_ids(file):
    ids_set = set() # for performance
    ids_list = []
    
    for line in file:
        # savenum is a common variable declared in these files.
        # It's usually declared so that they don't have to keep
        # repeating the quest id.
        if 'savenum' in line:
            id = parse_quest_id(line, "savenum = ", ";")
            if id != None and id not in ids_set:
                ids_set.add(id)
                ids_list.add(id)
            # Always continue if it's savenum because there's no 
            # way it can be parsed by another regex
            continue
       
        id = parse_quest_id(line, "SelectMsg(UID, 4, ", ",")
        if id != None:
            ids_set.add(id)
            ids_list.add(id)
            continue
            
        id = parse_quest_id(line, "SelectMsg(UID, 2, ", ",")
        if id != None:
            ids_set.add(id)
            ids_list.add(id)
            continue 
    return ids

# Attempts to find an id within a line
def parse_quest_id(line, expr, delim):
    idx = line.find(expr)
    if idx >= 0:
        substr = line[idx + len(expr) : line.find(delim, idx + len(expr))]
        id = int(substr)
        return id if id >= 0 else None
    return None

# Replaces all instances of a given id in a file
# Garbage... Unused. The regex pattern at the top is better
def replace_quest_ids(file, new_file):
    ids = get_quest_ids(file)
    regex = re.compile(r" 62", re.IGNORECASE)
    for line in file:
        new_line = regex.sub(" asdf", line)
        if new_line != line:
            print(new_line)
    
# def main_old():
    # if not os.path.exists(NEW_QUEST_DIR):
        # os.makedirs(NEW_QUEST_DIR)

    # for root, dirs, file_names in os.walk(OLD_QUEST_DIR):
        # for file_name in file_names:
            # if "Patric" not in file_name:
                # continue
            # file = open(os.path.join(OLD_QUEST_DIR, file_name), 'r')
            # new_file = open(os.path.join(NEW_QUEST_DIR, file_name), 'w')
            # replace_quest_ids(file, new_file)
            # file.close()
            # new_file.close()
        

if __name__== "__main__":
    main()