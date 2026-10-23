#include <floorBgmMask.h>
#include <floorManager.h>
#include <dbgMenu.h>
#include <GSflag.h>
#include <GSmsg.h>
#include <GSsnd.h>
#include <sound.h>

static FloorBgmMask _bgmMask;
static u8 _dispSoundVolume;

u8 _playSound(floorsound_dd_* entry);
u8 _playEnvSound(floorsound_dd_* entry);

void FloorBgmMask::update(f32 deltaTime)
{
    const char* modeName = "---";
    mSoundID = soundGetBGMID();
    if (mSoundID == 0) {
        changeMode(BGM_MASK_NONE);
    }
    switch (mMode) {
    case BGM_MASK_NONE:
        modeName = "noneMask";
        break;
    case BGM_MASK_FADE_OUT:
        modeName = "fadeOut";
        mTime -= deltaTime;
        if (mTime <= 0.0f) {
            changeMode(BGM_MASK_FADE_KEEP);
        }
        break;
    case BGM_MASK_FADE_KEEP:
        modeName = "fadeKeep";
        break;
    case BGM_MASK_FADE_IN_DELAY:
        modeName = "fadeInDelay";
        mTime -= deltaTime;
        if (mTime <= 0.0f) {
            soundSetVolumeID(mSoundID, 1000.0f * mFadeInTime, mRestoreVolume);
            mTime = mFadeInTime;
            changeMode(BGM_MASK_FADE_IN);
        }
        break;
    case BGM_MASK_FADE_IN:
        modeName = "fadeIn";
        mTime -= deltaTime;
        if (mTime <= 0.0f) {
            changeMode(BGM_MASK_NONE);
        }
        break;
    }
    if (_dispSoundVolume) {
        GSgfxLayerID layer = dbgMenuGetLayerID();
        if (layer != -1) {
            u32 volume = 0;
            if (mSoundID != 0) {
                volume = soundGetVolume(mSoundID);
            }
            GSprint(layer, 8, 26, 0xFFFFFFFF, "snd_id = %d, curVol %d [%s]", mSoundID, volume, modeName);
        }
    }
}

void FloorBgmMask::endMECallback()
{
}

void FloorBgmMask::startMECallback()
{
    switch (mMode) {
    case BGM_MASK_FADE_IN_DELAY:
        soundSetVolumeID(mSoundID, 1000.0f * mFadeInTime, mRestoreVolume);
        changeMode(BGM_MASK_NONE);
        break;
    }
}

void FloorBgmMask::stopMask()
{
    if (!isEnable()) return;
    if (mMode == BGM_MASK_NONE) return;
    soundSetVolumeID(mSoundID, 1000.0f * mFadeInTime, mRestoreVolume);
    changeMode(BGM_MASK_NONE);
}

void FloorBgmMask::endMask()
{
    if (!isEnable()) return;
    mTime = mFadeInDelay;
    changeMode(BGM_MASK_FADE_IN_DELAY);
}

void FloorBgmMask::startMask()
{
    if (!isEnable()) return;
    if (mMode == BGM_MASK_FADE_OUT || mMode == BGM_MASK_FADE_KEEP) return;
    if (mMode == BGM_MASK_FADE_IN_DELAY) {
        changeMode(BGM_MASK_FADE_KEEP);
        return;
    }
    if (mMode == BGM_MASK_NONE) {
        mSoundID = soundGetBGMID();
        if (mSoundID == 0) return;
        mOriginalVolume = 127;
        mOriginalVolume = soundGetVolume(mSoundID);
        mRestoreVolume = mOriginalVolume;
        mMaskedVolume = mOriginalVolume * mVolumeRate;
    }
    soundSetVolumeID(mSoundID, 1000.0f * mFadeOutTime, mMaskedVolume);
    changeMode(BGM_MASK_FADE_OUT);
    mTime = mFadeOutTime;
}

void FloorBgmMask::setFadeOutTime(f32 time)
{
    mFadeOutTime = time;
}

void FloorBgmMask::setFadeInTime(f32 time)
{
    mFadeInTime = time;
}

void FloorBgmMask::setVolumeRate(f32 rate)
{
    if (rate < 0.0f || rate > 1.0f) return;
    mVolumeRate = rate;
}

void FloorBgmMask::finalize()
{
    mFadeInTime = 0.0f;
    stopMask();
    mEnable = 1;
    mMode = BGM_MASK_NONE;
    mOriginalVolume = 0;
    mVolumeRate = 0.9f;
    mFadeInTime = 0.3f;
    mFadeOutTime = 0.3f;
    mFadeInDelay = 4.0f;
}

void FloorBgmMask::initialize()
{
}

FloorBgmMask::FloorBgmMask()
{
    mEnable = 1;
    mMode = BGM_MASK_NONE;
    mOriginalVolume = 0;
    mVolumeRate = 0.9f;
    mFadeInTime = 0.3f;
    mFadeOutTime = 0.3f;
    mFadeInDelay = 4.0f;
}

FloorBgmMask* floorSound_getBgmMaskObj()
{
    return &_bgmMask;
}

u8 floorSound_getDispSoundVolume()
{
    return _dispSoundVolume;
}

void floorSound_setDispSoundVolume(u8 enable)
{
    _dispSoundVolume = enable;
}

u8 floorSound_playEnvSound(s32 trigger)
{
    floorsound_dd_* entry;
    u32 floorID = floorGetCurrent();
    entry = floorSound_SearchTable(floorID, 0);
    while (entry) {
        if (entry->flag == 0 || (u8)GSflagTest(entry->flag)) {
            if (trigger == entry->trigger) {
                if (_playEnvSound(entry)) return 1;
            }
        }
        entry = floorSound_SearchTable(floorID, entry + 1);
    }
    return 0;
}

u8 floorSound_playSound()
{
    floorsound_dd_* entry;
    u32 floorID = floorGetCurrent();
    entry = floorSound_SearchTable(floorID, 0);
    while (entry) {
        if (entry->flag == 0 || (u8)GSflagTest(entry->flag)) {
            if (_playSound(entry)) return 1;
        }
        entry = floorSound_SearchTable(floorID, entry + 1);
    }
    return 0;
}

floorsound_dd_* floorSound_SearchTable(u32 floorID, floorsound_dd_* start)
{
    floorsound_dd_* end = floorsoundTbl + *floorsoundTbl_number;
    if (!start) start = floorsoundTbl;
    for (; start < end; ++start) {
        if (start->floorID == floorID) return start;
    }
    return 0;
}

u8 _playEnvSound(floorsound_dd_* entry)
{
    if (!soundCheckEnv(entry->soundID)) return 0;
    soundOpenPlayEnvFadeOutIn(entry->soundID, entry->fadeOutTime, entry->fadeInTime, entry->volume);
    return 1;
}

u8 _playSound(floorsound_dd_* entry)
{
    if (!entry) return 0;
    if (soundCheckEnv(entry->soundID)) return 0;
    if (GSsndCheckSE(entry->soundID)) {
        soundOpenPlay(entry->soundID, entry->fadeInTime, entry->volume);
    } else {
        if (entry->soundID == soundGetBGMID()) {
            if (entry->volume != GSsndGetVolume(entry->soundID)) {
                GSsndSetVolume(entry->volume, 0, 1, 0);
            }
        } else {
            soundOpenPlayFadeOutIn(entry->soundID, 500, entry->fadeInTime, entry->volume);
        }
    }
    return 1;
}
