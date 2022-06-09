#ifndef BUILD_DB_H
#define BUILD_DB_H

#include <stdlib.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT,
} StatementType;

typedef struct {
    u_int32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct {
    StatementType type;
    Row insert_row;
} Statement;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR,
} PrepareResult;

/**
 * Creates a new empty InputBuffer
 * @return new empty InputBuffer
 */
InputBuffer* new_input_buffer();

/**
 * Read the stdin into an InputBuffer
 */
void read_input(InputBuffer*);

/**
 * Close an existing InputBuffer
 */
void close_input_buffer(InputBuffer*);

/**
 * Returns a prepared result from a user entry
 * @return Prepared statement
 */
PrepareResult prepare_statement(InputBuffer*, Statement*);

/**
 * Executes a given statement
 */
void execute_statement(Statement*);

#endif //BUILD_DB_H
