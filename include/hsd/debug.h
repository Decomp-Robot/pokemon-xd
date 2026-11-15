#ifndef HSD_DEBUG_H
#define HSD_DEBUG_H

#include <hsd/assert.h>

#ifdef __cplusplus
extern "C" {
#endif
void HSD_Panic(const char *file, int line, const char *message);
#ifdef __cplusplus
}
#endif

#endif
