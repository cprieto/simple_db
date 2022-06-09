#ifndef BUILD_STATEMENT_H
#define BUILD_STATEMENT_H

#include "buffer.h"

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT,
} StatementType;

typedef struct {
    StatementType type;
} Statement;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
} PrepareResult;

PrepareResult prepare_statement(InputBuffer*, Statement*);

void execute_statement(Statement*);

#endif //BUILD_STATEMENT_H
