#ifndef HSD_COBJ_H
#define HSD_COBJ_H

#include <dolphin/mtx.h>
#include <hsd/debug.h>
#include <hsd/cobjtypes.h>

struct _HSD_CameraAnim;

struct _HSD_CObj;

struct _HSD_ClassInfo {
    u8 _00[0x30];
    void (*release)(void *object);
    void (*destroy)(void *object);
};

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
    u8 _24[0x2C];
    u8 projectionType;          // 0x50
    Mtx viewMtx;                // 0x54
};

enum HSD_CObjFlag {
    HSD_COBJ_USER_MATRIX = 0x02,
    HSD_COBJ_INV_VIEW_MATRIX_DIRTY = 0x80000000
};

inline void HSD_CObjSetFlags(_HSD_CObj *cobj, u32 flags)
{
    HSD_ASSERT(0x19E, cobj);
    cobj->flags |= flags;
}

inline void HSD_CObjClearFlags(_HSD_CObj *cobj, u32 flags)
{
    HSD_ASSERT(0x1AA, cobj);
    cobj->flags &= ~flags;
}

enum HSD_CObjProjectionType {
    HSD_COBJ_PERSPECTIVE = 1
};

extern "C" {
_HSD_CObj *HSD_CObjLoadDesc(_HSD_CObjDesc *desc);
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
void HSD_CObjSetProjectionType(_HSD_CObj *camera, u8 type);
void HSD_CObjSetPerspective(_HSD_CObj *camera, f32 fov, f32 aspect);
void HSD_CObjSetNear(_HSD_CObj *camera, f32 nearPlane);
void HSD_CObjSetFar(_HSD_CObj *camera, f32 farPlane);
int HSD_CObjMakeProjectionMtx(_HSD_CObj *camera, Mtx44 matrix);
void HSD_CObjSetupViewingMtx(_HSD_CObj *camera);
}

#endif
