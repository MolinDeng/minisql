#ifndef _BpTreeNode_H
#define _BpTreeNode_H

#include "../Global.h"

class BpTreeNode
{
public:
	BpTreeNode(const char* filename, int value, int KeyLength);
	~BpTreeNode();

	//Find pointer
	int Find(const char* key);

	//Insert key
	int Insert(const char* key, const int order, const int value, const int type = -1);

	//Remove key
	int Remove(const char* key, const int order, int num = -1, const int type = -1);

	//Get isleaf
	bool Getisleaf() { return isleaf; }

	//Split into two nodes. Return new node
	BpTreeNode* Split(const int value);

	//GetId
	int GetId() { return id; }

	//GetKey
	const char* GetKey(int num = 0) { return keys[num]; }

	//GetPointer
	const int GetPtr(int num) { return ptr[num]; }

	//Merge two nodes
	int Add(BpTreeNode* a, const int order);

	//Get size
	int GetSize() { return size; }

	//Drop node
	bool Drop(const int FirstEmpty);
private:
	//Node pointer
	vector<int> ptr;

	//Node key
	vector<char*> keys;

	//Node size
	int size;

	//Block id in file
	int id;

	//Judge if a leaf node
	bool isleaf;

	//KeyLength
	int length;

	//filename
	string _filename;
};
#endif