#include "FileManager.h"

FileManager::FileManager()
{
    loadFromFile();
}

String FileManager::getFileName(String title){
    for (int i = 0; i < fileTitleList.size(); i++)
    {
        if (fileTitleList.get(i) == title)
        {
            return fileNameList.get(i);
        }
        
    }    
    return "";
}

String FileManager::getNewFileName(String title){
    String fileName = getFileName(title);
    Serial.printf("in getNewFileName fileName = %s\n",title.c_str());
    if (fileName == "")
    {
        int maxId = 0;
        for (int i = 0; i < fileNameList.size(); i++)
        {
            if (fileNameList.get(i).toInt() > maxId)
            {
                maxId = fileNameList.get(i).toInt();
            }
            
        }
        
        maxId++;
        
        fileNameList.add(String(maxId));
        fileTitleList.add(title);
        saveToFile();
        return String(maxId);
    }else{
        return fileName;
    }
}

void FileManager::loadFromFile(){
    fileNameList.loadFromFile("/FileName.db");
    fileTitleList.loadFromFile("/FileTitle.db");
}

void FileManager::saveToFile(){
    fileNameList.saveInFile("/FileName.db");
    fileTitleList.saveInFile("/FileTitle.db");
}


char *FileManager::openFile(String path) {
    // path.replace('.',dotReplacedChar);
    // Launch SPIFFS file system  

    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }

    File file = SPIFFS.open(path,"r"); 
    if(!file){ 
        Serial.println("Failed to open file for reading"); 
        return "no_file"; 
    }
    char text[file.size()+10];
    int index = 0;
    while (file.available())
    {
        text[index++] = char(file.read());
    }
    text[index] = '\0';
    file.close();
    
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
        Serial.println("Failed to open file for writing"); 
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
    List files = List();
    
    Dir dir = SPIFFS.openDir(path);
    while (dir.next()) {    
        String fileName = dir.fileName();
        Serial.printf("in getFolderFiles, fileName : %s\n",fileName.c_str());
        files.add(fileName);
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
  bool res = true;
  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;

    HTTPClient http;
    http.setTimeout(10000);
    if (http.begin(client, url.c_str())) {  // HTTP
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          SPIFFS.begin();
          Serial.printf("before open file :%s\n", filename.c_str());
        //   String _fileName = getNewFileName(filename);
        //   Serial.printf("_fileName : %s \n",_fileName.c_str());
          File file = SPIFFS.open(filename,"w");
          Serial.printf("file opened. going to write %d byte.\n",http.getSize());
        if (file) {
            http.begin(url);
            int httpCode = http.GET();
            if (httpCode > 0) {
                if (httpCode == HTTP_CODE_OK) {
                    http.writeToStream(&file);
                }
            } else {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }
            // file.close();
            }
            http.end();        
        //   res = http.writeToStream(&file.w);
        //   Serial.printf("result of writing %s with name %s into spiffs : %d; size : %d\n",filename.c_str(),file.fullName() ,res,http.getSize());
        //   file.close();
        //   SPIFFS.end();
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



bool FileManager::downloadFile(String url, String fileName)
{
   
 SPIFFS.begin();
  HTTPClient http;

  Serial.println("[HTTP] begin...\n");
  Serial.println(fileName);
  Serial.println(url);
  http.begin(url);
 
  Serial.printf("[HTTP] GET...\n", url.c_str());
  // start connection and send HTTP header
  int httpCode = http.GET();
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      Serial.printf("[FILE] open file for writing %s whith size : %d\n", fileName.c_str(),http.getSize());
     
      File file = SPIFFS.open(fileName, "w");

      // file found at server
      if(httpCode == HTTP_CODE_OK) {

          // get lenght of document (is -1 when Server sends no Content-Length header)
          int len = http.getSize();

          // create buffer for read
          uint8_t buff[1280] = { 0 };

          // get tcp stream
          WiFiClient * stream = http.getStreamPtr();

          // read all data from server
          while(http.connected() && (len > 0 || len == -1)) {
              // get available data size
              size_t size = stream->available();
              if(size) {
                  // read up to 128 byte
                  int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                  // write it to Serial
                  //Serial.write(buff, c);
                  file.write(buff, c);
                  if(len > 0) {
                      len -= c;
                  }
              }
              delay(1);
          }

          Serial.println();
          Serial.println("[HTTP] connection closed or file end.\n");
          Serial.println("[FILE] closing file\n");
          file.close();
         
      }
     
  }
  http.end();

 

}

