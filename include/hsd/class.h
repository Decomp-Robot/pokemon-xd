#ifndef HSD_CLASS_H
#define HSD_CLASS_H

#include <types.h>

typedef struct _HSD_ClassInfo _HSD_ClassInfo;

typedef struct _HSD_ClassInfoHead {
    void (*info_init)(void);
    u32 flags;
    const char *library_name;
    const char *class_name;
    s16 obj_size;
    s16 info_size;
    _HSD_ClassInfo *parent;
    _HSD_ClassInfo *next;
    _HSD_ClassInfo *child;
    u32 nb_alloc;
    u32 nb_peak;
} _HSD_ClassInfoHead;

struct _HSD_ClassInfo {
    _HSD_ClassInfoHead head;
    void *(*alloc)(_HSD_ClassInfo *class_info);
    int (*init)(void *object);
    void (*release)(void *object);
    void (*destroy)(void *object);
    void (*amnesia)(_HSD_ClassInfo *class_info);
};

typedef struct _HSD_Class {
    _HSD_ClassInfo *class_info;
} _HSD_Class;

enum {
    HSD_CLASS_INFO_INITIALIZED = 1
};

#ifdef __cplusplus
extern "C" {
#endif

extern _HSD_ClassInfo hsdClass;
extern _HSD_ClassInfo hsdObj;

_HSD_ClassInfo *hsdSearchClassInfo(const char *class_name);
int hsdIsDescendantOf(_HSD_ClassInfo *class_info, _HSD_ClassInfo *parent_info);
void *hsdNew(_HSD_ClassInfo *class_info);
void hsdInitClassInfo(_HSD_ClassInfo *class_info, _HSD_ClassInfo *parent_info,
                      const char *library_name, const char *class_name,
                      int info_size, int obj_size);

void _hsdClassInfoInit(void);
void _hsdClassAmnesia(_HSD_ClassInfo *class_info);
void _hsdClassDestroy(void *object);
void _hsdClassRelease(void *object);
int _hsdClassInit(void *object);
void *_hsdClassAlloc(_HSD_ClassInfo *class_info);
void hsdFreeMemPiece(void *memory, int size);
void *hsdAllocMemPiece(int size);

#ifdef __cplusplus
}
#endif

#endif
