#include <Pokemon.h>
#include <pokemonDataBios.h>
#include <relglobal.h>
#include <libc/memory.h>

const u16 POKEMON_POOL_FRIEND_LEVEL_MAX = 255;

const long POKEMON_DARK_POINT_MAX = 6553500;
const unsigned long POKEMON_RARE_PARAM_LIMIT = 8;

void Pokemon::setReversMode(bool enabled)
{
    DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon != 0) {
        darkPokemon->setReversMode(enabled);
    }
}

bool Pokemon::isReversMode() const
{
    const DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon == 0) {
        return false;
    }
    return darkPokemon->isReversMode();
}

bool Pokemon::isLegend() const
{
    u16 species;
    u32 i;

    if (getPokemonDataId() == 0) {
        return false;
    }
    for (i = 0; i < *legendPokemonData_number; i++) {
        species = legendPokemonData[i];
        if (species == 0) {
            return false;
        }
        if (species == getPokemonDataId()) {
            return true;
        }
    }
    return false;
}

bool Pokemon::isHatena() const
{
    u16 species = getPokemonDataId();

    if (species == 0) {
        return true;
    }
    if (pokemonDB_GetName(species) == 0) {
        return true;
    }
    if (species >= *pokemon_data_number) {
        return true;
    }
    return false;
}

bool Pokemon::checkValid() const
{
    return pokemonCheckValid((Pokemon *)this);
}

void Pokemon::setPoolFriendLevel(u16 level)
{
    DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon != 0) {
        level = level < POKEMON_POOL_FRIEND_LEVEL_MAX ? level : POKEMON_POOL_FRIEND_LEVEL_MAX;
        darkPokemon->setPoolFriendLevel(level);
    }
}

void Pokemon::setPoolExp(unsigned long exp)
{
    DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon != 0) {
        darkPokemon->setPoolExp(exp);
    }
}

void Pokemon::setDarkPoint(float point)
{
    DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon != 0) {
        darkPokemon->setDarkPoint(point < POKEMON_DARK_POINT_MAX ? point : POKEMON_DARK_POINT_MAX);
    }
}

void Pokemon::setDarkPointDirect(long point)
{
    DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon != 0) {
        darkPokemon->setDarkPointDirect(point < POKEMON_DARK_POINT_MAX ? point : POKEMON_DARK_POINT_MAX);
    }
}

u16 Pokemon::getPoolFriendLevel() const
{
    const DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon == 0) {
        return 0;
    }
    return darkPokemon->getPoolFriendLevel();
}

unsigned long Pokemon::getPoolExp() const
{
    const DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon == 0) {
        return 0;
    }
    return darkPokemon->getPoolExp();
}

float Pokemon::getDarkPoint() const
{
    const DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon == 0) {
        return -1.0f;
    }
    return darkPokemon->getDarkPoint();
}

long Pokemon::getDarkPointDirect() const
{
    const DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon == 0) {
        return -1;
    }
    return darkPokemon->getDarkPointDirect();
}

const DarkPokemon *Pokemon::getDarkPokemon() const
{
    return darkPokemonGetDarkPokemon(getDarkpokemonDataID());
}

DarkPokemon *Pokemon::getDarkPokemon()
{
    return darkPokemonGetDarkPokemon(getDarkpokemonDataID());
}

bool Pokemon::isDarkPokemon() const
{
    u16 darkID = getDarkpokemonDataID();
    if (darkID == 0) {
        return false;
    }
    return !darkPokemonBiosGetReliveFlag(darkID);
}

u8 Pokemon::getSex() const
{
    return pokemonGetSex((Pokemon *)this);
}

u8 Pokemon::isTokuseiFlag() const
{
    if (pokemonDataBiosGetTokuseiDataId(getPokemonDataPtr(), 1) == 0) {
        return false;
    }
    return mTokuseiFlag != 0;
}

bool Pokemon::isRareParam(unsigned long trainerRnd, unsigned long pokemonRnd)
{
    return ((trainerRnd >> 16) ^ (trainerRnd & 0xFFFF) ^
            (pokemonRnd >> 16) ^ (pokemonRnd & 0xFFFF)) < POKEMON_RARE_PARAM_LIMIT;
}

u32 DarkPokemon::getWalkCount() const
{
    return mWalkCount;
}

void DarkPokemon::setWalkCount(u32 count)
{
    mWalkCount = count;
}

void DarkPokemon::setReversPattern(u8 pattern)
{
    mReversPattern = pattern;
}

u8 DarkPokemon::getReversPattern() const
{
    return mReversPattern;
}

s32 DarkPokemon::getInitDarkPointDirect() const
{
    return darkPokemonBiosGetDarkPoint(mDarkPokemonID);
}

void DarkPokemon::setCatchBallID(u16 ballID)
{
    mCatchBallID = ballID;
}

u8 DarkPokemon::getCatchBallID() const
{
    return mCatchBallID;
}

void DarkPokemon::setDarkPokemonID(u16 darkID)
{
    mDarkPokemonID = darkID;
}

