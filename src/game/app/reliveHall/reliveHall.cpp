#include <relivehall.h>
#include <pokemonBios.h>
#include <pokemonDB.h>
#include <darkPokemon.h>
#include <savedata.h>
#include <zokusei.h>
#include <waza.h>
#include <fightSeq.h>
#include <relglobal.h>
#include <heroMove.h>
#include <hero.h>
#include <menuScript.h>
#include <menuPocketTool.h>
#include <GSflag.h>
#include <GSthread.h>
#include <GSmem.h>
#include <gameSetup.h>
#include <string.h>
#include <esaba.h>

namespace {
long _dancerTempoTbl[CReliveStage::DANCER_MAX + 1] = {0, 5, 14, 27, 48};
long _aisyouBestTempoTbl[CReliveStage::DANCER_MAX + 1] = {0, 6, 6, 6, 12};
long _aisyouGoodTempoTbl[CReliveStage::DANCER_MAX + 1] = {0, 2, 2, 2, 4};

u16 _debugStage0DancerIDs[CReliveStage::DANCER_MAX] = {
    POKEMON_DATA_ID_SUDOWOODO,
    POKEMON_DATA_ID_TORCHIC,
    POKEMON_DATA_ID_ILLUMISE,
    POKEMON_DATA_ID_SHROOMISH
};
u16 _debugStage1DancerIDs[CReliveStage::DANCER_MAX] = {
    POKEMON_DATA_ID_ANORITH,
    POKEMON_DATA_ID_BEAUTIFLY,
    POKEMON_DATA_ID_MASQUERAIN,
    0
};
u16 _debugMaxTempoDancerIDs[CReliveStage::DANCER_MAX] = {
    POKEMON_DATA_ID_SUDOWOODO,
    POKEMON_DATA_ID_TORCHIC,
    POKEMON_DATA_ID_ILLUMISE,
    POKEMON_DATA_ID_SHROOMISH
};

u16 _aisyouJoutaiTbl[] = {
    JOUTAI_MOVE_RESULT_NO_EFFECT,
    JOUTAI_MOVE_RESULT_NOT_VERY_EFFECTIVE,
    JOUTAI_MOVE_RESULT_NORMAL,
    JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE
};
long _heroCallBackHandle;
void _heroCallBackFunc(long, FootStepCounter, long);
}

static const long ZOKUSEI_NOT_SET = -1;
static const long ZOKUSEI_SECONDARY_SLOT = 1;

static const long DEBUG_STAGE_QUANTITY = 8;
static const long DEBUG_DANCER_SPECIES_PER_STAGE = 2;

static CReliveHall *_pRelivehallCopy;

namespace {
inline ReliveHallAisyou getZokuseiAisyou(long attack, long defense)
{
    if (attack == ZOKUSEI_DATA_ID_NORMAL && defense == ZOKUSEI_DATA_ID_NORMAL) {
        return RELIVE_HALL_AISYOU_BEST;
    }
    int joutai = zokuseiGetWazaJoutai(attack, defense);
    for (int i = 0; i < sizeof(_aisyouJoutaiTbl) / sizeof(_aisyouJoutaiTbl[0]); i++) {
        if (joutai == _aisyouJoutaiTbl[i]) {
            int aisyou = i - 1;
            return (ReliveHallAisyou)(aisyou < 0 ? RELIVE_HALL_AISYOU_POOR : aisyou);
        }
    }
    return RELIVE_HALL_AISYOU_UNKNOWN;
}
}

inline long findEmptyDancer(CReliveStage *stage)
{
    for (long slot = 0; slot < CReliveStage::DANCER_MAX; slot++) {
        if (!pokemonCheckValid(&stage->mDancers[slot])) {
            return slot;
        }
    }
    return CReliveStage::DANCER_SLOT_NONE;
}

inline void setHallPlace(CReliveStage *stage, Pokemon *pokemon)
{
    long stageIndex = stage - CReliveHall::getPointer()->getStage(0);
    DarkPokemon *darkPokemon = pokemon->getDarkPokemon();
    if (darkPokemon != NULL) {
        darkPokemon->setPlace(DARK_POKEMON_WHERE_RELIVE_HALL, stageIndex);
    }
}

