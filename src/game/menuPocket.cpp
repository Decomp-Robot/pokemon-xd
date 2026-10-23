#include <menuPocket2.h>
#include <winSprite.h>

// Per-tab attributes, 0x20 bytes each, indexed by the window cursor.
struct PocketTabData {
    u8 _00;        // 0x00: unknown.
    u8 mSwapAble;  // 0x01: items on this tab may be swapped.
    u8 mSortAble;  // 0x02: items on this tab may be sorted.
    u8 _03;        // 0x03: unknown.
    u8 _04[0x1C];  // 0x04-0x1F: remainder; unknown here.
};

extern PocketTabData lPocketTabData[];

namespace {

// Whether the tab shown in the window may be sorted.
__declspec(weak) u8 _IsSortAble(tagWINDOW_WORK *window)
{
    if (menuPocket2Work.mMode == 3 || menuPocket2Work.mMode == 4 || menuPocket2Work.mMode == 5) {
        return 0;
    }

    return lPocketTabData[window->cursor].mSortAble;
}

// Whether the tab shown in the window may be swapped.
__declspec(weak) u8 _IsIrekaeAble(tagWINDOW_WORK *window)
{
    if (menuPocket2Work.mMode == 3 || menuPocket2Work.mMode == 4 || menuPocket2Work.mMode == 5) {
        return 0;
    }

    return lPocketTabData[window->cursor].mSwapAble;
}

}
