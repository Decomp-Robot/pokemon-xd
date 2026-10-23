#include <window.h>
#include <savedata.h>
#include <heroBios.h>
#include <LooseBallAnims.h>
/*
 * menuBBS.cpp
 *
 * The link (GBA) battle menus: the menu stack the connect-battle screens
 * navigate with, the setup of the rule set and trainer records a battle is
 * booked with, and the Pokemon entry screen that hands the party of every
 * player over to its Game Boy.
 */

#include <menuBBS.h>
#include <menuCBBattleStart.h>
#include <menuCBBios.h>
#include <Pokemon.h>
#include <GSmsg.h>
#include <types.h>

/* -------------------------------------------------------------------------
 * External data
 * ---------------------------------------------------------------------- */

extern MenuCBStack menuCBStack;
extern MenuCBPokemonEntryWork menuCBPokemonEntryWork;
extern MenuCBTaisenWork *menuCBTaisenWorkActive;
extern u8 menuCBItemEndFlag;
extern u8 menuCBDebugSkipAGBConnection;
extern u32 menuCBRule_RegulationItemNum;
extern const u16 menuCBBiosPortTrainerDataID[4];

/* Deoxys form parameters the GBA reports back, one record per form. */
typedef struct GBA_DeokisisuParam {
    /* 0x00 */ s32 mStat[5];
} GBA_DeokisisuParam;

extern const GBA_DeokisisuParam _GBA_DeokisisuParamTable[];

/* -------------------------------------------------------------------------
 * External functions
 * ---------------------------------------------------------------------- */

extern "C" {
extern u8 menuCBItemEndGet(void);
extern void menuCBItemEndClear(void);

extern void *memset(void *dest, int ch, unsigned long count);
extern void *memcpy(void *dest, const void *src, unsigned long count);
extern u32 menuCBBios_GetPokemonNumLimit(void);
extern void menuCBBios_InitDefaultRules(void);
extern s32 battlePokemonGetNature(Pokemon *pokemon);
extern s32 pokemonAdjustValueBySeikaku(s32 nature, s32 id, s32 value);
extern void pokemon_SetPhyAtk(Pokemon *pokemon, u16 value);
extern void pokemon_SetPhyDef(Pokemon *pokemon, u16 value);
extern void pokemon_SetSpeAtk(Pokemon *pokemon, u16 value);
extern void pokemon_SetSpeDef(Pokemon *pokemon, u16 value);
extern void pokemon_SetNimbleness(Pokemon *pokemon, u16 value);
extern s32 menuGBACGetRomVer(s32 index);
extern u8 menuCBRule_DEOKISISUCamouflage(void);
extern u16 menuCBRule_RegulationItemID(s32 index);
extern s32 menuCB_MenuRollback(s32 menuID);
extern s32 menuCB_MenuPop(void);
extern void menuCB_MenuPush(s32 menuID);
extern s32 menuCB_CurrentMenuID(void);
extern void menuCB_InitMenu(s32 menuID);
extern u8 gbaCommandIsConnected(s32 port);
extern void gbaCommandEntryPokemon(s32 result, u8 *pokemon);
extern void gbaCommandEntryRequest(s32 port, s32 num, s32 limit, s32 battleType);
extern void gbaCommandSendWazaText(s32 port, Hero *hero);
extern s32 gba_SIProbe_ErrorCntClear(s32 port);
extern void gbaCommandEntryResult(s32 port, void *result);
extern void menuCB_GBADisconnectedErrorJob(void);
extern void menuCBBattleStartTrainerFaceFree(void);
extern void toolentryTaisenInitPokemonOrder(s32 port);
extern s32 toolentryTaisengetEtnryPokemonOrderNum(s32 port);
extern s32 toolentryTaisenSetEtnryPokemonOrder(s32 port, s32 index);
extern s32 toolentryTaisenDeleteEtnryPokemonOrder(s32 port);
extern void toolentryTaisenSetEtnryPokemonOrderGBA(s32 port, s32 num, u32 *pokemon);
extern u16 toolentryTaisenGetEntryPokemonNum(long port);
extern u16 toolentryTaisenGetPokemonNum(s32 port);
extern s32 toolentryTaisenGetEntryPlayerNum(void);
extern s32 toolentryTaisenGetBattleType(void);
extern s32 toolentryTaisenGetBattleVersus(void);
extern s32 toolentryTaisenGetControlerType(s32 port);
extern void toolentryTaisenEntryPokemon(s32 port);
extern s32 toolentryTaisenGetPokemonItemDataId(long port);
extern u16 toolentryTaisenGetPokemonHomePlace(s32 port);
extern const FightRuleData *menuCBRule_GetDefaultRuleData(s32 index);

extern void menuCBSEPlay(unsigned long seID);
extern u8 pauseCheck(void);
extern u32 GSinputButtonsChanged(s32 port);
extern u32 GSinputButtonsPressed(s32 port);
extern s32 windowGetActiveID(void);
extern void menuOpen(s32 id, s32 arg);
extern void menuOpenCustom(s32 id, ...);
extern void menuCloseCustom(s32 id, s32 arg1, s32 arg2);
extern void soundOpenPlay(s32 id, s32 arg1, s32 arg2);
extern void GSsndPlay(s32 id, s32 arg1, s32 arg2);
extern void GSthreadSwitch(void);
extern s32 winMsgCheck(void);
extern void winMsgClose(s32 id);
extern void winMsgOpen(s32 type, s32 a, s32 b, s32 c);

}

extern void _menuCBPokemonEntryWork(void);
extern void _menuCBPokemonEntryDispPosition(void);
extern void _menuCBPokemonEntryEntCheckGBA(GSinputDevice device, long port);
extern u8 _menuCBPokemonEntryAllPlayer(void);
extern void _menuCBPokemonEntrySetEntryFlag(long port, bool flag);
extern u8 _menuCBPokemonEntryGetEntryFlag(long port);
extern s32 _menuCBPokemonEntryButtonCheck(unsigned long buttons, long port);
extern void _menuCBPokemonEntryWorkInit(void);
extern u16 _menuCBPokemonEntryPokemonNum(long port);

/* The rule-set entry points and the entry-screen helpers of this module. */
extern "C" {
extern void menuCBRulesetInitDefault(MenuCBRuleSet *rules);
extern void menuCBRulesetCopyToWork(MenuCBTaisenWork *w, MenuCBRuleSet *set);
extern void menuCBPokemonEntryOrderInput(s32 device, s32 port);
extern void menuCBPokemonEntrySlotPlaced(long port);
/* Re-runs the entry texture work after a link drops. */
extern void menuCBPokemonEntryTexWorkReInit(void);
extern u16 menuCBPokemonEntryNumLimit(long port);
}

u8 _menuCB_pokemonGetDeoxysSel(u16 port, bool flag);

/* -------------------------------------------------------------------------
 * Rule set of the save
 * ---------------------------------------------------------------------- */

/* Mark the rule set of the save as changed so that it is written back. */
extern "C" void menuCBRulesetSetDirty(u8 dirty)
{
    MenuCBRuleSet *rules = (MenuCBRuleSet *)savedataGetStatus(0, SAVE_DATA_FIGHT_RULE);

    rules->mDirty = dirty;
}

extern "C" u8 menuCBRulesetGetDirty(void)
{
    MenuCBRuleSet *rules;

    rules = (MenuCBRuleSet *)savedataGetStatus(0, SAVE_DATA_FIGHT_RULE);
    if (rules == 0) {
        return 0;
    }
    return rules->mDirty;
}

/* The work area the connect-battle menus keep their state in. */
extern "C" MenuCBTaisenWork *menuCBTaisenWorkGet(void)
{
    return &menuCBTaisenWork;
}

extern "C" void menuCBBios_InitContext(MenuCBTaisenWork *work)
{
    u32 i;

    menuCBTaisenWorkActive = &menuCBTaisenWork;
    if (!menuCBItemEndGet()) {
        /* One record per controller port; the copy the battle is fought with
           is kept in the matching "fight immediately" slot. */
        for (i = 0; i < 4; i++) {
            menuCBBios_InitTrainerFromDataID(&work->mTrainer[i], menuCBBiosPortTrainerDataID[i], 0);
            menuCBTrainerBios_SetControlerID(&work->mTrainer[i], LooseBallAnims::GetDesperationInfo(i));
            work->mTrainer[i].mResetDataId = i;
            menuCBBios_CopyTrainer(&work->mTrainerImmediately[i], &work->mTrainer[i]);
        }
        switch (work->connectMode) {
        case 1:
            menuCB_InitMenu(0xA4);
            work->mStartMenuType = 0;
            break;
        default:
            menuCB_InitMenu(0xA9);
            work->mStartMenuType = 4;
            break;
        }
    }
    menuCBItemEndClear();
}

/* Fill the rule set of the save with the colosseum defaults, unless it
   already holds them. */
