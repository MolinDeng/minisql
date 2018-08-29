#include <cstring>
#include <cstdio> 
#include <iostream>
#include <typeinfo>
#include <fstream>
#include <sstream>

#include "data.h"

#include "../CatalogManager/catalogManager.h"
#include "../BufferManager/BufferManager.h"
#include "../MiniSQL.h"

using namespace std;

template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}
Data::Data(const char* filename) :_filename(filename) {

	//cout << "C'tor::data  " << _filename << endl;

	BufferManager* buffermanager = MiniSQL::get_BM();
	Block* block = buffermanager->get_block(filename, 0);
	int blockCount = *(reinterpret_cast<int*>(block->data));
																										//cout << blockCount << endl;  //	
	int count = *(reinterpret_cast<int*>(block->data + 4));
																										//cout << count << endl;  //
    int recordcount = *(reinterpret_cast<int*>(block->data + 8));
																									    //cout << recordcount << endl;  //
	for(int m=0;m<=recordcount;m++){
		rowsId[m]= *(reinterpret_cast<int*>(block->data + 8+ m*4));

		//cout<<"ROWSID"<<rowsId[m]<<endl;
	}
	block->dirty = false;

	tableName = _filename;
	CatalogManager* manager = MiniSQL::get_CM();
	int attrcount = manager->getattr_count(tableName);
	Table* table = manager->gettable(tableName);
																										//cout << attrcount << endl;  //
	
	int i;
	for (i = 1; i <= blockCount; i++) {
		Block* block = buffermanager->get_block(filename, i);
		char* recordData = block->data;

		int k;
        int bias = 0;

		if (i != blockCount) {  
		
			for (int j = (i - 1)*count + 1; j <= i*count; j++) {	
				if (strcmp(recordData + bias, "EMPTY") != 0) {

					//cout << "not EMPTY" << endl;

                    Tuple tuple;
					for (k = 0; k < attrcount; k++) {
						if (table->attrs[k].attr_type == INT) {
							int a= *(reinterpret_cast<int*>(recordData + bias));
							tuple.attr_values[k] = to_string(a);
						}
						else if (table->attrs[k].attr_type == FLOAT) {
							float a = *(reinterpret_cast<float*>(recordData + bias));
							tuple.attr_values[k] = to_string(a);
						}
						else {
							char ch[table->attrs[k].attr_len+1];
							strncpy(ch , recordData + bias, table->attrs[k].attr_len);
							ch[table->attrs[k].attr_len] = '\0';
							tuple.attr_values[k] = ch;
							//cout << tuple.attr_values[k] << endl;
						}			
					    bias += table->attrs[k].attr_len;
					}
                    rows.push_back(tuple);
				}

			}
		}
		if (i == blockCount) {
			                                               //cout << "LastBlockCount:" << i << endl;
			for (int j = (blockCount - 1) * count+1; j <= recordcount; j++) {
				if (strcmp(recordData + bias, "EMPTY") != 0){

                    Tuple tuple;
					for (k = 0; k < attrcount; k++) {
						//cout << "ATTRBUTE" << k << endl;

						if (table->attrs[k].attr_type == INT) {
							int a = *(reinterpret_cast<int*>(recordData + bias));
							//cout << a << endl;
							tuple.attr_values[k] = to_string(a);
							//cout << stringToNum<int>(tuple.attr_values[k]) << endl;
						}
						else if (table->attrs[k].attr_type == FLOAT) {
							float a = *(reinterpret_cast<float*>(recordData + bias));
							tuple.attr_values[k] = to_string(a);
						}
						else {
							//cout << recordData + bias<< endl;
							char ch[table->attrs[k].attr_len+1];
							strncpy(ch , recordData + bias, table->attrs[k].attr_len);
							ch[table->attrs[k].attr_len] = '\0';
							tuple.attr_values[k] = ch;
							//cout << tuple.attr_values[k] << endl;
						}
						bias += table->attrs[k].attr_len;
					}
                    rows.push_back(tuple);
				}
				else{
					bias += table->length();
				}
			}
		}

		
		//memset(block->data, 0, MAX_BLOCK_SIZE);
		block->dirty = false;	
	}
	//cout << "C'tor END" << endl;
}

