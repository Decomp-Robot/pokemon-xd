#ifndef GS_DVD_H
#define GS_DVD_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 GSdvdOpenTest(const char *fileName);
void *GSdvdOpen(const char *fileName);
void *GSdvdCreate(const char *fileName);
s32 GSdvdRead(void *file, void *buffer, u32 size, s32 offset);
s32 GSdvdWrite(void *file, void *buffer, u32 size, s32 offset);
s32 GSdvdClose(void *file);

#ifdef __cplusplus
}
#endif

#endif
