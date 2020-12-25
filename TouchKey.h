/*
  Timer.h - Timer library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef TouchKey_h
#define TouchKey_h
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
// #include "iMolkatWebServer.h"

class TouchKey{
    public:
      TouchKey(char* lightName, int relayPin, int touchPin);
      bool getLightStatus();
      String getLightStatusApi();
      String switchKeyApi(String param, String value);
      char SWITCH_LIGHT_API_TITLE[30];
      char GET_STATUS_TITLE[30];
      bool checkLoop();
    private:
      String lightName_;
      String httpGet(String url);
      int relayPin_;
      int touchPin_;
      bool lightStatus_;
      bool keyDown = false;

    
};
#endif
