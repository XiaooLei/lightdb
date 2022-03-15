#pragma once
#include "entry.h"
#include "../include/status.h"
#include "../util/str.h"
#include "sys/types.h"
#include <error.h>
#include <string.h>
#include <sys/fcntl.h>
#include <malloc.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>


namespace lightdb{

class DBFile;

typedef std::unordered_map<uint16_t,std::unordered_map<uint32_t,DBFile*>> ArchivedFiles;
typedef std::unordered_map<uint16_t,uint32_t> FileIds;

inline Status PosixError(const std::string& context, int error_number) {
  if (error_number == ENOENT) {
    return Status::NotFound(context, strerror(error_number));
  } else {
    return Status::IOError(context, strerror(error_number));
  }
}

class DBFile{
    public:
    uint32_t Id;
    std::string Path;
    uint64_t Offset = 0;
    uint64_t WriteOffset = 0;
    FileRWMethod method;

    private:
    int _fd;
    std::string _fileName;

    public:
    DBFile(std::string path, uint32_t fileId, FileRWMethod method, uint32_t blockSize, uint16_t eType);

    ~DBFile();

    std::string FileName();

    Status Read(uint64_t offset, Entry& entry);

    Status Read(Entry& entry);


    Status ReadBuf(std::string& result,uint32_t size);

    bool Close();

    void Sync();

    Status Write(Entry* entry);

    void SetOffset(int64_t offset);

    Status FindValidEntries(std::vector<Entry*> entries, bool (ValidEntryFunc)(Entry*, int64_t offset, uint32_t fileId));

    bool IsValidEntry(Entry* e);




};

Status BuildType(std::string path, FileRWMethod method, int64_t blockSize, ArchivedFiles& archivedFiles, FileIds fileIds, DataType type);

Status Build(std::string path, FileRWMethod method, int64_t blockSize, ArchivedFiles& archivedFiles, FileIds& fileIds);

Status buildInternal(std::string path, FileRWMethod method, int64_t blockSize, uint16_t eType, ArchivedFiles& archiveFiles, FileIds& activeFileIds);

std::vector<std::string> readFileList(char *basePath);


}//namespace lightdb