#ifndef HSD_LIST_H
#define HSD_LIST_H

#include <hsd/objalloc.h>

typedef struct HSD_SList {
    struct HSD_SList *next;
    void *data;
} HSD_SList;

#ifdef __cplusplus
extern "C" {
#endif

HSD_SList *HSD_SListRemove(HSD_SList *list);
HSD_SList *HSD_SListPrepend(HSD_SList *list, void *data);
HSD_SList *HSD_SListPrependList(HSD_SList *list, HSD_SList *prev);
HSD_SList *HSD_SListAlloc(void);
HSD_ObjAllocData *HSD_DListGetAllocData(void);
HSD_ObjAllocData *HSD_SListGetAllocData(void);
void HSD_ListInitAllocData(void);

#ifdef __cplusplus
}
#endif

#endif
