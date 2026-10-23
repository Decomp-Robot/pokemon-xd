#ifndef GS_MSG_H
#define GS_MSG_H
#include <GSgfxTypes.h>
#ifdef __cplusplus
extern "C" {
#endif
s32 GSmsgIsCheck(u32 messageID);
void GSmsgKill(u32 messageID);
s32 GSmsgExec(u32 messageID, s32 arg1, s32 arg2);
#ifdef __cplusplus
s32 GSmsgMain(s32 layer, u32 messageID, u16 keyTrigger, bool paused);
#endif
s32 GScharCmp(const u16 *a, const u16 *b);
u16* GScharCpy(u16* destination, const u16* source);
u16 *GScharCat(u16 *destination, const u16 *source);
u32 GScharMakeFromSJIS(u16 *destination, const char *source);
s32 GSprint(GSgfxLayerID layer, s32 x, s32 y, u32 color, const char* format, ...);
s32 GSmsgPrint2(s32 layer, s32 x, s32 y, u32 color, u32 messageID);
u16 *GSmsgGetStringPtr(u32 messageID);
s32 GSmsgGetLength(u32 messageID);
u32 GSmsgGetMessageSize(s32 messageID);
s32 GSmsgPrintRight(s32 layer, s32 x, s32 y, s32 width, s32 height, u32 color, u32 messageID);
s32 GSmsgPrintRect(s32 layer, s32 x, s32 y, s16 width, s16 height, u32 color, s32 msgID);

u16 *GSmsgMakeGScharStr(u16 *message, s32 capacity, u32 messageID);
#ifdef __cplusplus
}
#endif
#endif
