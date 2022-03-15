
#pragma once
#include <unordered_map>

namespace lightdb{

const int EntryHeaderSize = 34;

const std::string ExtraSeparator = "\\0";

enum FileRWMethod {FileIO, MMap};

enum DataIndexMode {KeyValueMemMode, KeyOnlyMemMode};

const int DataStructureNum = 5;

const std::string mergeDirName = "lightdb_merge";



}//namespace lightdb;