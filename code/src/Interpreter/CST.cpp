#include "CST.h"

NodeManager node_manager;

extern void yyrestart(FILE* fp);
extern FILE* yyin;


//以下为语法树外部函数

CST* MallocNewNode()
{
    CST* pNew = new CST;
    node_manager.push_back(pNew);
    if (!pNew)
    {
        return pNew;
    }
    memset(pNew, 0, sizeof(*pNew));
    return pNew;
}

void ExecTree(CST* pNode)
{
    CST* current_node = pNode->lpSub;
    switch (current_node->type) {

        case CREATE:                      
            //move to next node, "table" or "index"
            current_node = current_node->lpNext;
            switch (current_node->type) {
                //create table
                case TABLE:
                    _CreateTable(current_node);
                    break;
                
                case INDEX: //CREATE INDEX NAME ON NAME '(' NAME ')'
                    _CreateIndex(current_node);
                    break;
                    
                default:
                    break;
            }
            break;
        
        case DROP:
            //move to next node, "table" or "index"
            current_node = current_node->lpNext;
            
            switch (current_node->type) {
                case TABLE:                    
                    _DropTable(current_node);
                    break;
                    
                case INDEX:
                    _DropIndex(current_node);
                    break;
                    
                default:
                    break;
            }
            break;
        
        case INSERT: //INSERT INTO NAME VALUES '(' attr_value_list ')'           
            _Insert(current_node);
            break;
        
        case DELETE:
            _Delete(current_node);
            break;
        
        case SELECT:
            _Select(current_node);
            break;
        
        case EXECFILE:
            _Execfile(current_node);
            break;

        default:
            break;
    }
}

CST* FatherAddSon(CST* pFather, CST* pSon)
{
    CST* pFS = pFather->lpSub;
    if (!pFS)//no son
    {
        pFather->lpSub = pSon;
        return pFather;
    }
    else
    {
        while(pFS->lpNext) //add to neighbor
        {
            pFS = pFS->lpNext;
        }
        
        pFS->lpNext = pSon;
        return pFather;
    }
}

//add num_of_sons sons to a new father node and return father
CST* NewFatherAddSon(int iType, int num_of_sons, ...)
{
    CST* pFather = MallocNewNode();
    if (!pFather)
    {
        return pFather;
    }
    
    pFather->type = iType;
    
    va_list ap;
    va_start(ap, num_of_sons);
    
    while(num_of_sons--)
        FatherAddSon(pFather, va_arg(ap, CST*));
    
    va_end(ap);
    
    return pFather;
}


void FreeTree(CST* pNode)
{
    if (pNode->lpNext)  FreeTree(pNode->lpNext);
    if (pNode->lpSub)   FreeTree(pNode->lpSub);
    //real free
    delete pNode;
    pNode = NULL;
}


//clear
void nm_clear()
{
    //printf("Free: ");
    for (int i = 0; i < node_manager.size(); ++i)
    {
        //printf("%s ", node_manager[i]->text);
        delete node_manager[i];
        node_manager[i] = NULL;
    }
    //printf("\n");
    node_manager.clear();
}

//

