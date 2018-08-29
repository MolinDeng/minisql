#pragma once

#include <algorithm>
#include <list>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <chrono>
#include "Interpreter/LEXYACC/yacc.hpp"

using namespace std;
using namespace chrono;

//allow user input 500 characters once in a line
#define MAX_ONE_LINE            256
// Bytes of a block
#define MAX_BLOCK_SIZE          4096
// Max number of block
#define MAX_BLOCK_COUNT         100

// prompt
#define WELCOME     "========Welcome to minisql!========"
#define PROMPT      "\nminisql> "
#define ENTER_SIGN  "      -> "
#define BYE_BYE     "Bye~ ^_^"


//	用于描述表中一个属性在表中的具体信息
struct Attribute
{
    string attr_name;
    int attr_type;	//属性的数据类型，分别为CHAR, FLOAT, INT
    int attr_key_type;//属性的主键类型，分别为PRIMARY, UNIQUE, NULL(EMPTY)
    int attr_len; 	//属性所存放的数据的长度，如果不是CHAR，则长度标记为1
    int attr_id;    //属性在表中是第几个
};

//	用于描述表的信息
struct Table
{
    string table_name;  //表名
    string pk; //主键名
    int attr_count;				//表中属性的总个数
    Attribute attrs[32];	//表的所有属性列表, 最多32个属性
    //return primary key id
    int getPrimaryKeyId() {
        for (int i = 0; i < attr_count; ++i)
        {
            if (attrs[i].attr_key_type == PRIMARY)
            {
                return i;
            }
        }
        //if no primary key
        return -1;
    }

    int searchAttrId(string att_name) {
        for (int i = 0; i < attr_count; ++i)
        {
            if (attrs[i].attr_name == att_name)
            {
                return i;
            }
        }
        //if no primary key
        return -1;
    }

    int length()
    {
        int len = 0;
        for (int i = 0; i < attr_count; ++i)
        {
            len += attrs[i].attr_len;
        }
        return len;
    }
};

//	用于描述判断条件的信息
struct Condition
{
    string attr_name;	//条件所对应的属性名
    int attr_type;      //属性名对用的类型
    string op_type;		//条件所用到的比较模式，分别为"<>", "=", ">=", "<=", "<", ">"
    string cmp_value;	//条件所需要进行比较的值
};
typedef list<Condition> Condition_list;

//	用于描述索引信息
struct Index
{
    string index_name;
    string table_name;
    string attr_name;	//	索引所对应的属性
};

// 用于存储一系列索引名
struct IndexName
{
    string name[100];
    int len;
};

//insert into card values("", "", 1234.4);
struct Record
{
    string table_name;  //表名
    vector<string> attr_values; // 要插入的值
    int num_values; //插入的字段的个数，应该与存在表的字段数相同
};
struct Tuple: public Table
{
    string attr_values[32];    
};

// data block
struct Block {
    string file_name;
    int id;

    bool dirty;
    bool pin;

    char data[MAX_BLOCK_SIZE];

    // C'tor
    Block(const char* _file_name, int _id): file_name(_file_name), id(_id) {
        dirty = pin = false;
    }
};