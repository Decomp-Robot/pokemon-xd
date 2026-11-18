#include <types.h>
#include <fightSeq.h>
#include <Pokemon.h>
#include <fade.h>
#include <time.h>
#include <sound.h>
#include <floor.h>
#include <floorManager.h>
#include <GSres.h>
#include <GSthread.h>
#include <GSvec.h>
#include <GSparticle.h>
#include <sequence.h>
#include <wazaSequence.h>
#include <menuModel.h>
#include <menuHologram.h>
#include <menuPokemonStatus.h>
#include <pokemonRelive.h>
#include <pokemonBios.h>
#include <winMsg.h>
#include <msgctrl.h>
#include <evolution.h>
#include <pokemonDB.h>
#include <menuSub.h>
#include <menuNameEntry.h>
#include <exribbon.h>
#include <hero.h>
#include <menuPokemon.h>
#include <menuPocketTool.h>
#include <script.h>
#include <deck.h>

#include <darkPokemon.h>
#include <pokemonWazaSequenceDisplay.h>

/*
 * pokemonRelive.cpp
 *
 * Relive (purification) part of the Pokemon menu: the ceremony that plays
 * while a Dark Pokemon is purified in the Relive hall. The relive commands
 * (preReliveSetParameter) store the ceremony pattern they were handed in
 * pParameter; -1 means that no pattern has been selected.
 */

// Parameters of the running relive ceremony.
class ReliveParameter {
public:
    ReliveParameter()
    {
    }

    u32 mUnknown0;

    // Selected ceremony pattern; -1 while there is none.
    s32 mPattern;
};

ReliveParameter pParameter;

Pokemon *mParameter;
GSthread *hokoraParticleThread;
s32 keepRelivePatternIsHologram;
_GSparticleGen *hokoraParticleGen;
bool hokoraParticleActive;

enum RelivePattern {
    RELIVE_PATTERN_NONE = -1,
    RELIVE_PATTERN_RELIVE_HALL = 0, // purification at the Relive Hall shrine
    RELIVE_PATTERN_HOLOGRAM = 1,    // purification started from the hologram menu
};

enum ReliveWazaSequenceId {
    RELIVE_WAZA_SEQUENCE_RELIVE = 77,
    RELIVE_WAZA_SEQUENCE_IDLE = 122,
    RELIVE_WAZA_SEQUENCE_FLYING = 315,
    RELIVE_WAZA_SEQUENCE_LUGIA = 259,
    RELIVE_WAZA_SEQUENCE_LUGIA_MODEL = 260,
};
enum ReliveWazaSequenceCategory { RELIVE_WAZA_CATEGORY = 4 };
enum ReliveSoundId { SE_HOKORA_PARTICLE = 1431, ME_RELIVE = 949, ME_WAZA_RETURN = 967, ME_RELIVE_RIBBON = 974 };
enum ReliveMessageId {
    MSG_RELIVE_LEVEL_UP = 50023,
    MSG_RELIVE_SELECT_POKEMON = 50502,
    MSG_RELIVE_WAZA_RETURN = 50503,
    MSG_RELIVE_EXP = 50504,
    MSG_RELIVE_RIBBON = 50505,
    MSG_RELIVE_NICKNAME_ASK = 50506,
    MSG_RELIVE_WAZA_NAME = 50510,
    MSG_RELIVE_WAZA_NAME_LAST = 50511,
};
#define RELIVE_FRIEND_BONUS 70
#define RELIVE_EVOLUTION_INVALID 0xFFFF
#define HOKORA_PARTICLE_STACK_SIZE 16384
#define HOKORA_PARTICLE_RES_ID 0x0FAE1400
#define RELIVE_CEREMONY_FLOOR_ID 901

#define FADE_TIME 0.5f
#define WAZA_SEQUENCE_TIMEOUT 4.0f
#define HOKORA_PARTICLE_TIME 4.0f
#define FADE_IN_TIME 1.0f
#define POKEMON_LEVEL_MAX 100

LoadSequenceEntry reliveWazaSeqTbl[2] = {
    {RELIVE_WAZA_SEQUENCE_RELIVE, RELIVE_WAZA_CATEGORY},
    {RELIVE_WAZA_SEQUENCE_IDLE, RELIVE_WAZA_CATEGORY},
};
LoadSequenceEntry hologramReliveWazaSeqTbl[1] = {
    {RELIVE_WAZA_SEQUENCE_RELIVE, RELIVE_WAZA_CATEGORY},
};
LoadSequenceEntry hologramPigeonReliveWazaSeqTbl[1] = {
    {RELIVE_WAZA_SEQUENCE_FLYING, RELIVE_WAZA_CATEGORY},
};
LoadSequenceEntry noWazaSeqTbl[1] = {
    {RELIVE_WAZA_SEQUENCE_IDLE, RELIVE_WAZA_CATEGORY},
};

