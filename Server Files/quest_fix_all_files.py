import os
import re
import pypyodbc

OLD_QUEST_DIR = "./Quests/"
NEW_QUEST_DIR = "./Quests Fixed All/"
LOG_DIR = "./Quests Fixed All Logs/"

REGEX_MAP = {
    lambda id: "SelectMsg\(UID, 2, %s," % id:
        lambda id: "SelectMsg(UID, 2, %s," % id,
    lambda id: "SelectMsg\(UID, 4, %s," % id:
        lambda id: "SelectMsg(UID, 4, %s," % id,
    lambda id: "CountMonsterQuestSub\(UID, %s," % id:
        lambda id: "CountMonsterQuestSub(UID, %s," % id,
    lambda id: "^[=]=.*%s.*;" % id: # savenum = 123;
        lambda id: " = %s;" % id,
}

def main():
    if not os.path.exists(NEW_QUEST_DIR):
        os.makedirs(NEW_QUEST_DIR)
    if not os.path.exists(LOG_DIR):
        os.makedirs(LOG_DIR)
      
    connection = pypyodbc.connect('DRIVER={SQL Server};'
        'SERVER=localhost\SQLExpress14;'
        'DATABASE=kn_online2;'
        'UID=knight;pwd=knight')
        
    for root, dirs, file_names in os.walk(OLD_QUEST_DIR):
        for file_name in file_names:
            #if "21810_Murka" not in file_name:
                #continue
            file = open(os.path.join(OLD_QUEST_DIR, file_name), "r")   
            file_ids = get_quest_ids(file)
            file.seek(0);
            sql_ids = get_quest_ids_sql(file_name, connection)
            
            #print(file_name, file_ids, sql_ids)   
            id_map = {}
            for i in range(min(len(file_ids), len(sql_ids))):
                # Assign these in reverse order. The higher leveled quests are more important, even if they might be assigned incorrectly, it's better than them not being assigned at all.                
                sql_id = sql_ids[-i]
                file_id = file_ids[-i]
                if file_id not in sql_ids: # the quest was probably assigned correctly
                    id_map[file_id] = sql_id
            
            if len(file_ids) == 0:
                print("Empty:", file_name, sql_ids, file_ids)
            if len(id_map) <= 0:
                file.close()
                continue
            if len(sql_ids) != len(file_ids):
                print(file_name, id_map)
            
            new_file = open(os.path.join(NEW_QUEST_DIR, file_name), "w")
            log_file = open(os.path.join(LOG_DIR, file_name), "w")
            replace_quest_ids(id_map, file, new_file, log_file)
    
            file.close()
            new_file.close()
            log_file.close()
    
    connection.close()

def replace_quest_ids(id_map, file, new_file, log_file):
    for line in file:
        new_line = line
        for old_id, new_id in id_map.items():
            for regex_old_func, regex_new_func in REGEX_MAP.items():
                regex = re.compile(regex_old_func(old_id), re.IGNORECASE)
                new_line = regex.sub(regex_new_func(new_id), line)
                if new_line != line:
                    print_and_log(log_file, "new_line: %s" % new_line)
                    break
            if str(old_id) in new_line:
                print_and_log(log_file, "%d still in %s" % (old_id, new_line))

            # Break anytime the line changes
            # It's possible to have cycles if the ids point
            # to each other
            if new_line != line:
                break
        new_file.write(new_line)
 
def print_and_log(file, str):
    #print(str)
    file.write(str)

def get_quest_ids(file):
    ids_set = set() # for performance
    ids_list = []
    
    for line in file:
        # savenum is a common variable declared in these files.
        # It's usually declared so that they don't have to keep
        # repeating the quest id.
        if 'savenum' in line.lower():
            id = parse_quest_id(line.replace(" ", ""), "savenum=", ";")
            if id != None and id not in ids_set:
                ids_set.add(id)
                ids_list.append(id)
            # Always continue if it's savenum because there's no 
            # way it can be parsed by another regex
            continue
       
        id = parse_quest_id(line, "SelectMsg(UID, 4, ", ",")
        if id != None and id not in ids_set:
            ids_set.add(id)
            ids_list.append(id)
            continue
            
        id = parse_quest_id(line, "SelectMsg(UID, 2, ", ",")
        if id != None and id not in ids_set:
            ids_set.add(id)
            ids_list.append(id)
            continue
    return ids_list

# Finds all of the quest ids in the database for a file.    
def get_quest_ids_sql(file_name, connection):
    ids_set = set()
    ids_list = []
    
    cursor = connection.cursor()
    sql_command = ("SELECT sEventDataIndex FROM QUEST_HELPER " +
        "WHERE strLuaFilename = '" + file_name + "' "
            "AND sEventDataIndex > 0 " +
        "ORDER BY bLevel, sEventDataIndex")
    cursor.execute(sql_command)
    results = cursor.fetchone()
    
    while results:
        id = results[0]
        if id not in ids_set:
            ids_set.add(id)
            ids_list.append(id)
        results = cursor.fetchone()
    return ids_list

# Attempts to find an id within a line
def parse_quest_id(line, expr, delim):
    line = line.lower()
    expr = expr.lower()
    delim = delim.lower()
    idx = line.find(expr)
    
    if idx >= 0:
        substr = line[idx + len(expr) : line.find(delim, idx + len(expr))]
        try:
            id = int(substr)
            return id if id >= 0 else None
        except ValueError:
            return None
    return None

if __name__== "__main__":
    main()