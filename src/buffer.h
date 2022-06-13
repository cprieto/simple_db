#ifndef BUILD_BUFFER_H
#define BUILD_BUFFER_H

#include <stdlib.h>
#include <sys/types.h>

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

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

#endif //BUILD_BUFFER_H
