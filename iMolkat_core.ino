#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LuaWrapper.h>
#include <WiFiManager.h>

#include "DbManager.h"
#include "Api.h"
#include "FileManager.h"

LuaWrapper lua_global;
ESP8266WebServer server(80);
FileManager fileManager;

String json_code;
const char* deviceName = "iMolkat";

Api apis[100];
int api_count = 0;

String api_name;
char url_text[200];

String httpGet(String url) {
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

int ConvertInttoString(lua_State *lua){
  const char *str = String(luaL_checkinteger(lua, 1)).c_str();
  // str = "HAddiii";
  lua_pushstring(lua, str); //str is the const char* that points to your string
  return 1; //we are returning one value, the string
}

bool db_select(lua_State *lua){
  Serial.println("HEREEEE");
  size_t *ldn;
  size_t *lsql;
  const char* dbName =  luaL_checklstring(lua, 1, ldn);
  const char* sql = luaL_checklstring(lua, 2, lsql);
  Serial.println("HEREEEE444444444444");
  Serial.println(sql);
  Serial.println(sql);

  DbManager dbManager = DbManager();
  dbManager.initialize("dbName");
  DynamicJsonDocument doc(1024);
  doc = dbManager.SelectQuery(sql);
  
  String res;
  serializeJson(doc, res);
  Serial.printf("end of db_select: %s",res.c_str());
  lua_pushstring(lua, res.c_str());
  
  return 1;
}

bool db_exec(lua_State *lua){
  size_t *ldn;
  size_t *lsql;
  const char* dbName =  luaL_checklstring(lua, 1, ldn);
  const char* sql = luaL_checklstring(lua, 2, lsql);
  Serial.println(sql);

  DbManager dbManager = DbManager();
  dbManager.initialize("dbName");
  return dbManager.ExecuteQuery("sql");
}

String getHtml() {
  String deviceIP = WiFi.localIP().toString();

  String clientHtml = "Hello World";
  return clientHtml;

}

void handleRoot() {
  server.send(200, "text/html", getHtml());
}

void responseHtmllua(lua_State *lua)
{
  String msg = String(luaL_checkstring(lua, 1));
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  server.send(200, "text/html", msg);
}
void responseHtml(String msg)
{
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  server.send(200, "text/html", msg);
}
void responseFile(String path)
{
  if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }

    File file = SPIFFS.open(path,"r"); 
    if(!file){ 
        Serial.println("Failed to open file for sending"); 
    }
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  server.streamFile(file,"text/html");
    file.close();
    
    // Serial.println(text);
    SPIFFS.end();
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
  Serial.println(lua_global.Lua_dostring(&api_func)); 
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



void Log(String message){
  Serial.printf("Log: %s\n", message.c_str());
}
void errLog(String message){
  Serial.printf("error log : %s", message.c_str());
}

void getApp(String url){
 FSInfo fsinfo;
    SPIFFS.info(fsinfo);
    Serial.printf("fsinfo.blockSize: %d,fsinfo.maxOpenFiles: %d,fsinfo.maxPathLength: %d,fsinfo.pageSize: %d,fsinfo.totalBytes: %d,fsinfo.usedBytes: %d",fsinfo.blockSize,fsinfo.maxOpenFiles,fsinfo.maxPathLength,fsinfo.pageSize,fsinfo.totalBytes,fsinfo.usedBytes); 
  String fileConfig = httpGet(url);
  Serial.println(fileConfig);
  DynamicJsonDocument configDoc(1024);
  
  deserializeJson(configDoc, fileConfig);
  JsonArray filesJsonArrays = configDoc["files"].as<JsonArray>();
  Serial.printf("jsonarray size : %d\n",filesJsonArrays.size());
  for(String urlPath : filesJsonArrays) {
    Serial.println(urlPath);
    String  filePath = urlPath;
    filePath.replace("http://www.imolkat.com/react","");
    filePath.replace("http://www.imolkat.com","");
    delay(5);
    Serial.printf("download %s result : %d\n",filePath.c_str(), fileManager.downloadFile(urlPath, filePath));
  }
    SPIFFS.info(fsinfo);
    Serial.printf("fsinfo.blockSize: %d,fsinfo.maxOpenFiles: %d,fsinfo.maxPathLength: %d,fsinfo.pageSize: %d,fsinfo.totalBytes: %d,fsinfo.usedBytes: %d",fsinfo.blockSize,fsinfo.maxOpenFiles,fsinfo.maxPathLength,fsinfo.pageSize,fsinfo.totalBytes,fsinfo.usedBytes); 
}

void handleFileServer(){
  String uri = server.uri();

  Serial.printf("url : %s\n", uri.c_str());
  
  responseFile(uri);

}
void manageFileServer(){

  List files = fileManager.getFolderFiles("/");

  Serial.printf("files count : %d", files.size());
  for (int i = 0; i < files.size(); i++)
  {
    Serial.printf("file : %s\n",files.get(i).c_str());
    server.on(files.get(i), handleFileServer);    
  }
  server.on("/format",[]{fileManager.format();responseHtml("formated");});
  server.on("/getapp",[]{getApp("http://www.imolkat.com/app.json");responseHtml("app installed");});
  server.on("/resetfs",[]{manageFileServer();responseHtml("server reseted");});
  server.on("/files",[]{if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }
    List files = List();
    
    Dir dir = SPIFFS.openDir("/");
    String fileLists="";
    while (dir.next()) {    
        String fileName = dir.fileName();
        Serial.printf("fileName : %s, fileSize: %d\n",fileName.c_str(),dir.fileSize());
        // fileLists = String()
        files.add(fileName);
    }
    SPIFFS.end();
    responseHtml("printed");
    });
  
}

