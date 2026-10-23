#include "file.h"
#include "critical_regions.h"

int fseek(FILE *f, long offset, int whence) {
    int ret;

    __begin_critical_region(stdio_access);
    ret = _fseek(f, offset, whence);
    __end_critical_region(stdio_access);
    return ret;
}

/**
 * Reposition a stream, adjusting for whatever is still buffered.
 *
 * A seek relative to the current position is first turned into an
 * absolute one, and a target that lands inside the buffer is satisfied
 * by moving the cursor alone.  Otherwise the underlying seek callback
 * is used.
 *
 * @return 0 on success, -1 on failure.
 */
int _fseek(FILE *f, long offset, int whence) {
    unsigned int state;
    long delta;
    long unbuffered;
    unsigned int buf_state;
    long base;
    long pos = offset;

    state = FILE_KIND(f);
    if (state != FILE_DISK || f->error != 0) {
        errno = EINVAL;
        return -1;
    }
    if (BUF_STATE(f) == BUF_WRITING && __flush_buffer(f, 0) != 0) {
        f->error = 1;
        f->buffer_length = 0;
        errno = EINVAL;
        return -1;
    }
    if (whence == SEEK_CUR) {
        unsigned int mode;
        unsigned char *p;

        whence = SEEK_SET;
        unbuffered = 0;
        mode = FILE_KIND(f);
        if ((mode != FILE_DISK && mode != FILE_CONSOLE) || f->error != 0) {
            errno = EINVAL;
            delta = -1;
        } else {
            buf_state = BUF_STATE(f);
            if (buf_state == BUF_NEUTRAL) {
                delta = f->position;
            } else {
                delta = f->buffer_position + (base = (long)((char *)f->buffer_ptr - (char *)(p = (unsigned char *)f->buffer)));
                if (buf_state >= BUF_REREADING) {
                    unbuffered = buf_state - BUF_READING;
                    delta -= unbuffered;
                }
                if (BINARY_IO(f) == 0) {
                    long i = base - unbuffered;

                    while (i != 0) {
                        if (*p++ == '\n') {
                            delta++;
                        }
                        i--;
                    }
                }
            }
        }
        pos += delta;
    }
    if (whence != SEEK_END && IO_MODE(f) != IO_READWRITE &&
        (BUF_STATE(f) == BUF_READING || BUF_STATE(f) == BUF_REREADING)) {
        if (pos >= f->position || pos < f->buffer_position) {
            f->buffer_state.io_state = BUF_NEUTRAL;
        } else {
            f->buffer_ptr = f->buffer + (pos - f->buffer_position);
            f->buffer_length = f->position - pos;
            f->buffer_state.io_state = BUF_READING;
        }
    } else {
        f->buffer_state.io_state = BUF_NEUTRAL;
    }
    if (BUF_STATE(f) == BUF_NEUTRAL) {
        if (f->position_fn != 0) {
            if (f->position_fn(f->handle, &pos, whence, f->idle_fn) != 0) {
                f->error = 1;
                f->buffer_length = 0;
                errno = EINVAL;
                return -1;
            }
        }
        f->eof = 0;
        f->position = pos;
        f->buffer_length = 0;
    }
    return 0;
}

long ftell(FILE *f) {
    long pos;
    long unbuffered;
    unsigned int state;
    long base;
    unsigned char *p;

    __begin_critical_region(stdio_access);
    unbuffered = 0;
    if ((FILE_KIND(f) != FILE_DISK && FILE_KIND(f) != FILE_CONSOLE) || f->error != 0) {
        pos = -1;
        errno = EINVAL;
    } else {
        state = BUF_STATE(f);
        if (state == BUF_NEUTRAL) {
            pos = f->position;
        } else {
            pos = f->buffer_position + (base = (long)(f->buffer_ptr - (char *)(p = (unsigned char *)f->buffer)));
            if (state >= BUF_REREADING) {
                unbuffered = state - BUF_READING;
                pos -= unbuffered;
            }
            if (BINARY_IO(f) == 0) {
                long i = base - unbuffered;

                while (i != 0) {
                    if (*p++ == '\n') {
                        pos++;
                    }
                    i--;
                }
            }
        }
    }
    __end_critical_region(stdio_access);
    return pos;
}
