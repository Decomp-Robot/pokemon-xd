#include "file.h"

/* Stream orientation encodings held in FILE.flags.orientation. */
#define STREAM_UNORIENTED 0
#define STREAM_BYTE_ORIENTED 1
#define STREAM_WIDE_ORIENTED 2

int fwide(FILE *stream, int mode) {
    int orientation;

    if (stream == (FILE *)0 || FILE_KIND(stream) == FILE_CLOSED) {
        return 0;
    }

    orientation = stream->flags.orientation;

    switch (orientation) {
    case STREAM_UNORIENTED:
        if (mode > 0) {
            stream->flags.orientation = STREAM_WIDE_ORIENTED;
        } else if (mode < 0) {
            stream->flags.orientation = STREAM_BYTE_ORIENTED;
        }
        return mode;
    case STREAM_WIDE_ORIENTED:
        return 1;
    case STREAM_BYTE_ORIENTED:
        return -1;
    }
}
