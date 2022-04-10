
#pragma once
#include <sys/types.h>
#include <string.h>

namespace lightdb{

#define MASK 0x1021
inline uint32_t CRC_Compute(const std::string data)
{
    char buf[data.size()];
    memcpy(buf, data.c_str(), sizeof(buf));
    int len = data.size();
    uint32_t rst = 0x0;
    int i,j;
    for(i=0 ; i < len ; i++)
    {
        rst ^= data[i] << 8;
        for(j=0 ; j < 8 ; j++)
        {
            if(rst & 0x8000)
                rst = (rst << 1) ^ MASK;
            else
                rst <<= 1;
        }
    }
    return rst & 0xFFFF;
}

}