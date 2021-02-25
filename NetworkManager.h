/*
  Timer.h - Timer library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef NetworkManager_h
#define NetworkManager_h

#include "List.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>

enum NetworkSettings{
    WifiAp,
    WifiPassword,
    IpAddress
};

class NetworkManager{
    public:
        NetworkManager();
        NetworkManager(int deviceIndex);
        String getOtherDevicePassword(String apName);
        String getDeviceApName();
        String getDevicePassword();
        bool setWifiData(String wifiName, String wifiPassword, String ipAddress);
        String getWifiName();
        String getWifiPassword();
        String getIPAddress();
        bool IntegrateAllDevices(String mainSsid, String mainPassword);
        bool configNetwork();
        bool configNetwork(String ssid, String password, String ip);
        String httpGet(String url);
    private:
        int _currentDeviceIndex;
        List _HotspotList;
        List _PasswordList;
        List _Settings;
        List _DevicesIP;
        WiFiManager wifiManager;

}
#endif