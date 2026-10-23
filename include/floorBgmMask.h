#ifndef FLOOR_BGM_MASK_H
#define FLOOR_BGM_MASK_H

#include <types.h>

class FloorBgmMask {
public:
    enum BgmMaskMode {
        BGM_MASK_NONE = 0,
        BGM_MASK_FADE_OUT = 1,
        BGM_MASK_FADE_KEEP = 2,
        BGM_MASK_FADE_IN_DELAY = 3,
        BGM_MASK_FADE_IN = 4
    };

    FloorBgmMask();
    void initialize();
    void finalize();
    void setVolumeRate(f32 rate);
    void setFadeInTime(f32 time);
    void setFadeOutTime(f32 time);
    void startMask();
    void endMask();
    void stopMask();
    void startMECallback();
    void endMECallback();
    void update(f32 deltaTime);
    void changeMode(BgmMaskMode mode) { mMode = mode; }
    void setActive(u8 active) { mEnable = active; }
    u8 isEnable() { return mEnable; }

private:
    BgmMaskMode mMode;
    u8 mEnable;
    u8 mOriginalVolume;
    u8 mRestoreVolume;
    u8 mMaskedVolume;
    u32 mSoundID;
    f32 mVolumeRate;
    f32 mFadeInTime;
    f32 mFadeOutTime;
    f32 mTime;
    f32 mFadeInDelay;
};

struct floorsound_dd_ {
    u8 trigger;
    u8 volume;
    u16 floorID;
    u16 soundID;
    u16 fadeInTime;
    u16 fadeOutTime;
    u16 flag;
};

extern "C" {
extern u32* floorsoundTbl_number;
extern floorsound_dd_* floorsoundTbl;
FloorBgmMask* floorSound_getBgmMaskObj();
u8 floorSound_getDispSoundVolume();
void floorSound_setDispSoundVolume(u8 enable);
u8 floorSound_playEnvSound(s32 trigger);
u8 floorSound_playSound();
floorsound_dd_* floorSound_SearchTable(u32 floorID, floorsound_dd_* start);
}

#endif
