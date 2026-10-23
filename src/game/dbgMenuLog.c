#include <types.h>
#include <winSprite.h>
#include <menuSub.h>

/* Debug log window: the message-log overlay of the debug menus. */

/* Tile shade colour of the debug log window. */
extern GXColor lbl_804EBDF0;
/* First log line displayed by the debug log window. */
extern s32 lbl_804EA690;
/* Set while the log window is being opened. */
extern u8 lbl_804EA694;

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

void windowSetParam(struct tagWINDOW_WORK *window, long no, long value);
long windowGetParam(struct tagWINDOW_WORK *window, long no);

u8 zz_010ee28_(long menuID);
void menuClose(long menuID);
long menuOpenCustom(long menuID, long a, long b, long c, long d, long e, ...);
void GSgfxZCompare(u8 mode, long value);

s32 zz_01080ac_(long layerID, long x, long y, long color, const char *format, ...);
long GSlogGetLineCount(void);
const char *GSlogGetLine(long line);

#pragma peephole off

s32 dbgMenuLogDraw(struct tagWINDOW_WORK *window)
{
    GXColor color = lbl_804EBDF0;
    struct tagWINDOW_KEY *pad = zz_0116a80_();
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

    if (lbl_804EA694) {
        lbl_804EA690 = GSlogGetLineCount() - lines;
        lbl_804EA694 = 0;
    }

    if (pad->mKeyRepeat & 0x1) {
        lbl_804EA690--;
    }
    if (pad->mKeyRepeat & 0x2) {
        lbl_804EA690++;
    }
    if (pad->mKeyRepeat & 0x8) {
        lbl_804EA690 += lines;
    }
    if (pad->mKeyRepeat & 0x4) {
        lbl_804EA690 -= lines;
    }

    max = GSlogGetLineCount() - lines;
    if (lbl_804EA690 < 0) {
        lbl_804EA690 = 0;
    }
    if (lbl_804EA690 > max) {
        lbl_804EA690 = max;
    }

    for (i = 0; i < lines; i++) {
        if (i + lbl_804EA690 >= 0) {
            zz_01080ac_(window->mLayerID, 0x14, y, 0xC0C0C0FF, GSlogGetLine(i + lbl_804EA690));
        }
        y += 0xD;
    }
    return 0;
}

s32 dbgMenuLogChangeDisp(void)
{
    if (zz_010ee28_(0x22)) {
        menuClose(0x22);
    } else {
        lbl_804EA694 = 1;
        menuOpenCustom(0x22, 0, 0, 0, 1, 0);
    }
    return 0;
}

#pragma peephole reset

#pragma force_active reset
