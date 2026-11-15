#ifndef HSD_ID_H
#define HSD_ID_H

#include <types.h>
#include <hsd/forward.h>

#ifdef __cplusplus
extern "C" {
#endif
void *HSD_IDGetDataFromTable(HSD_IDTable *table, u32 id, int *found);
#ifdef __cplusplus
}
#endif

#endif