void menuCBRulesetInitDefault(MenuCBRuleSet *rules)
{
    s32 i;
    s32 j;
    const FightRuleData *d;
    u16 *str;

    if (rules->mRule[0].mMsgId == 0x9124 && rules->mRule[1].mMsgId == 0xA2D4
        && rules->mRule[2].mMsgId == 0xB51C && rules->mRule[3].mMsgId == 0x47FD
        && rules->mRule[4].mMsgId == 0x7422 && rules->mRule[5].mMsgId == 0x5633) {
        menuCBBios_InitDefaultRules();
    } else {
        memset(rules, 0, sizeof(*rules));
        d = menuCBRule_GetDefaultRuleData(0);
        rules->mRule[0] = *d;
        d = menuCBRule_GetDefaultRuleData(1);
        rules->mRule[1] = *d;
        d = menuCBRule_GetDefaultRuleData(2);
        rules->mRule[2] = *d;
        d = menuCBRule_GetDefaultRuleData(2);
        rules->mRule[3] = *d;
        d = menuCBRule_GetDefaultRuleData(2);
        rules->mRule[4] = *d;
        d = menuCBRule_GetDefaultRuleData(2);
        rules->mRule[5] = *d;
        rules->mRule[3].mPokemonCountMode = 0;
        rules->mRule[4].mPokemonCountMode = 0;
        rules->mRule[5].mPokemonCountMode = 0;
        rules->mRule[3].mPokemonCount = 4;
        rules->mRule[4].mPokemonCount = 4;
        rules->mRule[5].mPokemonCount = 4;
        for (i = 0; i < menuCBRule_RegulationItemNum; i++) {
            if (menuCBRule_RegulationItemID(i) == 0xBF) {
                rules->mRule[1].m52[i] = 1;
                rules->mRule[2].m52[i] = 1;
                rules->mRule[3].m52[i] = 1;
                rules->mRule[4].m52[i] = 1;
                rules->mRule[5].m52[i] = 1;
            }
        }
        str = GSmsgGetStringPtr(0x438A);
        for (j = 0; j < 21; j++) {
            if (str != 0) {
                if (*str == 0xFFFF) {
                    str += 2;
                }
                rules->mRule[3].m1E[j] = *str;
                if (*str != 0) {
                    str++;
                }
            } else {
                rules->mRule[3].m1E[j] = 0;
            }
        }
        str = GSmsgGetStringPtr(0x438B);
        for (i = 0; i < 21; i++) {
            if (str != 0) {
                if (*str == 0xFFFF) {
                    str += 2;
                }
                rules->mRule[4].m1E[i] = *str;
                if (*str != 0) {
                    str++;
                }
            } else {
                rules->mRule[4].m1E[i] = 0;
            }
        }
        str = GSmsgGetStringPtr(0x438C);
        for (i = 0; i < 21; i++) {
            if (str != 0) {
                if (*str == 0xFFFF) {
                    str += 2;
                }
                rules->mRule[5].m1E[i] = *str;
                if (*str != 0) {
                    str++;
                }
            } else {
                rules->mRule[5].m1E[i] = 0;
            }
        }
        menuCBRulesetCopyToWork(&menuCBTaisenWork, rules);
        rules->mDirty = 0;
    }
}

/* Mirror a rule set into the work area the menus read. */
void menuCBRulesetCopyToWork(MenuCBTaisenWork *w, MenuCBRuleSet *set)
{
    s32 i;
    FightRuleData *dst;
    const FightRuleData *d;

    menuCBItemEndClear();
    memset(w, 0, 0xB5F4);
    d = menuCBRule_GetDefaultRuleData(0);
    dst = &w->mRulesetData[0];
    *dst = *d;
    d = menuCBRule_GetDefaultRuleData(1);
    dst = &w->mRulesetData[1];
    *dst = *d;
    d = menuCBRule_GetDefaultRuleData(2);
    dst = &w->mRulesetData[2];
    *dst = *d;
    d = menuCBRule_GetDefaultRuleData(2);
    dst = &w->mRulesetData[3];
    *dst = *d;
    d = menuCBRule_GetDefaultRuleData(2);
    dst = &w->mRulesetData[4];
    *dst = *d;
    d = menuCBRule_GetDefaultRuleData(2);
    dst = &w->mRulesetData[5];
    *dst = *d;
    w->mRulesetData[3].mPokemonCountMode = 0;
    w->mRulesetData[4].mPokemonCountMode = 0;
    w->mRulesetData[5].mPokemonCountMode = 0;
    w->mRulesetData[3].mPokemonCount = 4;
    w->mRulesetData[4].mPokemonCount = 4;
    w->mRulesetData[5].mPokemonCount = 4;
    for (i = 0; i < menuCBRule_RegulationItemNum; i++) {
        if (menuCBRule_RegulationItemID(i) == 0xBF) {
            w->mRulesetData[1].m52[i] = 1;
            w->mRulesetData[2].m52[i] = 1;
            w->mRulesetData[3].m52[i] = 1;
            w->mRulesetData[4].m52[i] = 1;
            w->mRulesetData[5].m52[i] = 1;
        }
    }
    memcpy(menuCBBios_GetRuleEntryList(3), &set->mRule[3].m1E[0], 0x2A);
    memcpy(menuCBBios_GetRuleEntryList(4), &set->mRule[4].m1E[0], 0x2A);
    memcpy(menuCBBios_GetRuleEntryList(5), &set->mRule[5].m1E[0], 0x2A);
}

