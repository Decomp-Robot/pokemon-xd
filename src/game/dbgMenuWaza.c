#include <GSthread.h>
#include <app/wazaSequence/wazaViewer.h>
#include <floor.h>
#include <floorManager.h>
#include <menu.h>
#include <types.h>

/* Waza (move) debug menu controls. */
#pragma force_active on

enum { FLOOR_ID_WAZA_VIEWER_DEBUG = 999 };

#pragma peephole off

int dbgMenuWazaDebugStop(void)
{
    if (wazaViewerData.debugRunning != 0) {
        GSfloorPop();
    }
    return 0;
}

int dbgMenuPokemonDebugSelect(void)
{
    struct WazaViewerWork *work;

    if (menuIsCheck(MENU_ID_DEBUG_POKEMON_SELECT)) {
        menuClose(MENU_ID_DEBUG_POKEMON_SELECT);
    } else {
        work = &wazaViewerData;
        work->pokemonSelectionState = 0;
        work->moveSelectionState = 0;
        menuOpenCustom(MENU_ID_DEBUG_POKEMON_SELECT, 0, 0, 0, 1, 0);
        menuSetPosition(MENU_ID_DEBUG_POKEMON_SELECT, 12, 10);
    }
    return 0;
}

int dbgMenuWazaDebugSelect(void)
{
    if (menuIsCheck(MENU_ID_DEBUG_MOVE_SELECT)) {
        menuClose(MENU_ID_DEBUG_MOVE_SELECT);
    } else {
        menuOpenCustom(MENU_ID_DEBUG_MOVE_SELECT, 0, 0, 0, 1, 0);
        menuSetPosition(MENU_ID_DEBUG_MOVE_SELECT, 12, 10);
    }
    return 0;
}

int dbgMenuWazaDebugStart(void)
{
    if (menuIsCheck(MENU_ID_TITLE)) {
        menuClose(MENU_ID_TITLE);
    }
    if (wazaViewerData.debugRunning == 0) {
        GSfloorPush(FLOOR_ID_WAZA_VIEWER_DEBUG);
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
