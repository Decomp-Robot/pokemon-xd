#include <GSvec.h>

namespace GSscript {

struct varPool_t {
    void* slots[256];
};

GSvec nullVec;

void** checkAllocedMemory(varPool_t* pool, void* memory)
{
    for (int i = 0; i < 256; i++) {
        if (pool->slots[i] == memory) {
            return &pool->slots[i];
        }
    }
    return 0;
}

void** varPoolSearchFree(varPool_t* pool)
{
    for (int i = 0; i < 256; i++) {
        if (pool->slots[i] == 0) {
            return &pool->slots[i];
        }
    }
    return 0;
}

}
