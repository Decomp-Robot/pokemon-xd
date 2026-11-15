#include <GSmemman.h>

void operator delete[](void* ptr) {
    GSmemFree(ptr);
}

void operator delete(void* ptr) {
    GSmemFree(ptr);
}

void* operator new[](unsigned long size) {
    if (GSmemIsInitialized()) {
        return GSmemAlloc(size);
    }
    return 0;
}

void* operator new(unsigned long size) {
    if (GSmemIsInitialized()) {
        if (size < 64) {
            int smallAllocation = 0;
        }
        return GSmemAlloc(size);
    }
    return 0;
}