inline void putDancer(CReliveStage *stage, Pokemon &pokemon)
{
    long slot = findEmptyDancer(stage);
    Pokemon *dancer = &stage->mDancers[slot];
    memcpy(dancer, &pokemon, sizeof(Pokemon));
    pokemonAllKaihuku(dancer);
    setHallPlace(stage, dancer);
    pokemonInit(&pokemon);
}

void CReliveStage::init()
{
    memset(this, 0, sizeof(CReliveStage));
    pokemonInitAry(mDancers, DANCER_MAX);
    pokemonInit(&mVisitor);
}

Pokemon *CReliveStage::getVisitorPointer()
{
    if (pokemonCheckValid(&mVisitor)) {
        return &mVisitor;
    }
    return NULL;
}

void CReliveStage::putVisitor(Pokemon &pokemon)
{
    memcpy(&mVisitor, &pokemon, sizeof(Pokemon));
    pokemonAllKaihuku(&mVisitor);

    setHallPlace(this, &mVisitor);
    pokemonInit(&pokemon);
}

Pokemon *CReliveStage::getDancerPointer(long slot)
{
    Pokemon *dancer = &mDancers[slot];
    return pokemonCheckValid(dancer) ? dancer : NULL;
}

long CReliveStage::getDancerQuantity()
{
    long quantity;
    for (quantity = 0; quantity < DANCER_MAX; quantity++) {
        if (!pokemonCheckValid(&mDancers[quantity])) {
            break;
        }
    }
    return quantity;
}

void CReliveStage::exchangeDancer(Pokemon &pokemon, long slot)
{
    Pokemon *dancer = &mDancers[slot];
    Pokemon temp = *dancer;
    memcpy(dancer, &pokemon, sizeof(Pokemon));
    memcpy(&pokemon, &temp, sizeof(Pokemon));
    pokemonAllKaihuku(dancer);

    setHallPlace(this, dancer);
}

void CReliveStage::exchangeVisitor(Pokemon &pokemon)
{
    Pokemon *visitor = &mVisitor;
    Pokemon temp = *visitor;
    memcpy(visitor, &pokemon, sizeof(Pokemon));
    memcpy(&pokemon, &temp, sizeof(Pokemon));
    pokemonAllKaihuku(visitor);

    setHallPlace(this, visitor);
}

void CReliveStage::pickupDancer(Pokemon &pokemon, long slot)
{
    Pokemon *dancer = &mDancers[slot];
    memcpy(&pokemon, dancer, sizeof(Pokemon));

    long moveQuantity = DANCER_MAX - (slot + 1);
    if (moveQuantity != 0) {
        memmove(dancer, &mDancers[slot + 1], moveQuantity * sizeof(Pokemon));
    }
    pokemonInit(&mDancers[DANCER_MAX - 1]);
}

void CReliveStage::insertDancer(Pokemon &pokemon, long slot)
{
    long moveQuantity = DANCER_MAX - (slot + 1);
    if (moveQuantity != 0) {
        memmove(&mDancers[slot + 1], &mDancers[slot], moveQuantity * sizeof(Pokemon));
    }

    Pokemon *dancer = &mDancers[slot];
    memcpy(dancer, &pokemon, sizeof(Pokemon));
    pokemonAllKaihuku(dancer);

    setHallPlace(this, dancer);
    pokemonInit(&pokemon);
}

void CReliveStage::pickupVisitor(Pokemon &pokemon)
{
    memcpy(&pokemon, &mVisitor, sizeof(Pokemon));
    pokemonInit(&mVisitor);
}

long CReliveStage::getPassion()
{
    static long _passionBonusTbl[CReliveHall::STAGE_MAX + 1] = {0, 0, 1, 5, 10, 15, 25, 35, 50, 100};

    long passion = getPassionWoBonus();
    if (isBonusGet()) {
        passion += _passionBonusTbl[CReliveHall::getPointer()->getReliveGiveStageQuantity()];
    }
    return passion;
}

bool CReliveStage::isBonusGet()
{
    long passion = getPassionWoBonus();
    return isPokemonZokuseiAllUnique(true) && passion == PASSION_MAX_WITHOUT_BONUS;
}

