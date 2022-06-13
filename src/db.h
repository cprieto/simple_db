#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

#ifndef BUILD_DB_H
#define BUILD_DB_H

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

// We will keep up to 400Kb of data
#define TABLE_MAX_PAGES 100

// Sizes required for allocation in memory
extern const uint32_t ID_SIZE;
extern const uint32_t USERNAME_SIZE;
extern const uint32_t EMAIL_SIZE;

// Total size for a row block
extern const uint32_t ROW_SIZE;

// Location of each attribute in a block
extern const uint32_t ID_OFFSET;
extern const uint32_t USERNAME_OFFSET;
extern const uint32_t EMAIL_OFFSET;

// The page size is the same as the page size in many OSes (Linux, Windows)
extern const uint32_t PAGE_SIZE;

// Each page can keep up to 14 rows
extern const uint32_t ROWS_PER_PAGE;

// We can keep up to 1400 rows in our memory db
extern const uint32_t TABLE_MAX_ROWS;

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
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

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

// This is the book with all pages in the system
typedef struct {
    uint32_t num_rows;
    void* pages[TABLE_MAX_PAGES];
} Table;

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
ExecuteResult execute_statement(Statement*, Table*);

/*
 * Block of memory to write the row
 * This is a block in a given page based in the row num.
 * @return a pointer where the row is located.
 */
void* row_slot(Table*, uint32_t);

/**
 * Creates a new empty page table
 * @return a new page table with null pages
 */
Table* new_table();

/**
 * Destroys an existing page table
 */
void free_table(Table*);

#endif // BUILD_DB_H
