#include <iostream>
#include <fstream>
#include <sstream>
#include "API.h"
#include "../MiniSQL.h"

/*
bool DEBUG_Create_Table(Table& table)
{
    cout << "table_name: " << table.table_name << " ";
    cout << "attr_count: " << table.attr_count << endl;
    cout << "attr_name: " << "\t";
    for (int i = 0; i < table.attr_count; i++)
        cout << table.attrs[i].attr_name << "\t";
    cout << endl << "attr_type: " << "\t";
    for (int i = 0; i < table.attr_count; i++)
        cout << table.attrs[i].attr_type << "\t";
    cout << endl << "attr_key_type: " << "\t";
    for (int i = 0; i < table.attr_count; i++)
        cout << table.attrs[i].attr_key_type << "\t";
    cout << endl << "attr_len: " << "\t";
    for (int i = 0; i < table.attr_count; i++)
        cout << table.attrs[i].attr_len << "\t";
    cout << endl << "attr_id: " << "\t";
    for (int i = 0; i < table.attr_count; i++)
        cout << table.attrs[i].attr_id << "\t";
    return true;
}

bool DEBUG_Create_Index(Index& index)
{
    cout << "table_name: " << index.table_name << endl;
    cout << "attr_name: " << index.attr_name << endl;
    cout << "index_name: " << index.index_name << endl;
    return true;
}

bool DEBUG_Drop_Table(string table_name)
{
    cout << "table_name: " << table_name << endl;
    return true;
}


bool DEBUG_Drop_Index(string index_name)
{
    cout << "index_name: " << index_name << endl;
    return true;
}

bool DEBUG_Insert(Record& record)
{
    cout << "table_name: " << record.table_name << endl;
    cout << "values: (";
    for (auto elem : record.attr_values)
        cout << elem << ", ";
    cout << ")" << endl;
    cout << "num of values: " << record.num_values << endl;
    return true;
}

int DEBUG_Select(string table_name, Condition_list clist, bool if_where)
{
    cout << "table_name: " << table_name << endl;
    if(!if_where)
        cout << "no \'where\' clause!" << endl;
    else {
        cout << "attr_name" << "\t" << "attr_type" << "\t" << "op_type" << "\t" << "value" << "\t" << endl;
        for (auto l : clist){
            cout << l.attr_name << "\t" << l.attr_type << "\t" << l.op_type << "\t" << l.cmp_value << "\t" << endl;
        }
    }
    return 0;
}

int DEBUG_Delete(string table_name, Condition_list clist, bool if_where)
{
    cout << "table_name: " << table_name << endl;
    if(!if_where)
        cout << "no \'where\' clause!" << endl;
    else {
        cout << "attr_name" << "\t" << "attr_type" << "\t" << "op_type" << "\t" << "value" << "\t" << endl;
        for (auto l : clist){
            cout << l.attr_name << "\t" << l.attr_type << "\t" << l.op_type << "\t" << l.cmp_value << "\t" << endl;
        }
    }
    return 0;
}
*/

template <class Type>
Type stringToNum(const string& str)
{
    istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}


//  创建表时的内部调用
bool API_Create_Table(Table& table)
{
    CatalogManager* catalog = MiniSQL::get_CM();
    RecordManager* record = MiniSQL::get_RM();
    if(catalog->addtable((table.table_name).c_str(), table.attrs, table.attr_count));
    {
        Table* t = catalog->gettable((table.table_name).c_str());
        if(record->createTable(t))
        {
            //Create primary key index
            int id = table.getPrimaryKeyId();
            string Attr_name = table.attrs[id].attr_name;
            string Index_name = to_string(time(0));
            string Table_name = table.table_name;
            Index index;
            index.attr_name = Attr_name;
            index.index_name = Index_name;
            index.table_name = Table_name;
            if(API_Create_Index(index))
            {
                cout << "Success to create " + table.table_name +" table." << endl;
                return true;
            }
            return false;
        }
        return false;
    }
    return false;
}

