#ifndef MSL_FILE_H
#define MSL_FILE_H

#include "../ext/dolphin_sdk/include/libc/stdio.h"

/* File kind, access mode and buffering are independent flag domains. */
#define FILE_KIND(f) ((unsigned int)((f)->flags.file_kind))
#define FILE_CLOSED 0
#define FILE_DISK 1
#define FILE_CONSOLE 2
#define FILE_UNAVAILABLE 3

#define IO_MODE(f) ((unsigned int)((f)->flags.io_mode))
#define IO_READ 1
#define IO_WRITE 2
#define IO_READWRITE 3
#define IO_APPEND 4

#define BUFFER_MODE(f) ((unsigned int)((f)->flags.buffer_mode))
#define BUFFER_UNBUFFERED 0
#define BUFFER_LINE_BUFFERED 1
#define BUFFER_FULLY_BUFFERED 2
#define BINARY_IO(f) ((unsigned int)((f)->flags.binary_io))

#define BUF_NEUTRAL 0
#define BUF_WRITING 1
#define BUF_READING 2
#define BUF_REREADING 3
#define BUF_STATE(f) ((unsigned int)((f)->buffer_state.io_state))
#define BUFFER_ALLOCATED(f) ((f)->buffer_state.free_buffer)

/* whence values, as passed to the seek callback. */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* Value the library leaves in errno for an invalid stream position. */
#define EINVAL 0x28

/* The head of the global FILE list. */
extern char __files[];

#define FILE_LIST ((FILE *)(void *)__files)

int __flush_buffer(FILE *f, unsigned int *written);
void __prep_buffer(FILE *f);
void __close_all(void);

extern void __stdio_atexit(void);
int __flush_all(void);
int _fseek(FILE *f, long offset, int whence);
unsigned long __fwrite(const void *buf, unsigned long size, unsigned long nmemb, FILE *f);

extern int errno;
extern int fwide(FILE *f, int mode);

#endif /* MSL_FILE_H */
