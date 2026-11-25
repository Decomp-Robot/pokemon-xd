#include <types.h>
#include <gameSetup.h>
#include <libc/memory.h>
#include <hero.h>
#include <heroMove.h>
#include <heroBios.h>
#include <Pokemon.h>
#include <relivehall.h>
#include <FightRuleData.h>
#include <menuCBBios.h>
#include <menuGBA.h>
#include <menuCBRule.h>
#include <menuCBBattle.h>
#include <menu.h>
#include <winMsg.h>
#include <winSprite.h>
#include <menuCB.h>
#include <flashsystem.h>
#include <fade.h>
#include <floor.h>
#include <floorManager.h>
#include <fightResource.h>
#include <GSmem.h>
#include <GSres.h>
#include <GSthread.h>
#include <GSflag.h>
#include <GSfsys.h>
#include <GSinput.h>
#include <savedata.h>
#include <memcard.h>
#include <gamedata.h>
#include <CAnt.h>
#include <hsdRandom.h>
#include <esaba.h>
#include <sound.h>
#include <menuSub.h>
#include <menuNameEntry.h>
#include <gbaCommand.h>
#include <pokecolo.h>
#include <menuTop.h>
#include <menuCB_Sub1.h>
#include <menuCBCommon.h>
#include <toolentry.h>
#include <menuCBPokemonEntry.h>

_MENUCBWORK _CB;

/* Scratch records the Colosseum entry screens prepare before a battle. */
struct _MENUCBTEMPWORK {
    u8 mReserved0[0x1320];
    Hero mHero[4];
    u8 mReserved1[0x90];
    FightRuleData mConstantRule;
    FightRuleData mWorkRule;
    u8 mReserved2[0xF0C0 - 0x3AB0];
};

_MENUCBTEMPWORK *_menuCB_Temp;
s32 _menuCB_RuleData_SaveJob_flag;
_MENUCBWORK *pGCB;
void *toolBattleTempSaveData;
bool menuCBFlashFlag[8] = {true};

/* Stages of the Relive Hall and dancers placed on each stage. */
static const s32 RELIVE_STAGE_COUNT = 9;
static const s32 RELIVE_DANCER_COUNT = 4;

enum CbRuleCursorAlpha {
    RULE_CURSOR_ALPHA_MIN = 15,
    RULE_CURSOR_ALPHA_MAX = 255
};


/* Music played on the versus menus. */
enum CbBgm { CB_BGM_VS_MENU = 1384 };

/* Floor fade script the versus-stage transition installs. */
enum CbFadeScript { CB_FADE_SCRIPT = 0x5960008 };

/* Menu IDs the session opens and closes. */
enum CbMenuId {
    CB_MENU_VS_MODE = 164,
    CB_MENU_VS_START = 165,
    CB_MENU_ROOT = 169,
    CB_MENU_ROOT_BACKDROP = 170,
    CB_MENU_PLAYER_DECIDE = 171,
    CB_MENU_RESULT = 172,
    CB_MENU_GBA_CONNECT = 176,
    CB_MENU_BATTLE = 177,
    CB_MENU_GC_ENTRY_HELP = 187,
    CB_MENU_GC_ENTRY_SUB = 188,
    CB_MENU_GC_ENTRY_TRAINER_1 = 189,
    CB_MENU_GC_ENTRY_TRAINER_2 = 190,
    CB_MENU_GC_ENTRY_TRAINER_3 = 191,
    CB_MENU_GC_ENTRY_PANEL = 194,
    CB_MENU_DUEL_HELP = 203,
    CB_MENU_DUEL_AUX_1 = 204,
    CB_MENU_DUEL_AUX_2 = 205,
    CB_MENU_DUEL_AUX_3 = 206,
    CB_MENU_RULE_HELP = 239,
    CB_MENU_RULE_PANEL_1 = 240,
    CB_MENU_RULE_PANEL_2 = 241,
    CB_MENU_RULE_PANEL_3 = 242,
    CB_MENU_RULE_PANEL_4 = 243,
    CB_MENU_RULE_PANEL_5 = 244,
    CB_MENU_RULE_MAIN = 245,
    CB_MENU_DUEL_PANEL = 247,
    CB_MENU_VS_RANK = 262,
    CB_MENU_GC_ENTRY_BACKDROP_1 = 269,
    CB_MENU_GC_ENTRY_BACKDROP_2 = 270,
    CB_MENU_GC_ENTRY_BACKDROP_3 = 271,
    CB_MENU_FLASH_END = 281
};

/* Floors the session enters and leaves. */
enum CbFloorId {
    CB_FLOOR_TOP = 900,
    CB_FLOOR_VS_LOBBY = 917,
    CB_FLOOR_VS_STAGE = 919
};

/* Window messages the session opens. */
enum CbMessageId {
    CB_MSG_NO_MEMORY_CARD = 130,
    CB_MSG_SAVE_FIRST = 156,
    CB_MSG_RULE_SAVE_CONFIRM = 203,
    CB_MSG_RULE_STANDARD_CONFIRM = 17425,
    CB_MSG_RULE_SAVED_CONFIRM = 17426,
    CB_MSG_ENTRY_CONFIRM = 17433,
    CB_MSG_CONNECT_CONTROLLER_1 = 17462
};

/* One row of the one-player/two-player selection dialogue. */
struct MenuCBTunaideSelect {
    /* 0x00 */ s32 battleMode;
    /* 0x04 */ s32 battleKind;
    /* 0x08 */ s32 homePlace;
    /* 0x0C */ s8 controlerId[4];
};

extern const MenuCBTunaideSelect tunaide_select[4] = {
    { 0, 0, 0, { 1, 2, 0, 0 } },
    { 0, 0, 1, { 2, 3, 0, 0 } },
    { 2, 2, 0, { 1, 2, 3, 4 } },
    { 2, 2, 1, { 1, 2, 3, 4 } },
};

extern const s32 tunaide_select_field[6] = { 0, 1, 2, 3, 4, 5 };

extern "C" {
void menuCB_RuleRowSelectAll(s32 select);
void menuCB_ReliveDancerCreate(void);
void menuCB_ReliveDancerFree(void);
void menuCB_RuleCursorReset(void);
void menuCB_RuleDataStore(void);
}

