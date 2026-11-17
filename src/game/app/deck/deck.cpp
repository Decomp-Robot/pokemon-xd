#include <deck.h>

/* Deck table state shared by the registration and lookup routines. */
DeckDarkPokemon* _deckDarkPokemon;
DeckTrainerAI* _deckTrainerAI;
u32 _deckTrainerSize;
u32 _deckPokemonSize;
u32 _deckDarkPokemonSize;

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

u8 DeckDarkPokemon::getUnderTakerFlag() const
{
    if (this == NULL) return 0;
    return mUnderTakerFlag;
}

u8 DeckDarkPokemon::getRevers() const
{
    if (this == NULL) return 0;
    return mRevers;
}

bool DeckDarkPokemon::isScenarioPokemon() const
{
    if (this == NULL) return false;
    return mScenarioPokemon == 1;
}

u16 DeckDarkPokemon::getDarkWaza(s8 index) const
{
    if (this == NULL) return 0;
    if (index < 0 || index >= 4) return 0;
    return mDarkWaza[index];
}

u16 DeckDarkPokemon::getBonusExp() const
{
    if (this == NULL) return 0;
    return mBonusExp;
}

u16 DeckDarkPokemon::getInitDarkPoint() const
{
    if (this == NULL) return 0;
    return mInitDarkPoint;
}

u16 DeckDarkPokemon::getDeckPokemonID() const
{
    if (this == NULL) return 0;
    return mDeckPokemonID;
}

u8 DeckDarkPokemon::getPlayerLevel() const
{
    if (this == NULL) return 0;
    return mPlayerLevel;
}

u8 DeckDarkPokemon::getSnatchRatio() const
{
    if (this == NULL) return 0;
    return mSnatchRatio;
}

u8 DeckDarkPokemon::getWorth() const
{
    if (this == NULL) return 0;
    return mWorth;
}

u8 DeckPokemon::getRandomSetting() const
{
    if (this == NULL) return 0;
    return mRandomSetting;
}

u8 DeckPokemon::getTokuseiFlag() const
{
    if (this == NULL) return 0;
    return mTokuseiFlag;
}

u8 DeckPokemon::getSex() const
{
    if (this == NULL) return 0;
    return mSex;
}

u8 DeckPokemon::getSeikakuID() const
{
    if (this == NULL) return 0;
    return mSeikakuID;
}

u8 DeckPokemon::getComboPartnerAll() const
{
    if (this == NULL) return 0;
    return mComboPartnerAll;
}

u16 DeckPokemon::getWazaID(u8 index) const
{
    if (this == NULL) return 0;
    if (index >= 4) return 0;
    return mWazaID[index];
}

u8 DeckPokemon::getStatusEffort(u8 index) const
{
    if (this == NULL) return 0;
    if (index >= 6) return 0;
    return mStatusEffort[index];
}

u8 DeckPokemon::getStatusRnd(u8 index) const
{
    if (this == NULL) return 0;
    if (index >= 6) return 0;
    return mStatusRnd[index];
}

u8 DeckPokemon::getKeyPlayer() const
{
    if (this == NULL) return 0;
    return mKeyPlayer;
}

u8 DeckPokemon::getRole() const
{
    if (this == NULL) return 0;
    return mRole;
}

u16 DeckPokemon::getHoldItem() const
{
    if (this == NULL) return 0;
    return mHoldItem;
}

u8 DeckPokemon::getFriendLevel() const
{
    if (this == NULL) return 0;
    return mFriendLevel;
}

u8 DeckPokemon::getLevel() const
{
    if (this == NULL) return 0;
    return mLevel;
}

u16 DeckPokemon::getPokemonID() const
{
    if (this == NULL) return 0;
    return mPokemonID;
}

u8 DeckTrainer::getComboActive(u8 index) const
{
    if (this == NULL) return 0;
    if (index >= 4) return 0;
    return mComboActive[index];
}

u16 DeckTrainer::getComboID(u8 index) const
{
    if (this == NULL) return 0;
    if (index >= 4) return 0;
    return mComboID[index];
}

u16 DeckTrainer::getFixedActionID() const
{
    if (this == NULL) return 0;
    return mFixedActionID;
}

void DeckTrainer::setAIID(u16 id)
{
    if (this == NULL) return;
    mAIID = id;
}

u16 DeckTrainer::getAIID() const
{
    if (this == NULL) return 0;
    return mAIID;
}

void DeckTrainer::setPokemonDataID(u8 index, u16 id)
{
    if (this == NULL) return;
    if (index >= 6) return;
    mPokemonDataID[index] = id;
}

u16 DeckTrainer::getDarkPokemonDataID(u8 index) const
{
    if (this == NULL) return 0;
    if (index >= 6) return 0;
    if (isDarkPokemon(index)) return mPokemonDataID[index];
    return 0;
}

u16 DeckTrainer::getPokemonDataID(u8 index) const
{
    if (this == NULL) return 0;
    if (index >= 6) return 0;
    u32 darkID = mPokemonDataID[index];
    if (isDarkPokemon(index)) {
        DeckDarkPokemon* record;
        if (_deckDarkPokemonSize <= darkID) {
            record = NULL;
        } else {
            record = &_deckDarkPokemon[darkID];
        }
        if (record == NULL) return 0;
        return record->getDeckPokemonID();
    }
    return darkID;
}

void DeckTrainer::setTrainerMsg(u8 index, u16 id)
{
    if (this == NULL) return;
    if (index >= 4) return;
    mTrainerMsg[index] = id;
}

u16 DeckTrainer::getTrainerMsg(u8 index) const
{
    if (this == NULL) return 0;
    if (index >= 4) return 0;
    return mTrainerMsg[index];
}

void DeckTrainer::setIntroWZX(u16 id)
{
    if (this == NULL) return;
    mIntroWZX = id;
}

u16 DeckTrainer::getIntroWZX() const
{
    if (this == NULL) return 0;
    return mIntroWZX;
}

void DeckTrainer::setTrainerPKX(u16 id)
{
    if (this == NULL) return;
    mTrainerPKX = id;
}

u16 DeckTrainer::getTrainerPKX() const
{
    if (this == NULL) return 0;
    return mTrainerPKX;
}

void DeckTrainer::setTrainerItem(u8 index, u16 id)
{
    if (this == NULL) return;
    if (index >= 4) return;
    mTrainerItem[index] = id;
}

u16 DeckTrainer::getTrainerItem(u8 index) const
{
    if (this == NULL) return 0;
    if (index >= 4) return 0;
    return mTrainerItem[index];
}

void DeckTrainer::setTrainerName(u16 id)
{
    if (this == NULL) return;
    mTrainerName = id;
}

u16 DeckTrainer::getTrainerName() const
{
    if (this == NULL) return 0;
    return mTrainerName;
}

void DeckTrainer::setTrainerKind(u8 kind)
{
    if (this == NULL) return;
    mTrainerKind = kind;
}

u8 DeckTrainer::getTrainerKind() const
{
    if (this == NULL) return 0;
    return mTrainerKind;
}
