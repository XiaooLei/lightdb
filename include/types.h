#pragma once
namespace lightdb{
// 如果枚举变量未分配初始值，那么在默认情况下，第一个枚举成员的默认值为“0”，后续枚举成员的值为前一个枚举成员的值加一
enum  DataType {String, List, Hash, Set, ZSet, All};

enum OperationType {

    //string operation
    StringSet,
    StringRem,
	StringExpire,// 将key加入到数据库的expire表中
	StringPersist,

    //list operation
	ListLPush,
	ListRPush,
	ListLPop,
	ListRPop,
	ListLRem,
	ListLInsert,
	ListLSet,
	ListLTrim,
	ListLClear,
	ListLExpire,

	//hash operation
    HashHSet,
	HashHDel,
	HashHClear,
	HashHExpire,

	//set operation
	SetSAdd,
	SetSRem,
	SetSMove,
	SetSClear,
	SetSExpire,

	//zset operation
	ZSetZAdd,
	ZSetZRem,
	ZSetZClear,
	ZSetZExpire,
        

};

}
