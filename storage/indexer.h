#pragma once
#include "entry.h"

namespace lightdb{

typedef struct Indexer{
    Meta* meta;
    uint32_t fileId;
    int64_t offset;

    Indexer():fileId(0),offset(0),meta(nullptr){}

    Indexer(Meta* meta, uint32_t fileId, int64_t offset):meta(meta), fileId(fileId), offset(offset){}

    //copy constructor
    Indexer(Indexer const &idx){
        if(idx.meta!= nullptr) {
            this->meta = new Meta();
            meta->key = idx.meta->key;
            meta->value = idx.meta->value;
            meta->extra = idx.meta->extra;
        }
        this->offset = idx.offset;
        this->fileId = idx.fileId;
    }

} Indexer;


}// namespace lightdb
