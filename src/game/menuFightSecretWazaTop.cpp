#include <types.h>
#include <window.h>
#include <winSprite.h>
#include <menuWazaStatus.h>

extern "C" void *windowAllocMemory(tagWINDOW_WORK *window, long size);

// Frame hook of the secret move window: on the first step it copies the move
// status handed in as the window argument into the window work area.
extern "C" s32 menuFightCtrlSecretWazaTop(tagWINDOW_WORK *window)
{
    MENU_WAZA_STATUS *status;

    if (window->mState == WINDOW_STATE_INIT) {
        status = (MENU_WAZA_STATUS *)windowAllocMemory(window, sizeof(MENU_WAZA_STATUS));
        if (status != 0)
            memcpy(status, (void *)window->mArgs[0], sizeof(MENU_WAZA_STATUS));
    }

    status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);

    return 0;
}
