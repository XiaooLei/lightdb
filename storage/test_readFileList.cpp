

#include "db_file.h"
#include <vector>
#include <stdio.h>
#include <cassert>
#include <iostream>
#include "../config/config.h"
using namespace lightdb;
int main(){
    ArchivedFiles archFiles;
    //printf("ddd\n");
    FileIds fileIds;
    Status s = buildInternal("/tmp/lightdb", FileIO, DefaultBlockSize, All, archFiles, fileIds);
    assert(s.ok());
    for(auto it = archFiles.begin(); it!=archFiles.end(); it++){
        for(auto it2 = it->second.begin(); it2!=it->second.end(); it2++){
            cout<<it2->second->FileName()<<endl;
        }
    }
    return 0;
}