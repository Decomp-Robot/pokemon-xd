#ifndef FLASH_SYSTEM_H
#define FLASH_SYSTEM_H

#include <types.h>

struct GStexture;
struct tagWINDOW_WORK;
struct tagSPRITE_WORK;

#ifdef __cplusplus
extern "C" {
#endif
s32 FlashSystemInitAll(void);
s32 FlashSystemInit(u32 flash, void* resource);
void FlashSystemRelease(u32 flash);
void FlashSystemPlay(u32 flash);
void FlashSystemDraw(u32 flash, struct tagWINDOW_WORK* window, struct tagSPRITE_WORK* sprite);
struct GStexture* FlashSystemGetGStexture(u32 flash, s32 textureID);
u32 FlashSystemGetNowFrame(u32 flash);
u32 FlashSystemGetEndFrame(u32 flash);
s32 FlashSystemSetNowFrame(u32 flash, u32 frame);
void FlashSystemSetLoopFlag(u32 flash, u8 loop);
s32 FlashSystemSetPlayFrameLabel(u32 flash, const char* start, const char* end);
s32 FlashSystemSetNowFrameLabel(u32 flash, const char* label);
u32 FlashSystemGetFrameFromLabel(u32 flash, const char* label);
u8 FlashSystemIsLastFrame(u32 flash);
void FlashSystemPlayAndWait(u32 flash, const char* start, const char* end);
#ifdef __cplusplus
}
#endif
#endif
