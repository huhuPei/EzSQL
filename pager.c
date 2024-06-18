#include "pager.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

Pager* pager_open(const char* filename) {
    int fd = open(filename,
                O_RDWR |      // Read/Write mode
                    O_CREAT,  // Create file if it does not exist
                S_IWUSR |     // User write permission
                    S_IRUSR   // User read permission
                );

    if (fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    printf("file_size = %ld\n", file_size);
    Pager* pager = (Pager*) malloc(sizeof(Pager));
    pager->fd = fd;    
    pager->file_size = file_size;
    pager->num_pages = file_size / PAGE_SIZE;
    
    if (file_size % PAGE_SIZE != 0) {
        printf("Db file is not a whole number of pages. Corrupt file.\n");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < MAX_PAGES; i++) {
        pager->pages[i] = NULL; 
    }
    return pager;
}

void* get_page(Pager* pager, uint32_t page_num) {
    if (page_num > MAX_PAGES - 1) {
        printf("Tried to fetch page number out of bounds. %d > %d\n", 
            page_num, MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_num] == NULL) {
        // Cache miss. Allocate memory and load from file.
        void* page = malloc(PAGE_SIZE);
        uint32_t file_pages = pager->file_size / PAGE_SIZE;

        // We might save a partial page at the end of the file
        if (pager->file_size % PAGE_SIZE) {
            file_pages += 1;
        }

        if (page_num < file_pages) {
            lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->fd, page, PAGE_SIZE);
            if (bytes_read == -1) {       
                printf("Error reading file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page;
        if (page_num >= pager->num_pages) pager->num_pages = page_num + 1;
    }
    return pager->pages[page_num];
}

void pager_flush(Pager* pager, uint32_t page_num) {
    if (pager->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_write = write(pager->fd, pager->pages[page_num], PAGE_SIZE);
    
    if (bytes_write == -1) {
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}