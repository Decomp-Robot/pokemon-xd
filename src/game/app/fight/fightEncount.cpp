#include <fightEncount.h>
#include <fightEncountBios.h>
#include <fight.h>
#include <fightFloor.h>
#include <fightTrainerDB.h>
#include <app/fight/fightTrainerBios.h>
#include <hero.h>
#include <deck.h>
#include <recovery.h>
#include <floor.h>
#include <people.h>
#include <mailMain.h>
#include <GSflag.h>
#include <GSfsys.h>
#include <fade.h>
#include <sound.h>
#include <GSsndPlayback.h>

FIGHT_ENCOUNT _fightEncount;

enum {
    FIGHT_ENCOUNT_FIELD_SOUND_FLAG = 782,
    FIGHT_ENCOUNT_SKIP_RETURN_FADE_FLAG = 803,
    FIGHT_ENCOUNT_RETURN_FADE_SCRIPT = 0x5960008,
    FIGHT_ENCOUNT_DEFAULT_BGM = 8,
    FIGHT_ENCOUNT_SOUND_FADE_TIME = 1000
};

void fightEncountAnnihilationRecovery()
{
    u16 slot;
    u8 maxLevel = 0;
    for (slot = 0; slot < 6; slot++) {
        u8 level = ((Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, slot))->getLevel();
        if (maxLevel < level) {
            maxLevel = level;
        }
    }
    heroDecPokedoru(0, maxLevel * 16);
    recoveryPokemon();
}

FIGHT_ENCOUNT *fightEncountGetStrObj()
{
    return &_fightEncount;
}

u8 fightEncountCheckZenmetu()
{
    void *encountData = fightEncountDataBiosGetPtr(fightGetFightEncountDataId());
    Hero *hero = Hero::getHeroPtr();
    if (!hero->getBattleResumeFloorID()) {
        return 0;
    }
    if (!fightEncountDataBiosGetZenmetuFlag(encountData)) {
        return 0;
    }
    if (fightFloorIsGcHeroWin(0, fightGetFightResultId()) == 1) {
        return 0;
    }
    GSfsysFree(DECK_RESOURCE_GROUP);
    FIGHT_ENCOUNT *encount = fightEncountGetStrObj();
    encount->clear();
    encount->setDirtyFlag(true);
    GSflagOff(FIGHT_ENCOUNT_SKIP_RETURN_FADE_FLAG);
    fightEncountAnnihilationRecovery();
    peopleAllBlockMoveScript();
    floorSetFloorChangeInfo(hero->getBattleResumeFloorID(), hero->getBattleResumeFloorIndex());
    floorPopAndChange(hero->getBattleResumeFloorID());
    floorSetFadeScript(0, FIGHT_ENCOUNT_RETURN_FADE_SCRIPT);
    floorWaitForChange();
    return 1;
}

