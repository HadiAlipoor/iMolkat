#ifndef iMolkatWebServer_h
#define iMolkatWebServer_h

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>

#include "DbManager.h"
#include "FileManager.h"


ESP8266WebServer server(80);

FileManager fileManager;

const char* deviceName = "iMolkat";

// typedef std::function<void(void)> iHandlerFunction;


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

String getHtml() {
  String deviceIP = WiFi.localIP().toString();

  String clientHtml = "Hello World";
  return clientHtml;

}

void handleRoot() {
  server.send(200, "text/html", getHtml());
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
#endif