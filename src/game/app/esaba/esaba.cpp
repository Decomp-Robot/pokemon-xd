#include <types.h>
#include <GSflag.h>
#include <GSthread.h>
#include <hsdRandom.h>
#include <heroMove.h>
#include <hero.h>
#include <gamedata.h>
#include <gameSetup.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <msgctrl.h>
#include <winMsg.h>
#include <app/pcbox/pcbox.h>
#include <undertaker.h>
#include <esaba.h>
#include <menu.h>
#include <menuScript.h>

// Pokemon that can appear at a Poke Spot.
struct EsabaPokemonData {
    u16 unknown;
    u16 species;
    u32 rate;        // appearance chance, in percent
    u32 digestSteps; // steps the pokemon needs to eat one snack
};

struct EsabaPokemonTable {
    u32 *number; // entry count
    EsabaPokemonData *data;
};

// One Poke Spot; every field but the table is a GS flag ID.
struct EsabaData {
    s32 esaFlag;     // snacks left in the spot
    s32 appearFlag;  // steps walked toward the next arrival
    s32 stepFlag;    // steps toward the next retry / snack eaten
    s32 pokemonFlag; // species at the spot, 0 when empty
    s32 rndFlag;     // personality value of that pokemon
    s32 stopFlag;    // set while the spot is suspended
    EsabaPokemonTable **pokemonTable;
};

// Arrival settings shared by every spot.
struct EsabaAppData {
    u16 appearStepMin;  // steps before arrivals are rolled
    u16 appearStepMax;  // steps after which a pokemon always arrives
    u16 retryStep;      // steps between arrival rolls
    u16 retryChance;    // one in this many rolls succeeds
    u16 exFlagValue;    // exFlag value that unlocks the EX pokemon
    u16 unknown1;
    u32 unknown2;
    u32 exGameSetupEntry; // flag_etc entry whose progress unlocks them
    s32 exFlag;
};

extern "C" {
extern EsabaData *esabadata;
extern EsabaPokemonData *esabapokemondata_EX;
extern EsabaAppData *esabaappdata;
}

enum EsabaPlace { ESABA_PLACE_MAX = 3 };
enum EsabaExPokemon {
    ESABA_EX_RARE = 0,  // EX table entry of the rare EX pokemon
    ESABA_EX_NORMAL = 1 // EX table entry of the common EX pokemon
};

// The two EX pokemon are created with the hero as their catching trainer.
enum EsabaPokemonLevel { ESABA_EX_LEVEL = 10 };

enum EsabaItem { ESABA_ESA_ITEM = 511 };
enum EsabaSnackCapacity { ESABA_ESA_MAX = 10 };

enum EsabaMessage {
    ESABA_MSG_SET_ADD = 51501,     // spot already holds snacks
    ESABA_MSG_SET_EMPTY = 51502,   // spot is empty
    ESABA_MSG_RETURN_BAG = 51503,
    ESABA_MSG_PUT = 51504,
    ESABA_MSG_RETURN_FULL = 51506,
    ESABA_MSG_RETURN_PC = 51507
};

#define ESABA_PROGRESS_FLAG 964 // story progress GS flag

// Same shiny threshold as Pokemon::isRareParam.
#define ESABA_RARE_PARAM_LIMIT 8

enum EsabaSpotArea {
    ESABA_FIRST_SPOT_AREA = 90,
    ESABA_SECOND_SPOT_AREA = 91,
    ESABA_THIRD_SPOT_AREA = 92
};

static long _esabaHandle;

static inline bool _esabaIsMiraboPlace(int place)
{
    int miraboPlace = underTakerGetMiraboPlace();
    u32 placeID[ESABA_PLACE_MAX] = {
        ESABA_FIRST_SPOT_AREA, ESABA_SECOND_SPOT_AREA, ESABA_THIRD_SPOT_AREA
    };
    return miraboPlace == placeID[place];
}

void _esabaCallBackFunc(long context, FootStepCounter counter, long steps);

static inline EsabaPokemonTable *_esabaGetPokemonTable(u32 place)
{
    if (place >= *esabadata_number) {
        return NULL;
    }
    return *esabadata[place].pokemonTable;
}

