#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdarg.h>

/* ------------------------------------------------------------------ */
/* Buffered stream bookkeeping                                         */
/* ------------------------------------------------------------------ */

/*
 * The library's FILE.  Only the fields the buffered layer touches are
 * named; the open routines own the rest of the object.
 */
typedef void (*MWIdleProc)(void);

typedef struct FILE {
    int handle;              /* 0x00 OS file descriptor */
    struct {
        unsigned short        : 2;           /* 0x04 bits 6-7 */
        unsigned short io_mode : 3;        /* 0x04 bits 3-5 */
        unsigned short buffer_mode : 2;     /* 0x04 bits 1-2 */
        unsigned short file_kind : 3;             /* 0x04 bit 0, 0x05 bits 6-7 */
        unsigned short orientation : 2;      /* 0x05 bits 4-5 */
        unsigned short binary_io : 1;             /* 0x05 bit 3 */
        unsigned short        : 3;           /* 0x05 bits 0-2 */
    } flags;                /* 0x04 */
    unsigned char pad_06[2];
    /*
     * The byte at 0x08 packs the state of the buffer and the flag that
     * says the library owns the buffer and must release it.
     */
    struct {
        unsigned char io_state : 3;       /* 0x08 bits 5-7 */
        unsigned char free_buffer : 1; /* 0x08 bit 4 */
        unsigned char : 4;             /* 0x08 bits 0-3 */
    } buffer_state;          /* 0x08 */
    unsigned char eof; /* 0x09 */
    unsigned char error;      /* 0x0a sticky I/O error */
    unsigned char pad_b;
    unsigned char is_dynamically_allocated;  /* 0x0c FILE object was malloc'd */
    unsigned char pad_f[0x18 - 0x10];
    unsigned long position;    /* 0x18 absolute position of buffer start */
    char *buffer;            /* 0x1c */
    long buffer_size;        /* 0x20 */
    char *buffer_ptr;            /* 0x24 */
    unsigned long buffer_length;     /* 0x28 */
    long buffer_alignment;      /* 0x2c buffer_size - 1 */
    unsigned long saved_buffer_length;
    unsigned long buffer_position; /* 0x34 */
    long (*position_fn)(int handle, long *position, int whence, MWIdleProc idle); /* 0x38 */
    long (*read_fn)(int handle, char *buf, unsigned long *count, MWIdleProc idle);
    long (*write_fn)(int handle, char *buf, unsigned long *buffer_length, MWIdleProc idle); /* 0x40 */
    int (*close_fn)(int handle);                                          /* 0x44 */
    MWIdleProc idle_fn;         /* 0x48 */
    struct FILE *next_file;     /* 0x4c */
} FILE;

int fflush(FILE *stream);
int fclose(FILE *stream);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
unsigned long fwrite(const void *buffer, unsigned long size, unsigned long count, FILE *stream);

int puts(const char *s);
int printf(const char *, ...);
int sprintf(char *s, const char *format, ...);
int vprintf(const char *format, va_list arg);
int vsprintf(char *s, const char *format, va_list arg);

#endif
