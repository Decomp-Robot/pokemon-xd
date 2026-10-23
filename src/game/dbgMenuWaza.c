#include <menu.h>
#include <floor.h>
#include <types.h>

/* Waza (move) debug sub menus. */
#pragma force_active on

/* Debug work block the waza debug menus share (0x38 bytes). */
struct DbgMenuWork {
    u8 pad0[0x16];
    u8 mPokemonSelectDirty;  /* 0x16 */
    u8 mWazaSelectDirty;     /* 0x17 */
    u8 pad18[0x30 - 0x18];
    s32 mDebugRunning;       /* 0x30 */
};

extern u8 dbgMenuWazaWork[];

void menuSetPosition(s32 menuID, s32 x, s32 y);
void GSfloorPop(void);
void GSthreadSwitch(void);

#pragma peephole off

int dbgMenuWazaDebugStop(void)
{
    if (((struct DbgMenuWork *)dbgMenuWazaWork)->mDebugRunning != 0) {
        GSfloorPop();
    }
    return 0;
}

int dbgMenuPokemonDebugSelect(void)
{
    struct DbgMenuWork *work;

    if (menuIsOpen(MENU_ID_DEBUG_POKEMON_SELECT)) {
        menuClose(MENU_ID_DEBUG_POKEMON_SELECT);
    } else {
        work = (struct DbgMenuWork *)dbgMenuWazaWork;
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
    if (((struct DbgMenuWork *)dbgMenuWazaWork)->mDebugRunning == 0) {
        GSfloorPush(FLOOR_ID_WAZA_DEBUG);
        GSthreadSwitch();
    }
    return 0;
}

#pragma peephole reset


int dbgMenuWazaMain(void)
{
    return 1;
}

#pragma force_active reset
