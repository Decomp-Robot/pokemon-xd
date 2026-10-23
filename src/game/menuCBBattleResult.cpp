#include <winMsg.h>
#include <pokecolo.h>
#include <toolentry.h>
#include <msgctrl.h>
#include <gbaCommand.h>
#include <menuCBBios.h>
#include <types.h>

/*
 * menuCBBattleResult.cpp
 *
 * The result screen of a link (GBA) battle: it shows how the battle that was
 * just played ended and, while the players still have to pick where their
 * Pokemon go, runs the selection dialogue.
 */

/* Selection lists handed to menuSubOpenSelect*: the message IDs of the
   choices, one list per number of players. */
extern u32 submenu_msg2[3];
extern u32 submenu_msg3[4];
extern u32 submenu_msg1[3];

u32 submenu_msg2[3] = { 0x4300, 0x4302, 0x4303 };
u32 submenu_msg3[4] = { 0x4300, 0x4301, 0x4302, 0x4303 };
u32 submenu_msg1[3] = { 0x4300, 0x4301, 0x4303 };

extern u32 menuCBHeroTransferChoices[2];
extern u32 menuCBRegionalResultChoices[2];

/* Cleared when a link-battle menu closes. */
extern s32 menuCBResultCompletionState;

/* -------------------------------------------------------------------------
 * External functions
 * ---------------------------------------------------------------------- */

extern "C" {
s32 toolentryTaisenGetBattleMode(void);
s32 toolentryTaisenGetBattleType(void);
s32 toolentryTaisenGetEntryPlayerNum(void);
s32 toolentryTaisenGetControlerType(s32 index);
void toolentryCopyHero(void);

void menuSetEnablePort(s32 port);
s32 menuSubOpenSelect_gba(u8 type, const u32 *selectList, s32 listNum, s16 x,
                          s16 y, s32 work, s32 cursor, u8 a, u8 b);
s32 menuSubOpenSelect3(u8 type, const u32 *selectList, s32 listNum, s16 x,
                       s16 y, s32 work, s32 cursor, u8 a, u8 b);
void GSthreadSwitch(void);
u8 GSinputIsConnected(s32 pad);
s32 GSinputGetSubType(s32 pad);
}

/* -------------------------------------------------------------------------
 * Functions of this module
 * ---------------------------------------------------------------------- */

/* Menu callback that runs the link-battle result screen the current battle
   mode asks for. */
extern "C" void menuCB_BattleResult(_MENUCBWORK *work);

u16 _menuCB_BattleResultGetHomePlace(void);
u8 _menuCBResultCheckController1sub(void);
void _menuCBResultCheckController1(void);
s32 _menuCBResultCheckGBAPort(void);

extern "C" s32 menuCBResultMain(_MENUCBWORK *work);
s32 _menuCB_BattleResultTonikaku(_MENUCBWORK *work);
s32 _menuCBResultGetHomePlace(void);

/* -------------------------------------------------------------------------
 * Battle result screen
 * ---------------------------------------------------------------------- */

extern "C" void menuCB_BattleResult(_MENUCBWORK *work)
{
    switch (toolentryTaisenGetBattleMode()) {
    case 1:
        _menuCB_BattleResultTonikaku(work);
        break;
    case 0:
    default:
        menuCBResultMain(work);
        break;
    }
}

/*
 * Battle result screen: shows the result of the battle that was just played
 * and, while the players still have to pick where their Pokemon go, runs the
 * selection dialogue. Returns the menu to open next.
 */
