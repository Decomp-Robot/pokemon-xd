#include <menu.h>
#include <GSgfxC.h>
#include <GSmsg.h>
#include <types.h>
#include <winSprite.h>
#include <menuSub.h>

/* Debug log window: the message-log overlay of the debug menus. */

/* First log line displayed by the debug log window; the flag recentres the
 * view on the newest lines the first time the opened window is drawn. */
extern s32 _viewStartLine;
extern u8 _viewStartLineInit;

#pragma force_active on

void windowSetParam(struct tagWINDOW_WORK *window, long no, long value);
long windowGetParam(struct tagWINDOW_WORK *window, long no);

u8 menuIsCheck(long menuID);

long GSlogGetLineCount(void);
const char *GSlogGetLine(long line);

#pragma peephole off

s32 dbgMenuLogDraw(struct tagWINDOW_WORK *window)
{
    GXColor color = { 0, 0, 0, 128 };
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

    if (_viewStartLineInit) {
        _viewStartLine = GSlogGetLineCount() - lines;
        _viewStartLineInit = 0;
    }

    if (pad->mKeyRepeat & 0x1) {
        _viewStartLine--;
    }
    if (pad->mKeyRepeat & 0x2) {
        _viewStartLine++;
    }
    if (pad->mKeyRepeat & 0x8) {
        _viewStartLine += lines;
    }
    if (pad->mKeyRepeat & 0x4) {
        _viewStartLine -= lines;
    }

    max = GSlogGetLineCount() - lines;
    if (_viewStartLine < 0) {
        _viewStartLine = 0;
    }
    if (_viewStartLine > max) {
        _viewStartLine = max;
    }

    for (i = 0; i < lines; i++) {
        if (i + _viewStartLine >= 0) {
            GSprint(window->mLayerID, 0x14, y, 0xC0C0C0FF, GSlogGetLine(i + _viewStartLine));
        }
        y += 0xD;
    }
    return 0;
}

s32 dbgMenuLogChangeDisp(void)
{
    if (menuIsCheck(MENU_ID_DEBUG_LOG)) {
        menuClose(MENU_ID_DEBUG_LOG);
    } else {
        _viewStartLineInit = 1;
        menuOpenCustom(0x22, 0, 0, 0, 1, 0);
    }
    return 0;
}

#pragma peephole reset

#pragma force_active reset