static inline u16 _esabaGetPokemonSpecies(u32 place, int index)
{
    EsabaPokemonTable *table = _esabaGetPokemonTable(place);
    if (table) {
        return table->data[index].species;
    }
    return 0;
}

static inline u32 _esabaGetPokemonRate(u32 place, int index)
{
    EsabaPokemonTable *table = _esabaGetPokemonTable(place);
    return table ? table->data[index].rate : 0;
}

static inline u32 _esabaGetPokemonDigest(u32 place, int index)
{
    EsabaPokemonTable *table = _esabaGetPokemonTable(place);
    return table ? table->data[index].digestSteps : 0;
}

static inline void _esabaSetEsaNum(int place, int number)
{
    GSflagSet(esabadata[place].esaFlag, number);
    if (number == 0) {
        GSflagSet(esabadata[place].appearFlag, 0);
        GSflagSet(esabadata[place].stepFlag, 0);
    }
}

static inline void _esabaSetPokemonRnd(u16 place, u32 random)
{
    GSflagSet(esabadata[place].rndFlag, random);
}

static inline void _esabaSetAppearPokemon(u16 place, u16 species)
{
    GSflagSet(esabadata[place].pokemonFlag, species);
    GSflagSet(esabadata[place].stepFlag, 0);
    switch (species) {
    case POKEMON_SPECIES_USOHACHI:
    case POKEMON_SPECIES_GONBE: {
        Pokemon pokemon;
        u32 trainerRnd = heroGetStatus(NULL, HERO_STATUS_TRAINER_RND, 0);
        pokemonCreate(&pokemon, species, ESABA_EX_LEVEL, gamedataGetStatus(NULL, GAMEDATA_STATUS_ATTEST));
        pokemonBiosSetCatchTrainerRnd(&pokemon, trainerRnd);
        _esabaSetPokemonRnd(place, pokemonCreateRndFit(&pokemon, -1, -1, 0, trainerRnd));
        break;
    }
    default:
        _esabaSetPokemonRnd(place, pokemonCreateRnd());
        break;
    }
}

// Puts the arriving pokemon at the spot and shows its arrival notice.
static inline void _esabaAppearPokemonEvent(u16 place, u16 species)
{
    _esabaSetAppearPokemon(place, species);
    heroMoveInitEvent();
    heroMoveSetColision(1);
    GSthreadSwitch();
    menuScriptInfoEsa();
    heroMoveTermEvent();
}

extern "C" void esabaSetPokemon(u16 place, u16 species)
{
    if (species == 0) {
        GSflagSet(esabadata[place].pokemonFlag, 0);
        GSflagSet(esabadata[place].appearFlag, 0);
        return;
    }
    _esabaAppearPokemonEvent(place, species);
}

extern "C" u16 esabaGetPokemonMax(u16 place)
{
    EsabaPokemonTable *table = _esabaGetPokemonTable(place);
    if (table) {
        return *table->number;
    }
    return 0;
}

static inline int _esabaGetPokemonIndex(u16 place)
{
    u16 max = esabaGetPokemonMax(place);
    for (int i = 0; i < max; i++) {
        if (_esabaGetPokemonSpecies(place, i) == GSflagGet(esabadata[place].pokemonFlag)) {
            return i;
        }
    }
    return 0;
}

extern "C" int esabaGetDigestRate(u16 place)
{
    u16 species = GSflagGet(esabadata[place].pokemonFlag);
    if (species == esabapokemondata_EX[ESABA_EX_RARE].species) {
        return esabapokemondata_EX[ESABA_EX_RARE].digestSteps;
    }
    if (species == esabapokemondata_EX[ESABA_EX_NORMAL].species) {
        return esabapokemondata_EX[ESABA_EX_NORMAL].digestSteps;
    }
    return _esabaGetPokemonDigest(place, _esabaGetPokemonIndex(place));
}

extern "C" int esabaGetEsaItemNum(void)
{
    return Hero::getHeroPtr()->itemQuantity(ESABA_ESA_ITEM);
}

static inline int _esabaGetEsaNum(int place)
{
    return GSflagGet(esabadata[place].esaFlag);
}

