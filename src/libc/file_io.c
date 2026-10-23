#include "file.h"
#include "alloc.h"

int fflush(FILE *f) {
    long pos;

    if (f == ((void *)0)) {
        return __flush_all();
    }
    if (f->error != 0 || FILE_KIND(f) == FILE_CLOSED) {
        return -1;
    }
    if (IO_MODE(f) == IO_READ) {
        return 0;
    }
    if (BUF_STATE(f) >= BUF_REREADING) {
        f->buffer_state.io_state = BUF_READING;
    }
    if (BUF_STATE(f) == BUF_READING) {
        f->buffer_length = 0;
    }
    if (BUF_STATE(f) != BUF_WRITING) {
        f->buffer_state.io_state = BUF_NEUTRAL;
        return 0;
    }
    /* Preserve the position before flushing a disk stream. */
    if (FILE_KIND(f) != FILE_DISK) {
        pos = 0;
    } else {
        pos = ftell(f);
    }
    if (__flush_buffer(f, ((void *)0)) != 0) {
        f->error = 1;
        f->buffer_length = 0;
        return -1;
    }
    f->buffer_state.io_state = BUF_NEUTRAL;
    f->position = pos;
    f->buffer_length = 0;
    return 0;
}

int fclose(FILE *f) {
    int ret;
    int close_ret;

    if (f == 0) return -1;
    if (FILE_KIND(f) == FILE_CLOSED) return 0;
    ret = fflush(f);
    close_ret = f->close_fn(f->handle);
    f->flags.file_kind = FILE_CLOSED;
    f->handle = 0;
    if (BUFFER_ALLOCATED(f)) free(f->buffer);
    return (ret != 0 || close_ret != 0) ? -1 : 0;
}
