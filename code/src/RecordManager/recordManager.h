#ifndef _RECORD_MANAGER_H
#define _RECORD_MANAGER_H

#include "../Global.h"
#include "data.h"

using namespace std;

class RecordManager
{
	public:
		bool createTable(Table* table);  //creat a table,if success return true 
		bool dropTable(Table* table);    //drop a table,if success return true 
		Data* select(Table* table, Condition_list clist);                                        
		                                //select record from table with conditions 
		Data* select(Table* table); 
		                                //select record from table
        int insert(Record& record);    //insert data into table and return record id
        bool deleteValue(Table* table, Condition_list clist);  
		                                //remove data from table with conditions
		bool deleteValue(Table* table); //remove data from table
		
	//private:
	    //unordered_map<string, Data*> DataMap;   
};

#endif
