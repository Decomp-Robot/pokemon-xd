#include <menuCBBios.h>
#include <savedata.h>
#include <GSmsg.h>
#include <menuCB.h>
#include <string.h>

extern u32 menuCBRule_RegulationItemNum;

extern "C" {
void heroBiosCopy(Hero *dst, Hero *src);
void heroBiosSetHizukiFlag(Hero *hero, s32 flag);
u8 heroBiosGetHomePlace(Hero *hero);
u8 heroBiosGetSexDataId(Hero *hero);
u8 menuCBRule_IsBlankPokemon(Pokemon *pokemon);
void fightTrainerCreateFightTrainerDataIdToHero(u16 dataId, s32 desperation,
                                                Hero *hero, u32 battleId);
}

void menuCB_SetToolBattleModeFlag(u32 flag)
{
    toolbattle_mode_flag = flag;
}

s32 menuCB_GetToolBattleModeFlag(void)
{
    return toolbattle_mode_flag;
}

s32 menuCB_GetBattleController2P(void)
{
    return _CB.nameEntryMode;
}

u16 *menuCB_RuleTitleNameGet(s32 ruleId)
{
    return _CB.mRulesetData[ruleId].mTitle;
}

u16 menuCBTrainerBios_GetPlayerID(_MENUCBTRAINER *trainer)
{
    return trainer->mPlayerID;
}

void menuCBTrainerBios_SetHomePlace(_MENUCBTRAINER *trainer,
                                               u32 homePlace)
{
    trainer->mHomePlace = homePlace;
}

s32 menuCBTrainerBios_GetHomePlace(_MENUCBTRAINER *trainer)
{
    return trainer->mHomePlace;
}

void menuCBBios_CopyTrainer(_MENUCBTRAINER *dst,
                                       const _MENUCBTRAINER *src)
{
    memcpy(dst, src, sizeof(*dst));
}

s32 menuCBTrainerBios_GetControlerID(_MENUCBTRAINER *trainer)
{
    return trainer->mControlerID;
}

void menuCBTrainerBios_SetControlerID(_MENUCBTRAINER *trainer, s32 controlerID)
{
    trainer->mControlerID = controlerID;
}

void menuCBBios_InitVersusTrainer(_MENUCBTRAINER *trainer,
                                             Hero *hero)
{
    u16 dataId = 0x1388;

    switch (heroBiosGetHomePlace(hero)) {
    case 0:
        dataId = 0x1388;
        break;
    case 1:
        switch (heroBiosGetSexDataId(hero)) {
        case 0:
            dataId = 0x1389;
            break;
        case 1:
            dataId = 0x138A;
            break;
        case 2:
            break;
        }
        break;
    case 2:
        switch (heroBiosGetSexDataId(hero)) {
        case 0:
            dataId = 0x138B;
            break;
        case 1:
            dataId = 0x138C;
            break;
        case 2:
            break;
        }
        break;
    case 3:
        switch (heroBiosGetSexDataId(hero)) {
        case 0:
            dataId = 0x138D;
            break;
        case 1:
            dataId = 0x138E;
            break;
        case 2:
            break;
        }
        break;
    }

    menuCBBios_InitTrainerFromHero(trainer, hero, dataId);
}

void menuCBBios_InitTrainerFromHero(_MENUCBTRAINER *trainer,
                                               Hero *hero, u16 dataId)
{
    Hero work;

    heroBiosCopy(&work, hero);
    heroBiosSetHizukiFlag(&work, 0);
    _menuCBBios_RecoverAllPokemons(&work);
    menuCBBios_ClearTrainer(trainer);
    heroBiosCopy(&trainer->mHero, &work);
    heroBiosCopy(&trainer->mBattleHero, &work);
    trainer->mDataId = dataId;
    trainer->mHomePlace = menuCBBios_FightTrainerDataIDToHomePlace(dataId);
}

void menuCBBios_InitTrainerFromDataID(_MENUCBTRAINER *trainer,
                                                 u16 dataId, u32 battleId)
{
    Hero work;

    heroBiosCopy(&work, &trainer->mBattleHero);
    fightTrainerCreateFightTrainerDataIdToHero(
        dataId, menuCBBios_PortIDtoControlerID(0), &work, battleId);
    heroBiosSetHizukiFlag(&work, 0);
    _menuCBBios_RecoverAllPokemons(&work);
    menuCBBios_ClearTrainer(trainer);
    heroBiosCopy(&trainer->mHero, &work);
    heroBiosCopy(&trainer->mBattleHero, &work);
    trainer->mDataId = dataId;
    trainer->mHomePlace = menuCBBios_FightTrainerDataIDToHomePlace(dataId);
}

void menuCBBios_ClearTrainer(_MENUCBTRAINER *trainer)
{
    u16 defaultDataId;

    defaultDataId = menuCBTrainerBios_GetPlayerID(trainer);
    menuCBBios_InitTrainer(trainer, defaultDataId);
}

