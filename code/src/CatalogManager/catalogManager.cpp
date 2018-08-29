//#include <cchar*>
//#include <iostream>
//#include <unordered_set>

#include "../MiniSQL.h"
#include "../CatalogManager/catalogManager.h"

using namespace std;

CatalogManager::CatalogManager()
{   
	
    string Tablefilename = "Table";
    string Indexfilename = "Index";
    //load table
    BufferManager* buffermanager = MiniSQL::get_BM();
    Block* block = buffermanager->get_block(("catalog/"+Tablefilename).c_str(), 0);
    int recordCount = *(reinterpret_cast<int*>(block->data));

    memset(block->data, 0, MAX_BLOCK_SIZE);
    block->dirty = false;

    //cout<< "TABLE DEBUG BEGIN"<<endl;
    //cout<< recordCount <<endl;

    for(int i = 1; i <= recordCount;i++){
        block = buffermanager->get_block(("catalog/"+Tablefilename).c_str(), i);
        char* tableData = block->data;
        Table* table = new Table;
        table->table_name = tableData;
        table->pk = tableData + 31;
        table->attr_count = *(reinterpret_cast<int*>(tableData + 62));
        for(int j=0 ; j < table->attr_count ; j++){
            table->attrs[j].attr_name= tableData +66+j*31+4*j*4;
            table->attrs[j].attr_type=*(reinterpret_cast<int*>(tableData +66+(j+1)*31+4*j*4));
            table->attrs[j].attr_key_type=*(reinterpret_cast<int*>(tableData +66+(j+1)*31+(4*j+1)*4));
            table->attrs[j].attr_len=*(reinterpret_cast<int*>(tableData +66+(j+1)*31+(4*j+2)*4));
            table->attrs[j].attr_id=*(reinterpret_cast<int*>(tableData +66+(j+1)*31+(4*j+3)*4));
        }

        memset(block->data, 0, MAX_BLOCK_SIZE);
        block->dirty = false;
        tableMap[table->table_name] = table;
    }

    //for (auto table : tableMap){
        //cout << table.second->table_name <<endl;
        //cout << table.second->pk <<endl;
        //out << table.second->attr_count <<endl;
        //for(int j=0 ; j < table.second->attr_count ; j++){
            //cout <<table.second->attrs[j].attr_name<<endl;
            //cout <<table.second->attrs[j].attr_type<<endl;
            //cout <<table.second->attrs[j].attr_key_type<<endl;
            //cout <<table.second->attrs[j].attr_len<<endl;
            //cout <<table.second->attrs[j].attr_id<<endl;
        //}
    //}
    //cout <<"TABLE DEBUG END"<<endl;

    //load index
    block = buffermanager->get_block(("catalog/"+Indexfilename).c_str(), 0);
    recordCount = *(reinterpret_cast<int*>(block->data));

    memset(block->data, 0, MAX_BLOCK_SIZE);
    block->dirty = false;

    //cout<< "INDEX DEBUG BEGIN"<<endl;
    //cout<< recordCount <<endl;

   for(int i = 1; i <= recordCount;i++){
        block = buffermanager->get_block(("catalog/"+Indexfilename).c_str(), i);
        char* indexData = block->data;

        Index* index = new Index;
        index->index_name=indexData;
        index->table_name=indexData + 31;
        index->attr_name=indexData + 62;
        
        memset(block->data, 0, MAX_BLOCK_SIZE);
        block->dirty = false;
        indexMap[index->index_name] = index;
    }

    //for (auto index : indexMap){
        //cout << index.second->table_name <<endl;
        //cout << index.second->index_name <<endl;
        //cout << index.second->attr_name <<endl;
    //}
    //cout<< "INDEX DEBUG END"<<endl;

}

