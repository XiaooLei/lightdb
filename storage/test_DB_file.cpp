

#include <iostream>
#include "db_file.h"
using namespace lightdb;

int main(){
    DBFile dbFile("/tmp/lightdb", 1, FileIO, 16 * 1024 * 1024, String);
    std::cout<< "file Opened" <<std::endl;
    Entry* entry = Entry::NewEntryNow("keya","valuea","extraa",List, ListLClear);
    Status status = dbFile.Write(entry);
    std::cout<< status.Code() <<std::endl;
    return 0;
}