inline float getPassionAisyouRate(Pokemon *visitor, Pokemon *dancer)
{
    float rates[] = {
        1.0f, // RELIVE_HALL_AISYOU_POOR
        1.5f, // RELIVE_HALL_AISYOU_GOOD
        2.0f  // RELIVE_HALL_AISYOU_BEST
    };
    ReliveHallAisyou aisyou = reliveHallPokemonToAisyou(visitor, dancer);
    return aisyou == RELIVE_HALL_AISYOU_MISSING ? 0.0f : rates[aisyou];
}

long CReliveStage::getPassionWoBonus()
{
    Pokemon *visitor = getVisitorPointer();
    if (visitor == NULL) {
        return 0;
    }

    int tempo = getTempo();
    if (tempo == 0) {
        return 0;
    }

    Pokemon *centerDancer = getDancerPointer(mCenterDancer);
    return tempo * getPassionAisyouRate(visitor, centerDancer);
}

long CReliveStage::getTempo()
{
    long dancerQuantity = getDancerQuantity();
    long dancerTempo = _dancerTempoTbl[dancerQuantity];
    long tempo = 0;

    for (long i = 0; i < dancerQuantity; i++) {
        long aisyouTempo;
        switch (reliveHallPokemonToAisyou(getDancerPointer(i), getDancerPointer((i + 1) % dancerQuantity))) {
        case RELIVE_HALL_AISYOU_MISSING:
            aisyouTempo = 0;
            break;
        case RELIVE_HALL_AISYOU_POOR:
            aisyouTempo = 0;
            break;
        case RELIVE_HALL_AISYOU_GOOD:
            aisyouTempo = _aisyouGoodTempoTbl[dancerQuantity];
            break;
        case RELIVE_HALL_AISYOU_BEST:
            aisyouTempo = _aisyouBestTempoTbl[dancerQuantity];
            break;
        }
        tempo += aisyouTempo;
    }
    return tempo + dancerTempo;
}

bool CReliveStage::isPokemonZokuseiAllUnique(bool withVisitor)
{
    u8 *used = (u8 *)__alloca(*zokusei_data_number);
    memset(used, 0, *zokusei_data_number);

    long dancerQuantity = getDancerQuantity();
    long pokemonQuantity = dancerQuantity;
    if (withVisitor) {
        pokemonQuantity = dancerQuantity + 1;
    }

    for (long i = 0; i < pokemonQuantity; i++) {
        Pokemon *pokemon;
        if (i == dancerQuantity) {
            pokemon = getVisitorPointer();
            if (pokemon == NULL) {
                break;
            }
        } else {
            pokemon = getDancerPointer(i);
        }

        long lastZokusei = ZOKUSEI_NOT_SET;
        for (long j = 0; j < POKEMON_DATA_TYPE_COUNT; j++) {
            PokemonData *data = pokemon->getPokemonDataPtr();
            u8 zokusei = pokemonDataBiosGetZokuseiDataId(data, j);
            if (j == ZOKUSEI_SECONDARY_SLOT && zokusei == lastZokusei) {
                break;
            }
            lastZokusei = zokusei;
            if (used[zokusei]) {
                return false;
            }
            used[zokusei] = true;
        }
    }
    return true;
}

Pokemon *CReliveStage::heroWalkReliveCallBack(long steps, bool canRelive)
{
    if (getPassion() == 0) {
        return NULL;
    }

    Pokemon *visitor = getVisitorPointer();
    if (visitor->getPokemonDataId() == POKEMON_DATA_ID_LUGIA) {
        return NULL;
    }

    u16 darkId = pokemonBiosGetDarkpokemonDataId(visitor);
    if (darkPokemonBiosGetReliveFlag(darkId)) {
        return NULL;
    }
    if (visitor->getDarkPointDirect() == 0) {
        return NULL;
    }

    u16 walkSteps = darkPokemonBiosGetSteps(darkId);
    if (walkSteps >= relivehall_data->mSteps && canRelive) {
        long passion = getPassion();
        pokemonAddDp(visitor, -passion);
        walkSteps -= relivehall_data->mSteps;
        darkPokemonBiosSetSteps(darkId, walkSteps);
        if (visitor->getDarkPointDirect() != 0) {
            return NULL;
        }
        return visitor;
    }

    walkSteps += steps;
    darkPokemonBiosSetSteps(darkId, walkSteps);
    return NULL;
}