Data::~Data() {
	//printf("~data\n");
	int k,i;
	int j = 1;
	int blockcount;
	CatalogManager* manager = MiniSQL::get_CM();
    BufferManager* buffermanager = MiniSQL::get_BM();
	Table* table = manager->gettable(tableName);//
    int attrcount = manager->getattr_count(tableName);
	int count = MAX_BLOCK_SIZE /table->length(); //one block stores count records
							//printf("%d",rowsId[0]);
	
	if (rowsId[0] <= count)
		blockcount = 1;
	else if (rowsId[0] % count == 0)
		blockcount = rowsId[0] / count;
	else
		blockcount = rowsId[0] / count + 1;

	for (i = 1; i <= blockcount; i++) { //needs i blocks, i <= ceil(rowsId[0] / count)   
	   Block* block = buffermanager->get_block(_filename.c_str(), i);
	   int counts = 0;//记录一个block存的记录数
	   //cout << "BLOCK" << i << endl;
       int bias = 0;
	   for (j; j <= rowsId[0]; j++) {

		   //cout << "ROWS" << j << endl;
		   	  
		   if (rowsId[j] != -1) {
			   //cout << "rowsId[j]:" << rowsId[j] << endl;
			   for (k = 0; k < attrcount; k++) {
				
				   //cout << "ATTRBUTE" << k << endl;
				   if (table->attrs[k].attr_type == INT) {
						int a=stringToNum<int>(rows[rowsId[j] - 1].attr_values[k]);
						//cout << a << endl;
						memcpy(block->data + bias, &a, table->attrs[k].attr_len);

						//cout << *(reinterpret_cast<int*>(block->data + (j - 1)* table->length() + bias)) << endl;
				   }
					   
				   else if (table->attrs[k].attr_type == FLOAT) {
					   float a = stringToNum<float>(rows[rowsId[j] - 1].attr_values[k]);
					   memcpy(block->data + bias, &a, table->attrs[k].attr_len);
				   }

				   else {
					   char s[table->attrs[k].attr_len+1];
					   strncpy(s, (rows[rowsId[j] - 1].attr_values[k]).c_str(), table->attrs[k].attr_len);
					   s[table->attrs[k].attr_len] = '\0';
					   memcpy(block->data + bias, s, table->attrs[k].attr_len);

					   //cout << rows[rowsId[j] - 1].attr_values[k] << endl;
					   //cout << block->data + bias << endl;//
				   }
				   //cout << "bias" << endl;
				   bias += table->attrs[k].attr_len;
				   //leave null space			   
			   }
			
			   counts++;
		   }
		   else {
			   string str = "EMPTY";
			   memcpy(block->data + bias, str.c_str(), table->length());

			   bias += table->length();
			   counts++;
		   }
		   if (counts == count) {
			   //cout << "counts:" << counts << endl;
			   //cout << "count:" << count << endl;
			   //cout << "full" << endl;
		       break;
		   } //a block is full, come to next block
			 
	   }
	   block->dirty = true;
	}
	i--;

	Block* block = buffermanager->get_block(_filename.c_str(), 0);
	memcpy(block->data, &i, 4); //i block
	//cout << "D' i block:" << i << endl;
	memcpy(block->data + 4, &count, 4);//one block stores count records
	//cout << "count records:" <<count<< endl;
	for(int m=0;m<=rowsId[0];m++)
		memcpy(block->data + 8+ 4*m, &rowsId[m], 4);
	block->dirty = true;

	//cout << "D'tor END" << endl;
}

