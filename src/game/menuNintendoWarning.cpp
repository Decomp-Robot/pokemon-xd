#include <app/menu/menuNintendoWarning.h>
#include <types.h>

// Runs the Nintendo warning screen: reset it, call its main step, then close it.
extern "C" void menuNintendoWarning(void) {
    _NintendoWarningInit();
    _NintendoWarningMain();
    _NintendoWarningExit();
}
