#ifndef BUILD_BUFFER_H
#define BUILD_BUFFER_H

typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer();
void read_input(InputBuffer*);
void close_input_buffer(InputBuffer*);

#endif //BUILD_BUFFER_H