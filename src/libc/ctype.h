#ifndef MSL_CTYPE_H
#define MSL_CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/* MSL character classification and case-conversion maps, defined in ctype.c. */
extern char __ctype_map[256];
extern signed char __lower_map[256];
extern signed char __upper_map[256];

#ifdef __cplusplus
}
#endif

#endif /* MSL_CTYPE_H */
