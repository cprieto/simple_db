#ifndef BUILD_DB_H
#define BUILD_DB_H

#include "buffer.h"
#include "storage.h"

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT,
} StatementType;

typedef struct {
    StatementType type;
    Row insert_row;
} Statement;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG,
    PREPARE_NEGATIVE_ID,
} PrepareResult;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
    EXECUTE_ERROR,
} ExecuteResult;

/**
 * Returns a prepared result from a user entry
 * @return Prepared statement
 */
PrepareResult prepare_statement(InputBuffer*, Statement*);

/**
 * Executes a given statement
 */
ExecuteResult execute_statement(Statement*, Table*);

#endif // BUILD_DB_H
