#include "Apis.h"

Apis::Apis(){
    Apis::api_count = 0;
}
void Apis::setName(String name){
    Apis::nameList.add(name);
    Apis::inputsList.add("0");
    Apis::api_count++;
}
String Apis::getName(int index){
    return Apis::nameList.get(index);
}
void Apis::addInput(String input, int index){
    String apiInput = ",";
    if(!Apis::inputsList.get(index).isEmpty())
    {
        apiInput = Apis::inputsList.get(index);
    }
    input = apiInput + input + ",";
    Apis::inputsList.set(input, index);
    Apis::inputIndex.set(String(Apis::inputIndex.get(index).toInt() +1),index);
}
int Apis::getInputSize(int index){
    return Apis::inputIndex.get(index).toInt();
}

int Apis::getApiSize(){
    return Apis::api_count;
}
String Apis::getInput(int index, int apiIndex)
{
    int indexCount = 0;
    int start_index = 0;
    String inputs = Apis::inputsList.get(index);
    for (int i = 0; i < inputs.length()-1; i++)
    {
        if (inputs.substring(i,i+1) == ",")
        {
            if (start_index == 0)
            {
            start_index = i + 1;
            }
            else
            {
            String input = inputs.substring(start_index, i);
            if (indexCount == index)
            { 
                return input;
            }
            indexCount++;
            }
        }
    }
}