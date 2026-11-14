#ifndef GS_LOG_H
#define GS_LOG_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

int GSlogInit(u32 size, u32 stampMode);
const char* GSlogGetLine(u32 line);
u32 GSlogGetLineCount(void);
void GSlogSetIP(u8* address, u16 port);
void GSlogWrite(const char* format, ...);
void GSlogWritef(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif
