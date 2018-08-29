#pragma once

#include "../Global.h"

// Linked list node of data block. Used for LRU
struct Block_node {
    Block* block;
    Block_node* pre;
    Block_node* next;
    
    // C'tor
    Block_node(Block* _block): block(_block) {}

    // D'tor
    ~Block_node() { remove_node(); }
    // Add to position after node_
    void attach(Block_node* node_) {
        pre = node_; next = node_->next;
        node_->next->pre = this; node_->next = this;
    }

    // Remove from linked list
    void remove_node() { pre->next = next; next->pre = pre;}
};

class BufferManager {
public:
     // Constructor
    BufferManager();

    // Destructor
    ~BufferManager();

    // Get the id-th block in file
    Block* get_block(const char* filename, int id);

    // Remove all block with filename(used when delete file)
    void remove_block(const char* filename);

private:

    // Current block number
    int block_cnt;

    // Dummy head and tail for linked list
    Block_node* lru_head;
    Block_node* lru_tail;

    // Block node map
    unordered_map<string, Block_node*> node_map;

    // Delete node and its block from memory
    void delete_from_mem(Block_node* node, bool write = true);

    // Load the id-th block from file
    Block* load_from_file(const char* filename, int id);

    // Write the id-th block of file
    void write_to_file(const char* filename, int id);
};