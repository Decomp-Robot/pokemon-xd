#ifndef WIN_SEQ_H
#define WIN_SEQ_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void winSeqSetMenu(long menuID, s32 seq);
u8 winSeqCheckMove(s32 id);

#ifdef __cplusplus
}
#endif

#endif
