#include "file.h"

int __flush_buffer(FILE *f, unsigned int *written) {
    unsigned int count = (unsigned int)(f->buffer_ptr - f->buffer);
    int ret;

    if (count != 0) {
        f->buffer_length = (long)count;
        ret = (int)f->write_fn(f->handle, f->buffer, &f->buffer_length, f->idle_fn);
        if (written != ((void *)0)) {
            *written = (unsigned int)f->buffer_length;
        }
        if (ret != 0) {
            return ret;
        }
        f->position += f->buffer_length;
    }
    f->buffer_ptr = f->buffer;
    f->buffer_length = f->buffer_size;
    f->buffer_length = f->buffer_length - (f->position & f->buffer_alignment);
    f->buffer_position = f->position;
    return 0;
}

void __prep_buffer(FILE *f) {
    f->buffer_ptr = f->buffer;
    f->buffer_length = f->buffer_size;
    f->buffer_length = f->buffer_length - (f->position & f->buffer_alignment);
    f->buffer_position = f->position;
}

