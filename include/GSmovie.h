#ifndef GSMOVIE_H
#define GSMOVIE_H

#include <types.h>

struct GStexture;

#ifdef __cplusplus
extern "C" {
#endif
u8 GSmoviePlayCheck(u32 movieID);
void GSmovieStop(u32 movieID);
void GSmoviePlayWait(u32 movieID);
s32 GSmoviePlay(u32 movieID, s32 flags, s32 loopFlag, struct GStexture *texture);
#ifdef __cplusplus
}
#endif

#endif
