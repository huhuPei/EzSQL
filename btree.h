#include "pager.h"

#include <stdbool.h>

#include "table.h"

typedef enum { 
    NODE_INTERNAL, 
    NODE_LEAF
} NodeType;

typedef struct {
    void* k;
    uint32_t value_size;
} Cell;

typedef struct {
    Pager* pager;
    uint32_t row_size;
} BTree;

uint32_t* leaf_node_num_cells(void*);
void* leaf_node_cell(void*, uint32_t);
uint32_t* leaf_node_key(void*, uint32_t);
void* leaf_node_value(void*, uint32_t);
void initialize_leaf_node(void*);
void leaf_node_insert(void*, uint32_t, uint32_t, Row*);
bool leaf_node_full(void* node);
void print_leaf_node(void* node);