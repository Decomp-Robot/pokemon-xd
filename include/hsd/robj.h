#ifndef HSD_ROBJ_H
#define HSD_ROBJ_H

#include <types.h>
#include <hsd/forward.h>

typedef struct _HSD_RObjAnim _HSD_RObjAnim;

struct _HSD_RObj {
    _HSD_RObj *mNext;
    u8 _04[20];
    _HSD_AObj *mAObj;
};

#ifdef __cplusplus
extern "C" {
#endif
void HSD_RObjRemoveAll(_HSD_RObj *robj);
_HSD_RObj *HSD_RObjLoadDesc(_HSD_RObjDesc *desc);
void HSD_RObjResolveRefsAll(_HSD_RObj *robj, _HSD_RObjDesc *desc);
void HSD_RObjAddAnimAll(_HSD_RObj *robj, _HSD_RObjAnim *anim);
void HSD_RObjReqAnimAll(_HSD_RObj *robj, f32 frame);
void HSD_RObjAnimAll(_HSD_RObj *robj);
void HSD_RObjRemoveAnimAll(_HSD_RObj *robj);
#ifdef __cplusplus
}
#endif

#endif
