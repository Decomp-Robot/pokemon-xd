#include "file.h"
#include "alloc.h"
#include "critical_regions.h"

/* Number of bytes in each of the standard stream buffers. */
#define STDIO_BUFFER_SIZE 256

/* Console hooks the standard streams are wired to. */
extern int __read_console(int handle, char *buffer, unsigned long *count,
                          MWIdleProc idle);
extern long __write_console(int handle, char *buffer, unsigned long *count,
                            MWIdleProc idle);
extern int __close_console(int handle);

/*
 * The streams the library opens on startup: stdin, stdout and stderr are
 * console streams, each with a buffer of its own.  The fourth record only
 * terminates the list of open streams.
 */
unsigned char stdin_buff[STDIO_BUFFER_SIZE];
unsigned char stdout_buff[STDIO_BUFFER_SIZE];
unsigned char stderr_buff[STDIO_BUFFER_SIZE];

FILE __files[4] = {
    { 0,
      { IO_READ, BUFFER_LINE_BUFFERED, FILE_CONSOLE, 0, 0 },
      { 0, 0 },
      { 0, 0 },
      0,
      0,
      0,
      0,
      { 0, 0, 0, 0, 0, 0, 0, 0 },
      0,
      (char *)stdin_buff,
      STDIO_BUFFER_SIZE,
      (char *)stdin_buff,
      0,
      0,
      0,
      0,
      0,
      __read_console,
      __write_console,
      __close_console,
      0,
      &__files[1] },
    { 1,
      { IO_WRITE, BUFFER_LINE_BUFFERED, FILE_CONSOLE, 0, 0 },
      { 0, 0 },
      { 0, 0 },
      0,
      0,
      0,
      0,
      { 0, 0, 0, 0, 0, 0, 0, 0 },
      0,
      (char *)stdout_buff,
      STDIO_BUFFER_SIZE,
      (char *)stdout_buff,
      0,
      0,
      0,
      0,
      0,
      __read_console,
      __write_console,
      __close_console,
      0,
      &__files[2] },
    { 2,
      { IO_WRITE, BUFFER_UNBUFFERED, FILE_CONSOLE, 0, 0 },
      { 0, 0 },
      { 0, 0 },
      0,
      0,
      0,
      0,
      { 0, 0, 0, 0, 0, 0, 0, 0 },
      0,
      (char *)stderr_buff,
      STDIO_BUFFER_SIZE,
      (char *)stderr_buff,
      0,
      0,
      0,
      0,
      0,
      __read_console,
      __write_console,
      __close_console,
      0,
      &__files[3] },
    { 0 }
};

int __flush_all(void) {
    int ret = 0;
    FILE *f = FILE_LIST;

    while (f != ((void *)0)) {
        if (FILE_KIND(f) != FILE_CLOSED) {
            if (fflush(f) != 0) {
                ret = -1;
            }
        }
        f = f->next_file;
    }
    return ret;
}

void __close_all(void) {
    FILE *f = FILE_LIST;

    __begin_critical_region(stdio_access);
    while (f != ((void *)0)) {
        FILE *stream;

        if (FILE_KIND(f) != FILE_CLOSED) {
            fclose(f);
        }
        stream = f;
        f = stream->next_file;
        if (stream->is_dynamically_allocated != 0) {
            free(stream);
        } else {
            stream->flags.file_kind = FILE_UNAVAILABLE;
            if (f != ((void *)0) && f->is_dynamically_allocated != 0) {
                stream->next_file = ((void *)0);
            }
        }
    }
    __end_critical_region(stdio_access);
}
