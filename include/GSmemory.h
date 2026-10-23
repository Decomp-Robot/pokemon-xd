#ifndef GSMEMORY_H
#define GSMEMORY_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
void* GSmemAllocClr(u32 size);
void* GSmemAlloc(u32 size);
void GSmemFree(void* memory);
#ifdef __cplusplus
}
#endif
#endif
