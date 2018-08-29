#ifndef _INDEXMANAGER_H
#define _INDEXMANAGER_H

#include "../Global.h"

class IndexManager
{
public:
	//Create BpTree
	bool CreateIndex(const char* filename, int KeyLength);

	//Drop BpTree
	bool DropIndex(const char* filename);

	//Find pointer by key
	int Find(const char* filename, const char* key);

	//Insert key
	bool Insert(const char* filename, const char* key, const int value);

	//Remove key
	bool Remove(const char* filename, const char* key);
};
#endif
