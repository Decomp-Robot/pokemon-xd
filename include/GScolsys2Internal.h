#ifndef GSCOLSYS2_INTERNAL_H
#define GSCOLSYS2_INTERNAL_H

#include <GScolsys2.h>

/* Current floor and debug drawing resources. */
struct GScolsys2State {
    CCD_FILEHEAD* mFile; /* The loaded CCD collision file head. */
    GSCOLSYS2_FLOOR mFloor[1]; /* 0x04 */
    s32 mTableIndex; /* 0xDC4 */
    GSgfxVF* mVf;    /* 0xDC8 */
    void* mFixedDisplayList; /* 0xDCC */
};

extern GScolsys2State gsColSysState;

extern "C" {
CCD_FILEHEAD* GScolsys2GetCCDFileHead(void);
void PSMTXMultVec(GSmtx* mtx, GSvec* src, GSvec* dst);
f32 PSVECDotProduct(GSvec* a, GSvec* b);
f32 PSVECSquareDistance(GSvec* a, GSvec* b);
f32 PSVECDistance(GSvec* a, GSvec* b);
f32 PSVECMag(GSvec* a);
void PSVECSubtract(GSvec* a, GSvec* b, GSvec* out);
void PSVECAdd(GSvec* a, GSvec* b, GSvec* out);
void PSVECScale(GSvec* a, GSvec* out, f32 scale);

void GSgfxDLBeginSurface(u16 surface);
void GSgfxBeginSurface(void* list);
void GSgfxVertex3f(f32 x, f32 y, f32 z);
void GSgfxColor4u8(u8 r, u8 g, u8 b, u8 a);
void GSgfxLayerSetActive(GSgfxLayerID layer);
void GScameraLoadActiveCameraMatrix(s32 layer);
void GSgfxSelectVertexFormat(GSgfxVF* vf);
}

#endif
