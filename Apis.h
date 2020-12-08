/*
  Timer.h - Timer library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef Apis_h
#define Apis_h

#include "WString.h"
#include "List.h"

// include types & constants of Wiring core API
class Apis
{
  
  public:
    Apis();
    void setName(String name);
    String getName(int index);
    void addInput(String input,int index);
    int getInputSize(int index);
    int getApiSize();
    String getInput(int index, int apiIndex);
  private:
    List nameList;
    List inputsList;
    List inputIndex;
    int api_count;
};
#endif