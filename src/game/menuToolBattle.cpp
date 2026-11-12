#include <menuToolBattle.h>
#include <menuCBBattle.h>
#include <FightResult.h>
#include <menu.h>
#include <menuCBBios.h>
#include <menuCB_Sub1.h>
#include <menuCBCommon.h>
#include <menuCBPokemonEntry.h>
#include <menuCBBattleStart.h>
#include <flashsystem.h>
#include <fight.h>
#include <GSfsys.h>
#include <GSres.h>
#include <GSthread.h>
#include <floorManager.h>
#include <savedata.h>
#include <hero.h>
#include <winSprite.h>
#include <toolentry.h>
#include <pause.h>

enum {
    CB_CONNECT_SKIP_ENTRY = 1,
    CB_BATTLE_FOUR_PLAYERS = 2,
    CB_BATTLE_RANDOM_STAGE = 6,
    CB_MENU_ROOT = 169,
    CB_MENU_RESULT = 172,
    CB_MENU_BATTLE = 177,
    CB_MENU_PARTY_MULTI = 178,
    CB_MENU_PARTY_TWO = 179,
    CB_MENU_PARTY_ONE = 180,
    CB_MENU_PARTY_BACKGROUND = 259,
    CB_MENU_FLASH = 261,
    CB_MENU_WINLOSE = 272,
    CB_MENU_VS_END = 275,
    CB_RESOURCE_GROUP = 20,
    CB_BATTLE_RESOURCE_GROUP = 2114,
    CB_SOUND_RESULT = 1504,
    CB_SOUND_MENU_TRANSITION = 20
};
const u32 CB_FLASH = 0x20093000;
const u32 CB_FLASH_VS = 0x21533000;
const u32 CB_FLASH_WINLOSE = 0x21543000;
const u32 CB_BATTLE_GC_RESOURCE = 0x184F3400;
const u32 CB_BATTLE_GBA_RESOURCE = 0x195F3400;

void _menuCB_FlashEnd_WINLOSE(void)
{
    if (menuIsCheck(CB_MENU_WINLOSE)) {
        menuCloseCustom(CB_MENU_WINLOSE, 0, false);
    }
}

void _menuCB_FlashStart_WINLOSE(void)
{
    switch (fightGetFightResultId()) {
    case kFightResultHostVictory:
    case kFightResultWinLose:
        FlashSystemSetPlayFrameLabel(CB_FLASH_WINLOSE, "winlose_start", "winlose_stop");
        FlashSystemSetNowFrameLabel(CB_FLASH_WINLOSE, "winlose_start");
        break;
    case kFightResultOtherSideVictory:
    case kFightResultLoseWin:
        FlashSystemSetPlayFrameLabel(CB_FLASH_WINLOSE, "losewin_start", "losewin_stop");
        FlashSystemSetNowFrameLabel(CB_FLASH_WINLOSE, "losewin_start");
        break;
    case kFightResultDraw:
    case kFightResultOtherDraw:
    default:
        FlashSystemSetPlayFrameLabel(CB_FLASH_WINLOSE, "draw_start", "draw_stop");
        FlashSystemSetNowFrameLabel(CB_FLASH_WINLOSE, "draw_start");
        break;
    }
    FlashSystemSetLoopFlag(CB_FLASH_WINLOSE, 0);
    _CB.flashStartWinLose = 1;
    menuOpen(CB_MENU_WINLOSE, 0);
    menuCB_SE_Play(CB_SOUND_RESULT);
    do {
        GSthreadSwitch();
        if (!FlashSystemIsLastFrame(CB_FLASH_WINLOSE)) {
            continue;
        }
        if (!pauseIsPause()) {
            _CB.battleStartWait--;
        }
        if (_CB.battleStartWait == 0 ||
            (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL)) {
            break;
        }
    } while (true);
}

void _menuCB_FlashEnd_VS(bool wait)
{
    if (wait) {
        menuOpen(CB_MENU_VS_END, 0);
        do {
            GSthreadSwitch();
        } while (winSeqCheckMove(CB_MENU_VS_END));
    } else {
        _CB.flashEndVS = 1;
    }
}

void _menuCB_FlashStart_VS(bool play)
{
    if (play) {
        FlashSystemSetPlayFrameLabel(CB_FLASH_VS, "start", "end_keep");
        FlashSystemSetNowFrameLabel(CB_FLASH_VS, "start");
    } else {
        FlashSystemSetPlayFrameLabel(CB_FLASH_VS, "end_keep", "end_keep");
        FlashSystemSetNowFrameLabel(CB_FLASH_VS, "end_keep");
    }
    _CB.flashStartVS = 1;
    _CB.flashEndVS = 0;
}

void _menuCB_FlashEnd(void)
{
    if (menuIsCheck(CB_MENU_FLASH)) {
        menuCloseCustom(CB_MENU_FLASH, 0, false);
    }
}

