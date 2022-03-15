


#include "str.h"
#include <iostream>
using namespace lightdb;
int main(){
    auto splitNames = splitWithStl("0000001.data.str",".");
    for(auto it = splitNames.begin(); it!=splitNames.end(); it++){
        std::cout<<*it<<std::endl;
    }

    int value = stringToInt("000324");
    std::cout<<value<<std::endl;
    return 0;
}