u16 fightEncount(u16 encountId)
{
    u16 floorId;
    void *encountData;
    u32 encountFloor;
    void *wipe;
    Hero *hero;
    u8 message;
    FIGHT_ENCOUNT *encount;
    u32 fieldBgm;
    u32 fieldEnv;
    u16 effectSound;
    u32 fightKind;
    encount = fightEncountGetStrObj();
    encount->getField();
    encount->getFightMode();
    encount->getDestroyFlag();
    for (message = 0; message < 4; message++) {
        encount->getMsg(message);
    }
    if (!encountId) {
        return 0;
    }
    fightReadDeck(encountId);
    encountData = fightEncountDataBiosGetPtr(encountId);
    fightKind = fightEncountDataBiosGetFightKind(encountData);
    fightSetFightResultId(0);
    GSflagOff(FIGHT_ENCOUNT_FIELD_SOUND_FLAG);
    fightSetFightEncountDataId(encountId);
    fightColosseumTrainerIndex = -1;
    fightDataRewrite();
    encountFloor = GSfloorGetCurrent();
    fightFloor_SetEncountFloorId(0, encountFloor);
    floorId = fightFloorDB_GetFloorDataId(fightEncountDataBiosGetFightFloorDataId(encountData));
    mailMainReceiveTerminate();
    fightTrainerSetNameHearFlag(encountId);
    fieldBgm = soundGetBGMID();
    fieldEnv = soundGetEnvID();
    soundOpenPlay(FIGHT_ENCOUNT_DEFAULT_BGM, FIGHT_ENCOUNT_SOUND_FADE_TIME, 255);
    soundStopEnv(FIGHT_ENCOUNT_SOUND_FADE_TIME);
    if ((u8)fightKind == FIGHT_BATTLE_KIND_BATTLE_DISK &&
        fightEncountDataBiosGetDiskId(fightEncountDataBiosGetPtr(encountId)) == 0) {
        fadeSet(FADE_OUT_BLACK, 0.5f);
        u32 bgm = fightEncountGetBgmSndDataId(encountId);
        if (bgm) {
            soundOpenPlay(bgm, 0, 255);
        }
    } else {
        wipe = fightEncountWipeDataBiosGetPtr(fightEncountDataBiosGetWipeId(encountData));
        fadeSetEX(FADE_OUT_EFFECT, 1.0f,
                  fightEncountDataBiosGetWipeEffectTime(wipe),
                  fightEncountDataBiosGetWipeFunction(wipe),
                  fightEncountDataBiosGetWipeSnapshotUse(wipe));
        effectSound = fightEncountDataBiosGetWipeEffectSndID(wipe);
        if (effectSound) {
            GSsndPlay(effectSound, 0, 0);
        }
    }
    fightSoundOpenPlayInit();
    fightFreeDeck(encountId);
    GSfloorPush(floorId);
    floorSetFadeScript(0, 0);
    floorWaitForChange();
    floorSetPrevFloorID(floorId);
    GSfsysFree(DECK_RESOURCE_GROUP);
    encount->clear();
    encount->setDirtyFlag(true);
    if (fightEncountDataBiosGetZenmetuFlag(encountData) &&
        !fightFloorIsGcHeroWin(0, fightGetFightResultId())) {
        fightSetFightEncountDataId(0);
        GSflagOff(FIGHT_ENCOUNT_SKIP_RETURN_FADE_FLAG);
        fightEncountAnnihilationRecovery();
        peopleAllBlockMoveScript();
        hero = Hero::getHeroPtr();
        floorLinkWithSE(hero->getBattleResumeFloorID(), hero->getBattleResumeFloorIndex(), 0);
        floorSetFadeScript(0, FIGHT_ENCOUNT_RETURN_FADE_SCRIPT);
        floorWaitForChange();
        fightFloorSetStatus(0, 0, FIGHT_FLOOR_STATUS_ENCOUNT_DATA_ID, 0, 0);
        return fightGetFightResultId();
    }
    soundOpenPlay(fieldBgm, FIGHT_ENCOUNT_SOUND_FADE_TIME, 127);
    soundOpenPlayEnv(fieldEnv, FIGHT_ENCOUNT_SOUND_FADE_TIME, 127);
    GSflagOn(FIGHT_ENCOUNT_FIELD_SOUND_FLAG);
    floorExecSoundScript();
    floorClearReturnPoint();
    if (!GSflagTest(FIGHT_ENCOUNT_SKIP_RETURN_FADE_FLAG)) {
        fadeSet(FADE_IN_BLACK, 0.5f);
        fadeCheck(1);
    }
    fightFloorSetStatus(0, 0, FIGHT_FLOOR_STATUS_ENCOUNT_DATA_ID, 0, 0);
    return fightGetFightResultId();
}

u32 fightEncountGetEnvSndDataId(u16 encountId)
{
    return fightFloorDB_GetEnvSndId(fightEncountDataBiosGetFightFloorDataId(fightEncountDataBiosGetPtr(encountId)));
}

u32 fightEncountGetBgmSndDataId(u16 encountId)
{
    void *encountData = fightEncountDataBiosGetPtr(encountId);
    u32 bgm = fightEncountDataBiosGetBgmSndId(encountData);
    if (bgm) {
        return bgm;
    }
    bgm = fightFloorDB_GetBgmSndId(fightEncountDataBiosGetFightFloorDataId(fightEncountDataBiosGetPtr(encountId)));
    if (bgm) {
        return bgm;
    }
    for (u16 trainer = 0; trainer < 4; trainer++) {
        u16 trainerId = fightEncountDataBiosGetFightTrainerDataId(encountData, (u8)trainer);
        if (trainerId) {
            u8 kind = fightTrainerDB_GetKindDataId(trainerId);
            if (kind) {
                bgm = fightTrainerKindDataBiosGetBgmSndId(fightTrainerKindDataBiosGetPtr(kind));
                if (bgm) {
                    return bgm;
                }
            }
        }
    }
    return FIGHT_ENCOUNT_DEFAULT_BGM;
}