void menuCBBios_InitTrainer(_MENUCBTRAINER *trainer, u16 dataId)
{
    memset(trainer, 0, 0x1320);
    trainer->mPlayerID = dataId;
}

s32 menuCBBios_FightTrainerDataIDToHomePlace(u16 dataId)
{
    switch (dataId) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 0x1389:
    case 0x138A:
        return 1;

    case 0x138B:
    case 0x138C:
        return 2;

    case 0x138D:
    case 0x138E:
        return 3;

    case 0x1388:
        return 0;

    default:
        return 0;
    }
}

_MENUCBTRAINER *menuCBBios_GetTrainerFromVersusBattleID(s32 battleId)
{
    s32 i;

    switch (_CB.battleMode) {
    case 0:
    case 1:
        if (battleId >= 0 && battleId <= 1) {
            return &_CB.mTrainer[battleId];
        }
        break;

    case 2:
    default:
        for (i = 0; i < 4; i++) {
            if (battleId == _CB.mTrainer[i].mVersusBattleId) {
                return &_CB.mTrainer[i];
            }
        }
        break;
    }

    return 0;
}

s32 menuCBBios_GetToolBattleTrainerKind(void)
{
    return 0;
}

Hero *menuCBBios_GetRegisteredHero(void)
{
    return 0;
}

_MENUCBTRAINER *menuCBBios_GetTrainer(s32 controllerId)
{
    s32 port = menuCBBios_ControlerIDtoPortID(controllerId);

    if (port < 0) {
        return 0;
    }
    return &_CB.mTrainer[port];
}

_MENUCBTRAINER *menuCBBios_GetTrainerOrgImmediately(s32 index)
{
    if (index < 0 || index >= 4) {
        return 0;
    }
    return &_CB.mTrainerImmediately[index];
}

_MENUCBTRAINER *menuCBBios_GetTrainerImmediately(s32 index)
{
    if (index < 0 || index >= 4) {
        return 0;
    }
    return &_CB.mTrainer[index];
}

Hero *menuCBBios_GetHeroImmediately(s32 index)
{
    if (index < 0 || (u32)index >= 4) {
        return 0;
    }
    return &_CB.mTrainer[index].mHero;
}

void _menuCBBios_RecoverAllPokemons(Hero *hero)
{
    u16 slot;
    Pokemon *pokemon;

    for (slot = 0; slot < 6; slot++) {
        pokemon = heroBiosGetPokemonPtr(hero, slot);
        if (menuCBRule_IsBlankPokemon(pokemon) == 0) {
            pokemonAllKaihuku(pokemon);
        }
    }
}

s32 menuCBBios_ControlerIDtoPortID(s32 controllerId)
{
    s32 port;

    for (port = 0; port < 4; port++) {
        if (controllerId == staControlerMap[port]) {
            return port;
        }
    }
    return -1;
}

s32 menuCBBios_PortIDtoControlerID(s32 portID)
{
    return staControlerMap[portID];
}

extern "C" {

u32 menuCBRule_ParticipationPokemonMax(void);
FightRuleData *menuCBRule_ConstantRule2(s32 ruleId);
FightRuleData *menuCB_OreRule(void);
}

u32 menuCBBios_ParticipationPokemonMax(void)
{
    return menuCBRule_ParticipationPokemonMax();
}

s32 menuCBBios_CurrentRuleType(void)
{
    return _CB.ruleId;
}

FightRuleData *menuCBBios_CurrentRule(void)
{
    FightRuleData *oreRule = menuCB_OreRule();
    FightRuleData *ruleData;
    s32 ruleId;

    if (oreRule != 0) {
        return oreRule;
    }

    ruleData = menuCBRule_ConstantRule2(menuCBBios_CurrentRuleType());
    ruleId = menuCBBios_CurrentRuleType();

    switch (ruleId) {
    case 0:
    case 1:
    case 2:
        if (ruleData != 0) {
            return ruleData;
        }
        break;

    case 3:
    case 4:
    case 5:
    default:
        break;
    }

    return menuCBBios_GetRule(menuCBBios_CurrentRuleType());
}

FightRuleData *menuCBBios_GetRule(s32 ruleId)
{
    if (ruleId < 0 || (u32)ruleId >= 6) {
        return 0;
    }
    return &_CB.mRulesetData[ruleId];
}

void menuCBBios_RuleConstRuleSet(void)
{
    FightRuleData *ruleData =
        (FightRuleData *)savedataGetStatus(0, 0xE);

    ruleData[0] = *menuCBRule_GetDefaultRuleData(0);
    ruleData[1] = *menuCBRule_GetDefaultRuleData(1);
    ruleData[2] = *menuCBRule_GetDefaultRuleData(2);
    ruleData[0].mMsgId = 0x9124;
    ruleData[1].mMsgId = 0xA2D4;
    ruleData[2].mMsgId = 0xB51C;
}

