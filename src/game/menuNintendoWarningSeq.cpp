#include <GSgfx.h>
#include <app/menu/menuNintendoWarning.h>
#include <menu.h>
#include <types.h>

// Window the Nintendo warning screen is shown in.
#define WIN_ID_NINTENDO_WARNING 274
// menuOpen waits until the window is accepted or closed before returning.
#define MENU_OPEN_WAIT 1

void _NintendoWarningInit(void) {
    GSprimSetup2DRender(GSgfxLayerFindByID(GSGFX_LAYER_MENU));
    _warningSeq = WARNING_SEQ_OPENING;
    _warningSeqTimer = 0.0f;
    menuOpen(WIN_ID_NINTENDO_WARNING, MENU_OPEN_WAIT);
}

void _NintendoWarningMain(void) {
}

void _NintendoWarningExit(void) {
    GSgfxLayerFindByID(GSGFX_LAYER_MENU);
    menuClose(WIN_ID_NINTENDO_WARNING);
    menuCloseSync(WIN_ID_NINTENDO_WARNING, true);
}