void FIGHT_ENCOUNT::setDirtyFlag(bool value)
{
    if (this) {
        mDirty = value;
    }
}

bool FIGHT_ENCOUNT::getDirtyFlag() const
{
    if (this == 0) {
        return 0;
    }
    return mDirty;
}

void FIGHT_ENCOUNT::setRoundNo(unsigned long value)
{
    if (this) {
        mRoundNo = value;
    }
}

u32 FIGHT_ENCOUNT::getRoundNo() const
{
    if (this == 0) {
        return 0;
    }
    return mRoundNo;
}

void FIGHT_ENCOUNT::setDestroyFlag(u8 value)
{
    if (this) {
        mDestroyFlag = value;
    }
}

u32 FIGHT_ENCOUNT::getDestroyFlag() const
{
    if (this == 0) {
        return 0;
    }
    return mDestroyFlag;
}

void FIGHT_ENCOUNT::setTrainerKind(u8 value)
{
    if (this) {
        mTrainerKind = value;
    }
}

u8 FIGHT_ENCOUNT::getTrainerKind() const
{
    if (this == 0) {
        return 0;
    }
    return mTrainerKind;
}

void FIGHT_ENCOUNT::setFightMode(u8 value)
{
    if (this) {
        mFightMode = value;
    }
}

u8 FIGHT_ENCOUNT::getFightMode() const
{
    if (this == 0) {
        return 0;
    }
    return mFightMode;
}

void FIGHT_ENCOUNT::setSwapDeckNO(u16 value)
{
    if (this) {
        mSwapDeckNO = value;
    }
}

u16 FIGHT_ENCOUNT::getSwapDeckNO() const
{
    if (this == 0) {
        return 0;
    }
    return mSwapDeckNO;
}

void FIGHT_ENCOUNT::setSwapMode(u8 value)
{
    if (this) {
        mSwapMode = value;
    }
}

u32 FIGHT_ENCOUNT::getSwapMode() const
{
    if (this == 0) {
        return 0;
    }
    return mSwapMode;
}

void FIGHT_ENCOUNT::setAddID(u8 value)
{
    if (this) {
        mAddID = value;
    }
}

u8 FIGHT_ENCOUNT::getAddID() const
{
    if (this == 0) {
        return 0;
    }
    return mAddID;
}

void FIGHT_ENCOUNT::setAddPlace(u8 value)
{
    if (this) {
        mAddPlace = value;
    }
}

u8 FIGHT_ENCOUNT::getAddPlace() const
{
    if (this == 0) {
        return 0;
    }
    return mAddPlace;
}

void FIGHT_ENCOUNT::setAddMode(u8 value)
{
    if (this) {
        mAddMode = value;
    }
}

u32 FIGHT_ENCOUNT::getAddMode() const
{
    if (this == 0) {
        return 0;
    }
    return mAddMode;
}

void FIGHT_ENCOUNT::setMsg(u8 index, u16 message)
{
    if (this) {
        mMessages[index] = message;
    }
}

u16 FIGHT_ENCOUNT::getMsg(u8 index) const
{
    if (this == 0) {
        return 0;
    }
    return mMessages[index];
}

void FIGHT_ENCOUNT::setField(u16 field)
{
    if (this) {
        mField = field;
    }
}

u16 FIGHT_ENCOUNT::getField() const
{
    if (this == 0) {
        return 0;
    }
    return mField;
}

void FIGHT_ENCOUNT::clear()
{
    mField = 0;
    mAddMode = 0;
    mAddPlace = 0;
    mAddID = 0;
    mSwapMode = 0;
    mSwapDeckNO = 0;
    mFightMode = 0;
    mTrainerKind = 0;
    for (u8 message = 0; message < 4; message++) {
        mMessages[message] = 0;
    }
    mDestroyFlag = 0;
    mRoundNo = 0;
    mDirty = false;
}
