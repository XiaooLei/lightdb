#pragma once
#include<cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <unistd.h>

namespace lightdb{

inline int CreateDir(const char *sPathName)
{
    char DirName[256];
    strcpy(DirName, sPathName);
    int i;
    size_t len = strlen(DirName);
    for(i=1; i<len; i++){
        if(DirName[i]=='/'){
            DirName[i] = 0;
            if(access(DirName, 0)!=0){
                if(mkdir(DirName, 0755)==-1){
                    printf("mkdir   error\n");
                    return -1;
                }
            }
            DirName[i] = '/';
          }
      }
      return 0;
}

}
