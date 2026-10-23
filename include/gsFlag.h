#ifndef GS_FLAG_H
#define GS_FLAG_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Raw access to a story flag of the save. flagGetBooleanValue reads the flag
// back as the 0 or 1 the flag API keeps it as; flagSetTrue and flagSetFalse
// store one of those two values.
u32 GSflagGet(long flag);
void GSflagSet(long flag, long value);
s32 flagGetBooleanValue(long flag);
void flagSetTrue(long flag);
void flagSetFalse(long flag);

#ifdef __cplusplus
}
#endif

#endif
