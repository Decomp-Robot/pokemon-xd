#ifndef HSD_COBJ_H
#define HSD_COBJ_H

#include <dolphin/mtx.h>
#include <hsd/assert.h>
#include <hsd/wobj.h>
#include <hsd/cobjtypes.h>
#include <hsd/class.h>

struct _HSD_CObj {
    _HSD_ClassInfo *classInfo; // 0x00
    u16 referenceCount;       // 0x04
    u8 _06[0x02];
    u32 flags;                 // 0x08
    f32 viewportLeft;           // 0x0C
    f32 viewportRight;          // 0x10
    f32 viewportTop;            // 0x14
    f32 viewportBottom;         // 0x18
    u16 scissorLeft;            // 0x1C
    u16 scissorRight;           // 0x1E
    u16 scissorTop;             // 0x20
    u16 scissorBottom;          // 0x22
    _HSD_WObj *eye;             // 0x24
    _HSD_WObj *interest;        // 0x28
    union {
        f32 roll;
        Vec up;
    } orientation; // 0x2C
    f32 nearPlane;              // 0x38
    f32 farPlane;               // 0x3C
    union {
        struct {
            f32 fov;
            f32 aspect;
        } perspective;
        struct {
            f32 top;
            f32 bottom;
            f32 left;
            f32 right;
        } frustum;
        struct {
            f32 top;
            f32 bottom;
            f32 left;
            f32 right;
        } ortho;
    } projection;               // 0x40
    u8 projectionType;          // 0x50
    Mtx viewMtx;                // 0x54
    _HSD_AObj *mAObj;
};

enum HSD_CObjFlag {
    HSD_COBJ_USER_MATRIX = 0x02,
    HSD_COBJ_INV_VIEW_MATRIX_DIRTY = 0x80000000
};

#ifdef __cplusplus
#define HSD_COBJ_INLINE inline
#else
#define HSD_COBJ_INLINE static inline
#endif
HSD_COBJ_INLINE void HSD_CObjSetFlags(_HSD_CObj *cobj, u32 flags)
{
    HSD_ASSERTLINE(414, cobj);
    cobj->flags |= flags;
}

HSD_COBJ_INLINE void HSD_CObjClearFlags(_HSD_CObj *cobj, u32 flags)
{
    HSD_ASSERTLINE(426, cobj);
    cobj->flags &= ~flags;
}
#undef HSD_COBJ_INLINE

enum HSD_CObjProjectionType {
    HSD_COBJ_PERSPECTIVE = 1,
    HSD_COBJ_FRUSTUM = 2,
    HSD_COBJ_ORTHO = 3
};

#ifdef __cplusplus
extern "C" {
#endif
_HSD_CObj *HSD_CObjLoadDesc(_HSD_CObjDesc *desc);
int HSD_CObjSetCurrent(_HSD_CObj *camera);
MtxPtr HSD_CObjGetViewingMtxPtr(_HSD_CObj *camera);
void HSD_CObjGetEyePosition(_HSD_CObj *camera, Vec *position);
BOOL HSD_CObjGetUpVector(_HSD_CObj *camera, Vec *up);
void HSD_CObjGetInterest(_HSD_CObj *camera, Vec *interest);
void HSD_CObjSetEyePosition(_HSD_CObj *camera, Vec *position);
void HSD_CObjRemoveAnim(_HSD_CObj *camera);
void HSD_CObjAddAnim(_HSD_CObj *camera, _HSD_CameraAnim *animation);
void HSD_CObjGetOrtho(_HSD_CObj *camera, f32 *top, f32 *bottom,
                       f32 *left, f32 *right);
void HSD_CObjReqAnim(_HSD_CObj *camera, f32 frame);
void HSD_CObjAnim(_HSD_CObj *camera);
void HSD_CObjGetPerspective(_HSD_CObj *camera, f32 *fov, f32 *aspect);
f32 HSD_CObjGetNear(_HSD_CObj *camera);
f32 HSD_CObjGetFar(_HSD_CObj *camera);
void HSD_CObjSetOrtho(_HSD_CObj *camera, f32 top, f32 bottom, f32 left,
                      f32 right);
void HSD_CObjSetFrustum(_HSD_CObj *camera, f32 top, f32 bottom, f32 left,
                        f32 right);
int HSD_CObjGetProjectionType(_HSD_CObj *camera);
void HSD_CObjSetProjectionType(_HSD_CObj *camera, u8 type);
void HSD_CObjSetPerspective(_HSD_CObj *camera, f32 fov, f32 aspect);
void HSD_CObjSetViewportfx4(_HSD_CObj *camera, f32 left, f32 right, f32 top,
                            f32 bottom);
void HSD_CObjSetScissorx4(_HSD_CObj *camera, u16 left, u16 right, u16 top,
                          u16 bottom);
void HSD_CObjSetAspect(_HSD_CObj *camera, f32 aspect);
void HSD_CObjSetFov(_HSD_CObj *camera, f32 fov);
void HSD_CObjSetNear(_HSD_CObj *camera, f32 nearPlane);
void HSD_CObjSetFar(_HSD_CObj *camera, f32 farPlane);
int HSD_CObjMakeProjectionMtx(_HSD_CObj *camera, Mtx44 matrix);
void HSD_CObjSetupViewingMtx(_HSD_CObj *camera);
#ifdef __cplusplus
}
#endif

#endif
