#include <stdio.h>
#include <string.h>
#include "db.h"

// Insert operation
PrepareResult prepare_insert(InputBuffer* buffer, Statement* statement) {
    strtok(buffer->buffer, " "); // First is the keyword
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if (!id_string || !username || !email) {
        return PREPARE_SYNTAX_ERROR;
    }

    statement->type = STATEMENT_INSERT;

    int id = atoi(id_string);
    if (id < 0) return PREPARE_NEGATIVE_ID;

    if (strlen(username) > COLUMN_USERNAME_SIZE || strlen(email) > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    statement->insert_row.id = id;
    strcpy(statement->insert_row.username, username);
    strcpy(statement->insert_row.email, email);

    return PREPARE_SUCCESS;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
    // Oops! too many rows already!
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }

    // Get a cursor at the end of the table
    Cursor* cursor = table_end(table);

    // Save the row in that piece of memory (slot)
    serialize_row(&statement->insert_row, cursor_value(cursor));

    // Ok, this is how many rows we have now
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

// Select operation
ExecuteResult execute_select(Table* table) {
    Cursor* cursor = table_start(table);
    Row row;

    while (!cursor->end_of_table) {
        deserialize_row(cursor_value(cursor), &row);
        printf("(%d, %s, %s)\n", row.id, row.username, row.email);
        cursor_next(cursor);
    }

    free(cursor);

    return EXECUTE_SUCCESS;
}

// Execution operations
ExecuteResult execute_statement(Statement* statement, Table* table) {
    switch (statement->type) {
        case STATEMENT_INSERT:
            return execute_insert(statement, table);
        case STATEMENT_SELECT:
            return execute_select(table);
        default:
            return EXECUTE_ERROR;
    }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        return prepare_insert(input_buffer, statement);
    }

    if (strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}