// Waits for a purification sequence to finish, giving up after a while.
inline void _reliveWaitWaza(WazaSequence *waza)
{
    f32 time = 0.0f;
    while (WazaSequence_IsActive(waza)) {
        GSthreadSwitch();
        time += timeGetLastFrameSec();
        if (time > WAZA_SEQUENCE_TIMEOUT) {
            break;
        }
    }
}


// Brings the purified pokemon back on the ceremony stage after a menu or
// scene has replaced it.
inline void _reliveDisplayPokemon(Pokemon *pokemon)
{
    ModelSequence *model;
    WazaSequence *waza;

    loadSequence(&model, pokemon, noWazaSeqTbl, 1);
    fadeSetToBlackInIfBlack(FADE_TIME);
    fadeSet(FADE_IN_BLACK, FADE_TIME);
    Sequence_SetVisibility(model, true);
    waza = Sequence_GetWaza(model, noWazaSeqTbl[0].sequence, noWazaSeqTbl[0].category);
    if (waza != 0) {
        WazaSequence_Start(waza, 1);
        if (pokemon->getPokemonDataId() == POKEMON_DATA_ID_LUNATONE) {
            Sequence_DoAnimation(model, menuModelGetPokemonAnimationIndexByPointer(pokemon));
        }
        _reliveWaitWaza(waza);
        WazaSequence_Free(waza);
    }
    if (pokemon->getPokemonDataId() == POKEMON_DATA_ID_LUNATONE) {
        Sequence_DoAnimation(model, menuModelGetPokemonAnimationIndexByPointer(pokemon));
    }
}

// Forget-waza callback of the relive level-up: lets the player pick the move
// to forget, then brings the ceremony scene back.
s32 _cbWazaForget(Pokemon *pokemon, unsigned short wazaID, long context)
{
    s32 result;

    fadeSetToBlackOutIfNotBlack(FADE_TIME);
    fadeCheck(1);
    if (keepRelivePatternIsHologram) {
        menuHologramUnloadDatas();
    }
    result = menuPokemonStatusOpenWazaLearn(pokemon, wazaID);
    if (result >= 4) {
        result = -1;
    }
    if (keepRelivePatternIsHologram) {
        menuHologramReloadDatas();
        fadeSetToBlackInIfBlack(FADE_TIME);
        fadeCheck(1);
    } else {
        _reliveDisplayPokemon(pokemon);
    }
    return (s8)result;
}

inline void hokoraParticleStop(void)
{
    if (hokoraParticleActive == true) {
        GSparticleDestroyGenerator(hokoraParticleGen, true);
        hokoraParticleGen = 0;
        hokoraParticleActive = false;
    }
}

void _preReliveDisplayHokoraParticleThreadFunc(void)
{
    GSvec position(0.0f, 7.0f, 0.0f);

    if (hokoraParticleActive == true) {
        hokoraParticleStop();
    }
    hokoraParticleGen = GSparticleCreateGenerator(
        (_GSparticleBank *)GSresGetResource(floorGetCurrentGroupID(), HOKORA_PARTICLE_RES_ID), 0, 0);
    GSparticleSetGeneratorPosition(hokoraParticleGen, &position);
    hokoraParticleActive = true;
    soundOpenPlay(SE_HOKORA_PARTICLE, 0, 127);
    waitSequenceTime(HOKORA_PARTICLE_TIME);
    hokoraParticleStop();
    hokoraParticleThread = 0;
}

extern "C" void preReliveDisplayHokoraParticle(void)
{
    if ((u16)scriptGetDarkPointZeroPokemonNum() != 0 && hokoraParticleActive == false) {
        hokoraParticleThread = GSthreadCreate(1, GSfloorGetCurrentFloorGroupID(), HOKORA_PARTICLE_STACK_SIZE, 1, 1,
                                              _preReliveDisplayHokoraParticleThreadFunc);
    }
}