/* The "an item end flag went up" marker of the bios module. */
extern "C" u8 menuCBItemEndGet(void)
{
    return menuCBItemEndFlag;
}

extern "C" void menuCBItemEndClear(void)
{
    menuCBItemEndFlag = 0;
}

extern "C" void WS_ITEMEND(void)
{
    menuCBItemEndFlag = 1;
}

extern "C" s32 menuCBWindowMsgIsDone(void)
{
    return winMsgCheck();
}

extern "C" void scriptSetMemoFlag(void)
{
    winMsgClose(0);
}

/* Show a message window; the ID selects the script the window runs. */
extern "C" void menuCB_MessageDispOn(s32 msgID, s32 windowID)
{
    winMsgOpen(7, msgID, windowID, 0);
}

/* Port of the GBA that is in the battle but not answering, 0 if all of
   them are. */
extern "C" s32 menuCB_GetDisconnectedGBA(void)
{
    u32 i;

    for (i = 0; i < 4; i++) {
        s32 gbaPort = menuCBTaisenWork.mTrainerImmediately[i].mControllerId;
        u8 connected;

        if (gbaPort == 0) {
            continue;
        }
        if (menuCBBios_GetTrainerHomePlace(&menuCBTaisenWork.mTrainerImmediately[i]) == 0) {
            continue;
        }
        connected = gbaCommandIsConnected(gbaPort);
        if (connected == 0) {
            return gbaPort;
        }
    }
    return 0;
}

/* Copy the current language image into the sprite work. */
extern "C" void menuCBSpriteSetImage(tagSPRITE_WORK *sprite, MenuSpriteData *spriteData)
{
    const MenuSpriteImage *image = menuSpriteBiosGetValPtr(spriteData);

    sprite->mWidth = image->mWidth;
    sprite->mHeight = image->mHeight;
    sprite->mTextureResourceID = image->mResourceID;
    sprite->mTextureX = image->mX;
    sprite->mTextureY = image->mY;
}

/* -------------------------------------------------------------------------
 * Menu stack
 * ---------------------------------------------------------------------- */

/* Leave the menu `menuID` levels below the top of the stack, closing the
   windows that go with them. */
extern "C" s32 menuCB_MenuRollback(s32 menuID)
{
    menuCB_MenuPop();
    while (menuID != menuCB_CurrentMenuID()) {
        if (menuCBStack.mCount == 0) {
            break;
        }
        menuCBStack.mCount--;
    }
    return menuCB_CurrentMenuID();
}

extern "C" s32 menuCB_MenuPop(void)
{
    if (windowGetActiveID() == menuCB_CurrentMenuID()) {
        menuCloseCustom(menuCB_CurrentMenuID(), 0, 0);
    }
    menuCBStack.mStack[menuCBStack.mCount].windowID = 0;
    if (menuCBStack.mCount == 0) {
        return -1;
    }
    menuCBStack.mCount--;
    return menuCB_CurrentMenuID();
}

extern "C" void menuCB_MenuPush(s32 menuID)
{
    if ((u32)menuCBStack.mCount >= 8) {
        return;
    }
    menuCBStack.mCount++;
    menuCBStack.mStack[menuCBStack.mCount].menuID = menuID;
    menuCBStack.mStack[menuCBStack.mCount].windowID = 0;
}

extern "C" s32 menuCB_CurrentMenuID(void)
{
    return menuCBStack.mStack[menuCBStack.mCount].menuID;
}

extern "C" void menuCB_InitMenu(s32 menuID)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        menuCBStack.mStack[i].menuID = 0;
        menuCBStack.mStack[i].windowID = 0;
    }
    menuCBStack.mStack[0].menuID = menuID;
    menuCBStack.mCount = 0;
}

/* Debug switch that lets the menus run without a GBA in the port. */
extern "C" void menuCBDebug_SkipAGBConnection(u8 connected)
{
    menuCBDebugSkipAGBConnection = (u8)(!connected);
}

/* -------------------------------------------------------------------------
 * Deoxys forms
 * ---------------------------------------------------------------------- */
/* Hand the Deoxys of a port the stats its form asks for: the form
   parameters the GBA reports, the individual and effort values of the
   Pokemon itself and its level put together the way the games build a
   stat, then the nature of the Pokemon is taken into account. */
