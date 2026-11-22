#include <GSscript.h>
#include <GSmem.h>
#include <string.h>

namespace GSscript {

enum { STRING_BUFFER_SIZE = 256 };

struct varPool_t {
    void* slots[256];
};

void** checkAllocedMemory(varPool_t* pool, void* memory);
void** varPoolSearchFree(varPool_t* pool);

GSvec nullVec;
static varPool_t l_stringPool;
static varPool_t l_vectorPool;

static inline void freePooledString(void*& slot)
{
    if (slot != 0) {
        GSmemFree(slot);
        slot = 0;
    }
}

void tigaCellMemory::freeVariant(tigaVariant* variant)
{
    if (variant->kind == VariantString) {
        void** slot = checkAllocedMemory(&l_stringPool, variant->string);
        if (slot != 0) {
            *slot = 0;
        } else {
            variant->string = 0;
            variant->kind = VariantEmpty;
            return;
        }
        if (variant->string != 0) {
            mStringCount--;
            GSmemFree(variant->string);
        }
        variant->string = 0;
        variant->kind = VariantEmpty;
        return;
    }
    if (variant->kind == VariantArray) {
        // Array variants do not own cell memory.
        return;
    }
    if (variant->kind == VariantVector) {
        Tiga::m_cpCurrentObject->mObjectManager.deleteVector(variant->vector);
        variant->vector = 0;
        variant->kind = VariantEmpty;
    }
}

void tigaCellMemory::dupVariant(tigaVariant* variant)
{
    if (variant->kind == VariantString) {
        mStringCount++;
        char* buffer = (char*)GSmemAlloc(STRING_BUFFER_SIZE);
        strncpy(buffer, variant->string, STRING_BUFFER_SIZE - 1);
        variant->string = buffer;
        *varPoolSearchFree(&l_stringPool) = buffer;
    } else if (variant->kind == VariantArray) {
        // Array variants do not own cell memory.
    } else if (variant->kind == VariantVector) {
        GSvec* vector = Tiga::m_cpCurrentObject->mObjectManager.newVector();
        if (vector != 0) {
            *vector = *variant->vector;
            variant->vector = vector;
        }
    }
}

tigaCellMemory::~tigaCellMemory()
{
    if (mStringCount != 0 || mUnknown != 0) {
        if (mStringCount != 0) {
            for (int i = 0; i < 256; i++) {
                if (l_stringPool.slots[i] != 0) {
                    freePooledString(l_stringPool.slots[i]);
                }
            }
        }
    }
}

tigaCellMemory::tigaCellMemory()
{
    nullVec.set(0.0f, 0.0f, 0.0f);
    mStringCount = 0;
    mUnknown = 0;
    memset(&l_stringPool, 0, sizeof(l_stringPool));
    memset(&l_vectorPool, 0, sizeof(l_vectorPool));
}

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
