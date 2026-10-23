#ifndef GS_SND_H
#define GS_SND_H

#include <types.h>

enum MenuSoundID {
    MENU_SOUND_CONFIRM = 0x11,
    MENU_SOUND_CANCEL = 0x12,
    MENU_SOUND_MESSAGE = 0x13
};

enum GSsndStatus {
    GS_SOUND_STOPPED = 0,
    GS_SOUND_PLAYING = 2
};

#ifdef __cplusplus
extern "C" {
#endif

void GSsndPlayNow(long soundID);
void GSsndPlayVolume(s32 soundID, s32 time, s32 volume, s32 wait);
s32 GSsndGetStatus(s32 sndID);
void GSsndClose(s32 sndID);
void GSsndSetMasterVolume(s32 volume, s32 time, s32 music, s32 fx);

void GSsndStopNow(unsigned long soundID);
#ifdef __cplusplus
}
#endif

#endif
