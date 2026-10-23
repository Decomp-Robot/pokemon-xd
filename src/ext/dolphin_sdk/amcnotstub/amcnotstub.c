#include <dolphin.h>

// this file is a stub.
__declspec(weak) int AMC_IsStub(void);

// UNUSED: the game never calls this; dead-stripped at link.
__declspec(weak) int AMC_IsStub(void) {
    return 0;
}