u8 menuCB_SubWinDisp_GCENTRY(void);
void menuCB_SAVEDATA_recover(void);
void menuCB_GCEntry_Job(void);

s32 _menuCBYesNoJob(s32 answer);
void _menuCB_RuleData_SaveJob(s32 flag);
void menuCB_RuleCustomJobDisp(s32 entrySelect);
s32 _menuCB_SaveRule(void);
void _tempHeroInitAll(void);
extern "C" void menuColosseumBattleExit(void);

#pragma force_active on

void menuCB_ReliveDancerFree(void)
{
    s32 i;
    s32 j;

    for (i = 0; i < RELIVE_STAGE_COUNT; i++) {
        for (j = 0; j < RELIVE_DANCER_COUNT; j++) {
            if (_CB.reliveDancers[i][j] != NULL) {
                GSmemFree(_CB.reliveDancers[i][j]);
                _CB.reliveDancers[i][j] = NULL;
            }
        }
    }
}

void menuCB_ReliveDancerCreate(void)
{
    s32 i;
    s32 j;
    CReliveStage *stage;

    for (i = 0; i < RELIVE_STAGE_COUNT; i++) {
        for (j = 0; j < RELIVE_DANCER_COUNT; j++) {
            _CB.reliveDancers[i][j] = (Pokemon *)GSmemAllocAlign(sizeof(Pokemon), 32);
            pokemonInit(_CB.reliveDancers[i][j]);
        }
    }
    for (i = 0; i < RELIVE_STAGE_COUNT; i++) {
        stage = CReliveHall::getPointer()->getStage(i);
        for (j = 0; j < RELIVE_DANCER_COUNT; j++) {
            pokemonBiosCopy(_CB.reliveDancers[i][j], stage->getDancerPointer(j));
        }
    }
}

void menuCB_RuleDataStore(void)
{
    memcpy(savedataGetStatus(NULL, SAVE_DATA_FIGHT_RULE), _CB.mRulesetData,
           sizeof(_CB.mRulesetData));
}

void menuCB_RuleCursorReset(void)
{
    _CB.mEntry.mRuleCursorPrev = _CB.mEntry.mRuleCursor;
    _CB.mEntry.mRuleCursorPrevColor = RULE_CURSOR_ALPHA_MAX;
    _CB.mEntry.mRuleCursorColor = RULE_CURSOR_ALPHA_MIN;
}

void menuCB_RuleRowSelectAll(s32 select)
{
    s32 i;

    for (i = 0; i < RULE_ROW_COUNT; i++) {
        _CB.mMenuResult.mRuleRowSelectPrev[RULE_ROW_COUNT - 1 - i] = select;
    }
    for (i = 0; i < RULE_ROW_COUNT; i++) {
        _CB.mVersus.mRuleRowSelect[RULE_ROW_COUNT - 1 - i] = select;
    }
}

void menuCB_RuleCustomJobDisp(s32 entrySelect)
{
    s32 rowSelect;

    if (entrySelect == 0) {
        rowSelect = 0;
    }
    if (entrySelect == 1) {
        rowSelect = -1;
    }
    _CB.mEntryRule = &_CB.mRulesetData[_CB.mVersus.mRuleNo];
    menuCBRule_CopyRuleData(&_menuCB_Temp->mConstantRule,
                            menuCBRule_ConstantRule2(_CB.mVersus.mRuleNo));
    menuCBRule_CopyRuleData(&_menuCB_Temp->mWorkRule, _CB.mEntryRule);
    menuCB_RuleRowSelectAll(rowSelect);
    _CB.mEntry.mRuleCursor = 0;
    _CB.mEntry.selectedValue = 0;
    _CB.mMenuResult.ruleResult = 0;
    _CB.mVersus.ruleExitRequest = 0;
    _CB.mMenuResult.savedRuleRequest = 1;
    _CB.mMenuResult.standardRuleRequest = 1;
    _CB.mVersus.savedRuleButton = 1;
    _CB.mVersus.standardRuleButton = 1;
    _CB.mEntry.mRuleCursorPrev = -1;
    _CB.mEntry.ruleListScrollStep = 0;
    _CB.mEntry.ruleListScrollY = 500;
    menuCB_SE_Play(CB_SOUND_MENU_TRANSITION);
    menuOpen(CB_MENU_RULE_MAIN, 0);
    menuOpen(CB_MENU_RULE_HELP, 0);
    menuOpen(CB_MENU_RULE_PANEL_1, 0);
    menuOpen(CB_MENU_RULE_PANEL_2, 0);
    menuOpen(CB_MENU_RULE_PANEL_3, 0);
    menuOpen(CB_MENU_RULE_PANEL_4, 0);
    menuOpen(CB_MENU_RULE_PANEL_5, 0);
    do {
        if (_CB.mMenuResult.standardRuleRequest == 0) {
            winMsgOpen(WINMSG_WINDOW_MENU, CB_MSG_RULE_STANDARD_CONFIRM, 1, 1);
            if (_menuCBYesNoJob(0) == 0) {
                menuCBRule_CopyRuleData(_CB.mEntryRule,
                                        &_menuCB_Temp->mConstantRule);
                menuCB_RuleRowSelectAll(0);
                winMsgClose(0);
                menuCB_RuleCursorReset();
                _CB.mEntry.mRuleCursor = 0;
            } else {
                winMsgClose(0);
            }
            _CB.mMenuResult.standardRuleRequest = 1;
            _CB.mVersus.standardRuleButton = 1;
        }
        if (_CB.mMenuResult.savedRuleRequest == 0) {
            winMsgOpen(WINMSG_WINDOW_MENU, CB_MSG_RULE_SAVED_CONFIRM, 1, 1);
            if (_menuCBYesNoJob(0) == 0) {
                menuCBRule_CopyRuleData(_CB.mEntryRule,
                                        &_menuCB_Temp->mWorkRule);
                menuCB_RuleRowSelectAll(0);
                winMsgClose(0);
                menuCB_RuleCursorReset();
                _CB.mEntry.mRuleCursor = 0;
            } else {
                winMsgClose(0);
            }
            _CB.mMenuResult.savedRuleRequest = 1;
            _CB.mVersus.savedRuleButton = 1;
        }
        if (_CB.mVersus.ruleExitRequest == 2) {
            if (_CB.mVersus.mRuleRowSelect[RULE_ROW_ENTRY] == -1) {
                _CB.mVersus.ruleExitRequest = 0;
            }
            _CB.mMenuResult.ruleResult = 2;
        }
        if (_CB.mVersus.ruleExitRequest == 1) {
            if (menuCBRule_CompareRuleData(&_menuCB_Temp->mWorkRule,
                                           _CB.mEntryRule) == 0) {
                _menuCB_RuleData_SaveJob(0);
                if (_menuCB_RuleData_SaveJob_flag == 0) {
                    _CB.mMenuResult.ruleResult = 1;
                } else {
                    _CB.mEntry.selectedValue = 0;
                }
            } else {
                _CB.mMenuResult.ruleResult = 1;
            }
            _CB.mVersus.ruleExitRequest = 0;
        }
        GSthreadSwitch();
    } while (_CB.mMenuResult.ruleResult == 0);
    if (_CB.mVersus.ruleExitRequest == 2) {
        if (menuCBRule_CompareRuleData(&_menuCB_Temp->mWorkRule,
                                       _CB.mEntryRule) == 0) {
            _menuCB_RuleData_SaveJob(0);
            if (_menuCB_RuleData_SaveJob_flag == 0 &&
                memcardIsMemCardValid() != 0) {
                _CB.mMenuResult.ruleResult = 1;
            } else {
                _CB.mMenuResult.ruleResult = 2;
            }
        }
    }
    menuCB_SE_Play(CB_SOUND_MENU_CLOSE);
    menuCloseCustom(CB_MENU_RULE_PANEL_5, 0, 0);
    menuCloseCustom(CB_MENU_RULE_PANEL_4, 0, 0);
    menuCloseCustom(CB_MENU_RULE_PANEL_3, 0, 0);
    menuCloseCustom(CB_MENU_RULE_PANEL_2, 0, 0);
    menuCloseCustom(CB_MENU_RULE_PANEL_1, 0, 0);
    menuCloseCustom(CB_MENU_RULE_HELP, 0, 0);
    menuCloseCustom(CB_MENU_RULE_MAIN, 0, 1);
    if (_CB.mMenuResult.ruleResult == 2) {
        menuCBRule_CopyRuleData(_CB.mEntryRule, &_menuCB_Temp->mWorkRule);
        menuCB_RuleDataStore();
    }
    if (memcardIsMemCardValid() == 0) {
        menuCB_RuleDataStore();
        menuCBBios_RuleEditSaveON(1);
    }
}