void _CreateTable(CST* current_node)
{
    Table tbl;
    //primary key
    string primary_key_name = "";

    //initialize table
    tbl.attr_count = 0;
    
    //move to next node, "name"
    current_node = current_node->lpNext;

    //assign the name value
    tbl.table_name = string(current_node->text);
    
    //move to next node, "attribute_info"
    current_node = current_node->lpNext;
    
    //move to its son, "attribute_list"
    current_node = current_node->lpSub;

    //get next node, "primary key name"
    if (current_node->lpNext != NULL)
        primary_key_name = string(current_node->lpNext->text);
    
    //move to its son, first "attr"
    current_node = current_node->lpSub;

    //deal with each attribute
    while (current_node != NULL) {
        
        //deal with current attr
        CST* temp_in_attr = current_node->lpSub; //NAME
        
        //assign name value
        tbl.attrs[tbl.attr_count].attr_name = string(temp_in_attr->text);
        
        //move to its next, "data_type"
        temp_in_attr = temp_in_attr->lpNext;
        
        //assign data_type value
        tbl.attrs[tbl.attr_count].attr_type = temp_in_attr->lpSub->type;
        
        //assign attr_key_type value
        if (temp_in_attr->lpNext == NULL) {
            tbl.attrs[tbl.attr_count].attr_key_type = -1;
        }
        else {
            tbl.attrs[tbl.attr_count].attr_key_type = temp_in_attr->lpNext->type;
        }
        
        //assign attribute len value
        if (temp_in_attr->lpSub->lpNext != NULL) //char
        {
            tbl.attrs[tbl.attr_count].attr_len = atoi(temp_in_attr->lpSub->lpNext->text);
            
            //if char(-1), return error
            if (tbl.attrs[tbl.attr_count].attr_len <= 0) {
                printf("error: SQL syntax wrong near '%d))'", tbl.attrs[tbl.attr_count].attr_len);
                return;
            }
            
        }
        else //not char
            tbl.attrs[tbl.attr_count].attr_len = 4;
        
        //assign attribute id value
        tbl.attrs[tbl.attr_count].attr_id = tbl.attr_count;
        
        //count next attribute
        ++tbl.attr_count;
        
        //move to next attribute
        current_node = current_node->lpNext;
        
    }

    //check whether primary key legal
    int primary_id = -1;
    if(primary_key_name != "") {
        bool exist = false;
        for (int i = 0; i < tbl.attr_count; ++i)
        {
            if(tbl.attrs[i].attr_name == primary_key_name) {
                primary_id = i;
                exist = true;
            }
        }
        if (exist == false)
        {
            printf("error: illegal primary key '%s'\n", primary_key_name.c_str());
            return;
        }
        else
        {
            //note primary key
            tbl.attrs[primary_id].attr_key_type = PRIMARY;
        }
    }
    
    //call API to create table
    auto start = system_clock::now();
    bool res = API_Create_Table(tbl);
    auto end = system_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    // Print execution time
    if (res)
        cout << "1 table created. Query done in " 
             << double(duration.count()) * milliseconds::period::num / milliseconds::period::den 
             << "s." << endl;
}

void _CreateIndex(CST* current_node)
{
    Index idx;
    //move to next node, "index_name"
    current_node = current_node->lpNext;
    
    //assign index_name value
    idx.index_name = string(current_node->text);
    
    //move to next node, "table_name"
    current_node = current_node->lpNext;
    
    //assign table_name value
    idx.table_name = string(current_node->text);
    
    //move to next node, "attr_name"
    current_node = current_node->lpNext;
    
    //assign attr_name value
    idx.attr_name = string(current_node->text);
    
    //call DEBUG to create index
    auto start = system_clock::now();
    bool res = API_Create_Index(idx);
    auto end = system_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    
    // Print execution time
    if (res)
        cout << "1 index created. Query done in " 
             << double(duration.count()) * milliseconds::period::num / milliseconds::period::den
             << "s." << endl;

}

void _DropTable(CST* current_node)
{
    //move to next node, "table_name"
    current_node = current_node->lpNext;
    
    //call API to drop table
    auto start = system_clock::now();
    bool res = API_Drop_Table(string(current_node->text));
    auto end = system_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);

    // Print execution time
    if (res)
        cout << "1 table dropped. Query done in " 
                << double(duration.count()) * milliseconds::period::num / milliseconds::period::den
                << "s." << endl;
}

void _DropIndex(CST* current_node)
{
    //move to next node, "index_name"
    current_node = current_node->lpNext;
    
    //call API to drop index
    auto start = system_clock::now();
    bool res = API_Drop_Index(string(current_node->text));
    auto end = system_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);

    // Print execution time
    if (res)
        cout << "1 index dropped. Query done in " 
             << double(duration.count()) * milliseconds::period::num / milliseconds::period::den
             << "s." << endl;
}


void _Insert(CST* current_node)
{
    Record record;

    //move to next node, "table name"
    current_node = current_node->lpNext;
    
    //get table name
    record.table_name = string(current_node->text);

    //move to next node, "attr_value_list"
    current_node = current_node->lpNext;
    
    //get each value
    current_node = current_node->lpSub;

    while(current_node != NULL) 
    {
        //get attribute
        record.attr_values.push_back(string(current_node->text));
        //move to next node
        current_node = current_node->lpNext;
    }

    // get the num_values
    record.num_values = record.attr_values.size();
    
    //real API Insert
    auto start = system_clock::now();
    bool res = API_Insert(record);
    auto end = system_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    // Print execution time
    if (res)
        cout << "1 record inserted. Query done in " 
             << double(duration.count()) * milliseconds::period::num / milliseconds::period::den
             << "s." << endl;
}