void setup() {
  SPIFFS.begin();
  // SPIFFS.format();
  fileManager  = FileManager();

  WiFiManager wifiManager;
  //Static IP address configuration
  IPAddress staticIP(192, 168, 1, 60); //ESP static ip
  IPAddress gateway(192, 168, 1, 1);   //IP Address of your WiFi Router (Gateway)
  IPAddress subnet(255, 255, 0, 0);
  IPAddress primaryDNS(8, 8, 8, 8);   //optional
  IPAddress secondaryDNS(8, 8, 4, 4); //optional
  
  wifiManager.setSTAStaticIPConfig(staticIP, gateway, subnet, primaryDNS);

  wifiManager.autoConnect("DIAKO_AP");
  #ifndef STASSID
  #define STASSID "V20"
  #define STAPSK  "qazxsw21"
  #endif

  const char* ssid = STASSID;
  const char* password = STAPSK;


  Serial.begin(115200);


//  WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)local_
//  if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
//    Serial.println("STA Failed to configure");
//  }
//  WiFi.mode(WIFI_STA);
//  WiFi.begin(ssid, password);
//
//  // Wait for connection
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
//  Serial.println("");
//  Serial.print("Connected to ");
//  Serial.println(ssid);
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());
//
//  if (MDNS.begin("esp8266")) {
//    Serial.println("MDNS responder started");
//  }
//  

  // String lua_json = httpGet("http://www.imolkat.com/json.lua");
  String lua_code = httpGet("http://www.imolkat.com/code.lua");
  Serial.println(lua_code);
  lua_global.Lua_register("responseHtml", (const lua_CFunction) &responseHtmllua);
  lua_global.Lua_register("SelectQuery", (const lua_CFunction) &db_select);
  lua_global.Lua_register("ExecuteQuery", (const lua_CFunction) &db_exec);
  lua_global.Lua_register("tostring", (const lua_CFunction) &ConvertInttoString);
  // Serial.println(lua_global.Lua_dostring(&lua_json));
  // lua_code = lua_json + "\n" + lua_code;
  // Serial.println(lua_code);
  
  // Serial.println(lua_global.Lua_dostring(&lua_json));
  // luaL_openlibs(lua_global.get_lua_State());
  Serial.println(lua_global.Lua_dostring(&lua_code));
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
  // DbManager dbManager = DbManager("MyTest");
  // dbManager.ExecuteQuery("create table myTable(id, title)");
  getApp("http://www.imolkat.com/app.json");

  // Serial.println("getting App finished.***********************************************************************************");
  manageFileServer();

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  String run_setup = String("Setup()");
  Serial.println(lua_global.Lua_dostring(&run_setup));
  Serial.printf("true value is %d\n",true);
  Serial.println(String(ESP.getFreeHeap()));

}


void loop(void) {
  // String run_loop = String("Loop()");
  // lua_global.Lua_dostring(&run_loop);
  // Serial.println(String(ESP.getFreeHeap()));
  
  server.handleClient();
  MDNS.update();

}