void _menuCB_DeoxysParamCustom(Pokemon *pokemon, u16 port)
{
    s32 level = pokemonGetStatus(pokemon, 0, 0x7A, 0);
    s32 nature = battlePokemonGetNature(pokemon);
    s32 ivPhyAtk = pokemonGetStatus(pokemon, 0, 0x94, 0);
    s32 ivPhyDef = pokemonGetStatus(pokemon, 0, 0x95, 0);
    s32 ivSpeAtk = pokemonGetStatus(pokemon, 0, 0x96, 0);
    s32 ivSpeDef = pokemonGetStatus(pokemon, 0, 0x97, 0);
    s32 ivNimbleness = pokemonGetStatus(pokemon, 0, 0x98, 0);
    s32 effortPhyAtk = pokemonGetStatus(pokemon, 0, 0x8E, 0);
    s32 effortPhyDef = pokemonGetStatus(pokemon, 0, 0x8F, 0);
    s32 effortSpeAtk = pokemonGetStatus(pokemon, 0, 0x90, 0);
    s32 effortSpeDef = pokemonGetStatus(pokemon, 0, 0x91, 0);
    s32 effortNimbleness = pokemonGetStatus(pokemon, 0, 0x92, 0);
    s32 statPhyAtk = (_GBA_DeokisisuParamTable[_menuCB_pokemonGetDeoxysSel(port, 0)].mStat[0] * 2 + ivPhyAtk + effortPhyAtk / 4) * level / 100 + 5;
    s32 statPhyDef = (_GBA_DeokisisuParamTable[_menuCB_pokemonGetDeoxysSel(port, 0)].mStat[1] * 2 + ivPhyDef + effortPhyDef / 4) * level / 100 + 5;
    s32 statSpeAtk = (_GBA_DeokisisuParamTable[_menuCB_pokemonGetDeoxysSel(port, 0)].mStat[2] * 2 + ivSpeAtk + effortSpeAtk / 4) * level / 100 + 5;
    s32 statSpeDef = (_GBA_DeokisisuParamTable[_menuCB_pokemonGetDeoxysSel(port, 0)].mStat[3] * 2 + ivSpeDef + effortSpeDef / 4) * level / 100 + 5;
    s32 statNimbleness = (_GBA_DeokisisuParamTable[_menuCB_pokemonGetDeoxysSel(port, 0)].mStat[4] * 2 + ivNimbleness + effortNimbleness / 4) * level / 100 + 5;

    statPhyAtk = pokemonAdjustValueBySeikaku(nature, 0x88, statPhyAtk);
    statPhyDef = pokemonAdjustValueBySeikaku(nature, 0x89, statPhyDef);
    statSpeAtk = pokemonAdjustValueBySeikaku(nature, 0x8A, statSpeAtk);
    statSpeDef = pokemonAdjustValueBySeikaku(nature, 0x8B, statSpeDef);
    statNimbleness = pokemonAdjustValueBySeikaku(nature, 0x8C, statNimbleness);
    pokemon_SetPhyAtk(pokemon, statPhyAtk);
    pokemon_SetPhyDef(pokemon, statPhyDef);
    pokemon_SetSpeAtk(pokemon, statSpeAtk);
    pokemon_SetSpeDef(pokemon, statSpeDef);
    pokemon_SetNimbleness(pokemon, statNimbleness);
}

/* The Deoxys form of a port, taken from the ROM version of the GBA it
   plays with.  `flag` asks for the form to stay at 0 while the camouflage
   rule of the save is off. */
u8 _menuCB_pokemonGetDeoxysSel(u16 port, bool flag)
{
    u8 form = 0;
    u16 gbaIndex = (u16)(menuCBTaisenWork.mTrainer[port].mControllerId - 1);

    if (menuCBTaisenWork.connectMode == 1) {
        return 0;
    }
    if (menuCBRule_DEOKISISUCamouflage() || !flag) {
        u16 romVer = menuGBACGetRomVer(gbaIndex);

        switch (romVer) {
        case 1:
        case 2:
            form = 0;
            break;
        case 3:
            form = 1;
            break;
        case 4:
            form = 2;
            break;
        case 5:
            form = 3;
            break;
        default:
            form = 0;
            break;
        }
    }
    return form;
}

/* -------------------------------------------------------------------------
 * Pokemon entry screen
 * ---------------------------------------------------------------------- */

/* The entry screen itself: opens the window, waits until every player
   handed its party over and hands the entries on. Returns the menu the
   caller has to go to next. */
