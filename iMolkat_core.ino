#include <ESP8266HTTPClient.h>
#include <Ethernet.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);
String json_code; 

// Module connection pins (Digital Pins)
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define D9 3
#define D10 1

class Variable{
  public:
    struct VariableStruct{
      String type;
      String value;
    };
    Variable(){
    }
    void addNewVariable(int index, String type, String value){
      variableArrays[index].type = type;
      variableArrays[index].value = value;
    }

    void changeValue(int index, String value){
      variableArrays[index].value = value;
    }

    String getType(int index){
      return variableArrays[index].type;
    }
    String getStringValue(int index){
      return variableArrays[index].value;
    }
    int getIntValue(int index){
      return variableArrays[index].value.toInt();
    }
    bool getBoolValue(int index){
      if(variableArrays[index].value == "true")
        return true;
      else
        return false;
    }
  private:
    VariableStruct variableArrays[100];
};

String httpGet(String url){
  String payload ;
  //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
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
      Serial.printf("[HTTP} Unable to connect\n");
      return "HTTP Unable to connect";
    }
  }
  else {
    Serial.println("WiFi Disconnected");
    return "WiFi Disconnected";
  }
}

Variable publicVariables = Variable();
int pins[10];

void manageVariables(String jsonCode){
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, jsonCode);
  for(int i = 0;i < doc["public_variables"].size();i++){
    publicVariables.addNewVariable(doc["public_variables"][i]["index"].as<int>(), doc["public_variables"][i]["type"].as<String>(),doc["public_variables"][i]["value"].as<String>());
  }
}

void managePins(String jsonCode){
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, jsonCode);
  for(int i = 0;i < doc["pins"].size();i++){
    if(doc["pins"][i]["name"].as<String>() == "D0"){
      pins[doc["pins"][i]["index"].as<int>()] = D0;
    }else if(doc["pins"][i]["name"].as<String>() == "D1"){
      pins[doc["pins"][i]["index"].as<int>()] = D1;
    }else if(doc["pins"][i]["name"].as<String>() == "D2"){
      pins[doc["pins"][i]["index"].as<int>()] = D2;
    }else if(doc["pins"][i]["name"].as<String>() == "D3"){
      pins[doc["pins"][i]["index"].as<int>()] = D3;
    }else if(doc["pins"][i]["name"].as<String>() == "D4"){
      pins[doc["pins"][i]["index"].as<int>()] = D4;
    }else if(doc["pins"][i]["name"].as<String>() == "D5"){
      pins[doc["pins"][i]["index"].as<int>()] = D5;
    }else if(doc["pins"][i]["name"].as<String>() == "D6"){
      pins[doc["pins"][i]["index"].as<int>()] = D6;
    }else if(doc["pins"][i]["name"].as<String>() == "D7"){
      pins[doc["pins"][i]["index"].as<int>()] = D7;
    }else if(doc["pins"][i]["name"].as<String>() == "D8"){
      pins[doc["pins"][i]["index"].as<int>()] = D8;
    }else if(doc["pins"][i]["name"].as<String>() == "D9"){
      pins[doc["pins"][i]["index"].as<int>()] = D9;
    }else if(doc["pins"][i]["name"].as<String>() == "D10"){
      pins[doc["pins"][i]["index"].as<int>()] = D10;
    }

    if(doc["pins"][i]["set_pin"].as<String>() == "output"){
      pinMode(pins[doc["pins"][i]["index"].as<int>()], OUTPUT);
    }else if(doc["pins"][i]["set_pin"].as<String>() == "input"){
      pinMode(pins[doc["pins"][i]["index"].as<int>()], INPUT);
    }

    if(!doc["pins"][i]["default_value"].isNull()){
      if(doc["pins"][i]["default_value"].as<String>() == "high"){
        digitalWrite(pins[doc["pins"][i]["index"].as<int>()], HIGH);
      }else if(doc["pins"][i]["default_value"].as<String>() == "low"){
        digitalWrite(pins[doc["pins"][i]["index"].as<int>()], LOW);
      }
    }
  }
  
}
String getHtml() {
  String deviceIP = WiFi.localIP().toString();
  
  String clientHtml = "Hello World";
  return clientHtml;

}
void handleRoot() {
  server.send(200, "text/html", getHtml());
}


void responseHtml(String _status)
{
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", _status);
}

void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}




const char* deviceName = "iMolkat";

void manageResponse(DynamicJsonDocument outputs){
  String output = "";
  for(int i = 0;i < outputs.size();i++){
    if(!outputs[i]["public_variable"].isNull()){
      output = output + publicVariables.getStringValue(outputs[i]["public_variable"].as<int>());
    }else{
      output = output + outputs[i]["value"].as<String>();
    }
  }

  responseHtml(output);
}