bool CReliveStage::isReliveReady()
{
    Pokemon *visitor = getVisitorPointer();
    if (visitor == NULL) {
        return false;
    }
    if (isLugiaReliveReady()) {
        return true;
    }
    return visitor->getDarkPoint() <= 0.0f && getDancerQuantity() != 0;
}

bool CReliveStage::isLugiaReliveReady()
{
    Pokemon *visitor = getVisitorPointer();
    if (visitor == NULL) {
        return false;
    }

    return CReliveHall::getPointer()->isReliveReadyDarkLugiaExist() &&
           visitor->getPokemonDataId() == POKEMON_DATA_ID_LUGIA;
}

CReliveStage::State CReliveStage::getState()
{
    if (isReliveReady()) {
        return STATE_RELIVE_READY;
    }

    long tempo = getTempo();
    if (tempo == 0) {
        return STATE_NO_TEMPO;
    }
    return tempo == TEMPO_MAX ? STATE_MAX_TEMPO : STATE_HAS_TEMPO;
}

CReliveStage *CReliveHall::getStage(long stage)
{
    return &mStages[stage];
}

void CReliveHall::init()
{
    memset(this, 0, sizeof(CReliveHall));
    for (unsigned long i = 0; i < STAGE_MAX; ++i) {
        mStages[i].init();
    }
}

long CReliveHall::getReliveGiveStageQuantity()
{
    long quantity = 0;
    for (long i = 0; i < STAGE_MAX; i++) {
        CReliveStage *stage = getPointer()->getStage(i);
        long tempo = stage->getTempo();
        bool give = false;
        if (stage->isPokemonZokuseiAllUnique(false) && tempo == CReliveStage::TEMPO_MAX) {
            give = true;
        }
        if (give) {
            quantity++;
        }
    }
    return quantity;
}

Pokemon *CReliveHall::heroWalkReliveCallBack(long steps)
{
    Pokemon *result = NULL;
    for (unsigned long i = 0; i < STAGE_MAX; i++) {
        if (result == NULL) {
            result = mStages[i].heroWalkReliveCallBack(steps, true);
        } else {
            mStages[i].heroWalkReliveCallBack(steps, false);
        }
    }
    return result;
}

bool CReliveHall::isReliveReadyPokemonExist()
{
    for (unsigned long i = 0; i < STAGE_MAX; ++i) {
        if (mStages[i].isReliveReady()) {
            return true;
        }
    }
    return false;
}

long CReliveHall::getReliveReadyVisitorQuantity()
{
    long quantity = 0;
    for (unsigned long i = 0; i < STAGE_MAX; ++i) {
        if (mStages[i].isReliveReady()) {
            ++quantity;
        }
    }
    return quantity;
}

bool CReliveHall::isReliveReadyDarkLugiaExist()
{
    for (unsigned long i = 0; i < STAGE_MAX; i++) {
        Pokemon *visitor = mStages[i].getVisitorPointer();
        if (visitor != NULL && visitor->getPokemonDataId() == POKEMON_DATA_ID_LUGIA) {
            goto lugiaFound;
        }
    }
    return false;

lugiaFound:
    for (unsigned long j = 0; j < STAGE_MAX; j++) {
        if (mStages[j].getTempo() != CReliveStage::TEMPO_MAX) {
            return false;
        }
    }
    return true;
}

CReliveHall *CReliveHall::getPointer()
{
    return (CReliveHall *)savedataGetStatus(savedataGetStatus(NULL, SAVE_DATA_IMAGE), SAVE_DATA_RELIVE_HALL);
}

void CReliveHall::allocMakeBackup()
{
    _pRelivehallCopy = (CReliveHall *)GSmemAllocClr(sizeof(CReliveHall));
    memcpy(_pRelivehallCopy, this, sizeof(CReliveHall));
}

