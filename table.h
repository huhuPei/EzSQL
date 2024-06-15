#ifndef __EZ_SQL__
#define __EZ_SQL__

#include <stdint.h>
#include <stdbool.h>
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

#define TABLE_MAX_PAGES 100

typedef void*(*RowFunc)(uint32_t);
typedef struct {
    uint32_t num_rows;
    void* pages[TABLE_MAX_PAGES];
} Table;

void* locate_row(Table*, uint32_t);
void serialize_row(Row*, void*);
void deserialize_row(void*, Row*);
void print_row(Row*);
Table* new_table();
void free_table(Table*);
bool table_full(Table*);

#endif