// Hands a purified pokemon the experience it pooled while it was dark,
// one level at a time, showing each level-up and teaching the waza learned
// on the way. Returns true when the pokemon gained at least one level.
bool _expRecover(Pokemon *pokemon, unsigned long exp)
{
    u8 learnIndex;
    u8 oboeIndex = 0;
    s16 hp, attack, defense, spAttack, spDefense, speed;
    u32 restExp = exp;
    u8 newLevel;
    u32 curExp;
    bool levelUp = false;
    u32 nextExp;
    u8 level = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_LEVEL, 0);
    u16 itemID;
    u16 wazaID;
    MENU_LVUP_STATUS diff;
    MENU_LVUP_STATUS after;

    while (true) {
        if (restExp == 0) {
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_POOL_EXP, 0, restExp);
            break;
        }
        level = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_LEVEL, 0);
        if (level >= POKEMON_LEVEL_MAX) {
            break;
        }
        curExp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_EXP, 0);
        nextExp = pokemonGetLevelToExp(pokemon, level + 1);
        hp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_MAX, 0);
        attack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTACK, 0);
        defense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DEFENSE, 0);
        spAttack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_ATTACK, 0);
        spDefense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_DEFENSE, 0);
        speed = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPEED, 0);
        curExp += restExp;
        if (curExp >= nextExp) {
            restExp = curExp - nextExp;
            levelUp = true;
            pokemonBiosSetExp(pokemon, nextExp);
            pokemonResetBasisStatus(pokemon);
            itemID = pokemonGetSoubiItemSoubiDataId(pokemon);
            pokemonGetFriendFormPokemonFriendFilterId(pokemon, itemID, 0);
            newLevel = pokemonBiosGetLevel(pokemon);
            soundOpenPlayME(ME_WAZA_RETURN, 0, 255);
            msgctrlSetValue(MSGCTRL_NUM_INPUT, newLevel);
            winMsgOpenField(MSG_RELIVE_LEVEL_UP, true, false);

            after.mHp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_MAX, 0);
            after.mAttack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTACK, 0);
            after.mDefense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DEFENSE, 0);
            after.mSpAttack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_ATTACK, 0);
            after.mSpDefense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_DEFENSE, 0);
            after.mSpeed = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPEED, 0);
            diff.mHp = after.mHp - hp;
            diff.mAttack = after.mAttack - attack;
            diff.mDefense = after.mDefense - defense;
            diff.mSpAttack = after.mSpAttack - spAttack;
            diff.mSpDefense = after.mSpDefense - spDefense;
            diff.mSpeed = after.mSpeed - speed;
            diff.mShowPlus = 1;
            winMsgOpenLevelUpStatus(&diff, true);
            after.mShowPlus = 0;
            winMsgOpenLevelUpStatus(&after, true);
            winMsgCloseLevelUpStatus(true);

            oboeIndex = 0;
            while ((wazaID = pokemonGetOboeWazaDataId(pokemon, newLevel, &oboeIndex)) != 0) {
                if (pokemonSearchWazaDataId(pokemon, wazaID) == -1) {
                    if (evolutionWazaLearn(pokemon, wazaID, &learnIndex, EVOLUTION_WINDOW_FIELD, _cbWazaForget, 0) != 0) {
                        pokemonWazaCreate(pokemon, learnIndex, wazaID);
                    }
                }
                oboeIndex++;
            }
        } else {
            restExp = 0;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_EXP, 0, curExp);
        }
    }
    return levelUp;
}

// Puts the pokemon on the Relive Hall shrine and plays its purification
// sequences. Returns false when the pokemon could not be shown.
inline bool _reliveHokoraDisplay(Pokemon *pokemon)
{
    ModelSequence *model;
    s32 i;
    WazaSequence *waza;

    if (pokemonCheckValid(pokemon) != true) {
        return false;
    }
    bool loaded = loadSequence(&model, pokemon, reliveWazaSeqTbl, 2);
    if (loaded == true) {
        fadeSet(FADE_IN_BLACK, FADE_TIME);
        Sequence_SetVisibility(model, true);
        for (i = 0; i < 2; i++) {
            waza = Sequence_GetWaza(model, reliveWazaSeqTbl[i].sequence, reliveWazaSeqTbl[i].category);
            if (waza != 0) {
                WazaSequence_Start(waza, 1);
                if (pokemon->getPokemonDataId() == POKEMON_DATA_ID_LUNATONE) {
                    Sequence_DoAnimation(model, menuModelGetPokemonAnimationIndexByPointer(pokemon));
                }
                _reliveWaitWaza(waza);
                WazaSequence_Free(waza);
            }
        }
        if (pokemon->getPokemonDataId() == POKEMON_DATA_ID_LUNATONE) {
            Sequence_DoAnimation(model, menuModelGetPokemonAnimationIndexByPointer(pokemon));
        }
        return true;
    }
    return false;
}

