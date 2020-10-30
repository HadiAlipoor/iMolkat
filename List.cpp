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
    List::_size = 0;
    String declare_array = String("MyArray = {} function getvalue(index) return MyArray[index] end");
    Serial.println(List::lua.Lua_dostring(&declare_array));      
}

void List::add(String value)
{
    char add_to_array[200];
    List:_size++;
    snprintf(add_to_array,sizeof(add_to_array),"MyArray[%i] = '%s' print(MyArray[%i])", List::_size, value.c_str(), List::_size);
    String add_code = String(add_to_array);
    Serial.println(List::lua.Lua_dostring(&add_code));
}

int List::size()
{
    return List::_size;
}

String List::get(int index)
{
    char get_from_array[200];
    snprintf(get_from_array,sizeof(get_from_array),"tempString = getvalue(%i)",index);
    String get_code = String(get_from_array);
    Serial.println(List::lua.Lua_dostring(&get_code).c_str());
    Serial.println(lua_getglobal(List::lua.get_lua_State(), "tempString"));
    return lua_tostring(List::lua.get_lua_State(), -1);
}

int List::getIntValue(int index)
{

}
