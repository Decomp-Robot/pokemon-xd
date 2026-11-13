#include <deck.h>

void DeckTrainerAI::setTotalCorrection(u8 correction)
{
    if (this == NULL) return;
    mTotalCorrection = correction;
}

u8 DeckTrainerAI::getTotalCorrection() const
{
    if (this == NULL) return 0;
    return mTotalCorrection;
}

void DeckTrainerAI::setWazaKindParam(u8 index, u8 param)
{
    if (this == NULL) return;
    mWazaKindParam[index] = param;
}

u8 DeckTrainerAI::getWazaKindParam(u8 index) const
{
    if (this == NULL) return 0;
    return mWazaKindParam[index];
}

void DeckTrainerAI::setWazaKindID(u8 index, u16 id)
{
    if (this == NULL) return;
    mWazaKindID[index] = id;
}

u16 DeckTrainerAI::getWazaKindID(u8 index) const
{
    if (this == NULL) return 0;
    return mWazaKindID[index];
}

void DeckTrainerAI::setAttrTypeParam(u8 index, u8 param)
{
    if (this == NULL) return;
    mAttrTypeParam[index] = param;
}

u8 DeckTrainerAI::getAttrTypeParam(u8 index) const
{
    if (this == NULL) return 0;
    return mAttrTypeParam[index];
}

void DeckTrainerAI::setAttrTypeID(u8 index, u8 id)
{
    if (this == NULL) return;
    mAttrTypeID[index] = id;
}

u8 DeckTrainerAI::getAttrTypeID(u8 index) const
{
    if (this == NULL) return 0;
    return mAttrTypeID[index];
}

void DeckTrainerAI::setLastPPDetermRate(u8 rate)
{
    if (this == NULL) return;
    mLastPPDetermRate = rate;
}

u8 DeckTrainerAI::getLastPPDetermRate() const
{
    if (this == NULL) return 0;
    return mLastPPDetermRate;
}

void DeckTrainerAI::setLowHPDetermRate(u8 rate)
{
    if (this == NULL) return;
    mLowHPDetermRate = rate;
}

u8 DeckTrainerAI::getLowHPDetermRate() const
{
    if (this == NULL) return 0;
    return mLowHPDetermRate;
}

u8 DeckTrainerAI::getAnnihilateDetermRate() const
{
    if (this == NULL) return 0;
    return mAnnihilateDetermRate;
}

void DeckTrainerAI::setHitRate(u8 rate)
{
    if (this == NULL) return;
    mHitRate = rate;
}

u8 DeckTrainerAI::getHitRate() const
{
    if (this == NULL) return 0;
    return mHitRate;
}

void DeckTrainerAI::setItemUseRate(u8 rate)
{
    if (this == NULL) return;
    mItemUseRate = rate;
}

u8 DeckTrainerAI::getItemUseRate() const
{
    if (this == NULL) return 0;
    return mItemUseRate;
}

void DeckTrainerAI::setPokeChgRate(u8 rate)
{
    if (this == NULL) return;
    mPokeChgRate = rate;
}

u8 DeckTrainerAI::getPokeChgRate() const
{
    if (this == NULL) return 0;
    return mPokeChgRate;
}

void DeckTrainerAI::setStatusDetermValTotalMin(u8 value)
{
    if (this == NULL) return;
    mStatusDetermValTotalMin = value;
}

u8 DeckTrainerAI::getStatusDetermValTotalMin() const
{
    if (this == NULL) return 0;
    return mStatusDetermValTotalMin;
}

void DeckTrainerAI::setStatusDetermValTotalMax(u8 value)
{
    if (this == NULL) return;
    mStatusDetermValTotalMax = value;
}

u8 DeckTrainerAI::getStatusDetermValTotalMax() const
{
    if (this == NULL) return 0;
    return mStatusDetermValTotalMax;
}

u8 DeckTrainerAI::getStatusDetermValMin(u8 index) const
{
    if (this == NULL) return 0;
    return mStatusDetermVal[index].mMin;
}

u8 DeckTrainerAI::getStatusDetermValMax(u8 index) const
{
    if (this == NULL) return 0;
    return mStatusDetermVal[index].mMax;
}

