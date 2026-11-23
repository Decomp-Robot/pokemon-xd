#ifndef HSD_FOBJ_H
#define HSD_FOBJ_H

#include <types.h>
#include <dolphin/mtx.h>
#include <hsd/forward.h>
#include <hsd/objalloc.h>

typedef union _HSD_ObjData {
    f32 scalar;
    s32 integer;
    Vec vector;
} HSD_ObjData;

typedef void (*HSD_AnimUpdate)(void *object, u32 channel, HSD_ObjData *value);

/* Channels of the values an animation reports to its update callback. */
typedef enum _HSD_AnimChannel {
    HSD_ROBJ_UPDATE_CHANNEL = 1 /* update state of a constraint */
} HSD_AnimChannel;

/* Playback states kept in the low nibble of HSD_FObj::flags. */
typedef enum _FObjLoadState {
    FOBJ_LOAD_NONE = 0,
    FOBJ_LOAD_DATA0 = 1,
    FOBJ_LOAD_DATA = 2,
    FOBJ_LOAD_WAIT = 3,
    FOBJ_LOAD_FRAME = 4,
    FOBJ_LOAD_NEXT = 5,
    FOBJ_LOAD_END = 6
} FObjLoadState;

struct _HSD_FObj {
    HSD_FObj *next;    /* next object of the animation */
    u8 *ad;            /* read position in the animation data */
    u8 *bd;            /* start of the animation data */
    u32 size;          /* size of the animation data */
    u8 flags;          /* playback state and interpolation flags */
    u8 op;             /* kind of the current pack */
    u8 op_intrp;       /* kind of the pack currently interpolated */
    u8 chan;           /* channel reported to the update callback */
    u8 mode;           /* storage format of the key values */
    u8 slope_mode;     /* storage format of the key tangents */
    u16 nb_pack;       /* keys left in the current pack */
    s16 start;         /* frame offset of the animation */
    u16 pack;          /* length of the current pack in frames */
    f32 frame;         /* current position in the animation */
    f32 a;             /* value at the beginning of the pack */
    f32 b;             /* value at the end of the pack */
    f32 s;             /* tangent at the beginning of the pack */
    f32 t;             /* tangent at the end of the pack */
};

struct _HSD_FObjDesc {
    HSD_FObjDesc *next; /* next channel of the animation */
    u32 size;           /* size of the animation data */
    f32 start;          /* frame offset of the animation */
    u8 chan;            /* channel reported to the update callback */
    u8 mode;            /* storage format of the key values */
    u8 slope_mode;      /* storage format of the key tangents */
    u8 *data;           /* animation data */
};

#ifdef __cplusplus
extern "C" {
#endif
void HSD_FObjInitAllocData(void);
HSD_ObjAllocData *HSD_FObjGetAllocData(void);
HSD_FObj *HSD_FObjAlloc(void);
void HSD_FObjFree(HSD_FObj *fobj);
void HSD_FObjRemoveAll(HSD_FObj *fobj);
void HSD_FObjRemove(HSD_FObj *fobj);
HSD_FObj *HSD_FObjLoadDesc(HSD_FObjDesc *desc);
void HSD_FObjInterpretAnim(HSD_FObj *fobj, void *object,
                           HSD_AnimUpdate update, f32 rate);
void HSD_FObjStopAnimAll(HSD_FObj *fobj, void *object,
                         HSD_AnimUpdate update, f32 rate);
void HSD_FObjReqAnimAll(HSD_FObj *fobj, f32 frame);
void HSD_FObjInterpretAnimAll(HSD_FObj *fobj, void *object,
                              HSD_AnimUpdate update, f32 rate);
#ifdef __cplusplus
}
#endif

#endif
