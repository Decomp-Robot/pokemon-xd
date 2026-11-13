#include <types.h>
#include <window.h>
#include <winSprite.h>
#include <menu.h>
#include <menuFight.h>
#include <menuCBBios.h>
#include <menuWazaStatus.h>
#include <fightPokemon.h>

extern "C" s32 menuFightWazaDetailSlot[2];

enum {
    MENU_IWAZA_1P = 158,
    MENU_IWAIT_1P = 159,
    MENU_TARGET_1P = 160,
    MENU_IWAZA_2P = 161,
    MENU_IWAIT_2P = 162,
    MENU_TARGET_2P = 163,
    MENU_WAZA_DETAIL_1P = 276,
    MENU_WAZA_DETAIL_2P = 277,
    MENU_IKOUSAN_1P = 284,
    MENU_IKOUSAN_2P = 285,
    KEY_WAIT = 0x40,
    KEY_KOUSAN = 0x800,
    COMMAND_KOUSAN = 5,
    MENU_SOUND_CURSOR = 1,
    MENU_SOUND_ACCEPT = 2,
    MENU_SOUND_CANCEL = 3
};

extern "C" void menuFightCurIkousan(tagWINDOW_WORK *window)
{
    if (menuIsCheck(MENU_IKOUSAN_2P)) {
        window = windowSearchID(MENU_IKOUSAN_2P);
        tagWINDOW_KEY *key;
        if (menuCB_GetBattleController2P() == 1)
            key = windowGetPortKeyInfo(1);
        else
            key = windowGetPortKeyInfo(2);
        if (key->mKeyTrg & WINDOW_KEY_UP) {
            if (window->mCursor.y != 0) {
                window->mCursor.y--;
                menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
            }
        } else if (key->mKeyTrg & WINDOW_KEY_DOWN) {
            if (window->mCursor.y == 0) {
                window->mCursor.y++;
                menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
            }
        }
        if (menuCursorCmp(window->mPreviousCursor, window->mCursor)) {
            windowCreateCursorSprite(window);
            menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
        }
        window->mPreviousCursor = window->mCursor;
    }
    if (menuIsCheck(MENU_IKOUSAN_1P)) {
        window = windowSearchID(MENU_IKOUSAN_1P);
        tagWINDOW_KEY *key = windowGetPortKeyInfo(1);
        if (key->mKeyTrg & WINDOW_KEY_UP) {
            if (window->mCursor.y != 0) {
                window->mCursor.y--;
                menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
            }
        } else if (key->mKeyTrg & WINDOW_KEY_DOWN) {
            if (window->mCursor.y == 0) {
                window->mCursor.y++;
                menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
            }
        }
        if (menuCursorCmp(window->mPreviousCursor, window->mCursor)) {
            windowCreateCursorSprite(window);
            menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
        }
        window->mPreviousCursor = window->mCursor;
    }
}

