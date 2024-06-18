#include "cursor.h"

#include <stdbool.h>
#include <stdlib.h>

#include "table.h"
#include "btree.h"

Cursor* table_start(Table* table) {
    Cursor* cursor = (Cursor*) malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_num = table->root_page_num;
    cursor->cell_num = 0;
    void* root_node = get_page(table->pager, table->root_page_num);
    cursor->end_of_table = *leaf_node_num_cells(root_node) == 0;
    return cursor;
}

Cursor* table_end(Table* table) {
    Cursor* cursor = (Cursor*) malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_num = table->root_page_num;
    void* root_node = get_page(table->pager, table->root_page_num);
    cursor->cell_num = *leaf_node_num_cells(root_node);
    cursor->end_of_table = true;
    return cursor;
}

void* cursor_value(Cursor* cursor) {
    void* page = get_page(cursor->table->pager, cursor->page_num);
    return leaf_node_value(page, cursor->cell_num);
}

void cursor_next(Cursor* cursor) {
    uint32_t page_num = cursor->page_num;
    void* node = get_page(cursor->table->pager, page_num);
    cursor->cell_num++;
    if (cursor->cell_num >= *leaf_node_num_cells(node))
        cursor->end_of_table = true;
}

void cursor_insert(Cursor* cursor, uint32_t key, Row* value) {
    void* node = get_page(cursor->table->pager, cursor->page_num);
    leaf_node_insert(node, cursor->cell_num, key, value);
}