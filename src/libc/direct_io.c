#include "file.h"
#include "critical_regions.h"

extern void *memcpy(void *dst, const void *src, unsigned long n);

extern void *__memrchr(const void *s, int c, unsigned long n);

/**
 * Write a block to a stream, filling and flushing the buffer as needed.
 *
 * On a line-buffered text stream the copy stops after the last newline
 * so the line reaches the device promptly.  An unbuffered stream is
 * handed to the write callback in one piece.
 *
 * @return Number of whole elements written.
 */
unsigned long __fwrite(const void *buf, unsigned long size, unsigned long nmemb,
                      FILE *f) {
    int buffered;
    const char *p;
    unsigned long left;
    unsigned long done;
    unsigned int written;
    const char *nl;
    int buffer_kind;

    if (fwide(f, 0) == 0) {
        fwide(f, -1);
    }
    left = size * nmemb;
    if (left == 0 || f->error != 0 || FILE_KIND(f) == FILE_CLOSED) {
        return 0;
    }
    if (FILE_KIND(f) == FILE_CONSOLE) {
        __stdio_atexit();
    }
    buffered = 1;
    buffer_kind = BINARY_IO(f) == 0 || BUFFER_MODE(f) == BUFFER_FULLY_BUFFERED;
    if (!buffer_kind) {
        if (BUFFER_MODE(f) != BUFFER_LINE_BUFFERED) {
            buffered = 0;
        }
    }
    if (BUF_STATE(f) == BUF_NEUTRAL && (IO_MODE(f) & IO_WRITE) != 0) {
        if ((IO_MODE(f) & IO_APPEND) != 0) {
            if (fseek(f, 0, SEEK_END) != 0) {
                return 0;
            }
        }
        f->buffer_state.io_state = BUF_WRITING;
        __prep_buffer(f);
    }
    if (BUF_STATE(f) != BUF_WRITING) {
        f->error = 1;
        f->buffer_length = 0;
        return 0;
    }
    p = (const char *)buf;
    done = 0;
    if (left != 0 && (f->buffer_ptr != f->buffer || buffered)) {
        f->buffer_length = f->buffer_size - (f->buffer_ptr - f->buffer);
        do {
            written = (unsigned int)f->buffer_length;
            nl = 0;
            if (written > left) {
                written = (unsigned int)left;
            }
            if (BUFFER_MODE(f) == BUFFER_LINE_BUFFERED && written != 0) {
                nl = (const char *)__memrchr(p, '\n', written);
                if (nl != 0) {
                    written = (unsigned int)(nl + 1 - p);
                }
            }
            if (written != 0) {
                memcpy(f->buffer_ptr, p, written);
                p += written;
                done += written;
                left -= written;
                f->buffer_ptr += written;
                f->buffer_length -= written;
            }
            if (f->buffer_length == 0 || nl != 0 || BUFFER_MODE(f) == BUFFER_UNBUFFERED) {
                if (__flush_buffer(f, ((void *)0)) != 0) {
                    f->error = 1;
                    left = 0;
                    f->buffer_length = 0;
                    break;
                }
            }
        } while (left != 0 && buffered);
    }
    if (left != 0 && !buffered) {
        char *saved_buffer = f->buffer;
        long saved_size = f->buffer_size;

        f->buffer = (char *)(void *)p;
        f->buffer_size = (long)left;
        f->buffer_ptr = (char *)(void *)(p + left);
        if (__flush_buffer(f, &written) != 0) {
            f->error = 1;
            f->buffer_length = 0;
        }
        done += written;
        f->buffer = saved_buffer;
        f->buffer_size = saved_size;
        __prep_buffer(f);
        f->buffer_length = 0;
    }
    if (BUFFER_MODE(f) != BUFFER_FULLY_BUFFERED) {
        f->buffer_length = 0;
    }
    return (done + size - 1) / size;
}

unsigned long fwrite(const void *buf, unsigned long size, unsigned long nmemb,
                     FILE *f) {
    unsigned long ret;

    __begin_critical_region(stdio_access);
    ret = __fwrite(buf, size, nmemb, f);
    __end_critical_region(stdio_access);
    return ret;
}