CatalogManager::~CatalogManager()
{
    string Tablefilename = "Table";
    string Indexfilename = "Index";
    int i=1;
    BufferManager* buffermanager = MiniSQL::get_BM();
    Block* block;
    for (auto table : tableMap){
        if (table.second->table_name.empty()||table.second->attr_count == 0||table.second->pk.empty()) continue;
        block = buffermanager->get_block(("catalog/"+Tablefilename).c_str(), i);
        char* data = block->data;
        memcpy(data, table.second->table_name.c_str(), 31);

        //cout<<"TABLE DEBUG BEGIN"<<endl;

        //cout << data << endl;

        memcpy(data+31, table.second->pk.c_str(), 31);

        //cout<< data+31 <<endl;

        memcpy(data+62, &table.second->attr_count, 4);

        //cout<< *(reinterpret_cast<int*>(data + 62))<<endl;

        for(int j =0 ;j < table.second->attr_count;j++){
            memcpy(data+66+j*31+4*j*4, table.second->attrs[j].attr_name.c_str(), 31);

            //cout<< data+66+j*31+4*j*4 <<endl;

            memcpy(data+66+(j+1)*31+4*j*4, &table.second->attrs[j].attr_type, 4);

            //cout<< *(reinterpret_cast<int*>(data+66+(j+1)*31+4*j*4))<<endl;

            memcpy(data+66+(j+1)*31+(4*j+1)*4, &table.second->attrs[j].attr_key_type, 4);

            //cout<< *(reinterpret_cast<int*>(data+66+(j+1)*31+(4*j+1)*4))<<endl;

            memcpy(data+66+(j+1)*31+(4*j+2)*4, &table.second->attrs[j].attr_len, 4);

            //cout<< *(reinterpret_cast<int*>(data+66+(j+1)*31+(4*j+2)*4))<<endl;

            memcpy(data+66+(j+1)*31+(4*j+3)*4, &table.second->attrs[j].attr_id, 4);

            //cout<< *(reinterpret_cast<int*>(data+66+(j+1)*31+(4*j+3)*4))<<endl;
        }
        i++;
        block->dirty = true;
        delete table.second;
    }
    i=i-1;
    block = buffermanager->get_block(("catalog/"+Tablefilename).c_str(), 0); 
    char* data = block -> data;
    memcpy(data , &i, 4);
    block->dirty = true;

    //cout<< *(reinterpret_cast<int*>(data))<<endl;

    //cout<<"TABLE DEBUG END"<<endl;


    i=1;
    for (auto index : indexMap){
        if(index.second->table_name.empty() || index.second->index_name.empty() || index.second->attr_name.empty()) continue;

        block = buffermanager->get_block(("catalog/"+Indexfilename).c_str(), i);
        char* data = block -> data;
        memcpy(data, index.second->index_name.c_str(), 31);

        //cout<<"INDEX DEBUG BEGIN"<<endl;

        //cout << data << endl;

        memcpy(data+31, index.second->table_name.c_str(), 31);

        //cout << data+31 << endl;

        memcpy(data+62, index.second->attr_name.c_str(), 31);

        //cout << data+62 << endl;

        i++;
        block->dirty = true;
        delete index.second;
    }
    i=i-1;
    block = buffermanager->get_block(("catalog/"+Indexfilename).c_str(), 0);
    data = block -> data;
    block->dirty = true;
    memcpy(data, &i, 4);

    //cout<< *(reinterpret_cast<int*>(data))<<endl;

    //cout<<"INDEX DEBUG END"<<endl;
    system("pause");
}

bool CatalogManager::tableExists(const string tableName)
{
    if (tableMap.find(tableName) != tableMap.end())
        return true;
    else
        return false;
}

bool CatalogManager::indexExists(const string indexName)
{
    if (indexMap.find(indexName) != indexMap.end())
        return true;
    else
        return false;
}

bool CatalogManager::isUnique(const string attrName, const string tableName)
{
    Attribute A;
    Table* T= gettable(tableName);
    for(int i=0;i<T->attr_count;i++){
        if(T->attrs[i].attr_name==attrName){
            A=T -> attrs[i];
            break;
        }
    }
    if(A.attr_key_type == UNIQUE || A.attr_key_type == PRIMARY) return true;
    else return false;
}
    
bool CatalogManager::isPK(const Attribute attrName)
{
    if(attrName.attr_key_type == PRIMARY) return true;
    else return false;
}
    
bool CatalogManager::addtable(  const string tableName, const Attribute* attrName ,const int attrCount)
{ 
        if(tableExists(tableName)){
            cerr << "ERROR: [CatalogManager::addtable] Table `" << tableName << "` already exists!" << endl;
            return false;
        } 
        if(tableName.empty()){
            cerr << "ERROR: [CatalogManager::addtable] Tablename cannot be empty!" << endl;
            return false;
        }
        int i=0;
        int p=0;

        for (i=0;i<attrCount;i++){
            if(isPK(attrName[i])) break;
        }
        if(i>=attrCount){
            cerr << "ERROR: [CatalogManager::addtable] Cannot find primary key !" << endl;
            return false;
        }
        else{ 
            p=i;
        }

        for(int i=0;i<attrCount-1;i++)
            for(int j=i+1;j<attrCount;j++){
                if(attrName[i].attr_name==attrName[j].attr_name){
                    cerr << "ERROR: [CatalogManager::addtable] Duplicate column name `" << attrName[i].attr_name << "`!" << endl;
                    return false;
                }
            }

        Table* table = new Table;
        table->table_name = tableName;
        table->pk = attrName[p].attr_name;
        table->attr_count = attrCount;
        for(i=0;i<attrCount;i++)
            table->attrs[i] = attrName[i];
        tableMap[tableName] = table;

        return true;
}

