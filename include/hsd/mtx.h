#ifndef HSD_MTX_H
#define HSD_MTX_H

#include <dolphin/mtx.h>
#include <hsd/objalloc.h>

#ifdef __cplusplus
extern "C" {
#endif

void HSD_MtxInitAllocData(void);
HSD_ObjAllocData *HSD_MtxGetAllocData(void);
Mtx *HSD_MtxAlloc(void);
void HSD_MtxFree(Mtx mtx);
void HSD_VecInitAllocData(void);
HSD_ObjAllocData *HSD_VecGetAllocData(void);
Vec *HSD_VecAlloc(void);
void HSD_VecFree(Vec* vec);

void HSD_VecGetOrthogonal(Vec* v, Vec* orthogonal);
void HSD_MtxScaledAdd(Mtx a, Mtx b, Mtx result, f32 scale);
void HSD_MtxSRTQuat(Mtx m, Vec* scale, Quaternion* rotation,
                    Vec* translation, Vec* parent_scale);
void HSD_MtxSRT(Mtx m, Vec* scale, Vec* rotation,
                Vec* translation, Vec* parent_scale);
void HSD_MkRotationMtx(Mtx m, Vec* rotation);
void HSD_MtxGetScale(Mtx m, Vec* scale);
void HSD_MtxGetTranslate(Mtx m, Vec* translation);
void HSD_MtxGetRotationMtx(Mtx src, Mtx dst, char primary, char secondary);
void HSD_MtxGetRotation(Mtx m, Vec* rotate);
int HSD_MtxInverseConcat(Mtx left, Mtx right, Mtx result);

#ifdef __cplusplus
}
#endif

#endif