extern "C" void menuFightButtonIwaza1p(tagWINDOW_WORK *window)
{
    tagWINDOW_KEY *key;
    MENU_WAZA_STATUS *status;
    s32 selectedIndex;
    Pokemon *pokemon;
    u32 stickTrigger, stickOn, leftKey;
    MENU_FIGHT_TARGET_SLOT *entries;

    if (menuIsCheck(MENU_IKOUSAN_2P)) {
        if (menuIsCheck(MENU_WAZA_DETAIL_2P))
            menuClose(MENU_WAZA_DETAIL_2P);
        if (menuIsCheck(MENU_IWAIT_2P))
            menuClose(MENU_IWAIT_2P);
        window = windowSearchID(MENU_IKOUSAN_2P);
        if (menuCB_GetBattleController2P() == 1)
            key = windowGetPortKeyInfo(1);
        else
            key = windowGetPortKeyInfo(2);
        if (key->mKeyTrg & WINDOW_KEY_ACCEPT) {
            window->mAccept = 1;
            window->mCursorIndex = window->mCursor.y;
            menuPlaySe(window->mMenuID, MENU_SOUND_ACCEPT);
            return;
        } else if (key->mKeyTrg & WINDOW_KEY_CANCEL) {
            window->mCancel = 1;
            window->mCursorIndex = -1;
            menuPlaySe(window->mMenuID, MENU_SOUND_CANCEL);
            return;
        }
        menuFightButtonTimeOutCheck(window);
    }
    if (menuIsCheck(MENU_IKOUSAN_1P)) {
        if (menuIsCheck(MENU_WAZA_DETAIL_1P))
            menuClose(MENU_WAZA_DETAIL_1P);
        if (menuIsCheck(MENU_IWAIT_1P))
            menuClose(MENU_IWAIT_1P);
        window = windowSearchID(MENU_IKOUSAN_1P);
        key = windowGetPortKeyInfo(1);
        if (key->mKeyTrg & WINDOW_KEY_ACCEPT) {
            window->mAccept = 1;
            window->mCursorIndex = window->mCursor.y;
            menuPlaySe(window->mMenuID, MENU_SOUND_ACCEPT);
        } else if (key->mKeyTrg & WINDOW_KEY_CANCEL) {
            window->mCancel = 1;
            window->mCursorIndex = -1;
            menuPlaySe(window->mMenuID, MENU_SOUND_CANCEL);
        }
        menuFightButtonTimeOutCheck(window);
    }
    selectedIndex = -1;
    if (menuIsCheck(MENU_IWAZA_2P)) {
        window = windowSearchID(MENU_IWAZA_2P);
        status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
        pokemon = fightOutPokemonGetPokemonPtr(status->fightOutPokemon);
        if (menuCB_GetBattleController2P() == 1) {
            key = windowGetPortKeyInfo(1);
            stickTrigger = _menuFightGetRightStickInfo(0, true);
            stickOn = _menuFightGetRightStickInfo(0, false);
        } else {
            key = windowGetPortKeyInfo(2);
            stickTrigger = _menuFightGetRightStickInfo(1, true);
            stickOn = _menuFightGetRightStickInfo(1, false);
        }
        if (menuIsCheck(MENU_IWAIT_2P)) {
            if (!(key->mKeyOn & KEY_WAIT)) {
                menuClose(MENU_IWAIT_2P);
                menuSetDisp(MENU_IWAZA_2P, true);
            }
        } else if ((u16)stickTrigger) {
            if ((u16)stickTrigger & WINDOW_KEY_UP) selectedIndex = 0;
            else if ((u16)stickTrigger & WINDOW_KEY_RIGHT) selectedIndex = 1;
            else if ((u16)stickTrigger & WINDOW_KEY_DOWN) selectedIndex = 2;
            else if ((u16)stickTrigger & WINDOW_KEY_LEFT) selectedIndex = 3;
            else if ((u16)stickTrigger & WINDOW_KEY_TRIGGER_L) selectedIndex = -1;
            if (key->mKeyOn & WINDOW_KEY_TRIGGER_R) {
                menuFightWazaDetailSlot[1] = selectedIndex;
                if (menuIsCheck(MENU_WAZA_DETAIL_2P))
                    menuClose(MENU_WAZA_DETAIL_2P);
                if (status->moves[menuFightWazaDetailSlot[1]].moveName != 0)
                    menuOpenCustom(MENU_WAZA_DETAIL_2P, window->mMenuID, 0, 0, false, 2,
                                   window->mMenuID, &menuFightWazaDetailSlot[1]);
            } else {
                if (menuIsCheck(MENU_WAZA_DETAIL_2P))
                    menuClose(MENU_WAZA_DETAIL_2P);
                if (!pokemonWazaCheckValid(pokemon, (u16)selectedIndex))
                    selectedIndex = -1;
                if (selectedIndex >= 0) {
                    window->mAccept = 1;
                    window->mCursorIndex = selectedIndex;
                }
            }
        } else if (menuIsCheck(MENU_WAZA_DETAIL_2P)) {
            if (!(key->mKeyOn & WINDOW_KEY_TRIGGER_R) || !(u16)stickOn)
                menuClose(MENU_WAZA_DETAIL_2P);
        } else if (key->mKeyOn & KEY_WAIT) {
            menuSetDisp(MENU_IWAZA_2P, false);
            menuOpen(MENU_IWAIT_2P, 0);
        } else if (key->mKeyTrg & WINDOW_KEY_TRIGGER_L) {
            window->mCancel = 1;
            window->mCursorIndex = -1;
        } else if (key->mKeyTrg & KEY_KOUSAN) {
            window->mAccept = 1;
            window->mCursorIndex = COMMAND_KOUSAN;
        }
        menuFightButtonTimeOutCheck(window);
    }
    selectedIndex = -1;
    if (menuIsCheck(MENU_IWAZA_1P)) {
        window = windowSearchID(MENU_IWAZA_1P);
        status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
        pokemon = fightOutPokemonGetPokemonPtr(status->fightOutPokemon);
        key = windowGetPortKeyInfo(1);
        stickTrigger = _menuFightGetRightStickInfo(0, true);
        stickOn = _menuFightGetRightStickInfo(0, false);
        if (menuIsCheck(MENU_IWAIT_1P)) {
            if (!(key->mKeyOn & KEY_WAIT)) {
                menuClose(MENU_IWAIT_1P);
                menuSetDisp(MENU_IWAZA_1P, true);
            }
        } else if ((u16)stickTrigger) {
            if ((u16)stickTrigger & WINDOW_KEY_UP) selectedIndex = 0;
            else if ((u16)stickTrigger & WINDOW_KEY_RIGHT) selectedIndex = 1;
            else if ((u16)stickTrigger & WINDOW_KEY_DOWN) selectedIndex = 2;
            else if ((u16)stickTrigger & WINDOW_KEY_LEFT) selectedIndex = 3;
            if (key->mKeyOn & WINDOW_KEY_TRIGGER_R) {
                menuFightWazaDetailSlot[0] = selectedIndex;
                if (menuIsCheck(MENU_WAZA_DETAIL_1P))
                    menuClose(MENU_WAZA_DETAIL_1P);
                if (status->moves[menuFightWazaDetailSlot[0]].moveName != 0)
                    menuOpenCustom(MENU_WAZA_DETAIL_1P, window->mMenuID, 0, 0, false, 2,
                                   window->mMenuID, &menuFightWazaDetailSlot[0]);
            } else {
                if (menuIsCheck(MENU_WAZA_DETAIL_1P))
                    menuClose(MENU_WAZA_DETAIL_1P);
                if (!pokemonWazaCheckValid(pokemon, (u16)selectedIndex))
                    selectedIndex = -1;
                if (selectedIndex >= 0) {
                    window->mAccept = 1;
                    window->mCursorIndex = selectedIndex;
                }
            }
        } else if (menuIsCheck(MENU_WAZA_DETAIL_1P)) {
            if (!(key->mKeyOn & WINDOW_KEY_TRIGGER_R) || !(u16)stickOn)
                menuClose(MENU_WAZA_DETAIL_1P);
        } else if (key->mKeyOn & KEY_WAIT) {
            menuSetDisp(MENU_IWAZA_1P, false);
            menuOpen(MENU_IWAIT_1P, 0);
        } else if (key->mKeyTrg & WINDOW_KEY_TRIGGER_L) {
            window->mCancel = 1;
            window->mCursorIndex = -1;
        } else if (key->mKeyTrg & KEY_KOUSAN) {
            window->mAccept = 1;
            window->mCursorIndex = COMMAND_KOUSAN;
        }
        menuFightButtonTimeOutCheck(window);
    }
    selectedIndex = -1;
    if (menuIsCheck(MENU_TARGET_2P)) {
        window = windowSearchID(MENU_TARGET_2P);
        entries = (MENU_FIGHT_TARGET_SLOT *)windowGetParam(window, 0);
        if (menuCB_GetBattleController2P() == 1) {
            key = windowGetPortKeyInfo(1);
            leftKey = _menuFightGetLeftKeyInfo(0);
        } else {
            key = windowGetPortKeyInfo(2);
            leftKey = _menuFightGetLeftKeyInfo(1);
        }
        entries = (MENU_FIGHT_TARGET_SLOT *)windowGetParam(window, 0);
        if (key->mKeyTrg) {
            if ((u16)leftKey & WINDOW_KEY_UP) selectedIndex = 2;
            else if ((u16)leftKey & WINDOW_KEY_RIGHT) selectedIndex = 3;
            else if ((u16)leftKey & WINDOW_KEY_LEFT) selectedIndex = 0;
            else if ((u16)leftKey & WINDOW_KEY_DOWN) selectedIndex = 1;
            else if (key->mKeyTrg & WINDOW_KEY_TRIGGER_L) {
                window->mAccept = 1;
                window->mCancel = 1;
                window->mCursorIndex = -1;
            }
        }
        if (selectedIndex >= 0) {
            for (s32 i = 0; i < 4; i++) {
                s32 direction;
                switch (entries[i].mItemID) {
                case TARGET_PANEL_LEFT: direction = 0; break;
                case TARGET_PANEL_DOWN: direction = 1; break;
                case TARGET_PANEL_UP: direction = 2; break;
                case TARGET_PANEL_RIGHT: direction = 3; break;
                default: direction = -1; break;
                }
                if (selectedIndex == direction) {
                    if (selectedIndex > 2) selectedIndex = 2;
                    window->mAccept = 1;
                    window->mCursorIndex = selectedIndex;
                    return;
                }
            }
        }
        menuFightButtonTimeOutCheck(window);
    }
    selectedIndex = -1;
    if (menuIsCheck(MENU_TARGET_1P)) {
        window = windowSearchID(MENU_TARGET_1P);
        entries = (MENU_FIGHT_TARGET_SLOT *)windowGetParam(window, 0);
        stickTrigger = _menuFightGetRightStickInfo(0, true);
        leftKey = _menuFightGetLeftKeyInfo(0);
        key = windowGetPortKeyInfo(1);
        if (key->mKeyTrg) {
            if ((u16)leftKey & WINDOW_KEY_UP) selectedIndex = 0;
            else if ((u16)leftKey & WINDOW_KEY_RIGHT) selectedIndex = 1;
            else if ((u16)leftKey & WINDOW_KEY_DOWN) selectedIndex = 2;
            else if ((u16)leftKey & WINDOW_KEY_LEFT) selectedIndex = 3;
            else if (key->mKeyTrg & WINDOW_KEY_TRIGGER_L) {
                window->mAccept = 1;
                window->mCancel = 1;
                window->mCursorIndex = -1;
            }
        }
        if (selectedIndex >= 0) {
            for (s32 i = 0; i < 4; i++) {
                s32 direction;
                switch (entries[i].mItemID) {
                case TARGET_PANEL_LEFT: direction = 3; break;
                case TARGET_PANEL_DOWN: direction = 2; break;
                case TARGET_PANEL_UP: direction = 0; break;
                case TARGET_PANEL_RIGHT: direction = 1; break;
                default: direction = -1; break;
                }
                if (selectedIndex == direction) {
                    if (selectedIndex > 2) selectedIndex = 2;
                    window->mAccept = 1;
                    window->mCursorIndex = selectedIndex;
                    return;
                }
            }
        }
        menuFightButtonTimeOutCheck(window);
    }
}