void _Delete(CST* current_node) //delete from table_name where a1 > value1 and a2 < value2
{
    //move to table_name
    current_node = current_node->lpNext;// NAME

    string table_name = string(current_node->text);

    Condition_list clist;

    //delete without condition: delete from table_name;
    if (current_node->lpNext == NULL) 
    {
        auto start = system_clock::now();
        int removeCount = API_Delete(table_name, clist);
        auto end = system_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        // Print execution time
        if (removeCount >= 0)
            cout << removeCount << " record(s) deleted. Query done in " 
                 << double(duration.count()) * milliseconds::period::num / milliseconds::period::den 
                 << "s." << endl;
        return;
    }

    //get first condition
    current_node = current_node->lpNext->lpSub;// wh_name

    //get each condition, NAME COMPARISON real_value
    while(current_node != NULL) 
    {
        //move to son, (attr)NAME 
        CST* temp_in_cond = current_node->lpSub;

        //note attr_name
        string attr_name = string(temp_in_cond->text);
        
        //move to next, COMPARSION
        temp_in_cond = temp_in_cond->lpNext;
        
        //note COMPARSION
        string op = string(temp_in_cond->text);

        //move to next, VALUE
        temp_in_cond = temp_in_cond->lpNext;

        //construct a condition
        Condition cond;
        cond.attr_name = attr_name;
        cond.attr_type = temp_in_cond->type;
        cond.op_type = op;
        cond.cmp_value = string(temp_in_cond->text);

        clist.push_back(cond);

        current_node = current_node->lpNext;
    }
    
    auto start = system_clock::now();
    int removeCount = API_Delete(table_name, clist, true);// have where clause
    auto end = system_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    // Print execution time
    if (removeCount >= 0)
        cout << removeCount << " record(s) deleted. Query done in " 
             << double(duration.count()) * milliseconds::period::num / milliseconds::period::den 
             << "s." << endl;
}

void _Select(CST* current_node)
{
    //move to table_name
    current_node = current_node->lpNext;

    string table_name = string(current_node->text);

    Condition_list clist;

    //delete without condition: delete from table_name;
    if (current_node->lpNext == NULL) 
    {   
        // Do selection
        auto start = system_clock::now();
        int selectCount = API_Select(table_name, clist);
        auto end = system_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        // Print execution time
        if (selectCount >= 0)
            cout << selectCount 
                 << " record(s) selected. Query done in " 
                 << double(duration.count()) * milliseconds::period::num / milliseconds::period::den
                 << "s." << endl;
        return;
    }

    //get first condition
    current_node = current_node->lpNext->lpSub;

    //get each condition, NAME COMPARISON real_value
    while(current_node != NULL) 
    {
        //move to son, NAME 
        CST* temp_in_cond = current_node->lpSub;
        
        //note type of attribute
        string attr_name = string(temp_in_cond->text);
        
        //move to next, COMPARSION
        temp_in_cond = temp_in_cond->lpNext;
        
        //note COMPARSION
        string op = string(temp_in_cond->text);

        //move to next, VALUE
        temp_in_cond = temp_in_cond->lpNext;

        //construct a condition
        Condition cond;
        cond.attr_name = attr_name;
        cond.attr_type = temp_in_cond->type;
        cond.op_type = op;
        cond.cmp_value = string(temp_in_cond->text);

        clist.push_back(cond);

        current_node = current_node->lpNext;
    }

    // Do selection
    auto start = system_clock::now();
    int selectCount = API_Select(table_name, clist, true); // has where clause
    auto end = system_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    // Print execution time
    if (selectCount >= 0)
        cout << selectCount 
                << " record(s) selected. Query done in " 
                << double(duration.count()) * milliseconds::period::num / milliseconds::period::den
                << "s." << endl;
}

void _Execfile(CST* current_node)
{
    //move to next node, file name
    current_node = current_node->lpNext;

    //get file name
    if(current_node != NULL)
    {
        FILE* fp = fopen(current_node->text, "r");
        if (fp != NULL)
        {   
            yyin = fp;
            yyrestart(yyin);
        }
        else
        {
            printf("error: no such SQL script '%s' exists\n", current_node->text);
            return;
        }
    }
}