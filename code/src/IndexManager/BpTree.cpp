#include "../BufferManager/BufferManager.h"
#include "../MiniSQL.h"
#include "BpTree.h"
#include "BpTreeNode.h"


//Read HeadFile
BpTree::BpTree(const char* filename):_filename(filename)
{
	BufferManager* buffermanager = MiniSQL::get_BM();
	Block* header = buffermanager->get_block(filename, 0);
	char* data = header->data;

	order = *(reinterpret_cast<int*>(data));
	KeyLength = *(reinterpret_cast<int*>(data + 4));
	root = *(reinterpret_cast<int*>(data + 8));
	NodeCount = *(reinterpret_cast<int*>(data + 12));
	FirstEmpty = *(reinterpret_cast<int*>(data + 16));
}

//Update block
BpTree::~BpTree()
{
	BufferManager* buffermanager = MiniSQL::get_BM();
	Block* block = buffermanager->get_block(_filename.c_str(), 0);
	char* data = block->data;

	memcpy(data + 8, &root, 4);
	memcpy(data + 12, &NodeCount, 4);
	memcpy(data + 16, &FirstEmpty, 4);

	block->dirty = true;
}

//Find pointer
int BpTree::Find(const char* key)
{
	int res = root;
	bool isleaf;
	do
	{
		BpTreeNode* node = new BpTreeNode(_filename.c_str(), res, KeyLength);
		res = node->Find(key);
		isleaf = node->Getisleaf();
		delete node;
	} while (!isleaf);
	return res;
}

//Insert key
bool BpTree::Insert(const char* key, const int value, const int type)
{
	int res;
	bool isleaf;
	BpTreeNode* sp;
	BpTreeNode* node;

	if (root == -1)
	{
		BufferManager* buffermanager = MiniSQL::get_BM();
		Block* block = buffermanager->get_block(_filename.c_str(), FirstEmpty);
		char* data = block->data;

		int size = 0;
		int ptr = -1;
		memcpy(data, &size, 4);
		memcpy(data + 4, &ptr, 4);
		block->dirty = true;

		node = new BpTreeNode(_filename.c_str(), FirstEmpty, KeyLength);
		NodeCount++;
		root = GetFirstEmpty();
		delete node;

		return Insert(key, value);
	}

	res = root;
	vector<int> Value;
	int ValueLength = 0;
	do
	{
		Value.push_back(res);
		ValueLength++;
		node = new BpTreeNode(_filename.c_str(), res, KeyLength);
		res = node->Insert(key,order,value,type);
		if (res == -2)
		{
			sp = node;
			node = NULL;
		}
		delete node;
	} while (res > 0);
	if (res == -3)
	{
		cerr << "Error:[IndexManager]" + string(key) + " already existed." << endl;
		return false;
	}
	else if (res == -2)//Split into two nodes. Return new node
	{
		int ret = GetFirstEmpty();
		if (ValueLength == 1)
		{
			node = sp->Split(ret);
			ret = GetFirstEmpty();
			BufferManager* buffermanager = MiniSQL::get_BM();
			Block* block = buffermanager->get_block(_filename.c_str(), ret);
			char* data = block->data;

			int size = 1;
			int pointer_0 = sp->GetId();
			int pointer_1 = node->GetId();
			char* Key = new char[KeyLength];
			memcpy(Key, node->GetKey(), KeyLength);
			memcpy(data, &size, 4);
			memcpy(data + 4, &pointer_0, 4);
			memcpy(data + 8, Key, KeyLength);
			memcpy(data + 8 + KeyLength, &pointer_1, 4);

			block->dirty = true;

			BpTreeNode* new_node = new BpTreeNode(_filename.c_str(), ret, KeyLength);
			delete Key;
			delete new_node;
			delete node;
			delete sp;
			NodeCount = NodeCount + 2;
			root = ret;
			return true;
		}
		else if(ValueLength >= 2)
		{
			bool flag;
			node = sp->Split(ret);
			char* Key = new char[KeyLength];
			memcpy(Key, node->GetKey(), KeyLength);
			int pointer = node->GetId();
			flag = Insert(Key, pointer, Value[ValueLength - 2]);
			delete node;
			delete sp;
			delete Key;
			NodeCount++;
			return flag;
		}
	}
	else if (res == -1)
		return true;
}

