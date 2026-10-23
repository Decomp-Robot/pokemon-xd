#include <menu.h>
#include <types.h>
#include <winSprite.h>
#include <menuSub.h>

/* Debug log window: the message-log overlay of the debug menus. */

/* Tile shade colour of the debug log window. */
extern GXColor dbgLogTileColor;
/* First log line displayed by the debug log window. */
extern s32 dbgLogFirstVisibleLine;
/* Set while the log window is being opened. */
extern u8 dbgLogInitializeToLatest;

#pragma force_active on

void windowSetParam(struct tagWINDOW_WORK *window, long no, long value);
long windowGetParam(struct tagWINDOW_WORK *window, long no);

void GSgfxZCompare(u8 mode, long value);

s32 menuDrawFormattedText(long layerID, long x, long y, long color, const char *format, ...);
long GSlogGetLineCount(void);
const char *GSlogGetLine(long line);

#pragma peephole off

s32 dbgMenuLogDraw(struct tagWINDOW_WORK *window)
{
    GXColor color = dbgLogTileColor;
    struct tagWINDOW_KEY *pad = windowGetKeyInfo();
    GXColor tile;
    long i;
    long y;
    long lines;
    long height;
    long max;

    if (pad->mKeyTrg & 0x100) {
        long mode = windowGetParam(window, 0) + 1;
        if (mode >= 3) {
            mode = 0;
        }
        windowSetParam(window, 0, mode);
    }

    switch (windowGetParam(window, 0)) {
    case 0:
    default:
        lines = 10;
        height = 0x82;
        y = 0x145;
        break;
    case 1:
        lines = 10;
        height = 0x82;
        y = 0x27;
        break;
    case 2:
        lines = 1;
        height = 0xD;
        y = 0x1BA;
        break;
    }

    GSgfxZCompare(0, 7);
    tile = color;
    menuSubDrawTile(window->mLayerID, 0xF, y - 5, 0x25D, height + 0x12, &tile);

    if (dbgLogInitializeToLatest) {
        dbgLogFirstVisibleLine = GSlogGetLineCount() - lines;
        dbgLogInitializeToLatest = 0;
    }

    if (pad->mKeyRepeat & 0x1) {
        dbgLogFirstVisibleLine--;
    }
    if (pad->mKeyRepeat & 0x2) {
        dbgLogFirstVisibleLine++;
    }
    if (pad->mKeyRepeat & 0x8) {
        dbgLogFirstVisibleLine += lines;
    }
    if (pad->mKeyRepeat & 0x4) {
        dbgLogFirstVisibleLine -= lines;
    }

    max = GSlogGetLineCount() - lines;
    if (dbgLogFirstVisibleLine < 0) {
        dbgLogFirstVisibleLine = 0;
    }
    if (dbgLogFirstVisibleLine > max) {
        dbgLogFirstVisibleLine = max;
    }

    for (i = 0; i < lines; i++) {
        if (i + dbgLogFirstVisibleLine >= 0) {
            menuDrawFormattedText(window->mLayerID, 0x14, y, 0xC0C0C0FF, GSlogGetLine(i + dbgLogFirstVisibleLine));
        }
        y += 0xD;
    }
    return 0;
}

s32 dbgMenuLogChangeDisp(void)
{
    if (menuIsOpen(MENU_ID_DEBUG_LOG)) {
        menuClose(MENU_ID_DEBUG_LOG);
    } else {
        dbgLogInitializeToLatest = 1;
        menuOpenCustom(MENU_ID_DEBUG_LOG, 0, 0, 0, 1, 0);
    }
    return 0;
}

#pragma peephole reset

#pragma force_active reset
