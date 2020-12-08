#include "LuaManager.h"

LuaManager::LuaManager(){

}

String LuaManager::getLuaCode(){
    return "HIGH = 1;\n"
"LOW = 0;\n"
"OUTPUT = 1;\n"
"INPUT = 0;\n"
"Lamp1Status = false;\n"
"Lamp1BlinkingStatus = false;\n"
"\n"
"Lamp1PiN = 14;\n"
"\n"
"function Setup()\n"
"    pinMode(Lamp1PiN, OUTPUT)\n"
"end\n"
"\n"
"function Loop()\n"
"	\n"
"    if Lamp1BlinkingStatus then\n"
"        digitalWrite(Lamp1PiN, HIGH);\n"
"        delay(500);\n"
"        digitalWrite(Lamp1PiN, LOW);\n"
"        delay(500);\n"
"    end\n"
"end\n"
"\n"
"--#api\n"
"function Lamp1()\n"
"    if Lamp1Status then\n"
"        Lamp1Status = false\n"
"        digitalWrite(Lamp1PiN, LOW);\n"
"    else\n"
"        Lamp1Status = true\n"
"        digitalWrite(Lamp1PiN, HIGH);\n"
"    end\n"
"    responseHtml(Lamp1Status)\n"
"end\n"
"\n"
"--#api\n"
"function Lamp1blink()\n"
"    if Lamp1BlinkingStatus then\n"
"        Lamp1BlinkingStatus = false;\n"
"    else\n"
"        Lamp1BlinkingStatus = true;\n"
"    end\n"
"    responseHtml('status = ' + Lamp1BlinkingStatus)\n"
"end";
}

String LuaManager::file_open(char *path) {
    // Launch SPIFFS file system  
    if(!SPIFFS.begin()){ 
        Serial.println("An Error has occurred while mounting SPIFFS");  
    }

    File file = SPIFFS.open(path,"r"); 
    if(!file){ 
        Serial.println("Failed to open file for reading"); 
        return "no_file"; 
    }
    return file.readString();
}