//insert data into table
int Data::insert(Record& record) {
	CatalogManager* manager = MiniSQL::get_CM();
	Table* table = manager->gettable(record.table_name);
	int prid = table->getPrimaryKeyId();
	for (int i = 0; i < rows.size(); i++) {
		if (rows[i].attr_values[prid] == record.attr_values[prid]) {
			cerr << "ERROR: [recordManager::insert] Primarykey `" << table->pk << "` is not unique!" << endl;
			return -1;
		}
	}
	Tuple tuples;
	for (int i = 0; i < record.num_values; i++) {
        tuples.attr_values[i] = record.attr_values[i];
		//cout << "INSERT" << sizeof(record.attr_values[i]) << endl;
	}
		
    rows.push_back(tuples);
	int m;
	for (m = 1; m <= rowsId[0]; m++) {
		if (rowsId[m] == -1) {
			rowsId[m] = rows.size();
			return m;
		}		
	}
	if (m == rowsId[0] + 1) {
		rowsId[m] = rows.size();
		rowsId[0]++;
		int left = stringToNum<int>(rows[0].attr_values[2]);
		//cout << "rowsId[0]" << left << endl;
		return m;
	}
}

//delete all record in the table
bool Data::deleteValue() {
	for (int i = rows.size() - 1; i >= 0; i--) {
		rows.pop_back();
	}
	for (int m = 1; m <=rowsId[0]; m++) 
		rowsId[m] = -1;
	rowsId[0] = 0;
	return true;
}

//delete record with conditions
bool Data::deleteValue(Table* table,Condition_list clist) {

	//cout<<"DELETE"<<endl;

	int count = clist.size();
	int i=0;
	vector<Tuple>::iterator m;
	for (m = rows.begin(); m!=rows.end(); *m++) {
		list<Condition>::iterator v;
        int j=0;
		for (v = clist.begin(); v != clist.end(); *v++) {
			int id = table->searchAttrId((*v).attr_name);
			string b = (*m).attr_values[id];

			if ((*v).attr_type == STRING) {
				if (charCmp((*v).cmp_value, b, (*v).op_type) == false)
					break;
			}
			if ((*v).attr_type == INTNUM) {
				if (intCmp((*v).cmp_value, b, (*v).op_type) == false)
				break;
			}
			if ((*v).attr_type == FLOATNUM) {
				if (floatCmp((*v).cmp_value, b, (*v).op_type) == false)
					break;
			}
			j++;
		}
		i++;
		if (j == count) {
            for (int k = 1; k <= rowsId[0]; k++) {
				if (rowsId[k] > rowsId[i])
					rowsId[k]--;
			}
			rowsId[i] = -1;
			//i--;

            m = rows.erase(m);//???
			*m--;
			
			
		}

	}
	return true;
}

//convert string type to any other types
/*template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}*/

// Compare string
bool Data::charCmp(string a, string b, string op)
{
	const char *p = op.c_str();

	if (strcmp(p, "=") == 0)
		return a == b;
	else if (strcmp(p, "<>") == 0)
		return a != b;
	else if (strcmp(p, ">") == 0)
		return a < b;
	else if (strcmp(p , "<")==0)
		return a > b;
	else if (strcmp(p ,">=")==0)
		return a <= b;
	else if (strcmp(p, "<=")==0)
		return a >= b;
	else
		return false;
}

// Compare int
bool Data::intCmp(string a, string b, string op)
{
	int left = stringToNum<int>(a);
	int right = stringToNum<int>(b);
	const char *p = op.c_str();

	if (strcmp(p, "=") == 0)
		return left == right;
	else if (strcmp(p, "<>") == 0)
		return left != right;
	else if (strcmp(p, ">") == 0)
		return left < right;
	else if (strcmp(p, "<") == 0)
		return left > right;
	else if (strcmp(p, ">=") == 0)
		return left <= right;
	else if (strcmp(p, "<=") == 0)
		return left >= right;
	else
		return false;
}

// Compare float
bool Data::floatCmp(string a, string b, string op)
{
	float left = stringToNum<float>(a);
	float right = stringToNum<float>(b);
	const char *p = op.c_str();

	if (strcmp(p, "=") == 0)
		return left == right;
	else if (strcmp(p, "<>") == 0)
		return left != right;
	else if (strcmp(p, ">") == 0)
		return left < right;
	else if (strcmp(p, "<") == 0)
		return left > right;
	else if (strcmp(p, ">=") == 0)
		return left <= right;
	else if (strcmp(p, "<=") == 0)
		return left >= right;
	else
		return false;
}
