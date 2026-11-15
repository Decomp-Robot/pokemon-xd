#ifndef GS_DVD_H
#define GS_DVD_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GSdvdFile GSdvdFile;
typedef void (*GSdvdReadCompletionFunc)(s32 result, GSdvdFile *file);

u8 GSdvdInit(u32 maxFiles);
void GSdvdSetGCClient(const char *address, const char *netmask, const char *gateway, u8 apply);
void GSdvdSetReportServer(const char *address, u16 port, u8 apply);
void GSdvdSetDVDServer(const char *address, u16 port);
void GSdvdSetErrorMsg(u32 coverOpen, u32 noDisc, u32 wrongDisc, u32 retry, u32 fatalError);
s32 GSdvdGetDriveStatus(void);
u8 GSdvdSeekAsync(GSdvdFile *file, s32 offset, GSdvdReadCompletionFunc completion);
u8 GSdvdOpenTest(const char *fileName);
GSdvdFile *GSdvdOpen(const char *fileName);
GSdvdFile *GSdvdCreate(const char *fileName);
s32 GSdvdRead(GSdvdFile *file, void *buffer, u32 size, s32 offset);
s32 GSdvdWrite(GSdvdFile *file, void *buffer, u32 size, s32 offset);
s32 GSdvdRemove(const char *fileName);
s32 GSdvdClose(GSdvdFile *file);
u32 GSdvdGetLength(GSdvdFile *file);
u8 GSdvdReadAsync(GSdvdFile *file, void *buffer, u32 size, s32 offset,
                  GSdvdReadCompletionFunc completionFunc);
s32 GSdvdGetFileInfoStatus(GSdvdFile *file);
u8 GSdvdFstRefresh(void);

#ifdef __cplusplus
}
#endif

#endif