void CReliveHall::checkWithBackupSet()
{
    for (unsigned long i = 0; i < STAGE_MAX; ++i) {
        CReliveStage *stage = &mStages[i];
        CReliveStage *backup = &_pRelivehallCopy->mStages[i];
        if (memcmp(stage, backup, sizeof(CReliveStage)) != 0) {
            Pokemon *visitor = stage->getVisitorPointer();
            if (visitor != NULL) {
                darkPokemonBiosSetSteps(pokemonBiosGetDarkpokemonDataId(visitor), 0);
            }
        }
    }
    for (unsigned long i = 0; i < STAGE_MAX; ++i) {
        Pokemon *visitor = _pRelivehallCopy->mStages[i].getVisitorPointer();
        if (visitor == NULL) {
            continue;
        }
        u16 darkId = pokemonBiosGetDarkpokemonDataId(visitor);
        bool found = false;
        for (unsigned long j = 0; j < STAGE_MAX; ++j) {
            Pokemon *current = mStages[j].getVisitorPointer();
            if (current != NULL && pokemonBiosGetDarkpokemonDataId(current) == darkId) {
                found = true;
                break;
            }
        }
        if (!found) {
            darkPokemonBiosSetSteps(darkId, 0);
        }
    }
    if (_pRelivehallCopy != NULL) {
        GSmemFree(_pRelivehallCopy);
        _pRelivehallCopy = NULL;
    }
}

bool CReliveHall::isOpen()
{
    return GSflagGet(ESABA_PROGRESS_FLAG) >= flag_etc[GAME_SETUP_ENTRY_COLOSSEUM_BATTLE].progressValue;
}

namespace {
enum DebugShadowID {
    DEBUG_SHADOW_TEDDIURSA = 1,
    DEBUG_SHADOW_VULPIX,
    DEBUG_SHADOW_SPHEAL,
    DEBUG_SHADOW_BALTOY,
    DEBUG_SHADOW_MAREEP,
    DEBUG_SHADOW_GULPIN,
    DEBUG_SHADOW_SEEDOT,
    DEBUG_SHADOW_SPINARAK,
    DEBUG_SHADOW_LUGIA = 73
};

enum DebugVisitorRate {
    DEBUG_VISITOR_RATE_READY = 0,
    DEBUG_VISITOR_RATE_PARTIAL = 1,
    DEBUG_VISITOR_RATE_COUNT
};
}

void CReliveHall::debugPokemonSetup(long mode)
{
    static float _darkRateTbl[DEBUG_VISITOR_RATE_COUNT][CReliveHall::STAGE_MAX + 1] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0}
    };
    static u16 _debugVisitorPokemonIDs[CReliveHall::STAGE_MAX + 1] = {
        POKEMON_DATA_ID_MACHAMP,
        POKEMON_DATA_ID_POLITOED,
        POKEMON_DATA_ID_HOPPIP,
        POKEMON_DATA_ID_SKIPLOOM,
        POKEMON_DATA_ID_JUMPLUFF,
        POKEMON_DATA_ID_AIPOM,
        POKEMON_DATA_ID_SUNKERN,
        POKEMON_DATA_ID_SUNFLORA,
        POKEMON_DATA_ID_YANMA,
        0
    };
    static u16 _debugShadowPokemonIDs[CReliveHall::STAGE_MAX + 1] = {
        DEBUG_SHADOW_LUGIA,
        DEBUG_SHADOW_TEDDIURSA,
        DEBUG_SHADOW_VULPIX,
        DEBUG_SHADOW_SPHEAL,
        DEBUG_SHADOW_BALTOY,
        DEBUG_SHADOW_MAREEP,
        DEBUG_SHADOW_GULPIN,
        DEBUG_SHADOW_SEEDOT,
        DEBUG_SHADOW_SPINARAK,
        0
    };

    if (mode == RELIVE_HALL_DEBUG_PARTIAL_PURIFICATION || mode == RELIVE_HALL_DEBUG_READY_VISITORS) {
        init();
        for (long i = 0; i < DEBUG_STAGE_QUANTITY; i++) {
            CReliveStage *stage = getStage(i);
            Pokemon pokemon;
            long dancerCounts[CReliveHall::STAGE_MAX + 1] = {4, 3, 2, 1, 1, 1, 1, 1, 1, 0};
            long count = dancerCounts[i];
            for (long j = 0; j < count; j++) {
                u16 speciesId;
                if (i == 0) {
                    speciesId = _debugStage0DancerIDs[j];
                } else if (i == 1) {
                    speciesId = _debugStage1DancerIDs[j];
                } else {
                    speciesId = i * DEBUG_DANCER_SPECIES_PER_STAGE + j;
                }
                menuPocketDebugMakePokemonByID(pokemon, speciesId, false, 0.0f);
                putDancer(stage, pokemon);
            }
            menuPocketDebugMakePokemonByID(pokemon, _debugVisitorPokemonIDs[i], true,
                                           _darkRateTbl[mode == RELIVE_HALL_DEBUG_READY_VISITORS
                                                            ? DEBUG_VISITOR_RATE_READY
                                                            : DEBUG_VISITOR_RATE_PARTIAL][i]);
            stage->putVisitor(pokemon);
            stage->mCenterDancer = 0;
        }
    } else if (mode == RELIVE_HALL_DEBUG_MAX_TEMPO) {
        init();
        for (long i = 0; i < STAGE_MAX; i++) {
            CReliveStage *stage = getStage(i);
            Pokemon pokemon;
            for (long j = 0; j < CReliveStage::DANCER_MAX; j++) {
                menuPocketDebugMakePokemonByID(pokemon, _debugMaxTempoDancerIDs[j], false, 0.0f);
                putDancer(stage, pokemon);
            }
            menuPocketDebugMakeDarkPokemonByID(pokemon, _debugShadowPokemonIDs[i],
                                               _darkRateTbl[DEBUG_VISITOR_RATE_PARTIAL][i]);
            stage->putVisitor(pokemon);
            stage->mCenterDancer = 0;
        }
    }
}

