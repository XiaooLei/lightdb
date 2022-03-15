
#pragma once
#include <chrono>
namespace lightdb{

inline  uint64_t getCurrentTimeStamp(){
        auto timeNow = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
        return timeNow.count();
    }
}