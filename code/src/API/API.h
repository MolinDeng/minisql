#pragma once

#include "../GLobal.h"

using namespace std;

bool DEBUG_Create_Table(Table& table);

bool DEBUG_Drop_Table(string table_name);

bool DEBUG_Create_Index(Index& index);

bool DEBUG_Drop_Index(string index_name);

bool DEBUG_Insert(Record& record);

int DEBUG_Select(string table_name, Condition_list clist, bool if_where = false);

int DEBUG_Delete(string table_name, Condition_list clist, bool if_where = false);

//	创建表时的内部调用
bool API_Create_Table(Table& table);

//	删除表时的内部调用
bool API_Drop_Table(string table_name);

//	创建索引时的内部调用
bool API_Create_Index(Index& index);

//	删除索引时的内部调用
bool API_Drop_Index(string index_name);

//	插入纪录时的内部调用
bool API_Insert(Record& record);

//	选择纪录时的内部调用
int API_Select(string table_name, Condition_list clist, bool if_where = false);

//	删除纪录时的内部调用
int API_Delete(string table_name, Condition_list clist, bool if_where = false);

