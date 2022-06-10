#include <stdio.h>
#include <string.h>
#include "db.h"

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

void execute_statement(Statement* statement) {
    switch (statement->type) {
        case STATEMENT_INSERT:
            break;
        case STATEMENT_SELECT:
            printf("I would do a select now\n");
            break;
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