//  删除表时的内部调用
bool API_Drop_Table(string table_name)
{
    CatalogManager* catalog = MiniSQL::get_CM();
    RecordManager* record = MiniSQL::get_RM();
    Table* t = catalog->gettable(table_name);
    if(t == NULL)
    {
         cerr << "ERROR:Cannot find " + table_name + " table!" << endl;
         return false;
    }
    if(record->dropTable(t))
    {
        const IndexName* indexname = catalog->GetIndexName(table_name);
        int i = 0;
        for(i; i < indexname->len; i++)
        {
            bool flag;
            flag = API_Drop_Index(indexname->name[i]);
            if(flag == false)
            {
                delete indexname;
                return false;
            }
        }
        delete indexname;
        if(catalog->deletetable(table_name))
        {
            cout << "Success to drop " + table_name + " table." << endl;
            return true;
        }
        return false;
    }
    return false;
}

//  创建索引时的内部调用
bool API_Create_Index(Index& index)
{
    CatalogManager* catalog = MiniSQL::get_CM();
    RecordManager* record = MiniSQL::get_RM();
    IndexManager* index_manager = MiniSQL::get_IM();
    if(catalog->addIndex(index.index_name, index.table_name, index.attr_name))
    {
        Table* t = catalog->gettable((index.table_name).c_str());
        int id = t->searchAttrId(index.attr_name);
        int KeyLength = t->attrs[id].attr_len;
        if(index_manager->CreateIndex((index.index_name).c_str(), KeyLength))
        {
            int i = 0;
            Data* d = record->select(t);
            int sum = d->getRowsize();
            for(i; i < sum; i++)
            {
                bool flag;
                flag = index_manager->Insert((index.index_name).c_str(), (d->rows[i].attr_values[id]).c_str(), i + 1);
                if(flag == false)return false;
            }
            cout << "Success to create " + index.index_name + " indxe." << endl;
            return true;
        }
        return false;
    }
    return false;
}

//  删除索引时的内部调用
bool API_Drop_Index(string index_name)
{
    CatalogManager* catalog = MiniSQL::get_CM();
    IndexManager* index = MiniSQL::get_IM();
    if(index->DropIndex(index_name.c_str()))
    {
        if(catalog->deleteIndex(index_name))
        {
            cout << "Success to drop " + index_name + " index." << endl;
            return true;
        }
        return false;
    }
    else
    {
        cerr << "ERROR:Cannot find " + index_name + " index!" << endl;
        return false;
    }
}

//  插入纪录时的内部调用
bool API_Insert(Record& record)
{
    CatalogManager* catalog = MiniSQL::get_CM();
    RecordManager* record_manager = MiniSQL::get_RM();
    IndexManager* index = MiniSQL::get_IM();
    int no;

    Table* t = catalog->gettable(record.table_name);
    if(t == NULL)
    {
        cerr << "ERROR:Cannot find " + record.table_name + " table!" << endl;
        return false;
    }
    if((no = record_manager->insert(record)) >= 1)
    {
        const IndexName* indexname = catalog->GetIndexName(record.table_name);
        int p = 0;
        for(p; p < indexname->len; p++)
        {
            Index* i;
            int id;
            bool flag;
            i = catalog->getIndex(indexname->name[p]);
            id = t->searchAttrId(i->attr_name);
            flag = index->Insert((i->index_name).c_str(), (record.attr_values[id]).c_str(), no);
            if(flag == false)
            {
                delete indexname;
                return false;
            }
        }
        delete indexname;
        cout << "Success to insert." << endl;
        return true;         
    }
    return false;
}