void _menuCB_RuleData_SaveJob(s32 flag)
{
    _menuCB_RuleData_SaveJob_flag = flag;
    do {
        if (memcardIsMemCardValid() == 0) {
            break;
        }
        winMsgOpen(WINMSG_WINDOW_MENU, CB_MSG_RULE_SAVE_CONFIRM, 1, 1);
        _menuCB_RuleData_SaveJob_flag = _menuCBYesNoJob(0);
        if (_menuCB_RuleData_SaveJob_flag != 0) {
            break;
        }
        menuCB_RuleDataStore();
    } while (_menuCB_SaveRule() < 0);
    winMsgClose(1);
}

void menuCB_FlashEnd(bool closeFlag)
{
    if (closeFlag && menuIsCheck(CB_MENU_FLASH_END) == 1) {
        menuCloseCustom(CB_MENU_FLASH_END, 0, 0);
    }
}

void menuCB_FlashStart(bool playFromStart)
{
    const u32 flashID = 0x200A3000;

    if (playFromStart) {
        if (_CB.connectMode == 1) {
            FlashSystemSetPlayFrameLabel(flashID, "1_start", "1_end");
            FlashSystemSetNowFrameLabel(flashID, "1_start");
        } else {
            FlashSystemSetPlayFrameLabel(flashID, "2_start", "2_end");
            FlashSystemSetNowFrameLabel(flashID, "2_start");
        }
    } else {
        if (_CB.connectMode == 1) {
            FlashSystemSetPlayFrameLabel(flashID, "1_end", "1_end");
            FlashSystemSetNowFrameLabel(flashID, "1_end");
        } else {
            FlashSystemSetPlayFrameLabel(flashID, "2_end", "2_end");
            FlashSystemSetNowFrameLabel(flashID, "2_end");
        }
    }

    tagWINDOW_WORK *window = windowSearchID(CB_MENU_FLASH_END);
    if (window != NULL) {
        window->mOpenPriority = 0;
    }

    _CB.mEntry.mFlashSpriteFrames = 1;

    do {
        GSthreadSwitch();
    } while (FlashSystemIsLastFrame(flashID) == 0);
}

u8 menuCB_SubWinDisp_GCENTRY(void)
{
    if (_CB.mEntry.mSubmenuRunning == 1) {
        if (_CB.mMenuResult.mGCEntryCancel == 1) {
            return 0;
        }
        if (_CB.mVersus.mSubmenuConfirm == 1) {
            switch (menuCB_GCEntry_SubWindow_Disp()) {
            case 0:
                return 0;
            case 1:
                _CB.mEntry.mSubmenuRunning = 0;
                _CB.mVersus.mSubmenuConfirm = 0;
                return 1;
            }
        }
        return 0;
    }

    s32 result = menuCB_GCEntry_SubWindow_Disp();
    GSthreadSwitch();
    switch (result) {
    case 0:
    case 1:
        menuCBGCEntryPokemon();
        if (_CB.mEntry.dispState >= 2) {
            return 1;
        }
        _CB.mEntry.dispState = 0;
        switch (pokecoloGetCountryCode()) {
        case 0:
            if (_CB.mEntry.battleMode2D == 2) {
                menuCBReliveHallCursolInit();
            }
            break;
        case 1:
        case 2:
            break;
        }
        return 1;
    case 2:
        menuCB_GCEntry_PokemonStatusDisp();
        break;
    case 4:
        menuCB_GCEntry_ItemGiveJob();
        break;
    case 5:
        menuCB_GCEntry_ItemKeepJob();
        break;
    case 6:
        break;
    }
    _CB.mEntry.dispState = 0;
    return 1;
}

