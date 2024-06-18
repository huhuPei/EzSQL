#ifndef __EZ_SQL_CURSOR__
#define __EZ_SQL_CURSOR__

#include <stdint.h>
#include <stdbool.h>

#include "table.h"

typedef struct {
    Table* table;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table;  // The next position of the last element
} Cursor;

Cursor* table_start(Table*);
Cursor* table_end(Table*);
void* cursor_value(Cursor*);
void cursor_next(Cursor*);
void cursor_insert(Cursor*, uint32_t key, Row* value);
#endif