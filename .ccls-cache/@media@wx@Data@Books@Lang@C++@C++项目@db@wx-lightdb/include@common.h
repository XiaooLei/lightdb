
#pragma once
#include <unordered_map>

namespace lightdb{

const int EntryHeaderSize = 34;

const std::string ExtraSeparator = "\\0";

enum FileRWMethod {FileIO, MMap};

// KeyValueMemMode模式下，key和value都在内存中，因为没有磁盘寻道，所以读操作只是在对应的内存的数据结构中获取数据，在value非常小的情况下适合。
// KeyOnlyMemMode模式下，只有key在内存中，value在磁盘中
// 选择某一模式后，这个数据库的所有k-v对都会使用这个模式，无法单独作用于某一db文件或者某一entry
enum DataIndexMode {KeyValueMemMode, KeyOnlyMemMode};

const int DataStructureNum = 5;

const std::string mergeDirName = "lightdb_merge";



}//namespace lightdb;
