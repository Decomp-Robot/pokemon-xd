#include <dolphin.h>
#include <dolphin/ax.h>

// UNUSED: the game never calls this; dead-stripped at link.
static void* __AXFXAllocFunction(u32 bytes) {
    return OSAlloc(bytes);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __AXFXFreeFunction(void* p) {
    OSFree(p);
}

void* (*__AXFXAlloc)(u32) = __AXFXAllocFunction;
void (*__AXFXFree)(void*) = __AXFXFreeFunction;

// UNUSED: the game never calls this; dead-stripped at link.
