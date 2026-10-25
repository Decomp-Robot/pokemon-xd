#include <menu.h>
#include <GSgfxC.h>
#include <types.h>

s32 dbgMenuGSgfxChangeDisp(void)
{
    if (menuIsCheck(MENU_ID_DEBUG_GS_GFX) > 0) {
        menuClose(MENU_ID_DEBUG_GS_GFX);
    } else {
        menuOpenCustom(MENU_ID_DEBUG_GS_GFX, 0, NULL, 0, 1, 0);
        menuSetPosition(MENU_ID_DEBUG_GS_GFX, 380, 32);
    }

    return 0;
}

s32 dbgMenuLayerFifoDisp(void)
{
    if (GSgfxLayerGetStatsRenderMode() == 0) {
        GSgfxLayerSetStatsRenderMode(1);
    } else {
        GSgfxLayerSetStatsRenderMode(0);
    }

    return 0;
}