static inline void _esabaAddEsaNum(int place, int add)
{
    int number = _esabaGetEsaNum(place);
    number += add;
    _esabaSetEsaNum(place, number);
}

extern "C" long esabaGet(int place)
{
    long esaNum = GSflagGet(esabadata[place].esaFlag);
    long number = esaNum;
    if (menuPanelInputNumber(0, &number, 0, esaNum, true)) {
        _esabaAddEsaNum(place, -number);
        return number;
    }
    return 0;
}

extern "C" long esabaPut(int place)
{
    long number = GSflagGet(esabadata[place].esaFlag);
    if (menuPanelInputNumber(0, &number, 0, ESABA_ESA_MAX - number, true)) {
        _esabaAddEsaNum(place, (s16)number);
        return number;
    }
    return 0;
}

static inline void _esabaMoveEsaItem(long diff, long *bagNum, long *pcNum)
{
    long returnNum;
    long rest;

    if (diff == 0) {
        return;
    }
    if (diff > 0) {
        heroItemDecItemDataId(NULL, ESABA_ESA_ITEM, diff, -1);
        return;
    }
    returnNum = -diff;
    rest = heroItemAddItemDataId(NULL, ESABA_ESA_ITEM, returnNum, -1);
    if (rest == 0) {
        *bagNum = returnNum;
    } else if (rest > 0) {
        *bagNum = -(diff + rest);
        returnNum = rest;
        rest = pcboxAddItem(NULL, ESABA_ESA_ITEM, returnNum);
        if (rest != 0) {
            *pcNum = returnNum - rest;
        } else {
            *pcNum = returnNum;
        }
    }
}

extern "C" int esabaSet(int place)
{
    long esaNum = _esabaGetEsaNum(place);
    long number = esaNum;
    int itemNum = esabaGetEsaItemNum();
    u32 msg;
    long max;
    long diff;
    long bagNum;
    long pcNum;
    long rest;

    GSthreadSwitch();
    if (number != 0) {
        msg = ESABA_MSG_SET_ADD;
    } else {
        msg = ESABA_MSG_SET_EMPTY;
    }
    msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, number);
    winMsgOpen(WINMSG_WINDOW_MENU, msg, false, true);
    max = number + itemNum;
    if (max > ESABA_ESA_MAX) {
        max = ESABA_ESA_MAX;
    }
    if (menuPanelInputNumber(number, &number, 0, max, true)) {
        _esabaSetEsaNum(place, (s16)number);
        pcNum = 0;
        bagNum = 0;
        diff = number - esaNum;
        _esabaMoveEsaItem(diff, &bagNum, &pcNum);
    } else {
        diff = 0;
        winMsgClose(true);
    }

    if (diff != 0) {
        if (diff >= 0) {
            winMsgClose(true);
            msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, diff);
            winMsgOpen(WINMSG_WINDOW_MENU, ESABA_MSG_PUT, true, true);
            winMsgClose(true);
        } else {
            winMsgClose(true);
            if (bagNum == 0 && pcNum == 0) {
                winMsgOpen(WINMSG_WINDOW_MENU, ESABA_MSG_RETURN_FULL, true, true);
                winMsgClose(true);
                diff = 0;
                _esabaSetEsaNum(place, (s16)esaNum);
            } else {
                if (bagNum > 0) {
                    msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, bagNum);
                    winMsgOpen(WINMSG_WINDOW_MENU, ESABA_MSG_RETURN_BAG, true, true);
                    winMsgClose(true);
                }
                if (pcNum > 0) {
                    msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, pcNum);
                    winMsgOpen(WINMSG_WINDOW_MENU, ESABA_MSG_RETURN_PC, true, true);
                    winMsgClose(true);
                }
                rest = esaNum - bagNum - pcNum;
                if (rest != 0) {
                    diff = -(esaNum - rest);
                    _esabaSetEsaNum(place, (s16)rest);
                }
            }
        }
    }
    winMsgClose(true);
    return diff;
}

extern "C" void esabaHeroCallBackInit(void)
{
    _esabaHandle = heroMoveAddStepCallback(_esabaCallBackFunc, 0);
}