extern "C" s32 menuCBPokemonEntrySelectScreen(void)
{
    s32 opened;
    s32 i;
    s32 j;
    s32 pending;
    s32 battleType;
    s32 count;

    menuCBBattleStartWork.soundID = 0x5DB;
    soundOpenPlay(0x5DB, 0, 0xFF);
    _menuCBPokemonEntryWorkInit();
    gba_SIProbe_ErrorCntClear(-1);
    menuCBTaisenWork.mAbortRequest = -1;
    menuCBTaisenWork.mEntryState = 0;
    menuCBTaisenWork.selectResult = 0;
    menuCBTaisenWork.confirmationComplete = 0;
    menuCBTaisenWork.confirmationInputDelay = 2;
    menuCBTaisenWork.mPlayerLeftEntry = 0;
    menuCBSEPlay(0x14);
    menuOpen(0xC6, 0);
    menuOpen(0xC5, 0);
    opened = 0;
    do {
        if (menuCBTaisenWork.mEntryState == 1) {
            if (!opened) {
                menuOpenCustom(0xE2, windowGetActiveID(), &menuCBTaisenWork.selectResult, 0, 0, 0);
                opened = 1;
            }
            if (menuCBTaisenWork.confirmationComplete != 0) {
                if (menuCBTaisenWork.selectResult == 0) {
                    _menuCBPokemonEntrySetEntryFlag(0, 1);
                    menuCBTaisenWork.mEntryState = 2;
                } else {
                    toolentryTaisenInitPokemonOrder(0);
                    menuCBTaisenWork.mEntryState = 0;
                    menuCBTaisenWork.selectResult = 0;
                    menuCBTaisenWork.confirmationComplete = 0;
                    menuCBTaisenWork.confirmationInputDelay = 2;
                    menuCloseCustom(0xE2, 0, 0);
                    opened = 0;
                }
            }
        }
        if (menuCBTaisenWork.mPlayerLeftEntry == 1) {
            menuCBPokemonEntryWork.mDisconnected = 0;
        }
        if (_menuCBPokemonEntryAllPlayer() != 0) {
            menuCBTaisenWork.mEntryState = 0x7FFFFFFF;
        }
        if (menuCBPokemonEntryWork.mDisconnected == 0) {
            menuCBTaisenWork.mEntryState = 0x7FFFFFFF;
        }
        GSthreadSwitch();
    } while (menuCBTaisenWork.mEntryState != 0x7FFFFFFF);

    if (menuCBPokemonEntryWork.mDisconnected == 0) {
        menuCBSEPlay(0x15);
        menuCloseCustom(0xE2, 0, 0);
        menuCloseCustom(0xC5, 0, 0);
        menuCloseCustom(0xC6, 0, 1);
        menuCB_GBADisconnectedErrorJob();
        menuCBPokemonEntryTexWorkReInit();
        menuCBBattleStartTrainerFaceFree();
        return 0xA9;
    }
    for (;;) {
        GSthreadSwitch();
        pending = 0;

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 6; j++) {
                if (menuCBTaisenWork.mPartySlotFlag[i][j] == 1) {
                    pending += menuCBTaisenWork.mPartySlotValue[i][j];
                }
            }
        }
        if (pending == 0) {
            break;
        }
    }
    /* Wait for the parties the GBAs send back before the entries are used. */
    GSthreadSwitch();
    GSthreadSwitch();
    GSthreadSwitch();
    battleType = toolentryTaisenGetBattleType();
    if (toolentryTaisenGetBattleVersus() == 4) {
        switch (battleType) {
        case 0:
        case 1:
            count = 2;
            break;
        default:
            count = 4;
            break;
        }
    } else {
        switch (battleType) {
        case 0:
        case 1:
            count = 2;
            break;
        default:
            count = 1;
            break;
        }
    }
    for (i = 0; i < count; i++) {
        toolentryTaisenEntryPokemon(i);
    }
    if (menuCBTaisenWork.connectMode == 0) {
        for (i = 0; i < count; i++) {
            u16 slot;
            Pokemon *pokemon;
            _MENUCBTRAINER *trainer = menuCBBios_GetTrainerByPort(i);
            Hero *hero = &trainer->mHero;

            for (slot = 0; slot < 6; slot++) {
                pokemon = heroBiosGetPokemonPtr(hero, slot);

                if (pokemonGetStatus(pokemon, 0, 0x6E, 0) == 0x19A) {
                    _menuCB_DeoxysParamCustom(pokemon, i);
                }
            }
        }
    }
    return 0xB1;
}
extern "C" void menuCBPokemonEntryButtonTask_20041006(void)
{
    s32 playerNum = toolentryTaisenGetEntryPlayerNum();
    s32 player;
    long port;

    toolentryTaisenGetBattleType();
    toolentryTaisenGetPokemonHomePlace(0);
    toolentryTaisenGetPokemonHomePlace(1);
    toolentryTaisenGetPokemonHomePlace(2);
    toolentryTaisenGetPokemonHomePlace(3);
    for (player = 0; player < playerNum; player++) {
        if (toolentryTaisenGetBattleVersus() == 4) {
            if (_menuCBPokemonEntryGetEntryFlag(player)) {
                _MENUCBTRAINER *trainer = menuCBBios_GetTrainer(toolentryTaisenGetControlerType(player));

                if (trainer->mHomePlace == 1 || trainer->mHomePlace == 2 || trainer->mHomePlace == 3) {
                    port = player;
                    port = toolentryTaisenGetControlerType(port);
                    if (!gbaCommandIsConnected(port)) {
                        menuCBPokemonEntryWork.mDisconnected = 0;
                        if (menuCBPokemonEntryWork.mDisconnectedPort < 0) {
                            menuCBPokemonEntryWork.mDisconnectedPort = port;
                        }
                    }
                }
            }
        }
        if (!_menuCBPokemonEntryGetEntryFlag(player)) {
            port = player;
            if (toolentryTaisenGetBattleVersus() == 4) {
                _MENUCBTRAINER *trainer = menuCBBios_GetTrainer(toolentryTaisenGetControlerType(player));

                if (trainer->mHomePlace == 1 || trainer->mHomePlace == 2 || trainer->mHomePlace == 3) {
                    _menuCBPokemonEntryEntCheckGBA((GSinputDevice)toolentryTaisenGetControlerType(port), port);
                } else {
                    menuCBPokemonEntryOrderInput(toolentryTaisenGetControlerType(player), player);
                }
            }
            if (toolentryTaisenGetControlerType(port) == 1) {
                if (!toolentryTaisenGetPokemonHomePlace(port)) {
                    toolentryTaisengetEtnryPokemonOrderNum(port);
                    _menuCBPokemonEntryPokemonNum(port);
                }
            }
        }
    }
}