extern "C" s32 menuCBResultMain(_MENUCBWORK *work)
{
    s32 homePlace = -1;
    u16 battleResult = battleGetResult();
    u8 shown = 0;
    s32 state;
    s32 menuID;

    menuSetEnablePort(0);

    for (;;) {
        switch (battleResult) {
        case 1: {
            menuSetEnablePort(1);

            if (_menuCB_BattleResultGetHomePlace() == 0) {
                s32 gbaPort = _menuCBResultCheckGBAPort();

                msgctrlSetValue(0x2F, gbaPort);
                winMsgOpen(2, 0x442B, 1, 1);
                winMsgClose(1);
                shown = 1;
            } else {
                _menuCBResultCheckGBAPort();
                winMsgOpen(2, 0x442D, 1, 1);
                _menuCBResultCheckController1();
                winMsgClose(1);
                shown = 1;
            }
            break;
        }
        }
        if (shown != 0) {
            return 0xA9;
        }

        _CB.selectResult = 0;
        _CB.confirmationComplete = 0;
        menuSetEnablePort(1);
        winMsgOpen(2, 0x42FF, 1, 1);

        if (work->battleMode != 2) {
            switch (pokecoloGetCountryCode()) {
            case 0:
                _CB.selectResult = menuSubOpenSelect_gba(
                    0, submenu_msg2, 3, 0x3E, 0x89, 0, 3, 1, 1);
                break;
            case 1:
            case 2:
                _CB.selectResult = menuSubOpenSelect_gba(
                    0, &menuCBRegionalResultChoices[0], 2, 0x3E, 0x89, 0, 2, 1, 1);
                break;
            }
            if (_CB.selectResult == -2) {
                battleResult = 1;
                continue;
            }
            if (_CB.selectResult == -1) {
                _CB.selectResult = 2;
            }
            switch (pokecoloGetCountryCode()) {
            case 0:
                switch (_CB.selectResult) {
                case 0:
                    homePlace = 0;
                    break;
                case 1:
                    homePlace = 2;
                    break;
                case 2:
                    homePlace = -1;
                    break;
                }
                break;
            case 1:
            case 2:
                switch (_CB.selectResult) {
                case 0:
                    homePlace = 0;
                    break;
                case 1:
                default:
                    homePlace = -1;
                    break;
                }
                break;
            }
        } else {
            /* Four players: as many choices as there are players. */
            switch (pokecoloGetCountryCode()) {
            case 0:
                _CB.selectResult = menuSubOpenSelect_gba(
                    0, submenu_msg3, 4, 0x3E, 0x89, 0, 4, 1, 1);
                break;
            case 1:
            case 2:
                _CB.selectResult = menuSubOpenSelect_gba(
                    0, submenu_msg1, 3, 0x3E, 0x89, 0, 3, 1, 1);
                break;
            }
            if (_CB.selectResult == -2) {
                battleResult = 1;
                continue;
            }
            if (_CB.selectResult == -1) {
                _CB.selectResult = 3;
            }
            switch (pokecoloGetCountryCode()) {
            case 0:
                switch (_CB.selectResult) {
                case 0:
                    homePlace = 0;
                    break;
                case 1:
                    homePlace = 1;
                    break;
                case 2:
                    homePlace = 2;
                    break;
                case 3:
                default:
                    homePlace = -1;
                    break;
                }
                break;
            case 1:
            case 2:
                switch (_CB.selectResult) {
                case 0:
                    homePlace = 0;
                    break;
                case 1:
                    homePlace = 1;
                    break;
                case 2:
                default:
                    homePlace = -1;
                    break;
                }
                break;
            }
        }

        switch (homePlace) {
        case 0:
            menuID = 0xB1;
            break;
        case 1:
            menuID = 0xAC;
            break;
        case 2:
            menuID = 0xA9;
            break;
        case -1:
        default:
            menuID = -1;
            break;
        }

        winMsgClose(1);

        if (menuID == -1 || menuID == 0xA9) {
            state = 0;
            s32 running = 1;
            u16 port = _menuCBResultGetHomePlace();

            do {
                switch (state) {
                case 0:
                    if ((s32)port == 0) {
                        state = 1;
                    } else {
                        state = 2;
                    }
                    break;
                case 1:
                    winMsgOpen(2, 0x442C, 1, 1);
                    winMsgClose(1);
                    state = 4;
                    break;
                case 2:
                    winMsgOpen(2, 0x442D, 1, 1);
                    _menuCBResultCheckController1();
                    winMsgClose(1);
                    if (_menuCBResultCheckController1sub() != 0) {
                        state = 4;
                    } else {
                        state = 3;
                    }
                    break;
                case 3:
                    winMsgOpen(2, 0x4436, 1, 1);
                    _menuCBResultCheckController1();
                    winMsgClose(1);
                    state = 4;
                    break;
                case 4:
                    running = 0;
                    break;
                }
            } while (running != 0);
        }

        menuCBResultCompletionState = 0;
        return menuID;
    }
}

/* Non-zero when the Pokemon the player entered were caught by somebody else,
   so that their home is not the player the game started with. */
u16 _menuCB_BattleResultGetHomePlace(void)
{
    if (toolentryTaisenGetBattleType() == 2) {
        return toolentryTaisenGetHomePlace(0) != 0;
    }
    return 0;
}

/* Result flow for the battle modes that ask again before the fight: the
   players pick whether to fight with the entered hero Pokemon. Yes copies the
   hero into the battle entries and goes to the battle; no leaves the screen. */
s32 _menuCB_BattleResultTonikaku(_MENUCBWORK *work)
{
    _CB.selectResult = 0;
    _CB.confirmationComplete = 0;

    _CB.selectResult =
        menuSubOpenSelect3(0, menuCBHeroTransferChoices, 2, 0x3E, 0x89, 0, 2, 1, 1);
    if (_CB.selectResult == -1) {
        _CB.selectResult = 1;
    }

    switch (_CB.selectResult) {
    case 0:
        toolentryCopyHero();
        return 0xB1;
    case 1:
    default:
        return -1;
    }
}

/* Twin of _menuCB_BattleResultGetHomePlace, used by the battle result screen while it
   waits for the players. */
s32 _menuCBResultGetHomePlace(void)
{
    if (toolentryTaisenGetBattleType() == 2) {
        return toolentryTaisenGetHomePlace(0) != 0;
    }
    return 0;
}

/* Wait for controller 1 to reach the state the caller is waiting for. */
void _menuCBResultCheckController1(void)
{
    s32 wait = 1;

    do {
        if (_menuCBResultCheckController1sub() != 0) {
            wait = 0;
        } else {
            GSthreadSwitch();
        }
    } while (wait != 0);
}

/* Non-zero once controller 1 is connected and is not a GBA. */
u8 _menuCBResultCheckController1sub(void)
{
    if (GSinputIsConnected(1) != 0) {
        return GSinputGetSubType(1) == 0;
    }
    return 0;
}

/*
 * First player whose GBA is not answering, or 2 when there is none. Only the
 * players that own the Pokemon they entered are checked.
 */
s32 _menuCBResultCheckGBAPort(void)
{
    s32 playerNum = toolentryTaisenGetEntryPlayerNum();
    s32 type;
    s32 i;

    toolentryTaisenGetBattleMode();
    for (i = 0; i < playerNum; i++) {
        u16 home = toolentryTaisenGetHomePlace(i);

        type = toolentryTaisenGetControlerType(i);

        if (type == 0) {
            continue;
        }
        if ((u16)(home - 1) > 1 && home != 3) {
            continue;
        }
        if (!gbaCommandIsConnected(type)) {
            return type;
        }
    }
    return 2;
}