void menuCB_SAVEDATA_recover(void)
{
    memcpy(savedataGetStatus(toolBattleTempSaveData, SAVE_DATA_FIGHT_RULE),
           savedataGetStatus(NULL, SAVE_DATA_FIGHT_RULE),
           sizeof(_CB.mRulesetData));
    memcpy(savedataGetStatus(NULL, SAVE_DATA_IMAGE), toolBattleTempSaveData, MENU_CB_SAVE_IMAGE_SIZE);
}

void menuCB_GCEntry_Job(void)
{
    Hero *save = (Hero *)savedataGetStatus(NULL, SAVE_DATA_HERO);
    heroBiosCopy(&_CB.mHero, save);
    for (u32 i = 0; i < 6; i++) {
        pokemonInit(heroBiosGetPokemonPtr(&_CB.mHero, i));
    }
    menuCB_GCPokemonHPandWAZA_MAX();

    switch (pokecoloGetCountryCode()) {
    case 0:
        break;
    case 1:
    case 2:
        menuCB_ReliveDancerCreate();
        break;
    }

    _CB.mEntry.selectCount = 0;
    _CB.mEntry.dispState = 0;
    _CB.mEntry.pokeSelect = 0;
    _CB.mEntry.battleMode2D = 0;
    _CB.mEntry.page2DSub = 0;
    _CB.mEntry.page2D = 0;
    _CB.mMenuResult.mGCEntryCancel = 0;
    _CB.mVersus.mSubmenuConfirm = 0;
    _CB.mEntry.entryNo = 0;
    _CB.mEntry.pageScrollVelocity = 0;
    menuCB_SE_Play(CB_SOUND_MENU_TRANSITION);

    menuOpen(CB_MENU_GC_ENTRY_HELP, 0);
    menuOpen(CB_MENU_GC_ENTRY_TRAINER_1, 0);
    menuOpen(CB_MENU_GC_ENTRY_TRAINER_2, 0);
    menuOpen(CB_MENU_GC_ENTRY_TRAINER_3, 0);
    menuOpen(CB_MENU_GC_ENTRY_BACKDROP_1, 0);
    menuOpen(CB_MENU_GC_ENTRY_BACKDROP_3, 0);
    menuOpen(CB_MENU_GC_ENTRY_BACKDROP_2, 0);
    menuOpen(CB_MENU_GC_ENTRY_PANEL, 0);

    do {
        _CB.mEntry.mSubmenuRunning = 0;
        _CB.mEntry.dispState = 0;
        do {
            menuOpen(CB_MENU_GC_ENTRY_SUB, 1);
        } while (menuCB_SubWinDisp_GCENTRY() != 0);
    } while (_CB.mVersus.mSubmenuConfirm == 0);

    menuCB_SE_Play(CB_SOUND_MENU_CLOSE);
    menuCloseCustom(CB_MENU_GC_ENTRY_SUB, 0, 0);
    menuCloseCustom(CB_MENU_GC_ENTRY_PANEL, 0, 0);
    menuCloseCustom(CB_MENU_GC_ENTRY_BACKDROP_2, 0, 0);
    menuCloseCustom(CB_MENU_GC_ENTRY_BACKDROP_3, 0, 0);
    menuCloseCustom(CB_MENU_GC_ENTRY_BACKDROP_1, 0, 0);
    menuCloseCustom(CB_MENU_GC_ENTRY_TRAINER_3, 0, 0);
    menuCloseCustom(CB_MENU_GC_ENTRY_TRAINER_2, 0, 0);
    menuCloseCustom(CB_MENU_GC_ENTRY_TRAINER_1, 0, 0);
    menuCloseCustom(CB_MENU_GC_ENTRY_HELP, 0, 1);
    heroBiosCopy(save, &_CB.mHero);
    CAnt::systemFree();

    switch (pokecoloGetCountryCode()) {
    case 0:
        break;
    case 1:
    case 2:
        menuCB_ReliveDancerFree();
        break;
    }
}

void _imasuguBattleMenuMain(void)
{
    menuCB_SE_Play(CB_SOUND_MENU_TRANSITION);

    while (true) {
        if (_CB.mEntry.mVsSelectMenuReopen == 0) {
            menuOpen(CB_MENU_VS_MODE, 1);
            if (_CB.mMenuResult.mVsModeCancel == 1) {
                menuCB_SE_Play(CB_SOUND_MENU_CLOSE);
                menuCloseCustom(CB_MENU_VS_MODE, 0, 0);
                menuCBBios_UnlockContext();
                heroMoveClearPoolCounter();
                menuCloseCustom(CB_MENU_VS_MODE, 0, 1);
                menuCBFlashFlag[0] = false;
                GSfloorPop();
                menuTopSetBattleSelectMode(0);
                floorSetFadeScript(0, 0);
                return;
            }
        } else {
            menuOpen(CB_MENU_VS_MODE, 0);
        }

        _CB.mVsHeroNameIndex = -1;
        menuOpenCustom(CB_MENU_VS_RANK, windowGetActiveID(), &_CB.mVersus.mVsRank, 0, 1, 0);

        if (_CB.mVersus.mVsRank == 4 || _CB.mMenuResult.mVsSelectCancel == 1) {
            menuCloseCustom(CB_MENU_VS_RANK, 0, 1);
            _CB.mMenuResult.mVsSelectCancel = 0;
            _CB.mVersus.mVsRank = 0;
            _CB.mEntry.mVsSelectMenuReopen = 0;
            continue;
        }

        _CB.mEntry.mVsSelectMenuReopen = 1;
        _CB.battleMode = 1;
        _CB.ruleId = 0;
        _CB.mVsStage = (u16)HSD_Rand() % 3;

        switch (_CB.mVersus.mVsBattleKind) {
        case 0:
            _CB.mStartMenuType = 3;
            break;
        case 1:
            _CB.mStartMenuType = 4;
            break;
        }

        menuCB_InitBattle(&_CB);
        menuCBPokemonEntryLoadTex();
        menuCloseCustom(CB_MENU_VS_RANK, 2, 0);
        menuCloseCustom(CB_MENU_VS_MODE, 2, 1);
        menuCBpokemonFaceReadWait();
        menuOpenCustom(CB_MENU_VS_START, windowGetActiveID(), &_CB.mVersus.mBattleStartResult, 0, 1, 0);
        menuCloseCustom(CB_MENU_VS_START, 0, 1);

        if (_CB.mVersus.mBattleStartResult == 1 || _CB.mMenuResult.mBattleStartCancel == 1) {
            _CB.mMenuResult.mBattleStartCancel = 0;
            _CB.mVersus.mBattleStartResult = 0;
            menuCB_SE_Play(CB_SOUND_MENU_TRANSITION);
            continue;
        }

        for (s32 i = 0; i < 2; i++) {
            toolentryTaisenSetEtnryPokemonOrder(0, i);
            toolentryTaisenSetEtnryPokemonOrder(1, i);
        }

        _MENUCBTRAINER *trainer = menuCBBios_GetTrainerImmediately(0);
        trainer->mEntryList[2] = ENTRY_ORDER_UNUSED;
        trainer->mEntryList[3] = ENTRY_ORDER_UNUSED;
        trainer->mEntryList[4] = ENTRY_ORDER_UNUSED;
        trainer->mEntryList[5] = ENTRY_ORDER_UNUSED;
        trainer = menuCBBios_GetTrainerImmediately(1);
        trainer->mEntryList[2] = ENTRY_ORDER_UNUSED;
        trainer->mEntryList[3] = ENTRY_ORDER_UNUSED;
        trainer->mEntryList[4] = ENTRY_ORDER_UNUSED;
        trainer->mEntryList[5] = ENTRY_ORDER_UNUSED;

        toolentryTaisenEntryPokemon(0);
        toolentryTaisenEntryPokemon(1);
        soundStop(CB_BGM_VS_MENU, 500);
        menuCBBios_LockContext();
        menuColosseumBattleExit();
        menuCBFlashFlag[0] = true;
        GSfloorPush(CB_FLOOR_VS_STAGE);
        floorWaitForChange();
        menuTopJumpVSMode();
        menuCB_FlashStart(false);
        soundOpenPlay(CB_BGM_VS_MENU, 0, 255);
    }
}

