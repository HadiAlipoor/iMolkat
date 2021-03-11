

// #include "iMolkatWebServer.h"
#include "TouchKey.h"
// #include "FileManager.h"




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "WiFiManager.h"

#include "NetworkManager.h"
#include "DbManager.h"
#include "FileManager.h"


ESP8266WebServer server(80);

FileManager fileManager;

const char* deviceName = "iMolkat";

// typedef std::function<void(void)> iHandlerFunction;
int dgtlrd;

String deviceIP;

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
      Serial.printf("[HTTP] Unable to connect: %s\n",url.c_str());
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
  server.sendHeader("Location", "http://www.imolkat.com/panel/",true); //Redirect to our html web page 
  server.send(302, "text/plane",""); 
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
  String mime_type = "";
  Serial.println(path.substring(path.length()-4,path.length()));
  if (path.substring(path.length()-4,path.length()) == String(".css"))
  {
    mime_type = "text/css";
  }else if (path.substring(path.length()-5,path.length()) == ".html")
  {
    mime_type = "text/html";
  }else{
    mime_type = "text/html";
  }
  
    File file = SPIFFS.open(path,"r"); 
    if(!file){ 
        Serial.println("Failed to open file for sending"); 
    }
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  server.streamFile(file,mime_type);
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
  
}

void errLog(String message){
  Serial.printf("error log : %s", message.c_str());
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
  server.on("/dgtlrd",[]{responseHtml(String(dgtlrd));});
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


TouchKey touchKey1 = TouchKey("kitchen",14,5);
TouchKey touchKey2 = TouchKey("hall1",12,0);
TouchKey touchKey3 = TouchKey("hall2",16,4);
TouchKey touchKey4 = TouchKey("bedroom",13,2);


NetworkManager networkManager(0);
 
void setup() {
  Serial.begin(115200);
  SPIFFS.begin();

  // networkManager = NetworkManager(0);
  networkManager.handlApis();
  networkManager.configNetwork();
  networkManager.wifiManager.setSaveConfigCallback([]{
    networkManager.IntegrateAllDevices();
  });
  
  // FileManager fileManager  = FileManager();

Serial.println(touchKey1.SWITCH_LIGHT_API_TITLE);
Serial.println("touchKey1.SWITCH_LIGHT_API_TITLE***");

  //// OS APIs
    
  server.on(Uri("/os/setwifiinfo"),[]{
    String ssidParam = server.argName(0);
    String ssidValue = server.arg(0);
    String passParam = server.argName(1);
    String passValue = server.arg(1);
    String ipParam = server.argName(2);
    String ipValue = server.arg(2);
    if (ssidParam == "ssid" && passParam == "password" && ipParam == "ip")
    {
      networkManager.configNetwork(ssidValue,passValue,ipValue);
    }
    responseHtml("asdasd");
  });
  
  
  ////APIs--------------------------

  
  server.on(Uri(touchKey1.SWITCH_LIGHT_API_TITLE),[]{
    String param = server.argName(0);
    String value = server.arg(0);
    responseHtml(touchKey1.switchKeyApi(param, value));});
  server.on(Uri(touchKey1.GET_STATUS_TITLE),[]{responseHtml(touchKey1.getLightStatusApi());});

  server.on(Uri(touchKey2.SWITCH_LIGHT_API_TITLE),[]{
    String param = server.argName(0);
    String value = server.arg(0);
    responseHtml(touchKey2.switchKeyApi(param, value));});
  server.on(Uri(touchKey2.GET_STATUS_TITLE),[]{responseHtml(touchKey2.getLightStatusApi());});

  server.on(Uri(touchKey3.SWITCH_LIGHT_API_TITLE),[]{
    String param = server.argName(0);
    String value = server.arg(0);
    responseHtml(touchKey3.switchKeyApi(param, value));});
  server.on(Uri(touchKey3.GET_STATUS_TITLE),[]{responseHtml(touchKey3.getLightStatusApi());});

  server.on(Uri(touchKey4.SWITCH_LIGHT_API_TITLE),[]{
    String param = server.argName(0);
    String value = server.arg(0);
    responseHtml(touchKey4.switchKeyApi(param, value));});
  server.on(Uri(touchKey4.GET_STATUS_TITLE),[]{responseHtml(touchKey4.getLightStatusApi());});
  deviceIP = WiFi.localIP().toString();

  
    
  server.on("/", handleRoot);


  server.on("/api", []() {  

    String param = server.argName(0);
    String value = server.arg(0);
    Serial.println(param + " = " + value);
    
    // handleAPI(param, value);
  });

  ////APIs--------------------------

  // if(fileManager.getFolderFiles("/").size() <15){
  //   // fileManager.format();
  //   getApp("http://www.imolkat.com/app.json");
  // }

  manageFileServer();

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println(String(ESP.getFreeHeap()));

}

void loop(void) {
  touchKey1.checkLoop();  
  touchKey2.checkLoop();  
  touchKey3.checkLoop();  
  touchKey4.checkLoop();  


  // Serial.println(digitalRead(2));
  server.handleClient();
  networkManager.process();
  MDNS.update();
}