void _menuCB_FlashStart(bool play)
{
    if (play) {
        FlashSystemSetPlayFrameLabel(CB_FLASH, "start", "end");
        FlashSystemSetNowFrameLabel(CB_FLASH, "start");
    } else {
        FlashSystemSetPlayFrameLabel(CB_FLASH, "end", "end");
        FlashSystemSetNowFrameLabel(CB_FLASH, "end");
    }
    _CB.flashStart = 1;
    menuOpen(CB_MENU_FLASH, 0);
    do {
        GSthreadSwitch();
    } while (!FlashSystemIsLastFrame(CB_FLASH));
}

extern "C" void menuToolBattleExit(void)
{
    if (_CB.connectMode == CB_CONNECT_SKIP_ENTRY) {
        if (!fightReadResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GC_RESOURCE)) {
            fightFreeResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GC_RESOURCE);
        }
    } else {
        if (!fightReadResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GBA_RESOURCE)) {
            fightFreeResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GBA_RESOURCE);
        }
    }
    GSfsysFree(CB_RESOURCE_GROUP);
    GSresFreeGroup(CB_RESOURCE_GROUP);
}

extern "C" void menuToolBattleMain(void)
{
    bool repeat = true;
    int nextMenu;
    _menuCB_FlashStart(true);
    Hero *hero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
    u8 heroStyle = hero->mHeroStyle;
    hero->mHeroStyle = HERO_STYLE_NORMAL;
    while (repeat) {
        menuCBBios_CurrentlyWorkingToolBattleSet(1);
        menuCBBattleStartInit(&_CB, MENU_CB_BATTLE_START_PARTY);
        _CB.battleDisconnected = 0;
        _CB.battleStartWait = 15;
        menuCBPokemonEntryPokemonFaceFree();
        menuCBPokemonEntryLoadTex();
        menuCBpokemonFaceReadWait();
        menuCBBattleStartTrainerFaceFree();
        menuCBBattleStartTrainerLoadTex();
        menuCBtrainerFaceReadWait();
        if (_CB.battleMode == CB_BATTLE_FOUR_PLAYERS) {
            menuOpen(CB_MENU_BATTLE, 0);
            _menuCB_FlashStart_VS(true);
            menuOpen(CB_MENU_PARTY_BACKGROUND, 0);
            menuOpen(CB_MENU_PARTY_MULTI, 1);
            _menuCB_FlashEnd_VS(false);
            menuCloseCustom(CB_MENU_PARTY_MULTI, 0, false);
            menuCloseCustom(CB_MENU_PARTY_BACKGROUND, 0, false);
            menuCloseCustom(CB_MENU_BATTLE, 0, true);
        } else if (menuCB_BattleBA_PokemonEntryNum()) {
            menuOpen(CB_MENU_BATTLE, 0);
            _menuCB_FlashStart_VS(true);
            menuOpen(CB_MENU_PARTY_BACKGROUND, 0);
            menuOpen(CB_MENU_PARTY_TWO, 1);
            _menuCB_FlashEnd_VS(false);
            menuCloseCustom(CB_MENU_PARTY_TWO, 0, false);
            menuCloseCustom(CB_MENU_PARTY_BACKGROUND, 0, false);
            menuCloseCustom(CB_MENU_BATTLE, 0, true);
        } else {
            menuOpen(CB_MENU_BATTLE, 0);
            _menuCB_FlashStart_VS(true);
            menuOpen(CB_MENU_PARTY_BACKGROUND, 0);
            menuOpen(CB_MENU_PARTY_ONE, 1);
            if (_CB.connectMode != CB_CONNECT_SKIP_ENTRY) {
                _menuCB_FlashEnd_VS(false);
                menuCloseCustom(CB_MENU_PARTY_ONE, 0, false);
                menuCloseCustom(CB_MENU_PARTY_BACKGROUND, 0, false);
                menuCloseCustom(CB_MENU_BATTLE, 0, true);
            }
        }
        if (_CB.battleDisconnected) {
            menuCB_GBADisconnectedErrorJob();
            menuCB_MenuRollback(CB_MENU_ROOT);
            break;
        }
        nextMenu = _CB.connectMode == CB_CONNECT_SKIP_ENTRY ? 0 : menuCB_PokemonEntry(&_CB);
        switch (nextMenu) {
        case CB_MENU_ROOT:
            menuCB_MenuRollback(nextMenu);
            goto end;
        case CB_MENU_BATTLE:
        default:
            break;
        }
        GSfsysFree(CB_RESOURCE_GROUP);
        GSresFreeGroup(CB_RESOURCE_GROUP);
        _CB.skipBattleEffect = 1;
        _CB.battleResult = menuCB_Battle(&_CB);
        _CB.skipBattleEffect = 0;
        _menuCB_FlashStart(false);
        menuCBBattleEndSetPokemonHP();
        menuCBBattleStartInit(&_CB, MENU_CB_BATTLE_START_ENTERED_PARTY);
        _CB.battleDisconnected = 0;
        _CB.battleStartWait = 180;
        menuCBPokemonEntryPokemonFaceFree();
        menuCBPokemonEntryLoadTex();
        menuCBpokemonFaceReadWait();
        menuCBBattleStartTrainerFaceFree();
        menuCBBattleStartTrainerLoadTex();
        menuCBtrainerFaceReadWait();
        menuCB_SE_Play(CB_SOUND_MENU_TRANSITION);
        if (_CB.battleMode == CB_BATTLE_FOUR_PLAYERS) {
            menuOpen(CB_MENU_BATTLE, 0);
            _menuCB_FlashStart_VS(false);
            menuOpen(CB_MENU_PARTY_BACKGROUND, 0);
            menuOpen(CB_MENU_PARTY_MULTI, 1);
            _menuCB_FlashStart_WINLOSE();
            nextMenu = menuCB_BattleResult(&_CB);
            _menuCB_FlashEnd_VS(true);
            _menuCB_FlashEnd_WINLOSE();
            if (nextMenu != CB_MENU_BATTLE) {
                _menuCB_FlashEnd();
            }
            menuCloseCustom(CB_MENU_PARTY_MULTI, 0, false);
            menuCloseCustom(CB_MENU_PARTY_BACKGROUND, 0, false);
            menuCloseCustom(CB_MENU_BATTLE, 0, true);
        } else if (menuCB_BattleBA_PokemonEntryNum()) {
            menuOpen(CB_MENU_BATTLE, 0);
            _menuCB_FlashStart_VS(false);
            menuOpen(CB_MENU_PARTY_BACKGROUND, 0);
            menuOpen(CB_MENU_PARTY_TWO, 1);
            _menuCB_FlashStart_WINLOSE();
            nextMenu = menuCB_BattleResult(&_CB);
            _menuCB_FlashEnd_VS(true);
            _menuCB_FlashEnd_WINLOSE();
            if (nextMenu != CB_MENU_BATTLE) {
                _menuCB_FlashEnd();
            }
            menuCloseCustom(CB_MENU_PARTY_TWO, 0, false);
            menuCloseCustom(CB_MENU_PARTY_BACKGROUND, 0, false);
            menuCloseCustom(CB_MENU_BATTLE, 0, true);
        } else {
            menuOpen(CB_MENU_BATTLE, 0);
            _menuCB_FlashStart_VS(false);
            menuOpen(CB_MENU_PARTY_BACKGROUND, 0);
            menuOpen(CB_MENU_PARTY_ONE, 1);
            _menuCB_FlashStart_WINLOSE();
            nextMenu = menuCB_BattleResult(&_CB);
            _menuCB_FlashEnd_VS(true);
            _menuCB_FlashEnd_WINLOSE();
            if (nextMenu != CB_MENU_BATTLE) {
                _menuCB_FlashEnd();
            }
            menuCloseCustom(CB_MENU_PARTY_ONE, 0, false);
            menuCloseCustom(CB_MENU_PARTY_BACKGROUND, 0, false);
            menuCloseCustom(CB_MENU_BATTLE, 0, true);
        }
        if (nextMenu == CB_MENU_BATTLE || nextMenu == CB_MENU_RESULT) {
            if (_CB.connectMode != CB_CONNECT_SKIP_ENTRY) {
                if (_CB.battleStageMode == CB_BATTLE_RANDOM_STAGE) {
                    menuCB_InitBattle(&_CB);
                }
            } else {
                menuCB_InitBattle(&_CB);
            }
            if (_CB.battleDisconnected) {
                menuCB_GBADisconnectedErrorJob();
                menuCB_MenuRollback(CB_MENU_ROOT);
                break;
            }
        }
        switch (nextMenu) {
        case CB_MENU_BATTLE:
            toolentryCopyHero();
            break;
        case -1:
        case CB_MENU_ROOT:
        case CB_MENU_RESULT:
        default:
            menuCB_MenuRollback(nextMenu);
            repeat = false;
            break;
        }
    }
end:
    _menuCB_FlashEnd();
    menuCBBattleStartTrainerFaceFree();
    ((Hero *)savedataGetStatus(0, SAVE_DATA_HERO))->mHeroStyle = heroStyle;
    if (GSfloorIsFloorPushed()) {
        GSfloorPop();
    } else {
        GSfloorPop();
    }
}

extern "C" void menuToolBattleInit(void)
{
    if (_CB.connectMode == CB_CONNECT_SKIP_ENTRY) {
        fightReadResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GC_RESOURCE);
    } else {
        fightReadResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GBA_RESOURCE);
    }
    GSfsysReadWait(CB_RESOURCE_GROUP);
}
