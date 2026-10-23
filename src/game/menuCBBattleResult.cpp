#include <types.h>

/*
 * menuCBBattleResult.cpp
 *
 * The result screen of a link (GBA) battle: it shows how the battle that was
 * just played ended and, while the players still have to pick where their
 * Pokemon go, runs the selection dialogue.
 *
 * The module is a set of free functions. The entry points the menu system
 * calls are C functions; the helpers the screen code keeps to itself are C++
 * (which is why their linkage names are mangled).
 */

/* -------------------------------------------------------------------------
 * Work areas
 * ---------------------------------------------------------------------- */

/*
 * Work area for the link battles, shared with the menus that are still
 * undecoded: its address is the base for the constant-offset accesses all
 * over the link-battle menu code, so only the slots the result screen touches
 * are named and the rest is left as padding. (The trainer records the battle code
 * walks live at the start of the area: +0x18, 0x1320 bytes each.)
 */
typedef struct MenuCBTaisenWork {
    /* 0x0000 */ u8 unk_0000[0x8];
    /* 0x0008 */ s32 battleMode;
    /* 0x000C */ u8 unk_000C[0xAFE0 - 0x0C];
    /* 0xAFE0 */ s32 selectResult;
    /* 0xAFE4 */ u8 unk_AFE4[0x7C];
    /* 0xB060 */ s32 unk_B060;
} MenuCBTaisenWork;

extern MenuCBTaisenWork lbl_804299F8;

/* Selection lists handed to menuSubOpenSelect*: the message IDs of the
   choices, one list per number of players. */
extern u32 submenu_msg2[3];
extern u32 submenu_msg3[4];
extern u32 submenu_msg1[3];

u32 submenu_msg2[3] = { 0x4300, 0x4302, 0x4303 };
u32 submenu_msg3[4] = { 0x4300, 0x4301, 0x4302, 0x4303 };
u32 submenu_msg1[3] = { 0x4300, 0x4301, 0x4303 };

extern u32 lbl_804E7F40[2];
extern u32 lbl_804E7F48[2];

/* Cleared when a link-battle menu closes. */
extern s32 lbl_804EA828;

/* -------------------------------------------------------------------------
 * External functions
 * ---------------------------------------------------------------------- */

extern "C" {
s32 toolentryTaisenGetBattleMode(void);
s32 toolentryTaisenGetBattleType(void);
s32 toolentryTaisenGetEntryPlayerNum(void);
s32 toolentryTaisenGetControlerType(s32 index);
void toolentryCopyHero(void);
u16 zz_0231e2c_(s32 index);
s32 zz_get_battle_result(void);
s32 zz_game_get_region(void);

void menuSetEnablePort(s32 port);
void msgctrlSetValue(s32 index, s32 value);
void winMsgOpen(s32 a, s32 b, s32 c, s32 d);
void winMsgClose(s32 id);
s32 menuSubOpenSelect_gba(u8 type, const u32 *selectList, s32 listNum, s16 x,
                          s16 y, s32 work, s32 cursor, u8 a, u8 b);
s32 menuSubOpenSelect3(u8 type, const u32 *selectList, s32 listNum, s16 x,
                       s16 y, s32 work, s32 cursor, u8 a, u8 b);
void GSthreadSwitch(void);
u8 GSinputIsConnected(s32 pad);
s32 GSinputGetSubType(s32 pad);
u8 gbaCommandIsConnected(s32 port);
}

/* -------------------------------------------------------------------------
 * Functions of this module
 * ---------------------------------------------------------------------- */

/* Menu callback that runs the link-battle result screen the current battle
   mode asks for. */
extern "C" void menuCB_BattleResult(MenuCBTaisenWork *work);

u16 _menuCBResultGetHomePlace(void);
u8 _menuCBResultCheckController1sub(void);
void _menuCBResultCheckController1(void);
s32 _menuCBResultCheckGBAPort(void);

/* No-op default constructor of the trainer records that live in the work
   area (built by the startup constructor, see fn_80046530). */
extern "C" void menuTaisenTrainerCtor(void);
extern "C" s32 menuCBResultMain(MenuCBTaisenWork *work);
extern "C" s32 menuCBResultSelectHero(MenuCBTaisenWork *work);
extern "C" s32 menuCBResultCheckHomePlace(void);

/* -------------------------------------------------------------------------
 * Battle result screen
 * ---------------------------------------------------------------------- */

