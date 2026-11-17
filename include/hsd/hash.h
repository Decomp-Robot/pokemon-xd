#ifndef HSD_HASH_H
#define HSD_HASH_H

typedef struct _HSD_Hash _HSD_Hash;

#ifdef __cplusplus
extern "C" {
#endif

void *HSD_HashSearch(_HSD_Hash *hash, const void *key, int *found);

#ifdef __cplusplus
}
#endif

#endif
