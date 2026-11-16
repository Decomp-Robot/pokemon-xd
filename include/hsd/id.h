#ifndef HSD_ID_H
#define HSD_ID_H

#include <types.h>
#include <hsd/forward.h>
#include <hsd/objalloc.h>

#ifdef __cplusplus
extern "C" {
#endif
void _HSD_IDForgetMemory(void);
void *HSD_IDGetDataFromTable(HSD_IDTable *table, u32 id, int *found);
void HSD_IDRemoveByIDFromTable(HSD_IDTable *table, u32 id);
void HSD_IDInsertToTable(HSD_IDTable *table, u32 id, void *data);
void HSD_IDSetup(void);
void HSD_IDInitAllocData(void);
HSD_ObjAllocData *HSD_IDGetAllocData(void);
#ifdef __cplusplus
}
#endif

#endif
