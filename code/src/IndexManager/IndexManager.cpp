#include "IndexManager.h"
#include "../BufferManager/BufferManager.h"															//���崦��������ʵ��
#include "BpTree.h"

//Create Index file
bool IndexManager :: CreateIndex(const char* filename, int KeyLength)
{
	FILE* file = fopen(("data/index/" + string(filename) + ".mdb").c_str(), "wb");
	if(file == NULL)
	{
		cerr << "Error:[IndexManager]Fail to create " + string(filename) + " index." << endl;
		return false;
	}
	int order = (MAX_BLOCK_SIZE - 8) / (KeyLength + 4) + 1;
	int HeadFile[] = {order, KeyLength, -1, 0, 1};

	fwrite(HeadFile, 4, 5, file);
	fclose(file);
	return true;
}

//Drop Index file
bool IndexManager:: DropIndex(const char* filename)
{

	if ((remove(("data/index/" + string(filename) + ".mdb").c_str())) == 0)
	{
		return true;
	}
	else
	{
		cerr << "Error:[IndexManager]Fail to drop " + string(filename) + " index." << endl;
		return false;
	}
}

//Find pointer to key in record file
int IndexManager::Find(const char* filename, const char* key)
{
	BpTree* tree = new BpTree(("index/" + string(filename)).c_str());
	if (tree->GetKeyLength() == 0)
	{
		cerr << "Error:[IndexManager]Can't find " + string(filename) + " index." << endl;
		return -2;
	}
	int res =  tree -> Find(key);
	delete tree;
	if (res == -1)
	{
		cerr << "Error:[IndexManager]Can't find this key." << endl;
		return -1;
	}
	return res;
}

//Insert Key
bool IndexManager::Insert(const char* filename, const char* key, const int value)
{
	bool flag;
	BpTree* tree = new BpTree(("index/" + string(filename)).c_str());
	if (tree->GetKeyLength() == 0)
	{
		cerr << "Error:[IndexManager]Can't find " + string(filename) + " index." << endl;
		return false;
	}
	flag = tree->Insert(key,value);
	delete tree;
	return flag;
}

//Remove Key
bool IndexManager::Remove(const char* filename, const char* key)
{
	BpTree* tree = new BpTree(("index/" + string(filename)).c_str());
	if (tree->GetKeyLength() == 0)
	{
		cerr << "Error:[IndexManager]Can't find " + string(filename) + " index." << endl;
		return false;
	}
	tree->Remove(key);
	delete tree;
	return true;
}