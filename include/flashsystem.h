#ifndef FLASH_SYSTEM_H
#define FLASH_SYSTEM_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
// Flash movie player shared by the PDA and title screens; the resource id is
// the GX texture handle of the movie, the labels are frame labels inside it.
// A stage hands the movie it read in to the player and takes it back when the
// stage is left. The player plays a fixed number of movies at a time, so
// taking one in answers 0 when it has no free slot left for it.
s32 FlashSystemInit(u32 flash, void *resource);
void FlashSystemRelease(u32 flash);
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
