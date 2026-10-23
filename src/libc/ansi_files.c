#include "file.h"
#include "alloc.h"
#include "critical_regions.h"

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
