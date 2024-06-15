#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "table.h"

#define size_of_field(Struct, Field) sizeof(((Struct*)0)->Field)

const uint32_t ID_SIZE = size_of_field(Row, id);
const uint32_t USERNAME_SIZE = size_of_field(Row, username);
const uint32_t EMAIL_SIZE = size_of_field(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

void* locate_row(Table* table, uint32_t row_num){
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    if (!table->pages[page_num]) {
        table->pages[page_num] = malloc(PAGE_SIZE);
    }
    uint32_t offset = row_num % ROWS_PER_PAGE * ROW_SIZE;
    return table->pages[page_num] + offset;
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

Table* new_table() {
    Table* table = (Table*) malloc(sizeof(Table));
    table->num_rows = 0;
    for (int i = 0; i < TABLE_MAX_PAGES; i++) {
        table->pages[i] = NULL;
    }    
    return table;
}

void free_table(Table* table) {
    for (int i = 0; i < TABLE_MAX_PAGES; i++) {
        if (table->pages[i]) free(table->pages[i]);        
    }
    free(table);
}

bool table_full(Table* table) {
    return table->num_rows >= TABLE_MAX_ROWS;
}