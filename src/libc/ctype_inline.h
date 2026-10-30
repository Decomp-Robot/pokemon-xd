#ifndef MSL_CTYPE_INLINE_H
#define MSL_CTYPE_INLINE_H

#include "ctype.h"

#ifdef __cplusplus
extern "C" {
#endif

inline int isspace(int c) { return (unsigned char)__ctype_map[(unsigned char)c] & 0x06; }
inline int isdigit(int c) { return (unsigned char)__ctype_map[(unsigned char)c] & 0x10; }
inline int isalpha(int c) { return (unsigned char)__ctype_map[(unsigned char)c] & 0xC0; }
inline int toupper(int c) { return (c == -1 ? -1 : ((unsigned char *)__upper_map)[(unsigned char)c]); }

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
