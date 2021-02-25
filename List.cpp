/*
  Timer.h - Timer library for Wiring - implementation
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// include core Wiring API

// include this library's description file
#include "List.h"



// Constructor /////////////////////////////////////////////////////////////////
// Function that handles the creation and setup of instances



List::List()
{
    _size = 0;
    listString = "";
    seprator = '$';
}

void List::add(String value)
{
    _size++;
    listString += String(value + seprator);
}

int List::size()
{
    return _size;
}
char *List::getCharArray(int index){
    String value = get(index); 
    char res[sizeof(value)];
    value.toCharArray(res,sizeof(value));
    return res;
}
String List::get(int index)
{
    if (index >= _size) 
    {
        return "";
    }
    
    int currentIndex = 0;
    int startIndex = 0;
    for (int i = 1; i < listString.length(); i++)
    {
        if (listString.substring(i,i+1)==String(seprator))
        {
            if (currentIndex++ == index)
            {
                String res = listString.substring(startIndex,i);
                res.trim();
                return res;
            }            
            startIndex = i+1;
        }        
    }    
}

void List::set(String value, int index)
{
    if (index >= _size) 
    {
        return;
    }

    int currentIndex = 0;
    int startIndex = 1;
    String tempListString=String(seprator);
    for (int i = 1; i < listString.length(); i++)
    {
        if (listString.substring(i,i+1)==String(seprator))
        {
            if (currentIndex++ == index)
            {
                add(value);
            }else{
                add(listString.substring(startIndex,i));
            }
        }
    }
}

String List::open_file(char *path) {
    // Launch SPIFFS file system  
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }
    Serial.printf("trying to open db file : %s\n",path);
    File file = SPIFFS.open(path,"r"); 
    if(!file){ 
        Serial.println("Failed to open file for reading"); 
        return "no_file"; 
    }
    return file.readString();
}

bool List::file_write(char *path) {
    // Launch SPIFFS file system  
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }
    Serial.printf("in file_write : %s ; text : %s\n$$END_TEXT\n",path, listString.c_str());
    File file = SPIFFS.open(path,"w"); 
    if(!file){ 
        Serial.println("Failed to open file for reading"); 
        return false; 
    } 
    
    file.print(listString);
    file.close();
    return true;
}

bool List::saveInFile(char *path){
    return file_write(path);
}
bool List::loadFromFile(char *path){
    String fileContent = open_file(path);
    if(fileContent != "no_file"){
        listString = fileContent;
        for (int i = 1; i < listString.length(); i++)
        {
            if (listString.substring(i,i+1)==String(seprator))
            {
                _size++;
            }        
        }   
        return true;
    }else{
        return false;
    }
}