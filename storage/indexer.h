#pragma once
#include "entry.h"

namespace lightdb{

// Indexer保存了每个key的索引信息，在哪个文件中（用文件Id进行区分），在文件中的偏移位置，所有key的Indexer都会被存储在跳表中
typedef struct Indexer{
    Meta* meta;
    uint32_t fileId;
    int64_t offset;

    Indexer(): fileId(0), offset(0){
        meta = new Meta();
    }

    Indexer(Meta* meta, uint32_t fileId, int64_t offset):fileId(fileId), offset(offset){
        if(meta != nullptr){
            this->meta = new Meta();
            *this->meta = *meta;
        }
    }

    Indexer& operator=(const Indexer& indexer){
        delete meta;
        if(indexer.meta!= nullptr){
            this->meta = new Meta();
            *this->meta = *indexer.meta;
        }else{
            this->meta = nullptr;
        }
        this->fileId = indexer.fileId;
        this->offset = indexer.offset;
    }

    ~Indexer(){
        if(meta!= nullptr) {
            delete meta;
            meta = nullptr;
        }
    }

    //copy constructor
    Indexer(Indexer const &idx){
        meta = new Meta();
        meta->key = idx.meta->key;
        meta->value = idx.meta->value;
        meta->extra = idx.meta->extra;
        this->offset = idx.offset;
        this->fileId = idx.fileId;
    }

} Indexer;

}// namespace lightdb