extern "C" bool esabaGetPokemonIsRare(int place)
{
    if (GSflagGet(esabadata[place].pokemonFlag) != 0) {
        u32 trainerRnd = heroGetStatus(NULL, HERO_STATUS_TRAINER_RND, 0);
        u32 rnd = GSflagGet(esabadata[place].rndFlag);
        return ((trainerRnd >> 16) ^ (trainerRnd & 0xFFFF) ^ (rnd >> 16) ^ (rnd & 0xFFFF)) < ESABA_RARE_PARAM_LIMIT;
    }
    return false;
}

extern "C" void esabaSetPokemonRnd(int place, u32 random)
{
    GSflagSet(esabadata[place].rndFlag, random);
}

extern "C" u32 esabaGetPokemonRnd(int place)
{
    return GSflagGet(esabadata[place].rndFlag);
}

extern "C" u16 esabaGetPokemonID(int place)
{
    u32 species = GSflagGet(esabadata[place].pokemonFlag);
    return species;
}

extern "C" void esabaSetEsaNum(int place, int number)
{
    _esabaSetEsaNum(place, number);
}

extern "C" int esabaGetEsaNum(int place)
{
    return GSflagGet(esabadata[place].esaFlag);
}

void _esabaCallBackAppFunc(u16 place, FootStepCounter counter, long steps);
void _esabaCallBackEsaFunc(u16 place, FootStepCounter counter, long steps);
void _esabaAppearPokemon(u16 place);
void _esabaAppearPokemonSub2(u16 place);
void _esabaAppearPokemonSub(u16 place);

void _esabaCallBackFunc(long context, FootStepCounter counter, long steps)
{
    u16 place;

    underTakerCallBackFunc(context, counter, steps);
    for (place = 0; place < (u16)*esabadata_number; place++) {
        if (GSflagGet(esabadata[place].esaFlag) != 0) {
            if (GSflagGet(esabadata[place].pokemonFlag) != 0) {
                _esabaCallBackEsaFunc(place, counter, steps);
            } else {
                _esabaCallBackAppFunc(place, counter, steps);
            }
        }
    }
}

void _esabaCallBackAppFunc(u16 place, FootStepCounter counter, long steps)
{
    u32 appearSteps;
    u16 retryStep;
    u16 retryChance;
    u32 retrySteps;

    if (_esabaIsMiraboPlace(place)) {
        return;
    }
    if (GSflagTest(esabadata[place].stopFlag)) {
        return;
    }
    appearSteps = GSflagGet(esabadata[place].appearFlag);
    appearSteps += steps;
    if (appearSteps >= esabaappdata->appearStepMin) {
        if (appearSteps >= esabaappdata->appearStepMax) {
            _esabaAppearPokemon(place);
        } else {
            retryStep = esabaappdata->retryStep;
            retrySteps = GSflagGet(esabadata[place].stepFlag);
            retrySteps += steps;
            if (retrySteps >= retryStep) {
                retryChance = esabaappdata->retryChance;
                if ((u16)HSD_Rand() % retryChance == 0) {
                    _esabaAppearPokemon(place);
                } else {
                    GSflagSet(esabadata[place].stepFlag, retrySteps - retryStep);
                }
            } else {
                GSflagSet(esabadata[place].stepFlag, retrySteps);
            }
        }
    }
    GSflagSet(esabadata[place].appearFlag, appearSteps);
}

void _esabaCallBackEsaFunc(u16 place, FootStepCounter counter, long steps)
{
    u16 species = GSflagGet(esabadata[place].pokemonFlag);
    u32 eatSteps;
    u32 digestSteps;
    u32 esaNum;

    if (GSflagTest(esabadata[place].stopFlag)) {
        return;
    }
    if (species == esabapokemondata_EX[ESABA_EX_RARE].species) {
        digestSteps = esabapokemondata_EX[ESABA_EX_RARE].digestSteps;
    } else if (species == esabapokemondata_EX[ESABA_EX_NORMAL].species) {
        digestSteps = esabapokemondata_EX[ESABA_EX_NORMAL].digestSteps;
    } else {
        digestSteps = _esabaGetPokemonDigest(place, _esabaGetPokemonIndex(place));
    }
    eatSteps = GSflagGet(esabadata[place].stepFlag);
    eatSteps += steps;
    if (eatSteps >= digestSteps) {
        eatSteps -= digestSteps;
        esaNum = GSflagGet(esabadata[place].esaFlag) - 1;
        if (esaNum == 0) {
            GSflagSet(esabadata[place].pokemonFlag, 0);
            GSflagSet(esabadata[place].appearFlag, 0);
        }
        GSflagSet(esabadata[place].esaFlag, esaNum);
    }
    GSflagSet(esabadata[place].stepFlag, (u16)eatSteps);
}

