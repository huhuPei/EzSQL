#ifndef __EZ_SQL_PAGER__
#define __EZ_SQL_PAGER_

#include <stdint.h>

#define PAGE_SIZE 4096
#define MAX_PAGES 100
typedef struct {
    int fd;
    uint32_t file_size;
    void* pages[MAX_PAGES];
} Pager;

Pager* pager_open(const char*);
void* get_page(Pager*, uint32_t);
void pager_flush(Pager*, uint32_t, uint32_t);

#endif