#include "circular_buffer.h"
#include <stdlib.h>
#include <errno.h>

circular_buffer_t *new_circular_buffer(size_t capacity)
{
    circular_buffer_t *buffer = malloc(sizeof(circular_buffer_t));
    if (!buffer) {
        return NULL;
    }
    
    buffer->data = malloc(capacity * sizeof(buffer_value_t));
    if (!buffer->data) {
        free(buffer);
        return NULL;
    }
    
    buffer->capacity = capacity;
    buffer->read_index = 0;
    buffer->write_index = 0;
    buffer->count = 0;
    
    return buffer;
}

void delete_buffer(circular_buffer_t *buffer)
{
    if (buffer) {
        free(buffer->data);
        free(buffer);
    }
}

int16_t read(circular_buffer_t *buffer, buffer_value_t *value)
{
    if (!buffer || !value) {
        errno = EINVAL;
        return EXIT_FAILURE;
    }
    
    if (buffer->count == 0) {
        errno = ENODATA;
        return EXIT_FAILURE;
    }
    
    *value = buffer->data[buffer->read_index];
    buffer->read_index = (buffer->read_index + 1) % buffer->capacity;
    buffer->count--;
    
    return EXIT_SUCCESS;
}

int16_t write(circular_buffer_t *buffer, buffer_value_t value)
{
    if (!buffer) {
        errno = EINVAL;
        return EXIT_FAILURE;
    }
    
    if (buffer->count == buffer->capacity) {
        errno = ENOBUFS;
        return EXIT_FAILURE;
    }
    
    buffer->data[buffer->write_index] = value;
    buffer->write_index = (buffer->write_index + 1) % buffer->capacity;
    buffer->count++;
    
    return EXIT_SUCCESS;
}

int16_t overwrite(circular_buffer_t *buffer, buffer_value_t value)
{
    if (!buffer) {
        errno = EINVAL;
        return EXIT_FAILURE;
    }
    
    if (buffer->count == buffer->capacity) {
        // Buffer is full, overwrite the oldest value
        buffer->data[buffer->write_index] = value;
        buffer->write_index = (buffer->write_index + 1) % buffer->capacity;
        buffer->read_index = (buffer->read_index + 1) % buffer->capacity;
        // count stays the same since we're overwriting
    } else {
        // Buffer is not full, act like normal write
        buffer->data[buffer->write_index] = value;
        buffer->write_index = (buffer->write_index + 1) % buffer->capacity;
        buffer->count++;
    }
    
    return EXIT_SUCCESS;
}

void clear_buffer(circular_buffer_t *buffer)
{
    if (buffer) {
        buffer->read_index = 0;
        buffer->write_index = 0;
        buffer->count = 0;
    }
}
