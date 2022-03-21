

#include "time.h"
#include <unistd.h>
#include <iostream>

int main(){
    for(int i = 0; i < 10; i++){
        printf("time:%lu \n", lightdb::getCurrentTimeStamp());
        sleep(1); 
    }
}