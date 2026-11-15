#ifndef HSD_TOBJ_H
#define HSD_TOBJ_H

#include <types.h>
#include <hsd/forward.h>

struct _HSD_TObj {
    u8 _00[8];
    _HSD_TObj *mNext;
    u8 _0C[88];
    _HSD_AObj *mAObj;
};

#ifdef __cplusplus
extern "C" {
#endif
void HSD_TObjRemoveAnim(_HSD_TObj *texture);
void HSD_TObjAddAnim(_HSD_TObj *texture, _HSD_TexAnim *animation);
void HSD_TObjReqAnim(_HSD_TObj *texture, f32 frame);
#ifdef __cplusplus
}
#endif

#endif