//  选择纪录时的内部调用
int API_Select(string table_name, Condition_list clist, bool if_where)
{
    CatalogManager* catalog = MiniSQL::get_CM();
    RecordManager* record = MiniSQL::get_RM();
    IndexManager* index = MiniSQL::get_IM();

    Table* t = catalog->gettable(table_name);
    if(t == NULL)
    {
        cerr << "ERROR:Cannot find " + table_name + " table!" << endl;
        return 0;
    }

    int o = 0;
    for(o; o < t->attr_count; o++)
    {
        cout << t->attrs[o].attr_name << '\t';
    }
    cout << endl;

    if(if_where == false)
    {
        Data* d = record->select(t);
        int p = 0;
        int sum = d->getRowsize();
        for(p; p < sum; p++)
        {
            int q = 0;
            for(q; q < t->attr_count; q++)
            {
                if(t->attrs[q].attr_type == 276)
                {
                    cout << stringToNum<int>(d->rows[p].attr_values[q])<< '\t';
                }
                else if(t->attrs[q].attr_type == 277)
                {
                    cout << stringToNum<float>(d->rows[p].attr_values[q])<< '\t';
                }
                else if(t->attrs[q].attr_type == 278)
                {
                    cout << d->rows[p].attr_values[q] << '\t';
                }
            }
            cout << endl;
        }
        return p;
    }

    else if(if_where == true)
    {
        bool flag = true;
        //BpTree
        Data* d;
        vector<int> id;
        int idlen = 0;
        list<Condition>::iterator itor;
        itor = clist.begin();
        while(itor!=clist.end())  
        {  
            if((*itor).op_type == "=")
            {
                Index* i = catalog->getIndexByTableCol(table_name, (*itor).attr_name);
                if(i == NULL)
                {
                    flag = false;
                    break;
                }
                int p = 0;
                p = index->Find((i->index_name).c_str(), ((*itor).cmp_value).c_str());
                if(p == -1)
                {
                    flag = false;
                    break;
                }
                id.push_back(p);
                idlen++;
            } 
            else
            {
                flag = false;
                break;
            }
            if(idlen > 1)
            {
                if(id[idlen - 1] != id[idlen - 2])
                {
                    flag = false;
                    return 0;
                }
            }
            *itor++;
        } 
        if(flag)
        {
            d = record->select(t);
            int q = 0;
            for(q; q < t->attr_count; q++)
            {
                if(t->attrs[q].attr_type == 276)
                {
                    cout << stringToNum<int>(d->rows[id[idlen - 1] - 1].attr_values[q])<< '\t';
                }
                else if(t->attrs[q].attr_type == 277)
                {
                    cout << stringToNum<float>(d->rows[id[idlen - 1] - 1].attr_values[q])<< '\t';
                }
                else if(t->attrs[q].attr_type == 278)
                {
                    cout << d->rows[id[idlen - 1] - 1].attr_values[q] << '\t';
                }
            }
            cout << endl;
            return 1;
        }
        

        //Order
        d = record->select(t, clist);
        int p = 0;
        int sum = d->getRowsize();
        for(p; p < sum; p++)
        {
            int q = 0;
            for(q; q < t->attr_count; q++)
            {
                if(t->attrs[q].attr_type == 276)
                {
                    cout << stringToNum<int>(d->rows[p].attr_values[q])<< '\t';
                }
                else if(t->attrs[q].attr_type == 277)
                {
                    cout << stringToNum<float>(d->rows[p].attr_values[q])<< '\t';
                }
                else if(t->attrs[q].attr_type == 278)
                {
                    cout << d->rows[p].attr_values[q] << '\t';
                }
            }
            cout << endl;
        }
        return p;
    }
}

//  删除纪录时的内部调用
int API_Delete(string table_name, Condition_list clist, bool if_where)
{
    CatalogManager* catalog = MiniSQL::get_CM();
    RecordManager* record = MiniSQL::get_RM();
    IndexManager* index = MiniSQL::get_IM();

    Table* t = catalog->gettable(table_name);
    if(t == NULL)
    {
        cerr << "ERROR:Cannot find " + table_name + " table!" << endl;
        return 0;
    }
    if(if_where == false)
    {
        const IndexName* indexname = catalog->GetIndexName(table_name);
        int p = 0;
        Data* d = record->select(t);
        int sum = d->getRowsize();
        for(p; p < indexname->len; p++)
        {
            Index* i = catalog->getIndex(indexname->name[p]);
            int id = t->searchAttrId(i->attr_name);
            int q = 0;
            for(q; q < sum; q++)
            {
                string res = d->rows[q].attr_values[id];
                index->Remove(i->index_name.c_str(), res.c_str());
            }
        }
        delete indexname;
        record->deleteValue(t);
        cout << "Success to delete." << endl;
        return sum;
    }
    else if(if_where == true)
    {
        //order
        const IndexName* indexname = catalog->GetIndexName(table_name);
        int p = 0;
        Data *d = record->select(t, clist);
        int sum = d->getRowsize();
        for(p; p < indexname->len; p++)
        {
            Index* i = catalog->getIndex(indexname->name[p]);
            int id = t->searchAttrId(i->attr_name);
            int q = 0;
            for(q; q < sum; q++)
            {
                string res = d->rows[q].attr_values[id];
                index->Remove(i->index_name.c_str(), res.c_str());
            }
        }
        delete indexname;
        record->deleteValue(t, clist);
        cout << "Success to delete." << endl;
        return sum;           
    }
}