// Hologram ceremony of Lugia: its own model is brought onto the hologram
// stage and both play their part of the purification together. Returns the
// model of Lugia, which stays on the stage.
inline ModelSequence *_reliveHologramLugiaDisplay(Pokemon *pokemon, NullSequence *stage)
{
    ModelSequence *model;
    WazaSequence *stageWaza;
    WazaSequence *modelWaza;

    Sequence_LoadWaza(stage, RELIVE_WAZA_SEQUENCE_LUGIA, RELIVE_WAZA_CATEGORY, 0);
    model = Sequence_LoadPokemon(POKEMON_DATA_ID_LUGIA, pokemonBiosGetRnd(pokemon),
                                 pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SHINY, 0) != 0);
    Sequence_LoadWaza(model, RELIVE_WAZA_SEQUENCE_LUGIA_MODEL, RELIVE_WAZA_CATEGORY, 0);
    GSvec position(0.0f, 0.0f, 0.0f);
    GSvec rotation(0.0f, 0.0f, 0.0f);
    GSvec scale(1.0f, 1.0f, 1.0f);
    Sequence_GetPosition(stage, &position);
    Sequence_GetRotation(stage, &rotation);
    Sequence_GetScale(stage, &scale);
    Sequence_SetPosition(model, &position);
    Sequence_SetRotation(model, &rotation);
    Sequence_SetScale(model, &scale);
    stageWaza = Sequence_GetWaza(stage, RELIVE_WAZA_SEQUENCE_LUGIA, RELIVE_WAZA_CATEGORY);
    modelWaza = Sequence_GetWaza(model, RELIVE_WAZA_SEQUENCE_LUGIA_MODEL, RELIVE_WAZA_CATEGORY);
    WazaSequence_Start(stageWaza, 0);
    WazaSequence_Start(modelWaza, 0);
    f32 time = 0.0f;
    while (WazaSequence_IsActive(stageWaza) || WazaSequence_IsActive(modelWaza)) {
        GSthreadSwitch();
        time += timeGetLastFrameSec();
    }
    WazaSequence_Free(stageWaza);
    WazaSequence_Free(modelWaza);
    return model;
}

// Hologram ceremony: plays the purification sequences on the hologram stage
// that shows the pokemon.
inline void _reliveHologramDisplay(Pokemon *pokemon, NullSequence *stage)
{
    LoadSequenceEntry *table = hologramReliveWazaSeqTbl;
    s32 count = 1;
    u16 dataID = pokemon->getPokemonDataId();

    if (dataID == POKEMON_DATA_ID_PIDGEOTTO || dataID == POKEMON_DATA_ID_LUNATONE
        || dataID == POKEMON_DATA_ID_SOLROCK) {
        table = hologramPigeonReliveWazaSeqTbl;
        count = 1;
    }
    GSvec scale;
    Sequence_GetScale(stage, &scale);
    menuModelSetScale(scale.x);
    for (s32 i = 0; i < count; i++) {
        Sequence_LoadWaza(stage, table[i].sequence, table[i].category, 0);
    }
    Sequence_SetVisibility(stage, true);
    for (s32 j = 0; j < count; j++) {
        WazaSequence *waza = Sequence_GetWaza(stage, table[j].sequence, table[j].category);
        if (waza != 0) {
            WazaSequence_Start(waza, 0);
            while (WazaSequence_IsActive(waza)) {
                GSthreadSwitch();
            }
            WazaSequence_Free(waza);
        }
    }
    menuModelReSetScale();
}

