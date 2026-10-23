#ifndef DBGMENU_H
#define DBGMENU_H

#include <GSgfxTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

GSgfxLayerID dbgMenuGetLayerID(void);
void dbgMenuClose(void);
void dbgMenuSetOpenFlag(s32 flag);
s32 dbgMenuGetGsinputMsgId(s32 deviceId);

#ifdef __cplusplus
}
#endif

#endif
