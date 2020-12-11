/*
  Timer.h - Timer library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef iMolkat_core_h
#define iMolkat_core_h

#include "WString.h"
#include <vfs.h>
#include <SPI.h>
#include <FS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LuaWrapper.h>

#include "DbManager.h"
#include "Api.h"
// include types & constants of Wiring core API
class iMolkat_core
{
    public:
        iMolkat_core();
    private:
        void responseHtml(lua_State *lua);
        String httpGet(String url);
        int ConvertInttoString(lua_State *lua);
        bool db_select(lua_State *lua);
        bool db_exec(lua_State *lua);
        String getHtml();
        void handleRoot();
        void handleNotFound();
        void handleAPI();
        void manageApis(String lua_code);
        // String file_open(char *path);
        ESP8266WebServer _server;
        LuaWrapper lua_global;
        String json_code;
        const char* deviceName = "iMolkat";

        Api apis[100];
        int api_count = 0;

        String api_name;
        char url_text[200];
};

#endif