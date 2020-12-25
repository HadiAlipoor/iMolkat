#include "TouchKey.h"

TouchKey::TouchKey(char* lightName, int relayPin, int touchPin)
{
    relayPin_ = relayPin;
    touchPin_ = touchPin;
    lightName_ = lightName;
    lightStatus_ = false;

    sprintf(SWITCH_LIGHT_API_TITLE, "/api/%s%s", "switch_light_", lightName_.c_str());
    sprintf(GET_STATUS_TITLE, "/api/%s%s", "get_status_", lightName_.c_str());
    Serial.println(SWITCH_LIGHT_API_TITLE);
    Serial.println(GET_STATUS_TITLE);
    pinMode(relayPin, OUTPUT);
    pinMode(touchPin_, INPUT);
}

bool TouchKey::getLightStatus(){
    return lightStatus_;
}

String TouchKey::getLightStatusApi(){
    return String("{\"checked\":" + String(lightStatus_) + "}");
}

// void TouchKey::handleApis(iHandlerFunction switchKeyFunc, iHandlerFunction getStatusFunc){
//     server->on(Uri(SWITCH_LIGHT_API_TITLE),switchKeyFunc);
//     server->on(Uri(GET_STATUS_TITLE),getStatusFunc);

// }

String TouchKey::switchKeyApi(String param, String value){
    Serial.printf("%s = %s\n",param.c_str() ,value.c_str());
    
    if(value == "on"){
        digitalWrite(relayPin_, HIGH);
        lightStatus_ = true;        
    }else if(value == "off"){
        digitalWrite(relayPin_, LOW);
        lightStatus_ = false;
    }else if(value == ""){
        if (lightStatus_){
            digitalWrite(relayPin_, LOW);
            lightStatus_ = false;
        }else{
            digitalWrite(relayPin_, HIGH);
            lightStatus_ = true;        
        }
    }
    return String("{\"checked\":" + String(lightStatus_) + "}");
}

bool TouchKey::checkLoop(){
    if(keyDown && digitalRead(touchPin_)){
        ///key pressed
        if (lightStatus_){
            digitalWrite(relayPin_, LOW);
            lightStatus_ = false;
        }else{
            digitalWrite(relayPin_, HIGH);
            lightStatus_ = true;        
        }
        keyDown =false;
        return true;
    }else if(!keyDown && !digitalRead(touchPin_)){
        keyDown = true;
        return false;
    }
}

String TouchKey::httpGet(String url) {
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
      Serial.printf("[HTTP} Unable to connect: %s\n",url.c_str());
      return "HTTP Unable to connect";
    }
  }
  else {
    Serial.println("WiFi Disconnected");
    return "WiFi Disconnected";
  }
}