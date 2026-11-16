#ifndef GS_MSG_H
#define GS_MSG_H
#include <GSgfxTypes.h>
#include <bool.h>
#ifdef __cplusplus
class GSgfxLayer;
extern "C" {
extern GSgfxLayer *_TargetLayer;
#endif
typedef struct MSG_TASK_WORK {
    u8 active;
    u8 renderPass;
    u8 shadowMode;
    u8 talkSe;
    f32 originX;
    f32 originY;
    f32 cursorX;
    f32 cursorY;
    f32 advance;
    s16 width;
    s16 height;
    u32 messageId;
    u16 font;
    u8 charWidth;
    u8 charHeight;
    u32 color;
    u8 *messageData;
    u8 *savedStream;
    u8 *stream;
    u8 *streamStack[3];
    s8 streamDepth;
    s8 indentOff;
    s8 lineSpace;
    s8 baseLineBias;
    u8 flags;
    u8 keyWait;
    u8 keyEndWait;
    u8 checkEndWait;
    u8 previousTalkSoundState;
    s16 waitCount;
    u8 align;
    u8 ruby;
    f32 rubyX;
    f32 rubyY;
    u8 *rubyStreamPos;
    u8 rubyIndex;
    u8 rubyCount;
    u8 rubyDivisor;
    u8 rubyMultiplier;
    f32 rubyAdvance;
    f32 scaleX;
    f32 scaleY;
} MSG_TASK_WORK;

void GSmsgSetColor(MSG_TASK_WORK *task);
void GSmsgApplyFontMetrics(MSG_TASK_WORK *task);
void GSmsgCalcRubyLayout(MSG_TASK_WORK *task);
void *GSmsgOpen(void *data);
void *GSmsgFontOpen(void *data);
s32 GSmsgGetDataLanguage(const void *data);
s32 GSmsgClose(void *data);
s32 GSmsgFontClose(void *data);

unsigned long GSmsgGetCharLength(const u16* text);

s32 GSmsgIsCheck(u32 messageID);
void GSmsgKill(u32 messageID);
s32 GSmsgExec(u32 messageID, s32 arg1, s32 arg2);
s32 GSmsgMain(s32 layer, u32 messageID, u32 keyTrigger, bool paused);
s32 GScharCmp(const u16 *a, const u16 *b);
u16 *GScharLenCpy(u16 *destination, const u16 *source, s32 capacity);
u16* GScharCpy(u16* destination, const u16* source);
u16 *GScharCat(u16 *destination, const u16 *source);
u32 GScharMakeFromSJIS(u16 *destination, char *source);
s32 GSprint(GSgfxLayerID layer, s32 x, s32 y, u32 color, const char* format, ...);
s32 GSmsgPrint2(s32 layer, s32 x, s32 y, u32 color, u32 messageID);
u16 *GSmsgGetStringPtr(u32 messageID);
s32 GSmsgGetLength(u32 messageID);
u32 GSmsgGetMessageSize(s32 messageID);
s32 GSmsgPrintRight(s32 layer, s32 x, s32 y, s32 width, s32 height, u32 color, u32 messageID);
s32 GSmsgPrintRect(GSgfxLayerID layer, s32 x, s32 y, s16 width, s16 height, u32 color, u32 msgID);

s32 GScharConvToGBACode(void *destination, const u16 *source, s32 language);
s32 GScharConvToGCCode(u16 *destination, const u8 *source, s32 size, s32 language);

u16 *GSmsgMakeGScharStr(u16 *message, s32 capacity, u32 messageID);
#ifdef __cplusplus
}
#endif
#endif
