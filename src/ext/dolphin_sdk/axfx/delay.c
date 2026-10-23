#include <dolphin.h>
#include <dolphin/ax.h>
#include <dolphin/axfx.h>

#include "__axfx.h"

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
int AXFXDelaySettings(AXFX_DELAY* delay) {
    u32 i;
    s32* l;
    s32* r;
    s32* s;
    BOOL old;

    ASSERTMSGLINE(67, delay->delay[0] >= 10 && delay->delay[0] <= 5000 &&
                     delay->delay[1] >= 10 && delay->delay[1] <= 5000 &&
                     delay->delay[2] >= 10 && delay->delay[2] <= 5000 &&
                     delay->feedback[0] >= 0 && delay->feedback[0] <= 100 &&
                     delay->feedback[1] >= 0 && delay->feedback[1] <= 100 &&
                     delay->feedback[2] >= 0 && delay->feedback[2] <= 100 &&
                     delay->output[0] >= 0 && delay->output[0] <= 100 &&
                     delay->output[1] >= 0 && delay->output[1] <= 100 &&
                     delay->output[2] >= 0 && delay->output[2] <= 100,
                     "The value of specified parameter is out of range.");

    if (delay->delay[0] < 10 || delay->delay[0] > 5000 ||
        delay->delay[1] < 10 || delay->delay[1] > 5000 ||
        delay->delay[2] < 10 || delay->delay[2] > 5000 ||
        delay->feedback[0] < 0 || delay->feedback[0] > 100 ||
        delay->feedback[1] < 0 || delay->feedback[1] > 100 ||
        delay->feedback[2] < 0 || delay->feedback[2] > 100 ||
        delay->output[0] < 0 || delay->output[0] > 100 ||
        delay->output[1] < 0 || delay->output[1] > 100 ||
        delay->output[2] < 0 || delay->output[2] > 100)
    {
        return 0;
    }

    AXFXDelayShutdown(delay);
    old = OSDisableInterrupts();

    for (i = 0; i < 3; i++) {
        delay->currentSize[i] = (((delay->delay[i] - 5) << 5) + 0x9F) / 160U;
        delay->currentPos[i] = 0;
        delay->currentFeedback[i] = (delay->feedback[i] << 7) / 100U;
        delay->currentOutput[i] = (delay->output[i] << 7) / 100U;
    }

    delay->left = __AXFXAlloc(delay->currentSize[0] * 0xA0 * 4);
    delay->right = __AXFXAlloc(delay->currentSize[1] * 0xA0 * 4);
    delay->sur = __AXFXAlloc(delay->currentSize[2] * 0xA0 * 4);

    ASSERTMSGLINE(98, delay->left && delay->right && delay->sur, "Can't allocate the memory.");

    if (delay->left == NULL || delay->right == NULL || delay->sur == NULL) {
        AXFXDelayShutdown(delay);
        return 0;
    }

    l = delay->left;
    r = delay->right;
    s = delay->sur;

    for (i = 0; i < delay->currentSize[0] * 0xA0; i++) {
        *l++ = 0;
    }

    for (i = 0; i < delay->currentSize[1] * 0xA0; i++) {
        *r++ = 0;
    }

    for (i = 0; i < delay->currentSize[2] * 0xA0; i++) {
        *s++ = 0;
    }

    OSRestoreInterrupts(old);
    return 1;
}

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