void DeckTrainerAI::setRespectPokemonCondJuge(bool value)
{
    if (this == NULL) return;
    mRespectPokemonCondJuge = value;
}

bool DeckTrainerAI::getRespectPokemonCondJuge() const
{
    if (this == NULL) return false;
    return mRespectPokemonCondJuge;
}

void DeckTrainerAI::setRespectWazaRiskJudge(bool value)
{
    if (this == NULL) return;
    mRespectWazaRiskJudge = value;
}

bool DeckTrainerAI::getRespectWazaRiskJudge() const
{
    if (this == NULL) return false;
    return mRespectWazaRiskJudge;
}

void DeckTrainerAI::setRespectWazaHit(bool value)
{
    if (this == NULL) return;
    mRespectWazaHit = value;
}

bool DeckTrainerAI::getRespectWazaHit() const
{
    if (this == NULL) return false;
    return mRespectWazaHit;
}

void DeckTrainerAI::setRespectRole(bool value)
{
    if (this == NULL) return;
    mRespectRole = value;
}

bool DeckTrainerAI::getRespectRole() const
{
    if (this == NULL) return false;
    return mRespectRole;
}

void DeckTrainerAI::setRespectWazaAssmntVal(bool value)
{
    if (this == NULL) return;
    mRespectWazaAssmntVal = value;
}

bool DeckTrainerAI::getRespectWazaAssmntVal() const
{
    if (this == NULL) return false;
    return mRespectWazaAssmntVal;
}

void DeckTrainerAI::setSelectRandomWaza(bool value)
{
    if (this == NULL) return;
    mSelectRandomWaza = value;
}

bool DeckTrainerAI::getSelectRandomWaza() const
{
    if (this == NULL) return false;
    return mSelectRandomWaza;
}

void DeckTrainerAI::setSelectRandomDefence(bool value)
{
    if (this == NULL) return;
    mSelectRandomDefence = value;
}

bool DeckTrainerAI::getSelectRandomDefence() const
{
    if (this == NULL) return false;
    return mSelectRandomDefence;
}

void DeckTrainerAI::setCalcWazaDamage(bool value)
{
    if (this == NULL) return;
    mCalcWazaDamage = value;
}

bool DeckTrainerAI::getCalcWazaDamage() const
{
    if (this == NULL) return false;
    return mCalcWazaDamage;
}

void DeckTrainerAI::setAbilityDetermination(bool value)
{
    if (this == NULL) return;
    mAbilityDetermination = value;
}

bool DeckTrainerAI::getAbilityDetermination() const
{
    if (this == NULL) return false;
    return mAbilityDetermination;
}

void DeckTrainerAI::setAttrDetermination(bool value)
{
    if (this == NULL) return;
    mAttrDetermination = value;
}

bool DeckTrainerAI::getAttrDetermination() const
{
    if (this == NULL) return false;
    return mAttrDetermination;
}

void DeckTrainerAI::setRespectKeyPlayer(bool value)
{
    if (this == NULL) return;
    mRespectKeyPlayer = value;
}

bool DeckTrainerAI::getRespectKeyPlayer() const
{
    if (this == NULL) return false;
    return mRespectKeyPlayer;
}

void DeckTrainerAI::setSelectAceBoss(bool value)
{
    if (this == NULL) return;
    mSelectAceBoss = value;
}

bool DeckTrainerAI::getSelectAceBoss() const
{
    if (this == NULL) return false;
    return mSelectAceBoss;
}

void DeckTrainerAI::setOrder(bool value)
{
    if (this == NULL) return;
    mOrder = value;
}

bool DeckTrainerAI::getOrder() const
{
    if (this == NULL) return false;
    return mOrder;
}

void DeckTrainerAI::setNotSameWeekPoint(bool value)
{
    if (this == NULL) return;
    mNotSameWeekPoint = value;
}

bool DeckTrainerAI::getNotSameWeekPoint() const
{
    if (this == NULL) return false;
    return mNotSameWeekPoint;
}

void DeckTrainerAI::setSelectRandomPokemon(bool value)
{
    if (this == NULL) return;
    mSelectRandomPokemon = value;
}

bool DeckTrainerAI::getSelectRandomPokemon() const
{
    if (this == NULL) return false;
    return mSelectRandomPokemon;
}
