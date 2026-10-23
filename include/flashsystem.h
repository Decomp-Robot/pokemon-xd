#ifndef FLASH_SYSTEM_H
#define FLASH_SYSTEM_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
// Flash movie player shared by the PDA and title screens; the resource id is
// a GX texture handle such as 0x1FBD3000, the labels are frame labels inside
// the movie.
void FlashSystemSetPlayFrameLabel(u32 flash, const char *start, const char *end);
void FlashSystemSetNowFrameLabel(u32 flash, const char *label);
void FlashSystemSetLoopFlag(u32 flash, s32 loop);
u32 FlashSystemGetFrameFromLabel(u32 flash, const char *label);
u32 FlashSystemGetNowFrame(u32 flash);
u32 FlashSystemGetEndFrame(u32 flash);
void FlashSystemPlayAndWait(u32 flash, const char *start, const char *end);
u8 FlashSystemIsLastFrame(u32 flash);
#ifdef __cplusplus
}
#endif
#endif
