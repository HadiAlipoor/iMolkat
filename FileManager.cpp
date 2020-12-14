#include "FileManager.h"

FileManager::FileManager()
{
    dbManager.initialize("FileManager");
    dbManager.ExecuteQuery("create table files(id, fileName, title)");
}

String FileManager::getFileName(String title){
    DynamicJsonDocument doc(2048);
    doc = dbManager.SelectQuery("select * from files");
    Serial.printf("in getFileName with Title : %s ; doc size : %d\n",title.c_str(),doc["rows"].size());
    if(doc["rows"].size() > 0){
        JsonArray files = doc["rows"].as<JsonArray>();
        for(JsonObject file : files){
            if (file["title"] == title)
            {
                Serial.printf("in getFileName FileName : %s\n",file["FileName"].as<String>().c_str());

                return file["FileName"].as<String>();
            }        
        }
    }
    Serial.printf("in getFileName returns NULL\n");

    return "";
}

String FileManager::getNewFileName(String title){
    String fileName = getFileName(title);
    Serial.printf("in getNewFileName fileName = %s\n",title.c_str());
    if (fileName == "")
    {
        int id = dbManager.GetMaxId("files")+1;
        
        
        char sql[100];
        Serial.println("befor sprintf in getNewFileName");
        sprintf (sql , "insert into files (id, fileName, title) values (%d ,%d , %s )",id, id, title.c_str());
        Serial.println("after sprintf in getNewFileName");
        if(dbManager.ExecuteQuery(sql)){
            Serial.printf("getNewFileName: %d\n", id);
            return String(id);
        }else{
            return "error";
        }
    }else{
        return fileName;
    }
}

String FileManager::openFile(String path) {
    // path.replace('.',dotReplacedChar);
    // Launch SPIFFS file system  

    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }

    File file = SPIFFS.open(getFileName(path),"r"); 
    if(!file){ 
        Serial.println("Failed to open file for reading"); 
        return "no_file"; 
    }
    String text = file.readString();
    Serial.println(text);
    SPIFFS.end();
    return text;
}


bool FileManager::writeFile(String path, String text) {
    
    // path.replace('.',dotReplacedChar);
    Serial.printf("path : %s ; content : %s \n******************************\n",path.c_str(), text.c_str());
    // Launch SPIFFS file system  
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }

    File file = SPIFFS.open(getNewFileName(path),"w"); 
    if(!file){ 
        Serial.println("Failed to open file for reading"); 
        return false; 
    } else{
        
    }

    file.print(text);
    file.close();
    SPIFFS.end();
    return true;
}

List FileManager::getFolderFiles(char *path){
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }
    DynamicJsonDocument doc(2048);
    doc = dbManager.SelectQuery("select * from files");
    JsonArray filesArray = doc["rows"].as<JsonArray>();

    

    List files = List();
    
    Dir dir = SPIFFS.openDir(path);
    while (dir.next()) {    
        String fileName = dir.fileName();
        for(JsonObject file : filesArray){
            if (file["fileName"].as<String>() == fileName && file["title"].as<String>().substring(0,sizeof(path)) == path)
            {
                Serial.printf("open FileName: %s\n",fileName.c_str());
                files.add(file["title"].as<String>());
            }        
        }    
    }
    SPIFFS.end();
    return files;
}


bool FileManager::format(){
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }
    bool res = SPIFFS.format();
    SPIFFS.end();
    return res;

}

int FileManager::downloadFileToSpiffs(String url, String filename)
{
  Serial.printf("url in download File : %s\n",url.c_str());
  bool res = false;
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
          SPIFFS.begin();
          Serial.printf("before getNewFileName\n");
          String _fileName = getNewFileName(filename);
          Serial.printf("_fileName : %s \n",filename.c_str());
          File file = SPIFFS.open(_fileName,"w");
          res = http.writeToStream(&file);
          Serial.printf("result of writing %s into spiffs : %d; size : %d\n",filename.c_str() ,res,http.getSize());
          file.close();
          SPIFFS.end();
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        res = false;
      }

      http.end();
      return res;
    } else {
      Serial.printf("[HTTP} Unable to connect: %s\n",url.c_str());
      return false;
    }
  }
  else {
    Serial.println("WiFi Disconnected");
    return false;
  }
}