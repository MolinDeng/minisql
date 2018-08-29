#ifndef _CATALOG_MANAGER_H
#define _CATALOG_MANAGER_H

#include "../Global.h"

using namespace std;

class CatalogManager
{
private:
	 // table map
    unordered_map<string, Table*> tableMap;
    //unordered_map<char*, int> tableIdMap;

    // Index name map
    unordered_map<string, Index*> indexMap;
   // unordered_map<char*, int> indexIdMap;
   // 
public:
 	CatalogManager();

    ~CatalogManager();

	bool tableExists(const string tableName);

	bool indexExists(const string indexName);

	bool isUnique(const string attrName, const string tableName);
	
	bool isPK(const Attribute attrName);
	
	bool addtable(	const string tableName, const Attribute* attrName ,const int attrCount);

	bool deletetable(const string tableName);

	Table* gettable(const string tableName); //const;

	bool addIndex(	const string indexName, const string tableName, const string attrName);
	
	bool deleteIndex(const string indexName);

	Index* getIndex(const string indexName); //const;

	int getattr_count(const string tableName); //const;

	const IndexName* GetIndexName(const string tableName);

	Index* getIndexByTableCol(const string tableName, const string attrName);

	//bool storeToFile();
	


    

	//void getIndexBytableName(const char* tableName, vector<Index*>* Inx);

	//char* getattrNameByIndex(const char* index) const;

	//List<char*> fieldsOntableName(char* tableName);
	
	//List<char*> indicesOntableName(char* tableName);
	
	//int rowLength(char* tableName);
	
	//char* pkOntableName(char* tableName);
	
	//FieldType fieldType(char* tableName, char* field);


};
#endif