#include <hsd/assert.h>
#include <hsd/class.h>
#include <hsd/hash.h>

typedef struct _HSD_HashNode {
    struct _HSD_HashNode *next;
    const void *key;
    void *value;
} _HSD_HashNode;

typedef struct _HSD_HashInfo {
    _HSD_ClassInfo parent;
    u32 (*hash)(_HSD_Hash *hash, const void *key);
    int (*compare)(_HSD_Hash *hash, const void *key1, const void *key2);
} _HSD_HashInfo;

struct _HSD_Hash {
    _HSD_HashInfo *info;
    _HSD_HashNode **table;
    u32 table_size;
};

static inline _HSD_HashNode *HashSearchNode(_HSD_Hash *hash, const void *key, u32 idx)
{
    _HSD_HashNode *node = hash->table[idx];

    if (node == NULL) {
        return NULL;
    }
    while (node != NULL) {
        if (hash->info->compare(hash, node->key, key) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void *HSD_HashSearch(_HSD_Hash *hash, const void *key, int *found)
{
    _HSD_HashNode *node;
    u32 idx;

    idx = hash->info->hash(hash, key);
    HSD_ASSERTLINE(113, idx < hash->table_size);

    node = HashSearchNode(hash, key, idx);

    if (found != NULL) {
        *found = (node != NULL);
    }
    return node != NULL ? node->value : NULL;
}
