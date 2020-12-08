/*
  Timer.h - Timer library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef LuaManager_h
#define LuaManager_h

#include "WString.h"
#include <vfs.h>
#include <SPI.h>
#include <FS.h>

// include types & constants of Wiring core API
class LuaManager
{
    public:
        LuaManager();
        String getLuaCode();
    private:
        String file_open(char *path);
};

#endif