s32 _menuCBYesNoJob(s32 answer)
{
    switch (pokecoloGetCountryCode()) {
    case 0:
        return menuSubOpenYesNo(0, 60, 210, answer);
    case 1:
    case 2:
        return menuSubOpenYesNo(0, 60, 200, answer);
    default:
        return 0;
    }
}

void _tempHeroInitAll(void)
{
    u32 i;

    for (i = 0; i < 4; i++) {
        memset(&_menuCB_Temp->mHero[i], 0, sizeof(Hero));
        heroInit(&_menuCB_Temp->mHero[i]);
    }
}

s32 _menuCB_SaveRule(void)
{
    s32 result;

    menuCBBios_RuleEditSaveON(1);
    menuCBBios_CurrentlyWorkingToolBattleSet(0);
    result = memcardTaskCall(MEMCARD_TASK_SAVE_RULES, WINMSG_WINDOW_MENU, 0);
    menuCBBios_CurrentlyWorkingToolBattleSet(1);
    if (result != MEMCARD_RESULT_WRITTEN) {
        menuCBBios_RuleEditSaveON(0);
    }
    return result == MEMCARD_RESULT_WRITTEN ? 0 : -1;
}

s32 _colosseumbattlemenu_teamselect(s32 menu)
{
    static const s8 teamBattleSlot[4][4] = {
        { 0, 1, 2, 3 },
        { 0, 2, 1, 3 },
        { 0, 3, 1, 2 },
        { 0, 0, 0, 0 },
    };
    s32 teamSelect;
    s32 result;

    _CB.mGba.mTeamSelectGbaDisconnected = 0;
    _CB.mMenuResult.mTeamSelectCancel = 0;
    _CB.mVersus.mTeamSelect = 0;
    menuCB_SE_Play(CB_SOUND_MENU_TRANSITION);
    menuOpen(CB_MENU_RESULT, 1);
    menuCB_SE_Play(CB_SOUND_MENU_CLOSE);
    menuCloseCustom(CB_MENU_RESULT, 0, 1);

    teamSelect = _CB.mVersus.mTeamSelect;
    if (_CB.mGba.mTeamSelectGbaDisconnected != 0 || _CB.mMenuResult.mTeamSelectCancel == 1) {
        menuCB_GBADisconnectedErrorJob();
        return menuCB_MenuRollback(CB_MENU_ROOT);
    }

    result = CB_MENU_BATTLE;
    switch (teamSelect) {
    case 0:
    case 1:
    case 2:
        _CB.mTrainerImmediately[0].mVersusBattleId = teamBattleSlot[teamSelect][0];
        _CB.mTrainerImmediately[1].mVersusBattleId = teamBattleSlot[teamSelect][1];
        _CB.mTrainerImmediately[2].mVersusBattleId = teamBattleSlot[teamSelect][2];
        _CB.mTrainerImmediately[3].mVersusBattleId = teamBattleSlot[teamSelect][3];
        menuCB_InitBattle(&_CB);
        break;
    case 3:
    default:
        menuCB_GBADisconnectedErrorJob();
        result = menuCB_MenuRollback(CB_MENU_ROOT);
        break;
    }
    return result;
}

s32 _colosseumbattlemenu_playerdecide(s32 menu)
{
    _CB.mMenuResult.mTunaide02Cancel = 0;
    _CB.mEntry.mTunaide02TempBuff = 0;
    _CB.mGba.mGbaDisconnected = 0;
    menuCB_SE_Play(CB_SOUND_MENU_TRANSITION);
    menuOpen(CB_MENU_PLAYER_DECIDE, 1);
    menuCB_SE_Play(CB_SOUND_MENU_CLOSE);
    menuCloseCustom(CB_MENU_PLAYER_DECIDE, 0, 1);

    if (_CB.mGba.mGbaDisconnected != 0 || _CB.mMenuResult.mTunaide02Cancel == 1) {
        menuCB_GBADisconnectedErrorJob();
        return menuCB_MenuRollback(CB_MENU_ROOT);
    }
    if (_CB.battleMode != 2) {
        menuCB_InitBattle(&_CB);
        return CB_MENU_BATTLE;
    }
    return CB_MENU_RESULT;
}

