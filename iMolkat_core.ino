#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LuaWrapper.h>

LuaWrapper lua;
ESP8266WebServer server(80);
String json_code;

class Api
{
  public:
    Api(){
      Api::inputIndex = 0;
      Api::inputs = ",";
    }
    void setName(String name){
      Api::name = name;
    }
    String getName(){
      return Api::name;
    }
    void addInput(String input){
      Api::inputs = Api::inputs + input + ",";
      Api::inputIndex++;
    }
    int getSize(){
      return Api::inputIndex;
    }
    String getInput(int index)
    {
      int indexCount = 0;
      int start_index = 0;
      for (int i = 0; i < Api::inputs.length()-1; i++)
      {
        if (Api::inputs.substring(i,i+1) == ",")
        {
          if (start_index == 0)
          {
            start_index = i + 1;
          }
          else
          {
            String input = Api::inputs.substring(start_index, i);
            if (indexCount == index)
            { 
              return input;
            }
            indexCount++;
          }
        }
      }
    }

  private:
    String name;
    String inputs;
    int inputIndex;
};

Api apis[100];
int api_count = 0;

String httpGet(String url) {
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
      Serial.printf("[HTTP} Unable to connect\n");
      return "HTTP Unable to connect";
    }
  }
  else {
    Serial.println("WiFi Disconnected");
    return "WiFi Disconnected";
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


void responseHtml(lua_State *lua)
{
  String msg = String(luaL_checkstring(lua, 1));
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", msg);
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


void handleAPI()
{
  String api_name=server.uri().substring(5);
  String inputs_string = "";
  Serial.println(api_name);
  int output = server.args();
  // struct Argument
  // {
  //   String value;
  //   String parameter;
  // };
  // Argument arguments[100];
  // for (int j = 0; j < output; j++)
  // {
  //   Argument argg;
  //   argg.parameter = server.argName(j);
  //   argg.value = server.arg(j);
  //   Serial.println(argg.parameter + " = " + argg.value);
  //   arguments[j] = argg;
  // }
  // for (int i = 0; i < sizeof(apis); i++)
  // {
  //   if(apis[i].getName() == api_name)
  //   {
  //     for (int j = 0; j < apis[i].getSize(); j++)
  //     {
  //       inputs_string = inputs_string + apis[i].getInput(j);
  //       if (j != apis[i].getSize()-1)
  //       {
  //         inputs_string = inputs_string + ", ";
  //       }
  //     }
  //     break;
  //   }
  // }
  
  char run_api_func[200];
  snprintf(run_api_func,sizeof(run_api_func),"%s(%s)",api_name.c_str(),inputs_string.c_str());
  // Serial.printf("url_text : %s; function : %s()",String(url_text).c_str(),api_name.c_str());
  String api_func = String(run_api_func);
  Serial.println(api_func);
  Serial.println(lua.Lua_dostring(&api_func)); 
  // responseHtml("OK");     
}

void manageApis(String lua_code){
  for (int i = 0; i < lua_code.length()-8; i++)
  {
    if (lua_code.substring(i, i+6)=="--#api")
    {
      int start_index = 0;
      for (int j = i+3; j < lua_code.length()-10; j++)
      {        
        if (lua_code.substring(j,j+8)=="function")
        {
          start_index = j+9;
        }
        if(start_index!=0 && lua_code.substring(j,j+1)=="(")
        {
          String api_name = lua_code.substring(start_index,j);
          api_name.trim();
          apis[api_count].setName(api_name);
          int input_start_index = 0;
          if(lua_code.substring(j+1,j+2) != ")")
          {
            for (int k = j+2; k < lua_code.length()-10; k++)
            {
              if(lua_code.substring(k,k+1) == ",")
              {
                String input = lua_code.substring(input_start_index, k);
                input.trim();
                apis[api_count].addInput(input);
                input_start_index = k+1;
              }
              else if(lua_code.substring(k,k+1) == ")")
              {
                String input = lua_code.substring(input_start_index, k);
                input.trim();
                apis[api_count].addInput(input);
                break;
              }
            }
          }
          api_count++;
          break;
        }        
      }      
    }    
  }  
}
String api_name;
char url_text[200];
void setup() {
#ifndef STASSID
#define STASSID "YOUR_SSID"
#define STAPSK  "YOUR_PASSWORD"
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
  String lua_code = httpGet("http://www.imolkat.com/code.html");
  lua.Lua_register("responseHtml", (const lua_CFunction) &responseHtml);
  Serial.println(lua.Lua_dostring(&lua_code));
  manageApis(lua_code);
  
  server.on("/", handleRoot);

  for (int i = 0; i < api_count; i++)
  {
    api_name = apis[i].getName();
    snprintf(url_text,sizeof(url_text), "/api/%s",api_name.c_str());
    Serial.println(url_text);
    server.on(String(url_text), handleAPI);

  }
  

  server.on("/api", []() {

    String param = server.argName(0);
    String value = server.arg(0);
    Serial.println(param + " = " + value);
    // handleAPI(param, value);
  });


  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  String run_setup = String("Setup()");
  Serial.println(lua.Lua_dostring(&run_setup));
}


void loop(void) {
  String run_loop = String("Loop()");
  lua.Lua_dostring(&run_loop);
  server.handleClient();
  MDNS.update();

}
