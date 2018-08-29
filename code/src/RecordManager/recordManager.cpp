#include <typeinfo>
#include "recordManager.h"
#include "data.h"

using namespace std;

//create a table,if success return true 
bool RecordManager::createTable(Table* table) {
	string filename = table->table_name;
	FILE* file = fopen(("data/record/" + string(filename) + ".mdb").c_str(), "wb");
	if (file == NULL)
	{
		cerr << "Error:[recordManager]Fail to create " + string(filename) + " table." << endl;
		return false;
	}
	int HeadFile[] = { 0,0,0,0 };
	fwrite(HeadFile, 4, 4, file);
	fclose(file);
	//Data* datas = new Data(filename.c_str());
	//delete datas;
	return true;
}

//drop a table,if success return true 
bool RecordManager::dropTable(Table* table) {
	string filename = table->table_name;
	int i;
	deleteValue(table);
	i = remove(("data/record/" + string(filename) + ".mdb").c_str());
	if (i == 0) {
        return true;
	}		
	else
	{
		cerr << "Error:[recordManager]Fail to drop " + string(filename) + " table." << endl;
		return false;
	}
}

//select record from table
Data* RecordManager::select(Table* table) {
	string filename = table->table_name;
	Data* datas = new Data(filename.c_str());
	//cout << "m" << endl;
	//if(datas->rows.size()!=0)
	  //cout <<"Values:" << datas->rows[0].attr_values[0] << endl;
	//sort(datas->rows.begin(), datas->rows.end(), datas->myfunction);

	return datas;
}

//select record from table with conditions
Data* RecordManager::select(Table* table, Condition_list clist) {
	string filename = table->table_name;
	Data* data0 = new Data(filename.c_str());

	//sort(data0->rows.begin(), data0->rows.end(), data0->myfunction);
	Data* data1 = new Data;

	data1->tableName = filename;
	int count = clist.size();
	int j;
	for (int i = 0; i < data0->rows.size(); i++) {

		list<Condition>::iterator v = clist.begin();
		for (j = 0; j < count; j++ ) {

			int id = table->searchAttrId((*v).attr_name);
			string b = data0->rows[i].attr_values[id];
			//cout << "ATTR_TYPE" << (*v).attr_type << endl;
			if ((*v).attr_type == STRING) { //char
				//cout << "char" << endl;
				if (data0->charCmp((*v).cmp_value, b, (*v).op_type) == false)
					break;
			}
			if ((*v).attr_type == INTNUM) {
				//cout << "int" << endl;
				if (data0->intCmp((*v).cmp_value, b, (*v).op_type) == false)
					break;
			}
			if ((*v).attr_type == FLOATNUM) {
				//cout << "float" << endl;
				if (data0->floatCmp((*v).cmp_value, b, (*v).op_type) == false)
					break;
			}
			
			*v++;
		}
		if (j == count) {
			Tuple tuple;
			tuple = data0->rows[i];
            data1->rows.push_back(tuple);
		}
			
	}


	return data1;
}

//insert data into table
int RecordManager::insert(Record& record) {
								//printf("begin\n");
    string filename = record.table_name;
	FILE* file = fopen(("data/record/" + string(filename) + ".mdb").c_str(), "r");
	if (file == NULL) {
		cerr << "ERROR: [recordManager::insert] table " << record.table_name << "` doesn't exists!" << endl;
		return -1;
	}
	Data* data = new Data(filename.c_str());
								//printf("new\n");
								//sort(data->rows.begin(), data->rows.end(), data->myfunction);
	int id = data->insert(record);
								//printf("insert\n");
	delete data;
								//printf("delete\n");
	fclose(file);
	return id;
}

//delete all record in the table
bool RecordManager::deleteValue(Table* table) {
	string filename = table->table_name;
	FILE* file = fopen(("data/record/" + string(filename) + ".mdb").c_str(), "r");
	if (file == NULL) {
		cerr << "ERROR: [recordManager::delete] Table `" << table->table_name << "` is NULL!" << endl;
		return false;
	}
	Data* data = new Data(filename.c_str());
	//sort(data->rows.begin(), data->rows.end(), data->myfunction);
	bool flag;
	flag = data->deleteValue();
	delete data;
	fclose(file);
	return flag;
}

//delete record with conditions
bool RecordManager::deleteValue(Table* table, Condition_list clist) {
	string filename = table->table_name;
	FILE* file = fopen(("data/record/" + string(filename) + ".mdb").c_str(), "r");
	if (file == NULL) {
		cerr << "ERROR: [recordManager::delete] Table `" << table->table_name << "` is NULL!" << endl;
		return false;
	}
	Data* data = new Data(filename.c_str());
	//sort(data->rows.begin(), data->rows.end(), data->myfunction);
	bool flag;
	flag = data->deleteValue(table,clist);
	delete data;
	fclose(file);
	return flag;
}