// Plays the purification ceremony for pokemon and makes it a normal pokemon
// again: heals it, returns its moves, its pooled experience and friendship,
// evolves it when it levelled up, and offers to give it a nickname.
// Returns the model created for the hologram ceremony, if any.
ModelSequence *reliveCeremonyAll(long pattern, Pokemon *pokemon, unsigned long sourceSequence)
{
    ModelSequence *lugiaModel = 0;
    u16 addedSpecies;
    POKEMON_EVOLUTION_INFO info;
    u16 oldWaza[4];
    u16 evoSpecies;
    s32 oldWazaNum;
    s32 newWazaNum;
    u16 darkID;
    u32 exp;
    s32 i;

    keepRelivePatternIsHologram = false;
    switch (pattern) {
    case RELIVE_PATTERN_RELIVE_HALL:
        if (!_reliveHokoraDisplay(pokemon)) {
            return 0;
        }
        break;
    case RELIVE_PATTERN_HOLOGRAM:
        keepRelivePatternIsHologram = true;
        if (pokemon->getPokemonDataId() == POKEMON_DATA_ID_LUGIA) {
            lugiaModel = _reliveHologramLugiaDisplay(pokemon, (NullSequence *)sourceSequence);
        } else {
            _reliveHologramDisplay(pokemon, (NullSequence *)sourceSequence);
        }
        break;
    }

    addedSpecies = 0;
    if (pokemonCheckValid(pokemon) != true) {
        return 0;
    }

    oldWazaNum = 0;
    while (oldWazaNum < 4) {
        u16 wazaID = pokemonBiosGetPokemonWazaDataId(pokemon, oldWazaNum);
        if (wazaID == 0) {
            break;
        }
        oldWaza[oldWazaNum++] = wazaID;
    }

    pokemonAllKaihuku(pokemon);
    darkID = pokemonBiosGetDarkpokemonDataId(pokemon);
    darkPokemonBiosSetReliveFlag(darkID, 1);
    if (pokemonIsJoutaiDataId(pokemon, JOUTAI_DARK_HYPER_MODE)) {
        pokemonInitJoutaiDataId(pokemon, JOUTAI_DARK_HYPER_MODE);
    }
    soundOpenPlayME(ME_RELIVE, 0, 255);

    newWazaNum = 0;
    for (i = 0; i < 4; i++) {
        u16 wazaID = pokemonBiosGetPokemonWazaDataId(pokemon, i);
        if (wazaID == 0) {
            break;
        }
        bool known = false;
        for (s32 j = 0; j < oldWazaNum; j++) {
            if (wazaID == oldWaza[j]) {
                known = true;
                break;
            }
        }
        if (!known) {
            newWazaNum++;
        }
    }

    if (newWazaNum != 0) {
        u16 *name = pokemon_GetNickname(pokemon);
        msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)name);
        winMsgOpenField(MSG_RELIVE_WAZA_RETURN, true, false);
        for (i = 0; i < newWazaNum; i++) {
            soundOpenPlayME(ME_WAZA_RETURN, 0, 255);
            u16 wazaID = pokemonBiosGetPokemonWazaDataId(pokemon, i);
            msgctrlSetValue(MSGCTRL_VALUE_WAZA_ID, wazaID);
            s32 msgID = MSG_RELIVE_WAZA_NAME;
            winMsgOpenField((i == newWazaNum - 1) ? MSG_RELIVE_WAZA_NAME_LAST : msgID, true, false);
        }
    }

    exp = pokemonBiosGetPoolExp(pokemon);
    exp += DeckDarkPokemon::getDeckDarkPokemonObj(darkID)->getBonusExp();
    if (exp != 0) {
        msgctrlSetValue(MSGCTRL_NUM_INPUT, exp);
        winMsgOpenField(MSG_RELIVE_EXP, true, false);
        winMsgCloseField(true);
    }

    u16 friendship = pokemonBiosGetFriend(pokemon);
    pokemonBiosSetFriend(pokemon, friendship + pokemonBiosGetPoolFriend(pokemon) + RELIVE_FRIEND_BONUS);
    pokemonBiosSetPoolFriend(pokemon, 0);

    if (_expRecover(pokemon, exp) == true) {
        evoSpecies = pokemonEvolutionCheck(pokemon, EVOLUTION_CHECK_LEVEL_UP, 0, &addedSpecies, &info);
        if (evoSpecies != 0 && evoSpecies != RELIVE_EVOLUTION_INVALID) {
            fadeSetToBlackOutIfNotBlack(FADE_TIME);
            fadeCheck(1);
            if (keepRelivePatternIsHologram) {
                menuHologramUnloadDatas();
            }
            if (pokemonEvolutionAll(pokemon, evoSpecies, addedSpecies, &info, 0, 1, 1, 0) == 0) {
                if (keepRelivePatternIsHologram) {
                    menuHologramReloadDatas();
                    fadeSetToBlackInIfBlack(FADE_TIME);
                    fadeCheck(1);
                } else {
                    _reliveDisplayPokemon(pokemon);
                }
                msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)pokemon_GetNickname(pokemon));
            } else {
                msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)pokemon_GetNickname(pokemon));
                if (keepRelivePatternIsHologram) {
                    menuHologramReloadDatas();
                    fadeSetToBlackInIfBlack(FADE_TIME);
                    fadeCheck(1);
                } else {
                    _reliveDisplayPokemon(pokemon);
                }
            }
        }
    }

    soundOpenPlayME(ME_RELIVE_RIBBON, 0, 255);
    exribbonSetNarionalRibbon(pokemon);
    winMsgOpenField(MSG_RELIVE_RIBBON, true, false);
    winMsgOpenField(MSG_RELIVE_NICKNAME_ASK, true, true);
    s32 answer = menuSubOpenYesNoField();
    winMsgCloseField(true);
    if (answer == 0) {
        fadeSetToBlackOutIfNotBlack(FADE_TIME);
        fadeCheck(1);
        if (keepRelivePatternIsHologram) {
            menuHologramUnloadDatas();
        }
        Sequence_SetVisibility(lugiaModel, false);
        Sequence_Free(lugiaModel);
        lugiaModel = 0;
        menuNameEntryOpenNoFade(NAME_ENTRY_MODE_POKEMON, (s32)pokemon);
        if (keepRelivePatternIsHologram) {
            menuHologramReloadDatas();
        } else {
            _reliveDisplayPokemon(pokemon);
        }
    }
    return lugiaModel;
}

