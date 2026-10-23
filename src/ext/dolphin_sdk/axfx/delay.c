#include <dolphin.h>
#include <dolphin/ax.h>
#include <dolphin/axfx.h>

#include "__axfx.h"

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
int AXFXDelayShutdown(AXFX_DELAY* delay) {
    BOOL old;

    old = OSDisableInterrupts();
    if (delay->left) {
        __AXFXFree(delay->left);
    }

    if (delay->right) {
        __AXFXFree(delay->right);
    }

    if (delay->sur) {
        __AXFXFree(delay->sur);
    }

    delay->left = NULL;
    delay->right = NULL;
    delay->sur = NULL;

    OSRestoreInterrupts(old);
    return 1;
}