bool CatalogManager::deletetable(const string tableName)
{
    if(!tableExists(tableName)) 
    {
        cerr << "ERROR: [CatalogManager::deletetable] Table `" << tableName << "` does not exist!" << endl;
        return false;
    }
    else{ 
        //string IndexOnTable[100];

        delete tableMap[tableName];
        tableMap.erase(tableName);
        /*
        int i=0;
        for (auto index : indexMap){
            if(tableName==index.second->table_name) 
                IndexOnTable[i]=index.second->index_name;

            i++;
        }
        int count=i;
        for(i=0;i<count;i++){
            deleteIndex(IndexOnTable[i]);
        }
        */
        return true;
    }
   
}

const IndexName* CatalogManager::GetIndexName(const string tableName)
{
    IndexName* indexname = new  IndexName;
    int i=0;
    indexname->len = 0;
    for (auto index : indexMap){

        if(tableName==index.second->table_name)
        {
            indexname->name[i]=index.second->index_name;
            i++;
            indexname->len++;
        } 
    }
    return indexname;
}

Table* CatalogManager::gettable(const string tableName) //const
{
    if(!tableExists(tableName))
    {
        cerr << "ERROR: [CatalogManager::gettable] Table `" << tableName << "` does not exist!" << endl;
        return NULL;
    } 
    else return tableMap.at(tableName);
}

bool CatalogManager::addIndex(  const string indexName, const string tableName, const string attrName)
{
    if(!tableExists(tableName))
    {
         cerr << "ERROR: [CatalogManager::addIndex] Table `" << tableName << "` does not exist!" << endl;
        return false;
    } 

    if(tableName.empty()){
        cerr << "ERROR: [CatalogManager::addIndex] Tablename cannot be empty!" << endl;
        return false;
    }
    if(indexName.empty()){
        cerr << "ERROR: [CatalogManager::addIndex] Indexname cannot be empty!" << endl;
        return false;
    }
    if(indexExists(indexName)){
            cerr << "ERROR: [CatalogManager::addIndex] Index `" << indexName << "` already exists!" << endl;
            return false;
    } 
    if(!isUnique(attrName,tableName)){
         cerr << "ERROR: [CatalogManager::addIndex] Attribute `" << attrName << "` is not unique!" << endl;
         return false;
    }

    Index* exist = NULL;
    for (auto item : indexMap)
    {
        Index* index0 = item.second;
        if (index0->table_name==tableName && index0->attr_name==attrName)
            exist=index0;
    }
    if(exist!=NULL)
    {
        cerr << "ERROR: [CatalogManager::addIndex] Index with table name `" << tableName << "` and attribute name `" << attrName << "` already exists(Index name `" << indexName << "`)!" << endl;
        return false;
    }


    Index* index = new Index;
    index->index_name=indexName;
    index->table_name=tableName;
    index->attr_name=attrName;
    indexMap[indexName] = index;

    return true;
}
    
bool CatalogManager::deleteIndex(const string indexName)
{
    if(!indexExists(indexName))
    {
        cerr << "ERROR: [CatalogManager::deleteIndex] Index `" << indexName << "` does not exist!" << endl;
        return false;
    } 
    else{ 
        delete indexMap[indexName];
        indexMap.erase(indexName);
        return true;
    }
}

Index* CatalogManager::getIndex(const string indexName) //const
{
    if(!indexExists(indexName))
    {
        cerr << "ERROR: [CatalogManager::deleteIndex] Index `" << indexName << "` does not exist!" << endl;
        return NULL;
    } 
    else return indexMap.at(indexName);
}


int CatalogManager::getattr_count(const string tableName) //const
{
    if(!tableExists(tableName))
    {
        cerr << "ERROR: [CatalogManager::getattr_count] Table `" << tableName << "` does not exist!" << endl;
        return -1;
    } 
    else {
        Table* T=gettable(tableName);
        return T->attr_count;
    }
}


Index* CatalogManager::getIndexByTableCol(const string tableName, const string attrName)
{
    if(!tableExists(tableName))
    {
        cerr << "ERROR: [CatalogManager::getIndexByTableCol] Table `" << tableName << "` does not exist!" << endl;
        return NULL;
    } 
    Index* exist;
    for (auto item : indexMap)
    {
        Index* index0 = item.second;
        if (index0->table_name==tableName && index0->attr_name==attrName){
            exist=index0;
            return exist;
        }
        
    }
    if(exist == NULL)
    {
        cerr << "ERROR: [CatalogManager::getIndexByTableCol] Index with table name `" << tableName << "` and attribute name `" << attrName << "` does not exist!" << endl;
        return NULL;
    }
}

//#ifdef DEBUG
// Print all tables and indices info
//void CatalogManager::debugPrint() const
//{
    //cerr << "DEBUG: [CatalogManager::debugPrint] debugPrint begin" << endl;
    //for (auto table : tableMap)
        //table.second->debugPrint();
    //for (auto index : indexMap)
        //index.second->debugPrint();
    //cerr << "DEBUG: [CatalogManager::debugPrint] debugPrint end" << endl;
//}
//#endif