ReliveHallAisyou reliveHallPokemonToAisyou(Pokemon *pokemon, Pokemon *partner)
{
    Pokemon *pair[2];
    pair[0] = pokemon;
    pair[1] = partner;
    if (pokemon == NULL || partner == NULL) {
        return RELIVE_HALL_AISYOU_MISSING;
    }
    int zokusei[2][POKEMON_DATA_TYPE_COUNT];
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < POKEMON_DATA_TYPE_COUNT; j++) {
            zokusei[i][j] = (u8)pokemonDataBiosGetZokuseiDataId(pair[i]->getPokemonDataPtr(), j);
        }
    }
    ReliveHallAisyou best = RELIVE_HALL_AISYOU_POOR;
    for (int i = 0; i < POKEMON_DATA_TYPE_COUNT; i++) {
        for (int j = 0; j < POKEMON_DATA_TYPE_COUNT; j++) {
            int attack = zokusei[0][i];
            int defense = zokusei[1][j];
            ReliveHallAisyou aisyou = getZokuseiAisyou(attack, defense);
            if (best < aisyou) {
                best = aisyou;
            }
        }
    }
    return best;
}

namespace {
void _heroCallBackFunc(long, FootStepCounter, long steps)
{
    if (CReliveHall::isOpen()) {
        Pokemon *pokemon = CReliveHall::getPointer()->heroWalkReliveCallBack(steps);
        if (pokemon) {
            reliveHallInfomationPokemonDP0(pokemon);
        }
    }
}
}

extern "C" void reliveHallInfomationPokemonDP0(Pokemon *pokemon)
{
    if (!darkPokemonBiosGetInformationFlag(pokemonBiosGetDarkpokemonDataId(pokemon))) {
        heroMoveInitEvent();
        heroMoveSetColision(true);
        GSthreadSwitch();
        darkPokemonWhere where;
        u8 slot;
        pokemon->getDarkPokemon()->getPlace(where, slot);
        menuScriptInfoRelive(pokemon, where != DARK_POKEMON_WHERE_RELIVE_HALL);
        heroMoveTermEvent();
    }
}

extern "C" void reliveHallCallBackInit()
{
    _heroCallBackHandle = heroMoveAddStepCallback(_heroCallBackFunc, 0);
}

extern "C" void reliveHallDebugPokemonSetup(long mode)
{
    CReliveHall::getPointer()->debugPokemonSetup(mode);
}
