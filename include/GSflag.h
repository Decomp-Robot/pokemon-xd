#ifndef GS_FLAG_H
#define GS_FLAG_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
s32 GSflagTest(s32 flag);
u32 GSflagGet(s32 flag);
void GSflagSet(s32 flag, u32 value);
void GSflagOn(s32 flag);
void GSflagOff(s32 flag);
#ifdef __cplusplus
}
#endif
#endif
