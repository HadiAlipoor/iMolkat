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
        DbManager(const char *databaseName);
        bool ExecuteQuery(const char *sql);
        DynamicJsonDocument SelectQuery(const char *sql);
        bool CheckForDatabase();
        String subString(const char *text, int startIndex, int size);
        String db_open();
        bool db_write(String text);
        char *getDbPath();
        DynamicJsonDocument db_open_json();        
    private:
        bool handleInsert(const char *sql);
        bool handleUpdate(const char *sql);
        bool handleDelete(const char *sql);
        bool handleCreateTable(const char *sql);
        String getString(String text);
        const char *dbName;
        bool parseExecQuery(const char *sql);
};
#endif