ModelSequence *reliveCeremonyFromHologram(Pokemon *pokemon, unsigned long sourceSequence)
{
    MenuPtWorkSound sound;

    menuPtStopSound(&sound);
    return reliveCeremonyAll(RELIVE_PATTERN_HOLOGRAM, pokemon, sourceSequence);
}

// Script command: run the ceremony for the Pokemon chosen in preReliveMain,
// then leave the ceremony floor.
extern "C" void reliveMain(void)
{
    reliveCeremonyAll(RELIVE_PATTERN_RELIVE_HALL, mParameter, 0);
    fadeSetToBlackOutIfNotBlack(FADE_TIME);
    fadeCheck(1);
    pParameter.mPattern = RELIVE_PATTERN_NONE;
    mParameter = 0;
    GSfloorPop();
    floorSetFadeScript(0, 0);
    floorWaitForChange();
}

// Let the player pick a Pokemon that is ready for purification and move to
// the ceremony floor.
extern "C" void preReliveMain(bool wait)
{
    MenuPtWorkSound sound;
    s32 slot;

    if (scriptGetDarkPointZeroPokemonNum() != 0) {
        hokoraParticleStop();
        if ((u16)scriptGetDarkPointZeroPokemonNum() == 0) {
            pParameter.mPattern = RELIVE_PATTERN_NONE;
            return;
        }
        winMsgOpenField(MSG_RELIVE_SELECT_POKEMON, true, false);
        winMsgCloseField(true);
        slot = menuPokemonOpen(PARTY_MODE_SELECT_PURIFICATION_READY, 0, 0);
        if (slot != -1) {
            mParameter = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, slot);
            fadeSetToBlackOutIfNotBlack(FADE_TIME);
            fadeCheck(1);
            menuPtStopSound(&sound);
            GSfloorPush(RELIVE_CEREMONY_FLOOR_ID);
            floorSetFadeScript(0, 0);
            floorWaitForChange();
            fadeSetToBlackInIfBlack(FADE_IN_TIME);
            if (wait == true) {
                fadeCheck(1);
            }
            menuPtRestoreSound(&sound);
        }
    } else {
        pParameter.mPattern = RELIVE_PATTERN_NONE;
    }
}

#pragma force_active on
extern "C" void preReliveInit(void)
{
    pParameter.mPattern = RELIVE_PATTERN_NONE;
}
#pragma force_active reset

#pragma dont_inline on
extern "C" void preReliveSetParameter(int mode, int parameter, bool wait)
{
    pParameter.mPattern = parameter;
    if (mode == PRE_RELIVE_MODE_START_PURIFICATION || mode == PRE_RELIVE_MODE_START_HOLOGRAM) {
        preReliveMain(wait);
    }
}
#pragma dont_inline reset

extern "C" void preReliveExit(void)
{
    if (hokoraParticleThread != 0 && GSthreadIsRunning(hokoraParticleThread)) {
        GSthreadClose(hokoraParticleThread);
        hokoraParticleThread = 0;
    }

    hokoraParticleStop();
}
