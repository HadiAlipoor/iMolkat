#include "Api.h"

Api::Api(){
    Api::inputIndex = 0;
    Api::inputs = ",";
}
void Api::setName(String name){
    Api::name = name;
}
String Api::getName(){
    return Api::name;
}
void Api::addInput(String input){
    Api::inputs = Api::inputs + input + ",";
    Api::inputIndex++;
}
int Api::getSize(){
    return Api::inputIndex;
}
String Api::getInput(int index)
{
    int indexCount = 0;
    int start_index = 0;
    for (int i = 0; i < Api::inputs.length()-1; i++)
    {
    if (Api::inputs.substring(i,i+1) == ",")
    {
        if (start_index == 0)
        {
        start_index = i + 1;
        }
        else
        {
        String input = Api::inputs.substring(start_index, i);
        if (indexCount == index)
        { 
            return input;
        }
        indexCount++;
        }
    }
    }
}
