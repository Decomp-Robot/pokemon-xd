#ifndef GSAPI_GSFLAG_H
#define GSAPI_GSFLAG_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
u8 GSflagTest(s32 flag);
u32 GSflagGet(s32 flag);
void GSflagSet(s32 flag, u32 value);
void GSflagOn(s32 flag);
void GSflagOff(s32 flag);
#ifdef __cplusplus
}
#endif
#endif