s32 _gbac_connect_job(s32 menuID)
{
    Hero *heroes[4];
    s32 gbaMenu;

    _tempHeroInitAll();
    menuCBTrainerBios_SetControlerID(&_CB.mTrainer[0], 1);
    menuCBTrainerBios_SetControlerID(&_CB.mTrainer[1], 2);
    menuCBTrainerBios_SetControlerID(&_CB.mTrainer[2], 3);
    menuCBTrainerBios_SetControlerID(&_CB.mTrainer[3], 4);

    switch (_CB.battleMode) {
    case 0:
    case 1:
        if (_CB.mTrainerImmediately[0].mHomePlace == 0) {
            gbaMenu = MENU_GBAC_TYPE_1GBA;
            heroes[0] = &_menuCB_Temp->mHero[0];
            heroes[1] = &_menuCB_Temp->mHero[1];
            heroes[2] = NULL;
            heroes[3] = NULL;
        } else {
            gbaMenu = MENU_GBAC_TYPE_2GBA;
            heroes[0] = NULL;
            heroes[1] = &_menuCB_Temp->mHero[0];
            heroes[2] = &_menuCB_Temp->mHero[1];
            heroes[3] = NULL;
        }
        break;
    case 2:
    default:
        gbaMenu = (_CB.mTrainerImmediately[0].mHomePlace == 0) ? MENU_GBAC_TYPE_3GBA
                                                               : MENU_GBAC_TYPE_4GBA;
        heroes[0] = &_menuCB_Temp->mHero[0];
        heroes[1] = &_menuCB_Temp->mHero[1];
        heroes[2] = &_menuCB_Temp->mHero[2];
        heroes[3] = &_menuCB_Temp->mHero[3];
        break;
    }

    if (_CB.mTrainerImmediately[0].mHomePlace == 0) {
        menuCB_SAVEDATA_recover();
        menuCB_GCEntry_Job();
        if (_CB.mMenuResult.mGCEntryCancel == 1) {
            menuCB_SAVEDATA_recover();
            return menuCB_MenuRollback(CB_MENU_ROOT);
        }
    } else {
        CAnt::systemFree();
    }

    if (openMenuGBA(gbaMenu, MENU_GBAC_FLAG_BATTLE | MENU_GBAC_FLAG_RETRY |
                                       MENU_GBAC_FLAG_LOAD_SAVE,
                    heroes) < 0) {
        return menuCB_MenuRollback(CB_MENU_ROOT);
    }

    if (_CB.mTrainerImmediately[0].mHomePlace == 0) {
        heroBiosSetHomePlace(&_menuCB_Temp->mHero[0], 0);
    }

    switch (_CB.battleMode) {
    case 0:
    case 1:
        menuCBBios_InitVersusTrainer(&_CB.mTrainerImmediately[0],
                                     &_menuCB_Temp->mHero[0]);
        menuCBBios_InitVersusTrainer(&_CB.mTrainerImmediately[1],
                                     &_menuCB_Temp->mHero[1]);
        if (_CB.mTrainerImmediately[0].mHomePlace == 0) {
            menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[0], 1);
            menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[1], 2);
        } else {
            menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[0], 2);
            menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[1], 3);
        }
        menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[2], 0);
        menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[3], 0);
        break;
    case 2:
    default:
        menuCBBios_InitVersusTrainer(&_CB.mTrainerImmediately[0],
                                     &_menuCB_Temp->mHero[0]);
        menuCBBios_InitVersusTrainer(&_CB.mTrainerImmediately[1],
                                     &_menuCB_Temp->mHero[1]);
        menuCBBios_InitVersusTrainer(&_CB.mTrainerImmediately[2],
                                     &_menuCB_Temp->mHero[2]);
        menuCBBios_InitVersusTrainer(&_CB.mTrainerImmediately[3],
                                     &_menuCB_Temp->mHero[3]);
        menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[0], 1);
        menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[1], 2);
        menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[2], 3);
        menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[3], 4);
        break;
    }

    return CB_MENU_PLAYER_DECIDE;
}

