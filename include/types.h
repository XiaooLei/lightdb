#pragma once
namespace lightdb{
enum  DataType {String, List, Hash, Set, ZSet, All};

enum OperationType {

    //string operation
    StringSet,
    StringRem,
	StringExpire,
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