/* Nothing to fill in: the trainer records only need zeroed memory, which the
   bss work area already is. */
extern "C" void menuTaisenTrainerCtor(void)
{
}

extern "C" void menuCB_BattleResult(MenuCBTaisenWork *work)
{
    switch (toolentryTaisenGetBattleMode()) {
    case 1:
        menuCBResultSelectHero(work);
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
extern "C" s32 menuCBResultMain(MenuCBTaisenWork *work)
{
    s32 homePlace = -1;
    s32 battleResult = zz_get_battle_result();
    u8 shown = 0;
    s32 state;
    s32 menuID;

    menuSetEnablePort(0);

    for (;;) {
        switch ((s32)(u16)battleResult) {
        case 1: {
            menuSetEnablePort(1);

            if (_menuCBResultGetHomePlace() == 0) {
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

        lbl_804299F8.selectResult = 0;
        lbl_804299F8.unk_B060 = 0;
        menuSetEnablePort(1);
        winMsgOpen(2, 0x42FF, 1, 1);

        if (work->battleMode != 2) {
            switch (zz_game_get_region()) {
            case 0:
                lbl_804299F8.selectResult = menuSubOpenSelect_gba(
                    0, submenu_msg2, 3, 0x3E, 0x89, 0, 3, 1, 1);
                break;
            case 1:
            case 2:
                lbl_804299F8.selectResult = menuSubOpenSelect_gba(
                    0, &lbl_804E7F48[0], 2, 0x3E, 0x89, 0, 2, 1, 1);
                break;
            }
            if (lbl_804299F8.selectResult == -2) {
                battleResult = 1;
                continue;
            }
            if (lbl_804299F8.selectResult == -1) {
                lbl_804299F8.selectResult = 2;
            }
            switch (zz_game_get_region()) {
            case 0:
                switch (lbl_804299F8.selectResult) {
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
                switch (lbl_804299F8.selectResult) {
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
            switch (zz_game_get_region()) {
            case 0:
                lbl_804299F8.selectResult = menuSubOpenSelect_gba(
                    0, submenu_msg3, 4, 0x3E, 0x89, 0, 4, 1, 1);
                break;
            case 1:
            case 2:
                lbl_804299F8.selectResult = menuSubOpenSelect_gba(
                    0, submenu_msg1, 3, 0x3E, 0x89, 0, 3, 1, 1);
                break;
            }
            if (lbl_804299F8.selectResult == -2) {
                battleResult = 1;
                continue;
            }
            if (lbl_804299F8.selectResult == -1) {
                lbl_804299F8.selectResult = 3;
            }
            switch (zz_game_get_region()) {
            case 0:
                switch (lbl_804299F8.selectResult) {
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
                switch (lbl_804299F8.selectResult) {
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
            u16 port = menuCBResultCheckHomePlace();

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

        lbl_804EA828 = 0;
        return menuID;
    }
}

/* Non-zero when the Pokemon the player entered were caught by somebody else,
   so that their home is not the player the game started with. */
u16 _menuCBResultGetHomePlace(void)
{
    if (toolentryTaisenGetBattleType() == 2) {
        return zz_0231e2c_(0) != 0;
    }
    return 0;
}

/* Result flow for the battle modes that ask again before the fight: the
   players pick whether to fight with the entered hero Pokemon. Yes copies the
   hero into the battle entries and goes to the battle; no leaves the screen. */
extern "C" s32 menuCBResultSelectHero(MenuCBTaisenWork *work)
{
    lbl_804299F8.selectResult = 0;
    lbl_804299F8.unk_B060 = 0;

    lbl_804299F8.selectResult =
        menuSubOpenSelect3(0, lbl_804E7F40, 2, 0x3E, 0x89, 0, 2, 1, 1);
    if (lbl_804299F8.selectResult == -1) {
        lbl_804299F8.selectResult = 1;
    }

    switch (lbl_804299F8.selectResult) {
    case 0:
        toolentryCopyHero();
        return 0xB1;
    case 1:
    default:
        return -1;
    }
}

/* Twin of _menuCBResultGetHomePlace, used by the battle result screen while it
   waits for the players. */
extern "C" s32 menuCBResultCheckHomePlace(void)
{
    if (toolentryTaisenGetBattleType() == 2) {
        return zz_0231e2c_(0) != 0;
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
        u16 home = zz_0231e2c_(i);

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
