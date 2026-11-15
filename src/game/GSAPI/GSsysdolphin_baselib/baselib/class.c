#include <stddef.h>
#include <string.h>
#include <hsd/assert.h>
#include <hsd/class.h>
#include <hsd/memory.h>

typedef struct _HSD_Hash _HSD_Hash;

void *HSD_HashSearch(_HSD_Hash *hash, const void *key, int *found);

_HSD_ClassInfo hsdClass = { { _hsdClassInfoInit } };

static void *memory_list = NULL;
static int nb_memory_list = 0;
static _HSD_Hash *info_hash = NULL;

static inline void initClassInfo(_HSD_ClassInfo *class_info)
{
    if (!(class_info->head.flags & HSD_CLASS_INFO_INITIALIZED)) {
        class_info->head.info_init();
    }
}

static inline int getObjectSize(_HSD_ClassInfo *class_info)
{
    initClassInfo(class_info);
    return class_info->head.obj_size;
}

_HSD_ClassInfo *hsdSearchClassInfo(const char *class_name)
{
    return info_hash ? HSD_HashSearch(info_hash, class_name, NULL) : NULL;
}

int hsdIsDescendantOf(_HSD_ClassInfo *class_info, _HSD_ClassInfo *parent_info)
{
    _HSD_ClassInfo *ancestor;

    if (class_info == NULL || parent_info == NULL) {
        return 0;
    }
    initClassInfo(ancestor = class_info);
    initClassInfo(parent_info);
    for (; ancestor != NULL; ancestor = ancestor->head.parent) {
        if (ancestor == parent_info) {
            return 1;
        }
    }
    return 0;
}

void *hsdNew(_HSD_ClassInfo *class_info)
{
    _HSD_Class *object;

    initClassInfo(class_info);
    object = class_info->alloc(class_info);
    if (object == NULL) {
        return NULL;
    }
    memset(object, 0, getObjectSize(class_info));
    object->class_info = class_info;
    if (class_info->init(object) < 0) {
        class_info->destroy(object);
        return NULL;
    }
    return object;
}

void _hsdClassInfoInit(void)
{
    hsdClass.head.flags = HSD_CLASS_INFO_INITIALIZED;
    hsdClass.head.library_name = "sysdolphin_base_library";
    hsdClass.head.class_name = "hsd_class";
    hsdClass.head.obj_size = sizeof(_HSD_Class);
    hsdClass.head.info_size = sizeof(_HSD_ClassInfo);
    hsdClass.head.parent = NULL;
    hsdClass.head.child = NULL;
    hsdClass.head.next = NULL;
    hsdClass.head.nb_alloc = 0;
    hsdClass.head.nb_peak = 0;
    hsdClass.alloc = _hsdClassAlloc;
    hsdClass.init = _hsdClassInit;
    hsdClass.release = _hsdClassRelease;
    hsdClass.destroy = _hsdClassDestroy;
    hsdClass.amnesia = _hsdClassAmnesia;
}

void _hsdClassAmnesia(_HSD_ClassInfo *class_info)
{
    class_info->head.nb_alloc = 0;
    class_info->head.nb_peak = 0;
    if (class_info == &hsdClass) {
        nb_memory_list = 0;
        memory_list = NULL;
        info_hash = NULL;
    }
}

void _hsdClassDestroy(void *object)
{
    ((_HSD_Class *)object)->class_info->head.nb_alloc--;
    HSD_Free(object);
}

void _hsdClassRelease(void *object)
{
}

int _hsdClassInit(void *object)
{
    return 0;
}

void *_hsdClassAlloc(_HSD_ClassInfo *class_info)
{
    void *object = HSD_Alloc(class_info->head.obj_size);
    if (object != NULL) {
        class_info->head.nb_alloc++;
        if (class_info->head.nb_alloc > class_info->head.nb_peak) {
            class_info->head.nb_peak = class_info->head.nb_alloc;
        }
    }
    return object;
}

void hsdFreeMemPiece(void *memory, int size)
{
    HSD_Free(memory);
}

void *hsdAllocMemPiece(int size)
{
    return HSD_Alloc(size);
}

void hsdInitClassInfo(_HSD_ClassInfo *class_info, _HSD_ClassInfo *parent_info,
                      const char *library_name, const char *class_name,
                      int info_size, int obj_size)
{
    class_info->head.flags = HSD_CLASS_INFO_INITIALIZED;
    class_info->head.library_name = library_name;
    class_info->head.class_name = class_name;
    class_info->head.obj_size = obj_size;
    class_info->head.info_size = info_size;
    class_info->head.parent = parent_info;
    class_info->head.child = NULL;
    class_info->head.next = NULL;
    class_info->head.nb_alloc = 0;
    class_info->head.nb_peak = 0;
    if (parent_info != NULL) {
        initClassInfo(parent_info);
        HSD_ASSERTLINE(102, class_info->head.obj_size >= parent_info->head.obj_size);
        HSD_ASSERTLINE(103, class_info->head.info_size >= parent_info->head.info_size);
        memcpy(&class_info->alloc, &parent_info->alloc,
               parent_info->head.info_size - offsetof(_HSD_ClassInfo, alloc));
        class_info->head.next = parent_info->head.child;
        parent_info->head.child = class_info;
    }
}
