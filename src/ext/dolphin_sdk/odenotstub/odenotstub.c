#include <dolphin.h>

// prototypes
__declspec(weak) int Hu_IsStub();

// UNUSED: the game never calls this; dead-stripped at link.
__declspec(weak) int Hu_IsStub() {
    return 0;
}
