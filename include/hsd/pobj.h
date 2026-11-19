#ifndef HSD_POBJ_H
#define HSD_POBJ_H

#include <types.h>
#include <dolphin/gx/GXEnum.h>
#include <dolphin/mtx.h>
#include <hsd/forward.h>
#include <hsd/class.h>
#include <hsd/aobj.h>
#include <hsd/list.h>

#define HSD_POBJ_TYPE_MASK 0x3000
#define HSD_POBJ_SHAPE_ANIM 0x1000
#define HSD_POBJ_ANIM_MASK 0x8

#define HSD_SHAPE_SET_WEIGHTS 0x2
#define HSD_SHAPE_WEIGHT_CHANNEL_BASE 2

enum HSDVertexDescriptorConstants {
    HSD_VTX_ATTR_POSITION = 9,
    HSD_VTX_ATTR_NULL = 255,
    HSD_VTX_INPUT_DIRECT = 1
};

struct _HSD_VtxDescList {
    GXAttr attr;
    GXAttrType type;
    GXCompCnt comp_cnt;
    GXCompType comp_type;
    u8 frac;
    u8 pad;
    u16 stride;
    void *base;
};

typedef struct _HSD_ShapeSet _HSD_ShapeSet;
typedef struct _HSD_ShapeSet HSD_ShapeSet;

typedef union _HSD_ShapeSetValue {
    f32 shape_position;
    f32 *shape_weights;
} HSD_ShapeSetValue;

struct _HSD_ShapeSet {
    u16 flags;
    u16 nb_shape;
    s32 nb_vertex_index;
    _HSD_VtxDescList *vertex_desc;
    u8 **vertex_index;
    s32 nb_normal_index;
    _HSD_VtxDescList *normal_desc;
    u8 **normal_index;
    HSD_ShapeSetValue u;
};

typedef struct _HSD_ShapeAnim _HSD_ShapeAnim;
typedef struct _HSD_ShapeAnim HSD_ShapeAnim;

struct _HSD_ShapeAnim {
    _HSD_ShapeAnim *next;
    HSD_AObjDesc *aobjdesc;
};

typedef struct _HSD_PObjDesc _HSD_PObjDesc;
typedef struct _HSD_PObjDesc HSD_PObjDesc;

typedef struct _HSD_PObjInfo _HSD_PObjInfo;
typedef struct _HSD_PObjInfo HSD_PObjInfo;

typedef void (*HSD_PObjDispFunc)(_HSD_PObj *pobj, Mtx mtx, Mtx mtx2, u32 flags);
typedef void (*HSD_PObjSetupMtxFunc)(_HSD_PObj *pobj, Mtx mtx, Mtx mtx2, u32 flags);
typedef int (*HSD_PObjLoadFunc)(_HSD_PObj *pobj, _HSD_PObjDesc *desc);

struct _HSD_PObjInfo {
    _HSD_ClassInfo parent;
    HSD_PObjDispFunc disp;
    HSD_PObjSetupMtxFunc setup_mtx;
    HSD_PObjLoadFunc load;
    HSD_AnimUpdate update;
};

typedef union _HSD_PObjData {
    _HSD_ShapeSet *shape_set;
    _HSD_JObj *jobj;
    HSD_SList *envelope_list;
} HSD_PObjData;

/* A primitive object: one display list plus its vertex description. */
struct _HSD_PObj {
    _HSD_PObjInfo *class_info;
    _HSD_PObj *mNext;
    _HSD_VtxDescList *mVtxAttr;
    u16 mFlags;
    u16 mDisplayListBlocks;
    void *mDisplayList;
    HSD_PObjData u;
    _HSD_AObj *mAObj;
};

#ifdef __cplusplus
extern "C" {
#endif
void HSD_ClearVtxDesc(void);
void HSD_PObjAnimAll(HSD_PObj *pobj);
void HSD_PObjReqAnimAllByFlags(HSD_PObj *pobj, u32 flags, f32 frame);
void HSD_PObjAddAnimAll(HSD_PObj *pobj, HSD_ShapeAnim *shape_anim);
void HSD_PObjRemoveAnimAllByFlags(HSD_PObj *pobj, u32 flags);
#ifdef __cplusplus
}
#endif

#endif
