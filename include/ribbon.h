#ifndef RIBBON_H
#define RIBBON_H

#include <types.h>

typedef struct RibbonData {
    u32 unknown;
    u32 messageID;
} RibbonData;

#ifdef __cplusplus
extern "C" {
#endif

u32 getRibbonMsgID(s32 ribbon);
s32 getRibbonCnt(void);

#ifdef __cplusplus
}
#endif

#endif
