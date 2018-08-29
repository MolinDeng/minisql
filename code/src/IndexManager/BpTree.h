#ifndef _BPTREE_H
#define _BPTREE_H

#include "../Global.h"

using namespace std;

class BpTree
{
public:
	BpTree(const char* filename);
	~BpTree();

	//Insert key
	bool Insert(const char* key, const int value, const int type = -1);

	//Remove key
	bool Remove(const char* key, const int type = -1);

	//Find pointer
	int Find(const char* key);

	//Get first empty blcok's id
	int GetFirstEmpty();
	
	//Get KeyLength
	int GetKeyLength() { return KeyLength; }
private:
	//Order of BpTree
	int order;

	//Block id of root
	int root;

	//Length of key
	int KeyLength;

	//Count of node
	int NodeCount;

	//First empty block
	int FirstEmpty;

	//Filename
	string  _filename;
};
#endif