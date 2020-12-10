#include "DbManager.h";

DbManager::DbManager(const char *databaseName){
    dbName = databaseName;
    if (db_open() == "no_file")
    {
      db_write("");
    }    
}

char *DbManager::getDbPath(){
    char dbPath[30];    
    sprintf(dbPath,"/%s.db",dbName);
    return dbPath;
}

String DbManager::db_open() {
    // Launch SPIFFS file system  
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }

    File file = SPIFFS.open(getDbPath(),"r"); 
    if(!file){ 
        Serial.println("Failed to open file for reading"); 
        return "no_file"; 
    }
    // Serial.println(file.readString());
    return file.readString();
}

DynamicJsonDocument DbManager::db_open_json() {
    String dbFile = db_open();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, dbFile);
    return doc;
}

bool DbManager::db_write(String text) {
    // Launch SPIFFS file system  
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }

    File file = SPIFFS.open(getDbPath(),"w"); 
    if(!file){ 
        Serial.println("Failed to open file for reading"); 
        return false; 
    } 
    
    file.print(text);
    file.close();
    return true;
}

bool DbManager::CheckForDatabase(){
    if (!SPIFFS.begin()) {
       Serial.println("Failed to mount file system");
       return 0;
    }
    // SPIFFS.format();

   // list SPIFFS contents
   Dir dir = SPIFFS.openDir("/");
   while (dir.next()) {    
       String fileName = dir.fileName();
       size_t fileSize = dir.fileSize();
       if (fileName == String(String(DbManager::dbName) + ".db"))
       {
            Serial.printf("Database %s already exists. size : %s\n", fileName.c_str(), (long) fileSize);
           return 1;
       }
   }
   return 0;
}



