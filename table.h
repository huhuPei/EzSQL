#ifndef __EZ_SQL_TABLE__
#define __EZ_SQL_TABLE__

#include <stdint.h>
#include <stdbool.h>

#include "pager.h"

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

typedef void*(*RowFunc)(uint32_t);
typedef struct {
    uint32_t num_rows;
    Pager* pager;
} Table;

void* locate_row(Table*, uint32_t);
void serialize_row(Row*, void*);
void deserialize_row(void*, Row*);
void print_row(Row*);
Table* new_table();
void free_table(Table*);
Table* open_db(const char*);
void* close_db(Table*);
Pager* pager_open(const char*);
bool table_full(Table*);

#endif