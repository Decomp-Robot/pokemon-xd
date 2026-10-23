#ifndef MSL_CTYPE_INLINE_H
#define MSL_CTYPE_INLINE_H

#include "ctype.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Lower-case a character through the lower-case map. */
inline int __tolower(int c) {
    if (c == -1) {
        return -1;
    }
    return (unsigned char)__lower_map[c & 0xFF];
}

#ifdef __cplusplus
}
#endif

#endif /* MSL_CTYPE_INLINE_H */