//Remove key
bool BpTree::Remove(const char* key, const int type)
{
	BpTreeNode* node;
	BpTreeNode* add;
	int res = root;
	vector<int>Value;
	int ValueLength = 0;
	int num = -1;

	do
	{
		Value.push_back(res);
		ValueLength++;
		node = new BpTreeNode(_filename.c_str(), res, KeyLength);
		res = node->Remove(key, order, num, type);
		if (res == -2)
		{	
			if(node->GetId() != root)
			{
				add = node;
				node = NULL;
			}
			else res = -1;
		}
		delete node;
	} while (res > 0);
	if (res == -3)
	{
		cerr << "Error:[IndexManager]Can't find " + string(key) + "." << endl;
		return false;
	}
	else if (res == -2)
	{
		if (ValueLength == 1)return true;
		else if (ValueLength >= 2)
		{
			int flag;
			//father node
			node = new BpTreeNode(_filename.c_str(), Value[ValueLength - 2], KeyLength);
			if (num == 0)
			{
				num++;
				flag = 1;
			}
			else
			{
				num--;
				flag = -1;
			}
			//relative node
			BpTreeNode* new_node = new BpTreeNode(_filename.c_str(), node->GetPtr(num), KeyLength);
			if (flag == 1)
			{
				Remove(new_node->GetKey(), node->GetId());
				int temp;
				int condition;
				condition = add->Add(new_node, order);
				temp = FirstEmpty;
				FirstEmpty = new_node->GetId();
				new_node->Drop(temp);
				if (condition == -1)
				{
					delete node;
					delete add;
					NodeCount--;
					return true;
				}
				else if (condition == -2)
				{
					int ret;
					int flag_1;
					BpTreeNode* sp;
					ret = GetFirstEmpty();
					sp = add->Split(ret);
					char* k = new char[KeyLength];
					memcpy(k, sp->GetKey(), KeyLength);
					flag = Insert(k, sp->GetId(), node->GetId());
					delete k;
					delete sp;
					delete node;
					delete add;
					return flag;
				}
			}
			else if (flag == -1)
			{
				Remove(add->GetKey(), node->GetId());
				int temp;
				int condition;
				condition = new_node->Add(add, order);
				temp = FirstEmpty;
				FirstEmpty = add->GetId();
				add->Drop(temp);
				if (condition == -1)
				{
					delete node;
					delete new_node;
					NodeCount--;
					return true;
				}
				else if (condition == -2)
				{
					int ret;
					int flag_1;
					BpTreeNode* sp;
					ret = GetFirstEmpty();
					sp = new_node->Split(ret);
					char* k = new char[KeyLength];
					memcpy(k, sp->GetKey(), KeyLength);
					flag = Insert(k, sp->GetId(), node->GetId());
					delete k;
					delete sp;
					delete node;
					delete new_node;
					return flag;
				}
			}
		}
	}
	else if (res == -1)return true;
}

//Get fist empty block, Return id
int BpTree::GetFirstEmpty()
{
	int flag;
	char ifp[4];
	BufferManager* buffermanager = MiniSQL::get_BM();
	Block* block = buffermanager->get_block(_filename.c_str(), FirstEmpty);
	char* data = block->data;

	flag = FirstEmpty;
	memcpy(ifp, data, 4);
	if(strncmp(ifp, "ptr", 4) == 0)
		FirstEmpty = *(reinterpret_cast<int*>(data + 4));
	else
		FirstEmpty = flag + 1;
	return flag;
}
