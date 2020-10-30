/*
  Timer.h - Timer library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef List_h
#define List_h

#include "WString.h"
#include <LuaWrapper.h>

// include types & constants of Wiring core API
class List
{
  public:
    
    List();
    void add(String value);

    int size();

    String get(int index);
    int getIntValue(int index);
  private:
    LuaWrapper lua;
    int _size;
};

#endif