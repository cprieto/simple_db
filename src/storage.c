#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "storage.h"

// Sizes required for allocation in memory
const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);

// Total size for a row block
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

// Location of each attribute in a block
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

// The page size is the same as the page size in many OSes (Linux, Windows)
const uint32_t PAGE_SIZE = 4096;

// Each page can keep up to 14 rows
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;

// We can keep up to 1400 rows in our memory db
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

// Pager related methods (for memory cache of pages from disk)
void* get_page(Pager* pager, uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d > %d\n", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_num] == NULL) {
        // Cache miss. Allocate memory and load from file.
        void* page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager->file_length / PAGE_SIZE;

        // We might save a partial page at the end of the file
        if (pager->file_length % PAGE_SIZE) {
            num_pages += 1;
        }

        if (page_num <= num_pages) {
            lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error reading file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }

        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

Pager* page_open(const char* filename) {
    int file_handler = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (file_handler == -1) {
        printf("Unable to open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(file_handler, 0, SEEK_END);

    Pager* pager = malloc(sizeof(Pager));
    pager->file_descriptor = file_handler;
    pager->file_length = file_length;

    // Initialize all the current pager pages
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }

    return pager;
}

void page_flush(Pager* pager, uint32_t page_num, uint32_t page_size) {
    if (pager->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    // Calculate where the page should be in disk
    uint32_t file_pos = page_num * PAGE_SIZE;
    off_t offset = lseek(pager->file_descriptor, file_pos, SEEK_SET);
    if (offset == -1) {
        printf("Error seeking on file: %d\n.", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t written = write(pager->file_descriptor, pager->pages[page_num], page_size);
    if (written == -1) {
        printf("Error writing page: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

void *row_slot(Table* table, uint32_t row_num) {
    // based in the row number, get the page where this will be stored
    // e.g. row 2 in page 0, row 34 in page 2
    uint32_t page_num = row_num / ROWS_PER_PAGE;

    // Inside the page, what is the row order in that page?
    // For example, row 32 is 6
    uint32_t row_offset = row_num % ROWS_PER_PAGE;

    // Convert that offset into bytes
    // For example, row at position 6 has offset of 1746 bytes
    uint32_t byte_offset = row_offset * ROW_SIZE;

    // a page is just a block of memory of size PAGE_SIZE
    void* page = get_page(table->pager, page_num);

    // page has the start address of that page
    // so the row location would be at that PLUS the offset.
    // this is a block of memory we can write the row there.
    return page + byte_offset;
}

Table *db_open(const char* filename) {
    Table* table = malloc(sizeof(Table));
    table->pager = page_open(filename);

    // Calculate the num of rows based in file length
    table->num_rows = table->pager->file_length / ROW_SIZE;

    return table;
}

void db_close(Table* table) {
    Pager* pager = table->pager;
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (!pager->pages[i]) continue;

        page_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages[page_num]) {
            uint32_t additional_row_size = ROW_SIZE * num_additional_rows;
            page_flush(pager, page_num, additional_row_size);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int result = close(pager->file_descriptor);
    if (result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void *page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }

    free(pager);
    free(table);
}
