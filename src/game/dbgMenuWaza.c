#include <floorManager.h>
#include <menu.h>
#include <types.h>

/* Waza (move) debug sub menus and the small accessors that go with them. */
#pragma force_active on

/* Waza viewer state the waza debug menus share (0x38 bytes). */
struct WazaViewerWork {
    u8 unk00[0x16];
    u8 mPokemonSelectDirty;  /* 0x16 */
    u8 mWazaSelectDirty;     /* 0x17 */
    u8 unk18[0x30 - 0x18];
    s32 mDebugRunning;       /* 0x30 */
};

extern struct WazaViewerWork wazaViewerData;

u8 menuIsOpen(s32 menuID);
void menuSetPosition(s32 menuID, s32 x, s32 y);
void GSfloorPop(void);
void GSthreadSwitch(void);

#pragma peephole off

int dbgMenuWazaDebugStop(void)
{
    if (wazaViewerData.mDebugRunning != 0) {
        GSfloorPop();
    }
    return 0;
}

int dbgMenuPokemonDebugSelect(void)
{
    struct WazaViewerWork *work;

    if (menuIsOpen(MENU_ID_DEBUG_POKEMON_SELECT)) {
        menuClose(MENU_ID_DEBUG_POKEMON_SELECT);
    } else {
        work = &wazaViewerData;
        work->mPokemonSelectDirty = 0;
        work->mWazaSelectDirty = 0;
        menuOpenCustom(MENU_ID_DEBUG_POKEMON_SELECT, 0, 0, 0, 1, 0);
        menuSetPosition(MENU_ID_DEBUG_POKEMON_SELECT, 0xC, 0xA);
    }
    return 0;
}

int dbgMenuWazaDebugSelect(void)
{
    if (menuIsOpen(MENU_ID_DEBUG_MOVE_SELECT)) {
        menuClose(MENU_ID_DEBUG_MOVE_SELECT);
    } else {
        menuOpenCustom(MENU_ID_DEBUG_MOVE_SELECT, 0, 0, 0, 1, 0);
        menuSetPosition(MENU_ID_DEBUG_MOVE_SELECT, 0xC, 0xA);
    }
    return 0;
}

int dbgMenuWazaDebugStart(void)
{
    if (menuIsOpen(MENU_ID_TITLE)) {
        menuClose(MENU_ID_TITLE);
    }
    if (wazaViewerData.mDebugRunning == 0) {
        GSfloorPush(0x3E7);
        GSthreadSwitch();
    }
    return 0;
}

#pragma peephole reset

s32 dbgMenuWazaMain(void)
{
    return 1;
}

#pragma force_active reset