String DbManager::subString(const char *text, int startIndex, int size){
    String res = "";
    for (int j = 0; j < size; j++)
    {
        res = String(res + " ");
    }
    int index = 0;
    for (int i = startIndex; i < (startIndex + size); i++)
    {
        res.setCharAt(index++,text[i]);
    }
    
    return res;
}
bool DbManager::handleInsert(const char *sql){
    List columnsList = List();
    List dataList = List();
    String tableName = "";

    for (int i = 0; i < strlen(sql)-13; i++)
    {
        if (subString(sql,i,12) == "insert into ")
        {
            for (int j = i+12; j < i+33; j++)
            {
                if (subString(sql,j,1)=="(")
                {
                    tableName = subString(sql,i+12, j-(i+12));
                    tableName.trim();
                    int columnStartIndex = j+1;
                    for (int k = j; k < strlen(sql); k++)
                    {
                        if (subString(sql,k,1)==","){
                            String columnName = subString(sql,columnStartIndex,k-columnStartIndex);
                            columnName.trim();
                            columnsList.add(columnName);
                            columnStartIndex = k+1;
                        }else if (subString(sql,k,1)==")")
                        {
                            columnsList.add(subString(sql,columnStartIndex,k-columnStartIndex));
                            for (int x = k; x < strlen(sql); x++)
                            {
                                if (subString(sql,x,1)=="("){
                                    columnStartIndex = x+1;
                                    for (int y = x; y < strlen(sql); y++)
                                    {
                                        if (subString(sql,y,1)==","){
                                            dataList.add(subString(sql,columnStartIndex,y-columnStartIndex));
                                            columnStartIndex = y+1;
                                        }else if(subString(sql,y,1)==")"){
                                            dataList.add(subString(sql,columnStartIndex,y-columnStartIndex));
                                            ///////////////// add to json
                                            DynamicJsonDocument doc(1024);
                                            doc = db_open_json();
                                            JsonArray tableJsonArrays = doc["tables"].as<JsonArray>();
                                            for(JsonObject tableJson : tableJsonArrays) {
                                                if (tableJson["name"].as<String>() == tableName)
                                                {
                                                    JsonArray rowsJsonArrays = tableJson["rows"].as<JsonArray>();
                                                    DynamicJsonDocument rowJsonDoc(256);
                                                    JsonObject rowJson = rowJsonDoc.to<JsonObject>();
                                                    for (int i = 0; i < columnsList.size(); i++)
                                                    {
                                                        String tempRes = columnsList.get(i);
                                                        const int colNmeSze = columnsList.get(i).length();
                                                        char columnName[colNmeSze+1];
                                                        for (int h = 0; h <= colNmeSze; h++)
                                                        {
                                                            columnName[h] = tempRes.charAt(h);
                                                        }
                                                        columnName[colNmeSze+1]='\0';
                                                        rowJson[columnName] = dataList.get(i);
                                                    }
                                                    String rowsJsonArraysString;
                                                    serializeJson(rowsJsonArrays, rowsJsonArraysString);
                                                    rowsJsonArrays.add(rowJson);
                                                    serializeJson(rowsJsonArrays, rowsJsonArraysString);
                                                    String newDbString;
                                                    serializeJson(doc,newDbString);

                                                    return db_write(newDbString);
                                                }                                                
                                            }
                                        }
                                    }
                                }
                            }
                        }                        
                    }                    
                }                
            }            
        }        
    }    
    return false;
}
bool DbManager::handleUpdate(const char *sql){

    return true;
}
bool DbManager::handleDelete(const char *sql){

    return true;
}
bool DbManager::handleCreateTable(const char *sql){
    Serial.println(sql);
    List columnsList= List();
    for (int i = 0; i < strlen(sql)-13; i++)
    {
        if (subString(sql,i,13) == "create table ")
        {
            for (int j = i+13; j < i+33; j++)
            {
                if (subString(sql,j,1)=="(")
                {
                    String tableName = subString(sql,i+13, j-(i+13)).c_str();
                    tableName.trim();
                    int columnStartIndex = j+1;
                    for (int k = j; k < strlen(sql) ;k++)
                    {
                        if (subString(sql,k,1)==","){
                            columnsList.add(subString(sql,columnStartIndex,k-columnStartIndex));
                            columnStartIndex = k+1;
                        }else if (subString(sql,k,1 )==")")
                        {
                            columnsList.add(subString(sql,columnStartIndex,k-columnStartIndex));
                            DynamicJsonDocument doc(1024);
                            doc = db_open_json();
                            bool tableExists = false;
                            JsonArray tableJsonArrays = doc["tables"].as<JsonArray>();
                            for(JsonObject tableJson : tableJsonArrays) {
                                if (tableJson["name"].as<String>() == tableName)
                                {
                                    tableExists = true;
                                }
                            }
                            if (tableExists)
                            {
                                return false;
                            }
                            
                            JsonArray tableArrays = doc.createNestedArray("tables");
                            JsonObject newTable = tableArrays.createNestedObject();
                            newTable["name"]= tableName;
                            JsonArray columnArrays = newTable.createNestedArray("columns");
                            for (int x = 0; x < columnsList.size(); x++)
                            {
                                columnArrays.add(columnsList.get(x));
                            }
                            JsonArray rowsArray = newTable.createNestedArray("rows");
                            String newDbString;
                            serializeJson(doc,newDbString);
                            // Serial.printf("json => %s\n", newDbString.c_str());
                            return db_write(newDbString);
                        }                        
                    }                    
                }                
            }            
        }        
    }    
    return false;
}
bool DbManager::ExecuteQuery(const char *sql) {
    // for (int i = 0; i < strlen(sql); i++)
    // {
        int i = 0;
        if(subString(sql, i, 6) == "insert"){
            return handleInsert(sql);    
        }else if(subString(sql, i, 6) == "update"){
            return handleUpdate(sql);
        }else if(subString(sql, i, 6) == "delete"){
            return handleDelete(sql);
        }else if(subString(sql, i, 12) == "create table"){
            return handleCreateTable(sql);
        }
    // }
    return false;
}
DynamicJsonDocument DbManager::SelectQuery(const char *sql){
    List columnsList = List();
    bool selectAll = false;
    String tableName = "";
    DynamicJsonDocument resDoc(256);
    int StartIndexColumnName=0;
    int StartIndexTableName=0;
    for (int i = 0; i < strlen(sql); i++)
    {
        if (subString(sql,i,7) == "select ")
        {
            StartIndexColumnName = i + 7;
            for (int j = i+7; j < strlen(sql); j++)
            {
                if (subString(sql,j,1)=="*")
                {
                    selectAll = true;
                }else if(subString(sql,j,1)==","){
                    columnsList.add(subString(sql, StartIndexColumnName , j-StartIndexColumnName));
                    StartIndexColumnName = j + 1 ;
                }else if(subString(sql,j,4)=="from"){
                    if(!selectAll){
                        columnsList.add(subString(sql, StartIndexColumnName , j-StartIndexColumnName));
                    }
                    StartIndexTableName = j + 5;

                }else if(subString(sql,j,4)=="where"){
                    tableName = subString(sql,StartIndexTableName, j-StartIndexColumnName);
                    tableName.trim();
                }
            }
        }        
    }
    if (tableName == "")
    {
        tableName = subString(sql, StartIndexTableName , strlen(sql)-StartIndexTableName);
        tableName.trim();
        Serial.println(tableName);
        
        DynamicJsonDocument doc(1024);
        doc = db_open_json();
        JsonArray tableJsonArrays = doc["tables"].as<JsonArray>();
        for(JsonObject tableJson : tableJsonArrays) {
            if (tableJson["name"].as<String>() == tableName)
            {
                JsonArray rowsJsonArrays = tableJson["rows"].as<JsonArray>();
                JsonArray resRowsJsonArrays = resDoc.createNestedArray("rows");
                for (JsonObject columnJsonObject : rowsJsonArrays)
                    {
                        if (selectAll)
                        {
                            resRowsJsonArrays.add(columnJsonObject);
                        }else{                    
                            DynamicJsonDocument rowJsonDoc(256);
                            JsonObject rowJson = rowJsonDoc.to<JsonObject>();
                            for (int k = 0; k < columnsList.size(); k++)
                            {
                                char *tempRes = columnsList.getCharArray(k);
                                const int colNmeSze = sizeof(columnsList.getCharArray(k));
                                char columnName[colNmeSze+1];
                                for (int h = 0; h <= colNmeSze; h++)
                                {
                                    columnName[h] = tempRes[h];
                                }
                                columnName[colNmeSze+1]='\0';
                                rowJson[columnName] = columnJsonObject[columnName];
                            }
                            resRowsJsonArrays.add(rowJson);
                        }
                }
                return resDoc;
            }
        }
    }
    return resDoc;
}