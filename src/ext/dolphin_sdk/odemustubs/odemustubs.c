#include <dolphin.h>

// prototypes
__declspec(weak) int Hu_IsStub();
void DBOpen();
void DBClose();

__declspec(weak) int Hu_IsStub() {
    return 0;
}

void DBOpen() {}
void DBClose() {}
