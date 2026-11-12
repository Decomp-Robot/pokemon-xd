#ifndef HSD_LOBJ_H
#define HSD_LOBJ_H

#include <dolphin/mtx.h>
#include <hsd/aobj.h>
#include <hsd/class.h>
#include <hsd/lobjtypes.h>

struct _HSD_CObj;

#ifdef __cplusplus
extern "C" {
#endif
_HSD_LObj *HSD_LObjLoadDesc(_HSD_LObjDesc *ldesc);
void HSD_LObjDeleteCurrentAll(_HSD_LObj *lobj);
void HSD_LObjAddCurrentAll(_HSD_LObj *lobj);
void HSD_LObjSetupInit(_HSD_CObj *camera);
void HSD_LObjAddAnimAll(_HSD_LObj *lobj, _HSD_LObjAnimAll *animation);
void HSD_LObjRemoveAnimAll(_HSD_LObj *lobj);
void HSD_LObjReqAnimAll(_HSD_LObj *lobj, f32 frame);
void HSD_LObjReqAnim(_HSD_LObj *lobj, f32 frame);
void HSD_LObjAnim(_HSD_LObj *lobj);
void HSD_LObjAnimAll(_HSD_LObj *lobj);
BOOL HSD_LObjGetPosition(_HSD_LObj *lobj, Vec *position);
BOOL HSD_LObjGetInterest(_HSD_LObj *lobj, Vec *interest);
void HSD_LObjSetPosition(_HSD_LObj *lobj, Vec *position);
void HSD_LObjSetInterest(_HSD_LObj *lobj, Vec *interest);
void HSD_LObjGetColor(_HSD_LObj *lobj, GXColor *color);
void HSD_LObjSetColor(_HSD_LObj *lobj, GXColor color);
u16 HSD_LObjGetFlags(_HSD_LObj *lobj);
void HSD_LObjSetFlags(_HSD_LObj *lobj, u16 flags);
void HSD_LObjClearFlags(_HSD_LObj *lobj, u16 flags);
#ifdef __cplusplus
}
#endif

#endif
