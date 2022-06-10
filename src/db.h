#include <stdlib.h>

#ifndef BUILD_DB_H
#define BUILD_DB_H

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

// We will keep up to 400Kb of data
#define TABLE_MAX_PAGES 100

// Sizes required for allocation in memory
extern const u_int32_t ID_SIZE;
extern const u_int32_t USERNAME_SIZE;
extern const u_int32_t EMAIL_SIZE;

// Total size for a row block
extern const u_int32_t ROW_SIZE;

// Location of each attribute in a block
extern const u_int32_t ID_OFFSET;
extern const u_int32_t USERNAME_OFFSET;
extern const u_int32_t EMAIL_OFFSET;

// The page size is the same as the page size in many OSes (Linux, Windows)
extern const u_int32_t PAGE_SIZE;

// Each page can keep up to 14 rows
extern const u_int32_t ROWS_PER_PAGE;

// We can keep up to 1400 rows in our memory db
extern const u_int32_t TABLE_MAX_ROWS;

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

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
} ExecuteResult;

// This is the book with all pages in the system
typedef struct {
    u_int32_t num_rows;
    void* pages[TABLE_MAX_PAGES];
} Table;

/**
 * Serialize a row in a memory block
 */
void serialize_row(Row*, void*);

/**
 * Deserialize a row from a memory block
 */
void deserialize_row(void*, Row*);

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
void* row_slot(Table*, u_int32_t);

/**
 * Creates a new empty page table
 * @return a new page table with null pages
 */
Table* new_table();

/**
 * Insert a row into a page table
 * @return What happened with the insert
 */
ExecuteResult execute_insert(Statement*, Table*);

/**
 * Selects and display all the rows in the database
 * @return What happened with the select
 */
ExecuteResult execute_select(Table*);

/**
 * Destroys an existing page table
 */
void free_table(Table*);

#endif // BUILD_DB_H