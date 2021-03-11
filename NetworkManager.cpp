#include "NetworkManager.h"

NetworkManager::NetworkManager(int deviceIndex){
    wifiManager.erase();
    
    
    _currentDeviceIndex = deviceIndex;
    _HotspotList = List();
    _PasswordList = List();
    _Settings = List();
    _DevicesIP = List();
    
    _HotspotList.add("DIACO_AP1");
    _HotspotList.add("DIACO_AP2");
    _HotspotList.add("DIACO_AP3");
    _HotspotList.add("DIACO_AP4");
    _HotspotList.add("DIACO_AP5");
    
    _PasswordList.add("Uwje24Po");
    _PasswordList.add("D3Fgv345");
    _PasswordList.add("Rve3l56s");
    _PasswordList.add("k4l5EGw2");
    _PasswordList.add("orR4fFeE");

    
    _Settings.loadFromFile("/_Settings.ldb");
    _DevicesIP.loadFromFile("/_DevicesIP.ldb");

}

bool NetworkManager::IntegrateAllDevices(){
    String mainSsid = wifiManager.getWiFiSSID();
    String mainPassword = wifiManager.getWiFiPass();

    int numberOfNetworks = WiFi.scanNetworks();
    IPAddress localIP = WiFi.localIP();
    
    for(int i =0; i<numberOfNetworks; i++){
        String ssid = WiFi.SSID(i);
        Serial.print("Network name: ");
        Serial.println(ssid);
        if (ssid.length() > 9)
        {
            if (ssid.substring(0,8).equals("DIACO_AP"))
            {
                for (int j = 0; j < _HotspotList.size(); j++)
                {
                    if (ssid.equals(_HotspotList.get(j)))
                    {
                        String password = _PasswordList.get(j);
                        WiFi.begin(ssid,password);
                        
                        IPAddress deviceIp;
                        deviceIp[0]= localIP[0];
                        deviceIp[1]= localIP[1];
                        deviceIp[2]= localIP[2];
                        if (_DevicesIP.size() > 0)
                        {
                            int maxIp = _DevicesIP.get(_DevicesIP.size()-1).toInt()+1;
                            deviceIp[3]= maxIp;
                        }else{
                            deviceIp[3]= 72;
                        }
                        
                        httpGet("http://192.168.4.1/os/setwifiinfo?ssid="+mainSsid+"&password="+mainPassword+"&ip="+deviceIp.toString());
                        WiFi.disconnect();
                        
                    }
                }                
            }            
        }
        
        Serial.print("Signal strength: ");
        Serial.println(WiFi.RSSI(i));
        Serial.println("-----------------------");
      }
      return true;
}

bool NetworkManager::configNetwork(){
    //Static IP address configuration
    // IPAddress staticIP(192, 168, 0, 1); //ESP static ip
    // IPAddress gateway(192, 168, 0, 1);   //IP Address of your WiFi Router (Gateway)
    // IPAddress subnet(255, 255, 0, 0);
    // IPAddress primaryDNS(8, 8, 8, 8);   //optional
    // IPAddress secondaryDNS(8, 8, 4, 4); //optional
    // server = wifiManager.server;
    WiFi.mode(WIFI_STA); 
    wifiManager.setConfigPortalBlocking(false);
    wifiManager.setCaptivePortalEnable(false);
    // wifiManager.setSTAStaticIPConfig(staticIP, gateway, subnet, primaryDNS);
    wifiManager.autoConnect(getDeviceApName().c_str(),getDevicePassword().c_str());
    //set config save notify callback
    return true;
}
void NetworkManager::process(){
    wifiManager.process();
}
bool NetworkManager::configNetwork(String ssid, String password, String ip){  
    //Static IP address configuration
    IPAddress staticIP; //ESP static ip
    staticIP.fromString(ip);
    
    // wifiManager.setSTAStaticIPConfig(staticIP);//, gateway, subnet, primaryDNS);

    char const *_pass = password.c_str();
    char const *_ssid = ssid.c_str();
    wifiManager.autoConnect(_ssid,_pass);
    
    return true;
}

String NetworkManager::getOtherDevicePassword(String apName){
    for (int i = 0; i < _HotspotList.size(); i++)
    {
        if (_HotspotList.get(i) == apName)
        {
            return _PasswordList.get(i);
        }        
    }
    return "not_found";
}

String NetworkManager::getDeviceApName(){
    return _HotspotList.get(_currentDeviceIndex);
}

String NetworkManager::getDevicePassword(){
    return _PasswordList.get(_currentDeviceIndex);
}

bool NetworkManager::setWifiData(String wifiName, String wifiPassword, String ipAddress){
    _Settings.add(wifiName);
    _Settings.add(wifiPassword);
    _Settings.add(ipAddress);
    ///delete it before save!
    _Settings.saveInFile("/_Settings.ldb");
    return true;
}

String NetworkManager::getWifiName(){
    if(_Settings.size()>0)
        return _Settings.get(0);
    else
        return "null";
}

String NetworkManager::getWifiPassword(){
    if(_Settings.size()>0)
        return _Settings.get(1);
    else
        return "null";
}

String NetworkManager::getIPAddress(){
    if(_Settings.size()>0)
        return _Settings.get(2);
    else
        return "null";
}

String NetworkManager::httpGet(String url) {
  Serial.printf("url in httpGet : %s\n",url.c_str());
  String payload ;
  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;

    HTTPClient http;
    if (http.begin(client, url.c_str())) {  // HTTP
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          payload = http.getString();
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        payload = ("error: %s", http.errorToString(httpCode).c_str());
      }

      http.end();
      return payload;
    } else {
      Serial.printf("[HTTP] Unable to connect: %s\n",url.c_str());
      return "HTTP Unable to connect";
    }
  }
  else {
    Serial.println("WiFi Disconnected");
    return "WiFi Disconnected";
  }
}

void NetworkManager::saveConfigCallback(){
    
    IntegrateAllDevices();
    Serial.println("All devices integrated;");
}

void NetworkManager::handlApis(){
    wifiManager.preSetupConfigPortal();
    wifiManager.server->on("/os/test",std::bind(&NetworkManager::osTest, this));
}

void NetworkManager::osTest(){
    Serial.println(wifiManager.server->argName(0)); 
    wifiManager.responseHtml("Hello");
}