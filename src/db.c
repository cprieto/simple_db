#include <stdio.h>
#include <string.h>
#include "db.h"

// Sizes required for allocation in memory
const u_int32_t ID_SIZE = size_of_attribute(Row, id);
const u_int32_t USERNAME_SIZE = size_of_attribute(Row, username);
const u_int32_t EMAIL_SIZE = size_of_attribute(Row, email);

// Total size for a row block
const u_int32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

// Location of each attribute in a block
const u_int32_t ID_OFFSET = 0;
const u_int32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const u_int32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

// The page size is the same as the page size in many OSes (Linux, Windows)
const u_int32_t PAGE_SIZE = 4096;

// Each page can keep up to 14 rows
const u_int32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;

// We can keep up to 1400 rows in our memory db
const u_int32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

InputBuffer *new_input_buffer() {
    InputBuffer *input_buffer = (InputBuffer *) malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void read_input(InputBuffer *input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if (bytes_read <= 0) {
        printf("Error reading input");
        exit(EXIT_FAILURE);
    }

    // getline includes carriage return, end with \0
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        int read = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->insert_row.id), statement->insert_row.username, statement->insert_row.email);
        if (read < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_statement(Statement* statement, Table* table) {
    switch (statement->type) {
        case STATEMENT_INSERT:
            return execute_insert(statement, table);
        case STATEMENT_SELECT:
            return execute_select(table);
    }
}

void serialize_row(Row* row, void* destination) {
    memcpy(destination + ID_OFFSET, &(row->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(row->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(row->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* row) {
    memcpy(&(row->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(row->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(row->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void *row_slot(Table* table, u_int32_t row_num) {
    // based in the row number, get the page where this will be stored
    // e.g. row 2 in page 0, row 34 in page 2
    u_int32_t page_num = row_num / ROWS_PER_PAGE;

    // a page is just a block of memory of size PAGE_SIZE
    void* page = table->pages[page_num];

    if (page == NULL) {
        // Allocate memory if page is not existent!
        page = malloc(PAGE_SIZE);
        table->pages[page_num] = page;
    }

    // Inside the page, what is the row order in that page?
    // For example, row 32 is 6
    u_int32_t row_offset = row_num % ROWS_PER_PAGE;

    // Convert that offset into bytes
    // For example, row at position 6 has offset of 1746 bytes
    u_int32_t byte_offset = row_offset * ROW_SIZE;

    // page has the start address of that page
    // so the row location would be at that PLUS the offset.
    // this is a block of memory so we can write the row there.
    return page + byte_offset;
}

Table *new_table() {
    Table* table = malloc(sizeof(Table));
    table->num_rows = 0;

    for (u_int32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        // We set unused pages
        table->pages[i] = NULL;
    }

    return table;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
    // Oops! too many rows already!
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }

    // Get the piece of memory where I need to save the row
    void* slot = row_slot(table, table->num_rows);

    // Save the row in that piece of memory (slot)
    serialize_row(&(statement->insert_row), slot);

    // Ok, this is how many rows we have now
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Table* table) {
    Row row;
    for (u_int32_t i = 0; i < table->num_rows; i++) {
        void* slot = row_slot(table, i);
        deserialize_row(slot, &row);
        printf("(%d, %s, %s)\n", row.id, row.username, row.email);
    }

    return EXECUTE_SUCCESS;
}

void free_table(Table* table) {
    for (u_int32_t i = 0; i < table->num_rows; ++i) {
        free(table->pages[i]);
    }
    free(table);
}
