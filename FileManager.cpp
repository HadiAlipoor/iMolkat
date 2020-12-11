#include "FileManager.h"

FileManager::FileManager(){

}

String FileManager::openFile(char *path) {
    // Launch SPIFFS file system  
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }

    File file = SPIFFS.open(path,"r"); 
    if(!file){ 
        Serial.println("Failed to open file for reading"); 
        return "no_file"; 
    }
    // Serial.println(file.readString());
    return file.readString();
}

bool FileManager::writeFile(char *path, String text) {
    // Launch SPIFFS file system  
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }

    File file = SPIFFS.open(path,"w"); 
    if(!file){ 
        Serial.println("Failed to open file for reading"); 
        return false; 
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
        files.add(fileName);
    }
    SPIFFS.end();
    return files;
}

List FileManager::getFolderAllFiles(char *path){
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }

    List files = List();
    Dir dir = SPIFFS.openDir(path);
    while (dir.next()) {
        String fileName = dir.fileName();
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

List FileManager::getFolders(char *path){
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }

    List folders = List();
    Dir dir = SPIFFS.openDir(path);
    while (dir.next()) {    
        if (dir.isDirectory())
        {
            folders.add("dir.fileName()");
        }
    }
    SPIFFS.end();
    return folders;
}