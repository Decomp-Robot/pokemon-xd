#ifndef HSD_FOBJ_H
#define HSD_FOBJ_H

#include <types.h>
#include <dolphin/mtx.h>
#include <hsd/forward.h>

typedef union _HSD_ObjData {
    f32 scalar;
    s32 integer;
    Vec vector;
} HSD_ObjData;

typedef void (*HSD_AnimUpdate)(void *object, u32 channel, HSD_ObjData *value);

#ifdef __cplusplus
extern "C" {
#endif
void HSD_FObjRemoveAll(HSD_FObj *fobj);
HSD_FObj *HSD_FObjLoadDesc(HSD_FObjDesc *desc);
void HSD_FObjStopAnimAll(HSD_FObj *fobj, void *object,
                         HSD_AnimUpdate update, f32 rate);
void HSD_FObjReqAnimAll(HSD_FObj *fobj, f32 frame);
void HSD_FObjInterpretAnimAll(HSD_FObj *fobj, void *object,
                              HSD_AnimUpdate update, f32 rate);
#ifdef __cplusplus
}
#endif

#endif
