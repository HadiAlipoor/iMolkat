/*
  Timer.h - Timer library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef FileManager_h
#define FileManager_h

#include "WString.h"
#include <vfs.h>
#include <SPI.h>
#include <FS.h>
#include "List.h"

class FileManager{
    public:
        FileManager();
        String openFile(char *path);
        bool writeFile(char *path, String text);
        List getFolderFiles(char *path);
        List getFolderAllFiles(char *path);
        bool format();
        List getFolders(char *path);
    private:

};
#endif