/* What a GBA reports back while its player enters a party: the result code
   followed by the party it sent. */
void _menuCBPokemonEntryEntCheckGBA(GSinputDevice device, long port)
{
    s32 result;
    u8 raw[8];
    u32 word[6];
    s32 num;
    s32 i;

    toolentryTaisenGetPokemonItemDataId(port);
    gbaCommandEntryResult(device, &result);
    switch (result & 0xFF000000) {
    case 0x01000000: {
        s32 order = toolentryTaisengetEtnryPokemonOrderNum(port);
        s32 slot = toolentryTaisenSetEtnryPokemonOrder(port, order);

        if (slot >= 0) {
            if (!pauseCheck()) {
                GSsndPlay(0x19, 0, 0);
            }
            MenuCBPokemonEntryPlayer *player = &menuCBPokemonEntryWork.mPlayer[port];

            player->mPos[slot] = (5 - slot) * 24;
            player->mPos[slot + 6] = 0.0f;
            menuCBPokemonEntrySlotPlaced(port);
        }
        break;
    }
    case 0xFF000000: {
        s32 order = toolentryTaisengetEtnryPokemonOrderNum(port);

        if (order != toolentryTaisenDeleteEtnryPokemonOrder(port)) {
            if (!pauseCheck()) {
                GSsndPlay(0x12, 0, 0);
            }
        }
        break;
    }
    case 0:
        num = toolentryTaisenGetEntryPokemonNum(port);
        gbaCommandEntryPokemon(result, raw);
        for (i = 0; i < num; i++) {
            word[i] = raw[i];
        }
        toolentryTaisenSetEtnryPokemonOrderGBA(port, num, word);
        _menuCBPokemonEntrySetEntryFlag(port, 1);
        break;
    case 0x03000000:
        menuCBPokemonEntryWork.mDisconnected = 0;
        if (menuCBPokemonEntryWork.mDisconnectedPort < 0) {
            menuCBPokemonEntryWork.mDisconnectedPort = device;
        }
        break;
    }
}

extern "C" void menuCBPokemonEntrySlotPlaced(long port)
{
}

/* Read the buttons of one port and act on the party list of its player. */
extern "C" void menuCBPokemonEntryOrderInput(s32 device, s32 port)
{
    u32 changed = GSinputButtonsChanged(device);
    u32 pressed = GSinputButtonsPressed(device);
    u32 buttons = pressed & changed;
    s32 i;
    s32 order;
    u16 num;

    toolentryTaisenGetPokemonItemDataId(port);
    if (!pauseCheck() && (windowGetKeyInfo()->mKeyOn & 0x400) == 0 && windowSearchID(0xE2) == 0) {
        if ((buttons & 0x140) != 0) {
            if ((buttons & 0x40) != 0) {
                s32 order = toolentryTaisengetEtnryPokemonOrderNum(port);

                if (toolentryTaisenDeleteEtnryPokemonOrder(port) != order) {
                    GSsndPlay(0x12, 0, 0);
                }
            } else if ((buttons & 0x100) != 0) {
                _MENUCBTRAINER *trainer = menuCBBios_GetTrainerByPort(port);
                num = (u16)toolentryTaisenGetEntryPokemonNum(port);
                order = trainer->mEntryCount;

                if (order >= (u16)menuCBPokemonEntryNumLimit(port)) {
                    for (i = order; i < num; i++) {
                        trainer->mEntryList[i] = 0xF;
                        trainer->mEntryCount++;
                    }
                    GSsndPlay(0x11, 0, 0);
                }
            }
        } else if ((buttons & 0xC0F) != 0) {
            s32 order = _menuCBPokemonEntryButtonCheck(buttons, port);

            if (order >= 0) {
                s32 slot = toolentryTaisenSetEtnryPokemonOrder(port, order);

                if (slot >= 0) {
                    GSsndPlay(0x19, 0, 0);

                    MenuCBPokemonEntryPlayer *player = &menuCBPokemonEntryWork.mPlayer[port];

                    player->mPos[slot] = (5 - slot) * 24;
                    player->mPos[slot + 6] = 0.0f;
                }
            }
        }
    }
}

