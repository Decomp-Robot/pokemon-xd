#include <menu.h>
#include <GSsnd.h>
#include <sound.h>
#include <types.h>

/* Debug sound test menus (SE / ENV / BGM) and the sound request helper. */
extern u32 *soundAlternateEffectListCount;
extern u16 *soundAlternateEffectListIds;
extern u32 *soundEffectListCount;
extern u16 *soundEffectListIds;
extern u32 *soundEnvironmentListCount;
extern u16 *soundEnvironmentListIds;
extern u32 *soundMusicListCount;
extern u16 *soundMusicListIds;

void GSsndSetMasterVolume(s32 volume, s32 time, s32 music, s32 fx);
void sndPlay(s32 soundID, s32 bgm);

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

#pragma peephole off


s32 dbgMenuSoundTestFxVolume(s32 item)
{
    switch (item) {
    case 0x155:
        GSsndSetMasterVolume(0x7F, 0, 0, 1);
        break;
    case 0x156:
        GSsndSetMasterVolume(0, 0, 0, 1);
        break;
    }
    return 0;
}

s32 dbgMenuSoundTestMusicVolume(s32 item)
{
    switch (item) {
    case 0x153:
        GSsndSetMasterVolume(0x7F, 0, 1, 0);
        break;
    case 0x154:
        GSsndSetMasterVolume(0, 0, 1, 0);
        break;
    }
    return 0;
}

s32 dbgMenuSoundTestSE(void)
{
    s32 sel;

    while ((sel = menuOpen(2, 1)) != -1) {
        if (sel < *soundEffectListCount) {
            sndPlay(soundEffectListIds[sel], 0);
        }
    }
    menuClose(2);
    return -1;
}

s32 dbgMenuSoundTestAlternateSE(void)
{
    s32 sel;

    while ((sel = menuOpen(2, 1)) != -1) {
        if (sel < *soundAlternateEffectListCount) {
            sndPlay(soundAlternateEffectListIds[sel], 0);
        }
    }
    menuClose(2);
    return -1;
}

s32 dbgMenuSoundTestENV(void)
{
    s32 sel;
    s32 i;
    s32 status;

    while ((sel = menuOpen(2, 1)) != -1) {
        if (sel < *soundEnvironmentListCount) {
            for (i = 0; i < *soundEnvironmentListCount; i++) {
                if (sel != i) {
                    status = GSsndGetStatus(soundEnvironmentListIds[i]);
                    switch (status) {
                    case 1:
                    case 2:
                    case 3:
                        GSsndClose(soundEnvironmentListIds[i]);
                        break;
                    }
                }
            }
            sndPlay(soundEnvironmentListIds[sel], 0);
        }
    }
    menuClose(2);
    return -1;
}

s32 dbgMenuSoundTestBGM(void)
{
    s32 sel;
    s32 i;

    while ((sel = menuOpen(2, 1)) != -1) {
        if (sel < *soundMusicListCount) {
            for (i = 0; i < *soundMusicListCount; i++) {
                if (sel != i) {
                    GSsndGetStatus(soundMusicListIds[i]);
                }
            }
            sndPlay(soundMusicListIds[sel], 1);
        }
    }
    menuClose(2);
    return -1;
}

void sndPlay(s32 soundID, s32 bgm)
{
    s32 status = GSsndGetStatus(soundID);

    if (status == 0 || status < 0) {
        goto play;
    }
    if (status < 4) {
        GSsndClose(soundID);
        return;
    }
play:
    if (bgm) {
        soundOpenPlay(soundID, 0, 0x7F);
    } else {
        soundOpenPlayEnv(soundID, 0, 0x7F);
    }
}

#pragma peephole reset

#pragma force_active reset

