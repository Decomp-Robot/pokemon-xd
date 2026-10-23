#ifndef GS_DVD_H
#define GS_DVD_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GSdvdFile GSdvdFile;
typedef void (*GSdvdReadCompletionFunc)(s32 result, GSdvdFile *file);

u8 GSdvdOpenTest(const char *fileName);
GSdvdFile *GSdvdOpen(const char *fileName);
GSdvdFile *GSdvdCreate(const char *fileName);
s32 GSdvdRead(GSdvdFile *file, void *buffer, u32 size, s32 offset);
s32 GSdvdWrite(GSdvdFile *file, void *buffer, u32 size, s32 offset);
s32 GSdvdClose(GSdvdFile *file);
u32 GSdvdGetLength(GSdvdFile *file);
u8 GSdvdReadAsync(GSdvdFile *file, void *buffer, u32 size, s32 offset,
                  GSdvdReadCompletionFunc completionFunc);
s32 GSdvdGetFileInfoStatus(GSdvdFile *file);

#ifdef __cplusplus
}
#endif

#endif
