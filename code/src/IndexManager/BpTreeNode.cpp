#include "../MiniSQL.h"
#include "BpTreeNode.h"
#include "../BufferManager/BufferManager.h"


//Read Block
BpTreeNode::BpTreeNode(const char* filename, int value, int KeyLength) :length(KeyLength), _filename(filename), id(value)
{
	BufferManager* buffermanager = MiniSQL::get_BM();
	Block* block = buffermanager->get_block(filename, value);
	char* data = block->data;

	size = *(reinterpret_cast<int*>(data));
	ptr.push_back(*(reinterpret_cast<int*>(data + 4)));
	isleaf = ptr[0] < 0;
	int i = 0;
	for (i; i < size; i++)
	{
		char* k = new char[KeyLength];
		memcpy(k, data + 8 + i * (KeyLength + 4), KeyLength);
		keys.push_back(k);
		ptr.push_back(*(reinterpret_cast<int*>(data + 8 + KeyLength + i * (KeyLength + 4))));
	}
}

//Update Block
BpTreeNode::~BpTreeNode()
{
	BufferManager* buffermanager = MiniSQL::get_BM();
	Block* block = buffermanager->get_block(_filename.c_str(), id);
	char* data = block->data;

	if(size == -1)
		memcpy(data, "ptr", 4);
	else 
		memcpy(data, &size, 4);
	memcpy(data + 4, &ptr[0], 4);

	int i = 0;
	for (i; i < size; i++)
	{
		memcpy(data + 8 + i * (length + 4), keys[i], length);
		memcpy(data + 8 + length + i * (length + 4), &ptr[i + 1], 4);
		delete keys[i];
	}

	block->dirty = true;
}

//Find pointer
int BpTreeNode::Find(const char* key)
{
	int i = 0;
	if (isleaf)
	{
		for (i; i < size; i++)
		{
			if (strncmp(key, keys[i], length) == 0)return ptr[i + 1];
		}
		return -1;
	}
	else
	{
		for (i; i < size; i++)
		{
			if (strncmp(key, keys[i], length) > 0)return ptr[i];
		}
		return ptr[i];
	}
}

//Insert key
int BpTreeNode::Insert(const char* key, const int order, const int value, const int type)
{
	int i = 0;
	int flag = size - 1;
	if (isleaf)
	{
		for (i; i < size; i++)
		{
			if (strncmp(key, keys[i], length) > 0 && size < order - 1)
			{
				char* k = new char[length];
				memcpy(k, key, length);
				keys.push_back(keys[size - 1]);
				ptr.push_back(ptr[size]);
				for (flag; flag > i; flag--)
				{
					keys[flag] = keys[flag - 1];
					ptr[flag + 1] = ptr[flag];
				}
				keys[i] = k;
				ptr[i + 1] = value;
				size++;
				if (type > 0)isleaf = false;
				return -1;
			}
			else if (strncmp(key, keys[i], length) > 0 && size == order - 1) //Need to create new node
			{
				char* k = new char[length];
				memcpy(k, key, length);
				keys.push_back(keys[size - 1]);
				ptr.push_back(ptr[size]);
				for (flag; flag > i; flag--)
				{
					keys[flag] = keys[flag - 1];
					ptr[flag + 1] = ptr[flag];
				}
				keys[i] = k;
				ptr[i + 1] = value;
				size++;
				if (type > 0)isleaf = false;
				return -2;
			}
			else if (strncmp(key, keys[i], length) == 0)return -3;
		}
		if (size < order - 1)
		{
			char* k = new char[length];
			memcpy(k, key, length);
			keys.push_back(k);
			ptr.push_back(value);
			size++;
			if (type > 0)isleaf = false;
			return -1;
		}
		else if (size == order - 1)          //Need to create new node
		{
			char* k = new char[length];
			memcpy(k, key, length);
			keys.push_back(k);
			ptr.push_back(value);
			size++;
			if (type > 0)isleaf = false;
			return -2;
		}
	}
	else
	{
		if (type > 0 && type == id)
		{
			isleaf = true;
			return Insert(key, order, value, type);
		}
		for (i; i < size; i++)
		{
			if (strncmp(key, keys[i], length) > 0)return ptr[i];
		}
		return ptr[i];
	}
}

// Remove key
int BpTreeNode::Remove(const char* key, const int order, int num, const int type)
{
	int i = 0;

	if (isleaf)
	{
		for (i; i < size; i++)
		{
			if (strncmp(key, keys[i], length) == 0)
			{   
				delete keys[i];                  
				for (i; i < size - 1; i++)
				{
					keys[i] = keys[i + 1];
					ptr[i + 1] = ptr[i + 2];
				}
				keys.pop_back();
				ptr.pop_back();
				size--;
				if (type > 0)isleaf = false;
				if (size < (order - 1) / 2)
				{
					return -2;
				}
				return -1;
			}
		}
		return -3;
	}
	else
	{
		if (type > 0 && type == id)
		{
			isleaf = true;
			return Remove(key, order, num, type);
		}
		for (i; i < size; i++)
		{
			if (strncmp(key, keys[i], length) > 0)
			{
				num = i;
				return ptr[i];
			}
		}
		num = i;
		return ptr[i];
	}
}

//Split into two nodes. Return new node
BpTreeNode* BpTreeNode::Split(const int value)
{
	int new_size;
	int flag = 0;
	vector<int> Ptr;
	vector<char*>Keys;

	new_size = size - size / 2;

	for (flag; flag < new_size; flag++)
	{
		Keys.push_back(keys[size - flag - 1]);
		keys.pop_back();
		Ptr.push_back(ptr[size - flag]);
		ptr.pop_back();
	}
	if(isleaf)Ptr.push_back(-1);
	else Ptr.push_back(ptr[size - flag]);
	BufferManager* buffermanager = MiniSQL::get_BM();
	Block* block = buffermanager->get_block(_filename.c_str(), value);
	char* data = block->data;

	memcpy(data, &new_size, 4);
	memcpy(data + 4, &Ptr[new_size], 4);

	for (flag; flag > 0; flag--)
	{
		memcpy(data + 8 + (new_size - flag) * (4 + length), Keys[flag - 1], length);
		memcpy(data + 8 + length + (new_size - flag) * (4 + length), &Ptr[flag - 1], 4);
	}

	block->dirty = true;

	BpTreeNode* new_node = new BpTreeNode(_filename.c_str(), value, length);
	size = size - new_size;
	return new_node;
}

//Merge two nodes
int BpTreeNode::Add(BpTreeNode* a, const int order)
{
	int new_size;
	int i = 0;

	new_size = a->GetSize();
	for (i; i < new_size; i++)
	{
		char* k = new char[length];
		memcpy(k, a->GetKey(i), length);
		keys.push_back(k);
		ptr.push_back(a->GetPtr(i + 1));
	}
	size += new_size;
	if (size > order - 1)return -2;
	return -1;

}

//Drop node
bool BpTreeNode::Drop(const int FirstEmpty)
{
	int i = 0;
	for (i; i < size; i++)
	{
		delete keys[i];
	}
	size = -1;
	ptr[0] = FirstEmpty;
	delete this;
}