void handle_if_action(DynamicJsonDocument action){
  String variableType;
  if(!action["condition"]["first_part"]["public_variable"].isNull()){
    variableType = publicVariables.getType(action["condition"]["first_part"]["public_variable"].as<int>());
  }
  else{
    variableType = action["condition"]["first_part"]["type"].as<String>();
  }
  
  if(variableType == "String"){
      String first_part;
      if(!action["condition"]["first_part"]["public_variable"].isNull()){
        first_part = publicVariables.getStringValue(action["condition"]["first_part"]["public_variable"].as<int>());
      }else{
        first_part = action["condition"]["first_part"]["value"].as<String>();
      }
      String second_part;
      if(!action["condition"]["second_part"]["public_variable"].isNull()){
        second_part = publicVariables.getStringValue(action["condition"]["second_part"]["public_variable"].as<int>());
      }else{
        second_part = action["condition"]["second_part"]["value"].as<String>();
      }
      String comparison = action["condition"]["comparison"].as<String>();
      if(comparison == "=="){
        if(first_part == second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }else if(comparison == ">"){
        if(first_part > second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }else if(comparison == "=>" || comparison == ">="){ 
        if(first_part >= second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }else if(comparison == "<"){      
        if(first_part < second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }else if(comparison == "<=" || comparison == "=<"){
        if(first_part <= second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }else if(comparison == "!="){
        if(first_part != second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }
  }else if(variableType == "int"){
      int first_part;
      if(!action["condition"]["first_part"]["public_variable"].isNull()){
        first_part = publicVariables.getIntValue(action["condition"]["first_part"]["public_variable"].as<int>());
      }else{
        first_part = action["condition"]["first_part"]["value"].as<int>();
      }
      int second_part;
      if(!action["condition"]["second_part"]["public_variable"].isNull()){
        second_part = publicVariables.getIntValue(action["condition"]["second_part"]["public_variable"].as<int>());
      }else{
        second_part = action["condition"]["second_part"]["value"].as<int>();
      }
      String comparison = action["condition"]["comparison"].as<String>();
      if(comparison == "=="){
        if(first_part == second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }else if(comparison == ">"){
        if(first_part > second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }else if(comparison == "=>" || comparison == ">="){ 
        if(first_part >= second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }else if(comparison == "<"){      
        if(first_part < second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }else if(comparison == "<=" || comparison == "=<"){
        if(first_part <= second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }else if(comparison == "!="){
        if(first_part != second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }
  }else if(variableType == "bool"){
      String first_part;
      if(!action["condition"]["first_part"]["public_variable"].isNull()){
        first_part = publicVariables.getStringValue(action["condition"]["first_part"]["public_variable"].as<int>());
      }else{
        first_part = action["condition"]["first_part"]["value"].as<String>();
      }
      String second_part;
      if(!action["condition"]["second_part"]["public_variable"].isNull()){
        second_part = publicVariables.getStringValue(action["condition"]["second_part"]["public_variable"].as<int>());
      }else{
        second_part = action["condition"]["second_part"]["value"].as<String>();
      }
      String comparison = action["condition"]["comparison"].as<String>();
      if(comparison == "=="){
        if(first_part == second_part){
          Serial.println("true");
          do_action(action["then"]);
        }else{
          Serial.println("false");
          do_action(action["else"]);
        }
      }else if(comparison == "!="){
        if(first_part != second_part){
          do_action(action["then"]);
        }else{
          do_action(action["else"]);
        }
      }
  }
}
void do_action(DynamicJsonDocument action){
  for(int i = 0;i < action.size();i++){
    Serial.println(action[i]["command"].as<String>());
    if(action[i]["command"].as<String>() == "if"){
      handle_if_action(action[i]);
    }else if(action[i]["command"].as<String>() == "change_variable"){
      publicVariables.changeValue(action[i]["public_variable"].as<int>(), action[i]["value"].as<String>());
    }else if(action[i]["command"].as<String>() == "digital_write"){
      if(action[i]["value"].as<String>() == "high"){
        digitalWrite(pins[action[i]["pin"].as<int>()], HIGH);
      }else if(action[i]["value"].as<String>() == "low"){
        digitalWrite(pins[action[i]["pin"].as<int>()], LOW);
      }
    }else if(action[i]["command"].as<String>() == "delay"){
      delay(action[i]["milisecond"].as<int>());
    }else if(action[i]["command"].as<String>() == "response"){
      manageResponse(action[i]["outputs"]);
    }
  }
}
void handleAPI(String param, String value)
{
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, json_code);

  for(int i = 0;i < doc["apis"].size();i++){
    String jsonParam = doc["apis"][i]["param"];
    if(param == jsonParam){
      do_action(doc["apis"][i]["action"]);
    }
  }
}



void setup() {
#ifndef STASSID
#define STASSID "WIFI_SSID" //change it!
#define STAPSK  "WIFI_PASSWORD" //change it!
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

  //Static IP address configuration
IPAddress staticIP(192, 168, 1, 66); //ESP static ip
IPAddress gateway(192, 168, 1, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

  Serial.begin(115200);
  
  
  WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)local_
  if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  json_code = httpGet("http://www.imolkat.com/jasoncode.html");
  Serial.println(json_code);
  manageVariables(json_code);
  managePins(json_code);

  server.on("/", handleRoot);


  server.on("/api", []() {

    String param = server.argName(0);
    String value = server.arg(0);
    Serial.println(param + " = " + value);
    handleAPI(param, value);
  });

  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}


void loop(void) {
  server.handleClient();
  MDNS.update();

  DynamicJsonDocument doc(4096);
  deserializeJson(doc, json_code);
  Serial.println(doc["loop"].size());
  do_action(doc["loop"]);
}
