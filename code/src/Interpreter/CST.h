#pragma once
#include "../Global.h"
#include "../API/API.h"
#include <vector>

//SQL语法树
struct CST {
	int type;
	char text[256];
	CST *lpNext; // 指向同一层的节点
	CST *lpSub;  // 指向子节点
};

//以下为语法树外部函数

CST* MallocNewNode();

void ExecTree(CST* pNode);

CST* FatherAddSon(CST* pFather, CST* pSon);

//add num_of_sons sons to a new father node and return father
CST* NewFatherAddSon(int iType, int num_of_sons, ...);

//free the tree
void FreeTree(CST* pNode);


//节点管理器
typedef std::vector< CST* > NodeManager;


//clear
void nm_clear();


//以下为不同语句处理的函数
void _CreateTable(CST* current_node);

void _CreateIndex(CST* current_node);

void _DropTable(CST* current_node);

void _DropIndex(CST* current_node);

void _Insert(CST* current_node);

void _Delete(CST* current_node);

void _Select(CST* current_node);

void _Execfile(CST* current_node);






