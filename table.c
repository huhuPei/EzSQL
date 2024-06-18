#include "table.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "btree.h"
#include "pager.h"

#define size_of_field(Struct, Field) sizeof(((Struct*)0)->Field)
const uint32_t ID_SIZE = size_of_field(Row, id);
const uint32_t USERNAME_SIZE = size_of_field(Row, username);
const uint32_t EMAIL_SIZE = size_of_field(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

void* locate_row(Table* table, uint32_t page_num, uint32_t cell_num){
    void* page = get_page(table->pager, page_num);
    return leaf_node_value(page, cell_num);
}

void serialize_row(Row* row, void* dest) {
    memcpy(dest + ID_OFFSET, &(row->id), ID_SIZE);
    memcpy(dest + USERNAME_OFFSET, &(row->username), USERNAME_SIZE);
    memcpy(dest + EMAIL_OFFSET, &(row->email), EMAIL_SIZE);    
}

void deserialize_row(void* source, Row* row) {
    memcpy(&(row->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(row->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(row->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void print_row(Row* row) {
  printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

// Table* new_table() {
//     Table* table = (Table*) malloc(sizeof(Table));
//     table->num_rows = 0;
//     for (int i = 0; i < MAX_PAGES; i++) {
//         table->pager->pages[i] = NULL;
//     }    
//     return table;
// }

void free_table(Table* table) {
    for (int i = 0; i < MAX_PAGES; i++) {
        if (table->pager->pages[i]) free(table->pager->pages[i]);        
    }
    free(table);
}

bool table_full(Table* table) {
    void* node = get_page(table->pager, table->root_page_num);
    return leaf_node_full(node);
}

Table* open_db(const char* filename) {
    Pager* pager = pager_open(filename);
    Table* table = (Table*) malloc(sizeof(Table));
    table->pager = pager;
    table->root_page_num = 0;
    if (pager->num_pages == 0) {
    // New database file. Initialize page 0 as leaf node.
        void* root_node = get_page(pager, 0);
        initialize_leaf_node(root_node);
    }
    return table;
}

void* close_db(Table* table) {
    Pager* pager = table->pager;
    for (uint32_t i = 0; i < pager->num_pages; i++) {
        if (pager->pages[i]) {
            pager_flush(pager, i);
            free(pager->pages[i]);
            pager->pages[i] = NULL;
        } 
    }

    if (close(pager->fd)) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < MAX_PAGES; i++) {
       void* page = pager->pages[i];
       if (page) {
           free(page);
           pager->pages[i] = NULL;
       }
    }
    free(pager);
    free(table);
}