void menuCBBios_RuleEditSaveON(u8 set)
{
    FightRuleData *ruleData =
        (FightRuleData *)savedataGetStatus(0, 0xE);

    if (set == 1) {
        ruleData[0].mMsgId = 0x9124;
        ruleData[1].mMsgId = 0xA2D4;
        ruleData[2].mMsgId = 0xB51C;
        ruleData[3].mMsgId = 0x47FD;
        ruleData[4].mMsgId = 0x7422;
        ruleData[5].mMsgId = 0x5633;
    } else {
        ruleData[5].mMsgId = 0;
        ruleData[4].mMsgId = 0;
        ruleData[3].mMsgId = 0;
        ruleData[2].mMsgId = 0;
        ruleData[1].mMsgId = 0;
        ruleData[0].mMsgId = 0;
    }
}

void menuCBBios_CurrentlyWorkingToolBattleSet(u8 dirty)
{
    _TOOL_SAVEDATA *rules = (_TOOL_SAVEDATA*)savedataGetStatus(0, 0xE);

    rules->mDirty = dirty;
}

u8 menuCBBios_CurrentlyWorkingToolBattle(void)
{
    _TOOL_SAVEDATA *rules;

    rules = (_TOOL_SAVEDATA*)savedataGetStatus(0, 0xE);
    if (rules == 0) {
        return 0;
    }
    return rules->mDirty;
}

_MENUCBWORK *menuCBBios_WorkContext(void)
{
    return &_CB;
}

void menuCBBios_InitContext(_MENUCBWORK *work)
{
    static const u16 sta[4] = { 1, 2, 3, 4 };
    u32 i;

    menuCBTaisenWorkActive = &_CB;
    if (!menuCBBios_ContextIsLocked()) {
        for (i = 0; i < 4; i++) {
            menuCBBios_InitTrainerFromDataID(&work->mTrainer[i], sta[i], 0);
            menuCBTrainerBios_SetControlerID(&work->mTrainer[i], menuCBBios_PortIDtoControlerID(i));
            work->mTrainer[i].mPlayerID = i;
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
    menuCBBios_UnlockContext();
}

void menuCB_RULE_INIT(_TOOL_SAVEDATA *rules)
{
    s32 i;
    s32 j;
    const FightRuleData *d;
    u16 *str;

    if (rules->mRule[0].mMsgId == 0x9124 && rules->mRule[1].mMsgId == 0xA2D4
        && rules->mRule[2].mMsgId == 0xB51C && rules->mRule[3].mMsgId == 0x47FD
        && rules->mRule[4].mMsgId == 0x7422 && rules->mRule[5].mMsgId == 0x5633) {
        menuCBBios_RuleConstRuleSet();
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
                rules->mRule[3].mTitle[j] = *str;
                if (*str != 0) {
                    str++;
                }
            } else {
                rules->mRule[3].mTitle[j] = 0;
            }
        }
        str = GSmsgGetStringPtr(0x438B);
        for (i = 0; i < 21; i++) {
            if (str != 0) {
                if (*str == 0xFFFF) {
                    str += 2;
                }
                rules->mRule[4].mTitle[i] = *str;
                if (*str != 0) {
                    str++;
                }
            } else {
                rules->mRule[4].mTitle[i] = 0;
            }
        }
        str = GSmsgGetStringPtr(0x438C);
        for (i = 0; i < 21; i++) {
            if (str != 0) {
                if (*str == 0xFFFF) {
                    str += 2;
                }
                rules->mRule[5].mTitle[i] = *str;
                if (*str != 0) {
                    str++;
                }
            } else {
                rules->mRule[5].mTitle[i] = 0;
            }
        }
        menuCBBios_InitContextAtFirst(&_CB, rules);
        rules->mDirty = 0;
    }
}

void menuCBBios_InitContextAtFirst(_MENUCBWORK *w, _TOOL_SAVEDATA *set)
{
    s32 i;
    FightRuleData *dst;
    const FightRuleData *d;

    menuCBBios_UnlockContext();
    memset(w, 0, sizeof(*w));
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
    memcpy(menuCB_RuleTitleNameGet(3), &set->mRule[3].mTitle[0], 0x2A);
    memcpy(menuCB_RuleTitleNameGet(4), &set->mRule[4].mTitle[0], 0x2A);
    memcpy(menuCB_RuleTitleNameGet(5), &set->mRule[5].mTitle[0], 0x2A);
}

u8 menuCBBios_ContextIsLocked(void)
{
    return menuCBBiosContextLocked;
}

void menuCBBios_UnlockContext(void)
{
    menuCBBiosContextLocked = 0;
}

void menuCBBios_LockContext(void)
{
    menuCBBiosContextLocked = 1;
}
