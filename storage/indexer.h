#pragma once
#include "entry.h"

namespace lightdb{

// Indexer保存了每个key的索引信息，在哪个文件中（用文件Id进行区分），在文件中的偏移位置，所有key的Indexer都会被存储在跳表中
typedef struct Indexer{
    Meta* meta;
    /* int allocated = false; */
    uint32_t fileId;
    int64_t offset;

    /* Indexer():meta(nullptr), fileId(0), offset(0){} */
    Indexer(): fileId(0), offset(0){
        meta = new Meta();
    }

    Indexer(Meta* meta, uint32_t fileId, int64_t offset):meta(meta), fileId(fileId), offset(offset){
        /* if(meta!= nullptr) { */
        /*     allocated = true; */
        /* } */
    }

    ~Indexer(){
        /* if(meta!= nullptr && allocated == true) { */
        if(meta!= nullptr) {
            if(meta->keySize == 762930539){
                printf("stop here!\n");
            }
            delete meta;
            /* this->allocated = false; */
            meta = nullptr;
        }
    }

    //copy constructor
    Indexer(Indexer const &idx){
        /* if(idx.meta!= nullptr) { */
            /* this->meta = new Meta(); */
            /* this->allocated = true; */
        meta = new Meta();
        meta->key = idx.meta->key;
        meta->value = idx.meta->value;
        meta->extra = idx.meta->extra;
        /* }else{ */
            /* this->meta = nullptr; */
        /* } */
        this->offset = idx.offset;
        this->fileId = idx.fileId;
    }

} Indexer;


}// namespace lightdb
