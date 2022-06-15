#ifndef BUILD_STORAGE_H
#define BUILD_STORAGE_H

#include <stdint.h>
#include <stdbool.h>

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
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

typedef struct {
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
} Pager;

// This is the book with all pages in the system
typedef struct {
    uint32_t num_rows;
    Pager* pager;
} Table;

// Represents a position or cursor in a table
typedef struct {
    Table* table;
    // current row number
    uint32_t row_num;

    bool end_of_table;
} Cursor;

/**
 * Block of memory to write the row
 * This is a block in a given page based in the row num.
 * @return a pointer where the row is located.
 */
void *cursor_value(Cursor *cursor);

/**
 * Moves a cursor to the next element
 * @param cursor
 */
void cursor_next(Cursor *cursor);

/**
 * Returns a cursor at the beginning of the table
 * @param table
 * @return cursor
 */
Cursor* table_start(Table* table);

/**
 * Returns a cursor at the end of the table
 * @param table
 * @return cursor
 */
Cursor* table_end(Table* table);

/**
 * Opens a database file
 * If the file does not exist, it will create it
 * @return A table with potential cache entries
 */
Table* db_open(const char*);

/**
 * Closes an existing database file
 * it will flush data from the table to the database file
 */
void db_close(Table*);

/**
 * Serializes a row in the correct memory format
 */
void serialize_row(Row* row, void* destination);

/**
 * Deserialize a memory block into a row
 */
void deserialize_row(void* source, Row* row);

#endif //BUILD_STORAGE_H
