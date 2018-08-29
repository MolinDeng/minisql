#pragma once

#include "Interpreter/Interpreter.h"
#include "API/API.h"
#include "BufferManager/BufferManager.h"
#include "IndexManager/IndexManager.h"
#include "CatalogManager/catalogManager.h"
#include "RecordManager/recordManager.h"

class MiniSQL {
public:
    // C'tor
    MiniSQL();

    // D"tor
    ~MiniSQL();

    // Get buffer manager
    static BufferManager* get_BM();

    // Get catalog manager
    static CatalogManager* get_CM();

    // Get record manager
    static RecordManager* get_RM();

    // Get index manager
    static IndexManager* get_IM();

private:
    // Buffer manager
    static BufferManager* bufferManager;

    // Catalog manager
    static CatalogManager* catalogManager;

    // Record manager
    static RecordManager* recordManager;

    // Index manager
    static IndexManager* indexManager;

    static void create_file(string _filename, int _record_length);

    static bool is_file_exist(string _filename);
};