u16 DarkPokemon::getDarkPokemonID() const
{
    return mDarkPokemonID;
}

void DarkPokemon::setPokemonID(u16 id)
{
    mPokemonID = id;
}

u16 DarkPokemon::getPokemonID() const
{
    return mPokemonID;
}

bool DarkPokemon::isSnach() const
{
    return getStatus() >= DARK_POKEMON_STATUS_SNATCHED;
}

bool DarkPokemon::isRelive() const
{
    return getStatus() == DARK_POKEMON_STATUS_RELIVED;
}

void DarkPokemon::setTrainerName(const u16 *name)
{
    for (int i = 0; i < 10; i++) {
        mTrainerName[i] = name[i];
    }
    mTrainerName[10] = 0;
}

const u16 *DarkPokemon::getTrainerName() const
{
    return mTrainerName;
}

void DarkPokemon::setStatusRnd(u8 status, u8 rnd)
{
    if (status >= 6) {
        return;
    }
    mStatusRandom[status] = rnd;
}

u8 DarkPokemon::getStatusRnd(u8 status) const
{
    if (status < 6) {
        return mStatusRandom[status];
    }
    return 0;
}

void DarkPokemon::setInformation(bool information)
{
    mInformation = information;
}

bool DarkPokemon::isInformation() const
{
    return mInformation != 0;
}

void DarkPokemon::setUpdate(bool update)
{
    mUpdate = update;
}

bool DarkPokemon::isUpdate() const
{
    return mUpdate != 0;
}

void DarkPokemon::setEscapeTime(u32 time)
{
    if (mStatus >= DARK_POKEMON_STATUS_SNATCHED) {
        return;
    }
    mSnatchTimeState = time;
}

u32 DarkPokemon::getEscapeTime() const
{
    if (mStatus < DARK_POKEMON_STATUS_SNATCHED) {
        return mSnatchTimeState;
    }
    return 0;
}

void DarkPokemon::setSnatchTime(u32 time)
{
    if (mStatus < DARK_POKEMON_STATUS_SNATCHED) {
        return;
    }
    mSnatchTimeState = time;
}

void DarkPokemon::setRnd(u32 rnd)
{
    mRandom = rnd;
}

u32 DarkPokemon::getRnd() const
{
    return mRandom;
}

void DarkPokemon::setCatchFloorId(u16 floorID)
{
    mCatchFloorId = floorID;
}

u16 DarkPokemon::getCatchFloorId() const
{
    return mCatchFloorId;
}

void DarkPokemon::setOrgTrainer(u16 trainer)
{
    mOrgTrainer = trainer;
}

u16 DarkPokemon::getOrgTrainer() const
{
    return mOrgTrainer;
}

void DarkPokemon::setSnatchIndex(u8 index)
{
    mSnatchIndex = index;
}

u8 DarkPokemon::getSnatchIndex() const
{
    return mSnatchIndex;
}

void DarkPokemon::getPlace(darkPokemonWhere &where, u8 &index) const
{
    where = (darkPokemonWhere)mPlace;
    index = mPlaceIndex;
}

void DarkPokemon::setWazaNew(u8 index, bool value)
{
    if (index >= 3) {
        return;
    }
    if (value) {
        mWazaNew |= 1 << index;
    }
    else {
        mWazaNew &= ~(1 << index);
    }
}

bool DarkPokemon::isWazaNew(u8 index) const
{
    if (index >= 3) {
        return false;
    }
    return (mWazaNew & (1 << index)) != 0;
}

void DarkPokemon::setPoolFriendLevel(u16 level)
{
    mPoolFriendLevel = level;
}

u16 DarkPokemon::getPoolFriendLevel() const
{
    return mPoolFriendLevel;
}

void DarkPokemon::setPoolExp(unsigned long exp)
{
    mPoolExp = exp;
}

unsigned long DarkPokemon::getPoolExp() const
{
    return mPoolExp;
}

void DarkPokemon::setDarkPointDirect(long point)
{
    mDarkPoint = point;
}

long DarkPokemon::getDarkPointDirect() const
{
    return mDarkPoint;
}

void DarkPokemon::setReversMode(bool mode)
{
    mReversMode = mode;
}

bool DarkPokemon::isReversMode() const
{
    return mReversMode != 0;
}

void DarkPokemon::setSnatchFromWho(darkPokemonSnatchWho who)
{
    mSnatchFromWho = who;
}

darkPokemonSnatchWho DarkPokemon::getSnatchFromWho() const
{
    return (darkPokemonSnatchWho)mSnatchFromWho;
}

void DarkPokemon::setSnatchInfoLevel(u8 level)
{
    mSnatchInfoLevel = level;
}

darkPokemonStatus DarkPokemon::getStatus() const
{
    return (darkPokemonStatus)mStatus;
}

void DarkPokemon::addStatus(darkPokemonStatus status)
{
    if (mStatus < (u32)status) {
        mStatus = status;
    }
}

void DarkPokemon::setStatus(darkPokemonStatus status)
{
    mStatus = status;
}

void DarkPokemon::init()
{
    memset(this, 0, sizeof(*this));
}

