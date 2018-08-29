#ifndef __data_H__
#define __data_H__
#include <string>
#include <vector>
#include "../Global.h"

using namespace std;

class Data
{
public:
	Data(const char* filename);
	Data() {};
	~Data();
    int insert(Record& record);    //insert data into table and return record id
    bool deleteValue(Table* table,Condition_list clist);  
		                                //remove data from table with conditions
	bool deleteValue(); //remove data from table	
	
	string tableName;
	vector<Tuple> rows;
	int getRowsize(){
		return rows.size();
	}
	
	bool myfunction(int i, int j) { return (rowsId[i]<rowsId[j]); }
	bool charCmp(string a, string b, string op);
	bool intCmp(string a, string b, string op);
	bool floatCmp(string a, string b, string op);

private:
	    int rowsId[10000]; //store order of rows
        
        //filename
	    string _filename;
		
};

#endif
