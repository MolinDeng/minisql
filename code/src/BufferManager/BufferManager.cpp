#include "BufferManager.h"

BufferManager::BufferManager() {
    block_cnt = 0;

    // Initialize dummy head and tail
    lru_head = new Block_node(NULL);
    lru_tail = new Block_node(NULL);
    lru_head->pre = lru_head->next = lru_tail;
    lru_tail->pre = lru_tail->next = lru_head;
}

// Destructor
BufferManager::~BufferManager() {
    // Clean up linked list
    while (lru_head->next != lru_tail)
        delete_from_mem(lru_head->next);
    delete lru_head;
    delete lru_tail;
}

// Get the id-th block in file
Block* BufferManager::get_block(const char* filename, int id) {
    string block_name = string(filename) + "." + to_string(id);

    if (node_map.find(block_name) != node_map.end())
    {
        // Set block as most recently used
        Block_node* node = node_map[block_name];
        node->remove_node();
        node->attach(lru_head);
        return node->block;
    }

    if (block_cnt == MAX_BLOCK_COUNT)
    {
        // Current block number full
        // Find the least recently used block
        Block_node* node = lru_tail->pre;
        while (node->block->pin)
            node = node->pre;

        delete_from_mem(node);
    }

    return load_from_file(filename, id);
}

// Remove all block with filename(used when delete file)
void BufferManager::remove_block(const char* filename) {
    Block_node* next_node;
    for (Block_node* node = lru_head->next; node != lru_tail; node = next_node)
    {
        next_node = node->next;
        if (node->block->file_name == filename)
            delete_from_mem(node, false);
    }
}

// Delete node and its block from memory
void BufferManager::delete_from_mem(Block_node* node, bool write) {
    Block* block = node->block;
    write_to_file(block->file_name.c_str(), block->id);
    node_map.erase(block->file_name + "." + to_string(block->id));
    delete node;
    delete block;
    block_cnt--;
}

// Load the id-th block from file
Block* BufferManager::load_from_file(const char* filename, int id) {
    // Load block from file
    Block* block = new Block(filename, id);
    FILE* file = fopen(("data/" + string(filename) + ".mdb").c_str(), "rb");

    fseek(file, id*MAX_BLOCK_SIZE, SEEK_SET);

    fread(block->data, MAX_BLOCK_SIZE, 1, file);

    fclose(file);

    // Add block to linked list
    Block_node* node = new Block_node(block);

    node->attach(lru_head);

    node_map[string(filename) + "." + to_string(id)] = node;

    block_cnt++;

    return block;
}

// Write the id-th block of file
void BufferManager::write_to_file(const char* filename, int id) {
    Block* block = node_map[string(filename) + "." + to_string(id)]->block;
    if (block->dirty == false)
        return;

    FILE* file = fopen(("data/" + string(filename) + ".mdb").c_str(), "rb+");
    fseek(file, id*MAX_BLOCK_SIZE, SEEK_SET);
    fwrite(block->data, MAX_BLOCK_SIZE, 1, file);
    fclose(file);
}