s32 _colosseumbattlemenu_duel(s32 menuID)
{
    wchar_t entryName[21];
    wchar_t *title;
    s32 nameEntry;
    s32 i;
    u32 j;
    s32 result;

    memcpy(&_CB.mRulesetData[0], savedataGetStatus(NULL, SAVE_DATA_FIGHT_RULE),
           sizeof(_CB.mRulesetData));
    CAnt::systemInit();
    menuCB2DPokemonALLClear();
    cursorBiosInit();

    _CB.mEntry.mRuleSelect = 0;
    _CB.mMenuResult.mEntryDecision = 0;
    _CB.mVersus.mEntryCursor = 0;
    _CB.mEntry.mEntryCursorPrev = -1;
    menuGBACSetRomVer(GBA_ROM_VERSION_UNKNOWN, GBA_ROM_ENTRY_ALL);
    menuCB_SE_Play(CB_SOUND_MENU_TRANSITION);
    menuOpen(CB_MENU_ROOT, 0);
    menuOpen(CB_MENU_DUEL_AUX_3, 0);
    menuOpen(CB_MENU_DUEL_HELP, 0);
    menuOpen(CB_MENU_DUEL_AUX_1, 0);
    menuOpen(CB_MENU_DUEL_AUX_2, 0);
    menuOpen(CB_MENU_ROOT_BACKDROP, 0);
    menuOpen(CB_MENU_DUEL_PANEL, 0);

    while (true) {
        if (_CB.mEntry.mRuleSelect == 2) {
            _CB.mVersus.selectResult = 0;
            while (true) {
                if (_CB.mVersus.selectResult >= 16) {
                    _CB.mVersus.selectResult -= 16;
                }
                menuCB_TUNAIDE01_Rule_SELECT();
                switch (_CB.mVersus.selectResult) {
                case 1:
                    menuCB_RuleNameChengeBeforeJob(0);
                    menuCB_RuleCustomJobDisp(1);
                    menuCB_RuleNameChengeAfterJob(0);
                    continue;
                case 5:
                    break;
                case 17:
                    menuCB_RuleNameChengeBeforeJob(0);
                    menuCB_RuleCustomJobDisp(0);
                    menuCB_RuleNameChengeAfterJob(0);
                    continue;
                case 18:
                    menuCB_RuleNameChengeBeforeJob(1);
                    title = (wchar_t *)menuCB_RuleTitleNameGet(_CB.mVersus.mRuleNo);
                    for (i = 0; i < 21; i++) {
                        ((wchar_t *)entryName)[i] = _CB.mRulesetData[_CB.mVersus.mRuleNo].mTitle[i];
                    }
                    nameEntry = menuNameEntryOpen(NAME_ENTRY_MODE_STRING_BUFFER, (int)title);
                    menuCB_RuleNameChengeAfterJob(1);
                    if (nameEntry == 0) {
                        continue;
                    }
                    if (!memcardIsMemCardValid()) {
                        menuCB_RuleDataStore();
                        menuCBBios_RuleEditSaveON(1);
                        continue;
                    }
                    _menuCB_RuleData_SaveJob(0);
                    if (_menuCB_RuleData_SaveJob_flag != 0) {
                        for (i = 0; i < 21; i++) {
                            title[i] = entryName[i];
                        }
                        menuCB_RuleDataStore();
                    }
                    continue;
                case 19:
                    break;
                }
                break;
            }
        }

        if (_CB.mMenuResult.mEntryDecision == 2) {
            result = -1;
            menuGBACSetRomVer(GBA_ROM_VERSION_UNKNOWN, GBA_ROM_ENTRY_ALL);
            _CB.ruleId = 0;
            heroMoveClearPoolCounter();
            break;
        }
        if (_CB.mMenuResult.mEntryDecision == 1) {
            if (_CB.mVersus.mTunaideOpponents == TUNAIDE_OPPONENTS_GCN_VS_GBA ||
                _CB.mVersus.mTunaideOpponents == TUNAIDE_OPPONENTS_GCN_GBA_VS_GBA_GBA) {
                if (!memcardIsMemCardValid()) {
                    winMsgOpen(WINMSG_WINDOW_MENU, CB_MSG_NO_MEMORY_CARD, 1, 0);
                    winMsgClose(1);
                    _CB.mMenuResult.mEntryDecision = 0;
                    continue;
                }
                if (gamedatasaveGetStatus(NULL, GAMEDATASAVE_FIELD_SAVE_COUNT) == 0) {
                    winMsgOpen(WINMSG_WINDOW_MENU, CB_MSG_SAVE_FIRST, 1, 0);
                    winMsgClose(1);
                    _CB.mMenuResult.mEntryDecision = 0;
                    continue;
                }
            }

            winMsgOpen(WINMSG_WINDOW_MENU, CB_MSG_ENTRY_CONFIRM, 1, 1);
            if (_menuCBYesNoJob(0) != 0) {
                winMsgClose(0);
                _CB.mMenuResult.mEntryDecision = 0;
                continue;
            }
            winMsgClose(0);

            _CB.battleMode = tunaide_select[_CB.mVersus.mTunaideOpponents].battleMode;
            if (_CB.mVersus.mStageMode.battleStageMode == 6) {
                _CB.mVsStage = 0;
            } else {
                _CB.mVsStage = tunaide_select_field[_CB.mVersus.mStageMode.battleStageMode];
            }
            menuCBTrainerBios_SetHomePlace(&_CB.mTrainerImmediately[0],
                                           tunaide_select[_CB.mVersus.mTunaideOpponents].homePlace);
            menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[0],
                                             tunaide_select[_CB.mVersus.mTunaideOpponents].controlerId[0]);
            for (j = 1; j < 4; j++) {
                menuCBTrainerBios_SetHomePlace(&_CB.mTrainerImmediately[j], 1);
                menuCBTrainerBios_SetControlerID(&_CB.mTrainerImmediately[j],
                                                 tunaide_select[_CB.mVersus.mTunaideOpponents].controlerId[j]);
            }
            if (_CB.mVersus.mTunaideBattleStyle == TUNAIDE_BATTLE_STYLE_SINGLE) {
                _CB.battleMode = 0;
            } else {
                _CB.battleMode = 1;
            }
            if (_CB.mVersus.mTunaideOpponents >= TUNAIDE_OPPONENTS_GCN_GBA_VS_GBA_GBA) {
                _CB.battleMode = 2;
            }
            result = CB_MENU_GBA_CONNECT;
            break;
        }
        GSthreadSwitch();
    }

    menuCB_SE_Play(CB_SOUND_MENU_CLOSE);
    menuCloseCustom(CB_MENU_DUEL_PANEL, 0, 0);
    menuCloseCustom(CB_MENU_ROOT_BACKDROP, 0, 0);
    menuCloseCustom(CB_MENU_DUEL_AUX_2, 0, 0);
    menuCloseCustom(CB_MENU_DUEL_AUX_1, 0, 0);
    menuCloseCustom(CB_MENU_DUEL_HELP, 0, 0);
    menuCloseCustom(CB_MENU_DUEL_AUX_3, 0, 0);
    menuCloseCustom(CB_MENU_ROOT, 0, 1);
    if (_CB.mMenuResult.mEntryDecision == 2) {
        CAnt::systemFree();
    }
    _CB.ruleId = _CB.mVersus.mRuleNo;
    return result;
}

