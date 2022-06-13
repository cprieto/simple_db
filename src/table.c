//#include <stdlib.h>
//#include <stdint.h>
//#include <sys/types.h>
#include <stdlib.h>
#include "table.h"

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

void *row_slot(Table* table, uint32_t row_num) {
    // based in the row number, get the page where this will be stored
    // e.g. row 2 in page 0, row 34 in page 2
    uint32_t page_num = row_num / ROWS_PER_PAGE;

    // a page is just a block of memory of size PAGE_SIZE
    void* page = table->pages[page_num];

    if (page == NULL) {
        // Allocate memory if page is not existent!
        page = malloc(PAGE_SIZE);
        table->pages[page_num] = page;
    }

    // Inside the page, what is the row order in that page?
    // For example, row 32 is 6
    uint32_t row_offset = row_num % ROWS_PER_PAGE;

    // Convert that offset into bytes
    // For example, row at position 6 has offset of 1746 bytes
    uint32_t byte_offset = row_offset * ROW_SIZE;

    // page has the start address of that page
    // so the row location would be at that PLUS the offset.
    // this is a block of memory so we can write the row there.
    return page + byte_offset;
}

Table *new_table() {
    Table* table = malloc(sizeof(Table));
    table->num_rows = 0;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        // We set unused pages
        table->pages[i] = NULL;
    }

    return table;
}

void free_table(Table* table) {
    for (uint32_t i = 0; table->pages[i]; i++) {
        free(table->pages[i]);
    }
    free(table);
}