/*
  Timer.h - Timer library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef DbManager_h
#define DbManager_h

#include "WString.h"
#include <vfs.h>
#include <SPI.h>
#include <FS.h>
#include <ArduinoJson.h>
extern "C" {
#include "user_interface.h"
}
#include "List.h"

class DbManager{
    public:
        DbManager();
        bool initialize(char *databaseName);
        bool ExecuteQuery(char *sql);
        DynamicJsonDocument SelectQuery(const char *sql);
        bool CheckForDatabase();
        String subString(const char *text, int startIndex, int size);
        String db_open();
        bool db_write(String text);
        char *getDbPath();
        DynamicJsonDocument db_open_json();        
        int GetMaxId(char *tableName);
    private:
        bool handleInsert(char *sql);
        bool handleUpdate(char *sql);
        bool handleDelete(char *sql);
        bool handleCreateTable(char *sql);
        String getString(String text);
        char *dbName;
        char dbPath[100];
        bool parseExecQuery(char *sql);

};
#endif