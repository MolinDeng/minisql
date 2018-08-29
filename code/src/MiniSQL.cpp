#include "MiniSQL.h"

// Managers
BufferManager* MiniSQL::bufferManager = NULL;
CatalogManager* MiniSQL::catalogManager = NULL;
RecordManager* MiniSQL::recordManager = NULL;
IndexManager* MiniSQL::indexManager = NULL;

MiniSQL::MiniSQL() {
    // Check if catalog/tables.mdb exists
	if (!is_file_exist("catalog/Table"))
		create_file("catalog/Table", 62);

	// Check if catalog/indices.mdb exists
	if (!is_file_exist("catalog/Index"))
		create_file("catalog/Index", 93);
    bufferManager = new BufferManager();
    catalogManager = new CatalogManager();
    recordManager = new RecordManager();
    indexManager = new IndexManager();
}

MiniSQL::~MiniSQL() {
    delete catalogManager;
    delete indexManager;
    //delete bufferManager;
    delete recordManager;
	delete bufferManager;
}

void MiniSQL::create_file(string _filename, int _record_length) {
    _record_length++;

    FILE* file = fopen(("data/" + _filename + ".mdb").c_str(), "wb");
    char data[MAX_BLOCK_SIZE] = {0};
    memcpy(data, &_record_length, 4);
    // Set record count to 0
    memset(data + 4, 0, 4);
    // Set first empty record to -1
    memset(data + 8, 0xFF, 4);
    fwrite(data, MAX_BLOCK_SIZE, 1, file);
    fclose(file);
}

bool MiniSQL::is_file_exist(string _filename) {
    FILE* file = fopen(("data/" + _filename + ".mdb").c_str(), "rb");
    if (file)
    {
        fclose(file);
        return true;
    }
    return false;
}
// Get buffer manager
BufferManager* MiniSQL::get_BM() {
    return bufferManager;
}

// Get catalog manager
CatalogManager* MiniSQL::get_CM()
{
    return catalogManager;
}

// Get record manager
RecordManager* MiniSQL::get_RM()
{
     return recordManager;
}

// Get index manager
IndexManager* MiniSQL::get_IM() {
    return indexManager;
}

int main(int argc, const char* argv[]) {
    MiniSQL *minisql = new MiniSQL(); 
    Interpreter* interpreter = new Interpreter();
    interpreter->run();
    delete interpreter;
    delete minisql;
    return 0;
}