extern "C" void menuColosseumBattleMain(void)
{
    s32 menuID;
    u32 floorID;

    menuCB_CurrentMenuID();
    floorID = 0;
    menuCB_FlashStart(true);

    while (true) {
        soundOpenPlay(CB_BGM_VS_MENU, 0, 255);
        if (_CB.connectMode == 1) {
            menuGBACSetRomVer(GBA_ROM_VERSION_UNKNOWN, GBA_ROM_ENTRY_ALL);
            _imasuguBattleMenuMain();
            return;
        }
        _CB.mVsHeroNameIndex = 0;

        while (menuCB_CurrentMenuID() > 0) {
            menuCB_CurrentMenuID();
            menuID = menuCB_CurrentMenuID();
            switch (menuCB_CurrentMenuID()) {
            case CB_MENU_ROOT:
                menuID = _colosseumbattlemenu_duel(menuID);
                break;
            case CB_MENU_GBA_CONNECT:
                menuID = _gbac_connect_job(menuID);
                break;
            case CB_MENU_PLAYER_DECIDE:
                menuID = _colosseumbattlemenu_playerdecide(menuID);
                break;
            case CB_MENU_RESULT:
                menuID = _colosseumbattlemenu_teamselect(menuID);
                break;
            case CB_MENU_BATTLE:
                soundStop(CB_BGM_VS_MENU, 500);
                menuCBBios_LockContext();
                floorID = CB_FLOOR_VS_STAGE;
                break;
            }

            if (!GSinputIsConnected(1) && !gbaCommandIsConnected(1)) {
                winMsgOpen(WINMSG_WINDOW_MENU, CB_MSG_CONNECT_CONTROLLER_1, 0, 0);
                GSthreadSwitch();
                do {
                    GSthreadSwitch();
                } while (!GSinputIsConnected(1));
                winMsgClose(1);
            }

            if (menuID < 0) {
                if (menuCB_MenuPop() < 0) {
                    break;
                }
            } else if (menuID != menuCB_CurrentMenuID()) {
                if (windowGetActiveID() == menuCB_CurrentMenuID()) {
                    menuCloseCustom(menuCB_CurrentMenuID(), 0, 0);
                }
                menuCB_MenuPush(menuID);
            }

            if (floorID != 0) {
                if (windowGetActiveID() == menuCB_CurrentMenuID()) {
                    menuCloseCustom(menuCB_CurrentMenuID(), 0, 0);
                }
                break;
            }
        }

        if (floorID == 0) {
            floorID = CB_FLOOR_TOP;
            menuCBBios_UnlockContext();
            menuCB_WaitGCControler1();
        } else {
            menuCBBios_LockContext();
        }

        if (floorID == CB_FLOOR_TOP) {
            menuCBFlashFlag[0] = false;
            GSfloorPop();
            menuTopSetBattleSelectMode(1);
            floorSetFadeScript(0, 0);
            return;
        }

        menuCBFlashFlag[0] = true;
        fadeSet(FADE_OUT_BLACK, 0.5f);
        fadeCheck(1);
        menuColosseumBattleExit();
        GSfloorPush(floorID);
        floorSetFadeScript(0, CB_FADE_SCRIPT);
        floorWaitForChange();
        menuTopJumpVSMode();
        menuCB_FlashStart(false);
        fadeSet(FADE_IN_BLACK, 0.5f);
        fadeCheck(1);
        menuCB_CurrentMenuID();
        floorID = 0;
    }
}

void menuColosseumBattleExit(void)
{
    fadeCheck(1);
    if (menuCBBios_ContextIsLocked() == 0) {
        GSflagSet(HERO_PTR_FLAG, 0);
    }
    toolentryDebugPokemonDelete();
    if (_CB.connectMode == 1) {
        if (!fightReadResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GC_RESOURCE)) {
            fightFreeResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GC_RESOURCE);
        }
    } else {
        if (!fightReadResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GBA_RESOURCE)) {
            fightFreeResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GBA_RESOURCE);
        }
    }
    GSfloorGetCurrent();
    menuCB_FlashEnd(menuCBFlashFlag[0]);
    if (_menuCB_Temp != NULL) {
        GSmemFree(_menuCB_Temp);
    }
    _menuCB_Temp = NULL;
    menuCB_SAVEDATA_recover();
    toolBattleTempSaveData = memcardMemFreeForSavedata(toolBattleTempSaveData);
    menuCBBios_CurrentlyWorkingToolBattleSet(0);
    GSfsysFree(CB_RESOURCE_GROUP);
    GSresFreeGroup(CB_RESOURCE_GROUP);
    GSfsysFree(CB_BATTLE_MENU_RESOURCE_GROUP);
    GSresFreeGroup(CB_BATTLE_MENU_RESOURCE_GROUP);
}

extern "C" void menuColosseumBattleInit(void)
{
    if (toolBattleTempSaveData != NULL) {
        return;
    }
    _CB.mReserved0 = 0;
    menuCBBios_CurrentlyWorkingToolBattleSet(1);
    GSflagSet(HERO_PTR_FLAG, 0);
    if (GSflagGet(ESABA_PROGRESS_FLAG) >= flag_etc[GAME_SETUP_ENTRY_COLOSSEUM_BATTLE].progressValue) {
        _CB.pageWrap = 1;
    } else {
        _CB.pageWrap = 0;
    }
    if (_CB.connectMode == 1) {
        fightReadResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GC_RESOURCE);
    } else {
        fightReadResource(CB_BATTLE_RESOURCE_GROUP, CB_BATTLE_GBA_RESOURCE);
    }
    GSfsysReadWait(CB_BATTLE_MENU_RESOURCE_GROUP);
    GSfsysReadWait(CB_RESOURCE_GROUP);
    _menuCB_Temp = (_MENUCBTEMPWORK *)GSmemAllocAlign(sizeof(_MENUCBTEMPWORK), 32);
    menuCBBios_RuleConstRuleSet();
    toolBattleTempSaveData = memcardMemAllocForSavedata();
    memcpy(toolBattleTempSaveData, savedataGetStatus(NULL, SAVE_DATA_IMAGE), MENU_CB_SAVE_IMAGE_SIZE);
    toolentryDebugPokemonCreate();
    menuCBBios_InitContext(&_CB);
    menuCBBios_ContextIsLocked();
}

extern "C" void menuColosseumBattle(s32 connectMode)
{
    s32 i;

    menuCBBios_UnlockContext();
    _CB.connectMode = connectMode;
    /* Reset the transient menu state words before opening the session. */
    for (i = 0; i < CB_MENU_RESULT_WORDS; i++) {
        _CB.mMenuResultWords[i] = 0;
        _CB.mVersusWords[i] = 0;
    }
    for (i = 0; i < CB_MENU_ENTRY_WORDS; i++) {
        _CB.mEntryWords[i] = 0;
    }
    for (i = 0; i < CB_MENU_GBA_WORDS; i++) {
        _CB.mGbaWords[i] = 0;
    }
    menuCBFlashFlag[0] = true;
    GSfloorPush(CB_FLOOR_VS_LOBBY);
    floorSetFadeScript(0, 0);
    floorWaitForChange();
}

#pragma force_active reset