void _esabaAppearPokemon(u16 place)
{
    u32 exGameSetupEntry = esabaappdata->exGameSetupEntry;

    if (GSflagGet(ESABA_PROGRESS_FLAG) >= flag_etc[exGameSetupEntry].progressValue) {
        u16 exFlagValue = esabaappdata->exFlagValue;
        u32 exFlag = GSflagGet(esabaappdata->exFlag);
        if (exFlag == exFlagValue) {
            _esabaAppearPokemonSub(place);
            return;
        }
    }
    _esabaAppearPokemonSub2(place);
}

static inline bool _esabaIsExPokemonAppeared(EsabaExPokemon ex)
{
    bool appeared = false;
    u16 place;
    for (place = 0; place < (u16)*esabadata_number; place++) {
        if (esabapokemondata_EX[ex].species == esabaGetPokemonID(place)) {
            appeared = true;
            break;
        }
    }
    return appeared;
}

void _esabaAppearPokemonSub2(u16 place)
{
    u16 max = esabaGetPokemonMax(place);
    u16 total = 0;

    if (!_esabaIsExPokemonAppeared(ESABA_EX_NORMAL)
        && (u16)((u16)HSD_Rand() % 100) < (u16)esabapokemondata_EX[ESABA_EX_NORMAL].rate) {
        _esabaSetAppearPokemon(place, esabapokemondata_EX[ESABA_EX_NORMAL].species);
        heroMoveInitEvent();
        heroMoveSetColision(1);
        GSthreadSwitch();
        menuScriptInfoEsa();
        heroMoveTermEvent();
        return;
    }
    u16 roll = (u16)HSD_Rand() % 100;
    for (int index = 0; index < max; index++) {
        total += _esabaGetPokemonRate(place, index);
        if (total >= roll) {
            _esabaAppearPokemonEvent(place, _esabaGetPokemonSpecies(place, index));
            return;
        }
    }
}

void _esabaAppearPokemonSub(u16 place)
{
    u16 max = esabaGetPokemonMax(place);
    u16 total;
    u16 rareRate;
    u16 normalRate;
    u16 roll;

    HSD_Rand();
    total = 0;
    rareRate = esabapokemondata_EX[ESABA_EX_RARE].rate;
    normalRate = esabapokemondata_EX[ESABA_EX_NORMAL].rate;
    if (_esabaIsExPokemonAppeared(ESABA_EX_RARE)) {
        rareRate = 0;
    }
    if (_esabaIsExPokemonAppeared(ESABA_EX_NORMAL)) {
        normalRate = 0;
    }
    roll = (u16)HSD_Rand() % 100;
    if (roll < rareRate) {
        _esabaSetAppearPokemon(place, esabapokemondata_EX[ESABA_EX_RARE].species);
        heroMoveInitEvent();
        heroMoveSetColision(1);
        GSthreadSwitch();
        menuScriptInfoEsa();
        heroMoveTermEvent();
        return;
    }
    u16 limit = rareRate + normalRate;
    if (rareRate == 0) {
        limit = normalRate;
    }
    if (roll < limit) {
        _esabaSetAppearPokemon(place, esabapokemondata_EX[ESABA_EX_NORMAL].species);
        heroMoveInitEvent();
        heroMoveSetColision(1);
        GSthreadSwitch();
        menuScriptInfoEsa();
        heroMoveTermEvent();
        return;
    }
    roll = (u16)HSD_Rand() % 100;
    for (int index = 0; index < max; index++) {
        total += _esabaGetPokemonRate(place, index);
        if (total >= roll) {
            _esabaAppearPokemonEvent(place, _esabaGetPokemonSpecies(place, index));
            return;
        }
    }
}
