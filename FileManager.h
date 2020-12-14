/*
  Timer.h - Timer library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef FileManager_h
#define FileManager_h

#include "WString.h"
#include <vfs.h>
#include <SPI.h>
#include <FS.h>
#include "List.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "DbManager.h"

class FileManager{
    public:
        FileManager();
        char *openFile(String path);
        bool writeFile(String path, String text);
        List getFolderFiles(char *path);
        bool format();
        int downloadFileToSpiffs(String url, String fileName);
        bool downloadFile(String url, String filename);
    private:
        List fileNameList= List();
        List fileTitleList= List();
        void saveToFile();
        void loadFromFile();
        String getFileName(String title);
        String getNewFileName(String title);
};
#endif