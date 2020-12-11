/*
  Timer.h - Timer library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef Api_h
#define Api_h

#include "WString.h"
#include "List.h"

// include types & constants of Wiring core API
class Api
{
  
  public:
    Api();
    void setName(String name);
    String getName();
    void addInput(String input);
    int getSize();
    String getInput(int index);

  private:
    String name;
    String inputs;
    int inputIndex;
};
#endif