/* Party slot a direction or button key asks for, -1 if it is out of reach. */
s32 _menuCBPokemonEntryButtonCheck(unsigned long buttons, long port)
{
    s32 order = -1;
    s32 num = (u16)toolentryTaisenGetPokemonNum(port);

    if (buttons & 0x1) {
        order = 0;
    }
    if (buttons & 0x8) {
        order = 1;
    }
    if (buttons & 0x800) {
        order = 2;
    }
    if (buttons & 0x4) {
        order = 3;
    }
    if (buttons & 0x2) {
        order = 4;
    }
    if (buttons & 0x400) {
        order = 5;
    }
    if (num <= order) {
        return -1;
    }
    return order;
}

void _menuCBPokemonEntryWork(void)
{
    _MENUCBTRAINER *found[4];
    s32 i;

    for (i = 0; i < 4; i++) {
        found[i] = menuCBBios_GetTrainerFromVersusBattleID(i);
        if (found[i] != 0) {
            menuCBPokemonEntryWork.mVersusId[i] = (s8)found[i]->mVersusBattleId;
        } else {
            menuCBPokemonEntryWork.mVersusId[i] = -1;
        }
    }
}

/* Number of Pokemon the rule set lets `port` bring, never more than the
   ones it already entered. */
extern "C" u16 menuCBPokemonEntryNumLimit(long port)
{
    u16 limit;
    u16 num;

    if (menuCBTaisenWorkActive->mRulesetData[menuCBTaisenWorkActive->ruleId].mPokemonCountMode == 0) {
        limit = menuCBTaisenWorkActive->mRulesetData[menuCBTaisenWorkActive->ruleId].mPokemonCount;
    } else {
        limit = 1;
    }
    num = _menuCBPokemonEntryPokemonNum(port);
    if (num < limit) {
        return num;
    }
    return limit;
}

u16 _menuCBPokemonEntryPokemonNum(long port)
{
    u16 limit = menuCBBios_GetPokemonNumLimit();
    u16 num = toolentryTaisenGetPokemonNum(port);

    return num < limit ? num : limit;
}

void _menuCBPokemonEntryWorkInit(void)
{
    s32 battleType = toolentryTaisenGetBattleType();
    s32 playerNum = toolentryTaisenGetEntryPlayerNum();
    long port;

    menuCBPokemonEntryWork.mDisconnected = 1;
    menuCBPokemonEntryWork.mDisconnectedPort = -1;
    menuCBPokemonEntryWork.mBaseX = 0.0f;
    menuCBPokemonEntryWork.mBaseIndex = 0;
    menuCBPokemonEntryWork.mCursor = 0;
    menuCBPokemonEntryWork.mScroll = 0;
    menuCBPokemonEntryWork.mState = 0;
    menuCBPokemonEntryWork.mTimer = 0;
    for (port = 0; port < playerNum; port++) {
        _MENUCBTRAINER *trainer = menuCBBios_GetTrainerByPort(port);
        u32 gbaPort = menuCBTrainerBios_GetControlerID(trainer);
        Hero *hero = menuCBBios_GetHeroImmediately(port);
        s32 type;

        _menuCBPokemonEntrySetEntryFlag(port, 0);
        if (trainer->mHomePlace != 0) {
            gbaCommandSendWazaText(gbaPort, hero);
            type = battleType == 1 ? 2 : 1;
            gbaCommandEntryRequest(gbaPort, (u16)_menuCBPokemonEntryPokemonNum(port),
                (u16)menuCBPokemonEntryNumLimit(port), type);
        }
    }
    for (port = 0; port < 4; port++) {
        toolentryTaisenInitPokemonOrder(port);
    }
    _menuCBPokemonEntryWork();
    _menuCBPokemonEntryDispPosition();
}
