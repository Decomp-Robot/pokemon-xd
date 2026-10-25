#include <menu.h>
#include <sound.h>
#include <GSsnd.h>
#include <soundTestData.h>
#include <types.h>

enum {
    SOUND_TEST_ITEM_MUSIC_MAXIMUM = 339,
    SOUND_TEST_ITEM_MUSIC_MUTE = 340,
    SOUND_TEST_ITEM_EFFECTS_MAXIMUM = 341,
    SOUND_TEST_ITEM_EFFECTS_MUTE = 342
};

/* Debug sound test menus (SE / ENV / BGM) and the sound request helper.  Each
 * table has a count pointer and a sound id list loaded from its resource. */

void sndPlay(s32 soundID, s32 bgm);

#pragma force_active on

#pragma peephole off

s32 dbgMenuSoundTestChangeSE(s32 item)
{
    switch (item) {
    case SOUND_TEST_ITEM_EFFECTS_MAXIMUM:
        GSsndSetMasterVolume(127, 0, 0, 1);
        break;
    case SOUND_TEST_ITEM_EFFECTS_MUTE:
        GSsndSetMasterVolume(0, 0, 0, 1);
        break;
    }
    return 0;
}

s32 dbgMenuSoundTestChangeBGM(s32 item)
{
    switch (item) {
    case SOUND_TEST_ITEM_MUSIC_MAXIMUM:
        GSsndSetMasterVolume(127, 0, 1, 0);
        break;
    case SOUND_TEST_ITEM_MUSIC_MUTE:
        GSsndSetMasterVolume(0, 0, 1, 0);
        break;
    }
    return 0;
}

s32 dbgMenuSoundTestNAKIGOE(void)
{
    s32 sel;

    while ((sel = menuOpen(MENU_ID_NUMBER_INPUT, 1)) != -1) {
        if (sel < *snd_test_nakigoe_number) {
            sndPlay(snd_test_nakigoe[sel], 0);
        }
    }
    menuClose(MENU_ID_NUMBER_INPUT);
    return -1;
}

s32 dbgMenuSoundTestSE(void)
{
    s32 sel;

    while ((sel = menuOpen(MENU_ID_NUMBER_INPUT, 1)) != -1) {
        if (sel < *snd_test_se_number) {
            sndPlay(snd_test_se[sel], 0);
        }
    }
    menuClose(MENU_ID_NUMBER_INPUT);
    return -1;
}

s32 dbgMenuSoundTestENV(void)
{
    s32 sel;
    s32 i;
    s32 status;

    while ((sel = menuOpen(MENU_ID_NUMBER_INPUT, 1)) != -1) {
        if (sel < *snd_test_env_number) {
            for (i = 0; i < *snd_test_env_number; i++) {
                if (sel != i) {
                    status = GSsndGetStatus(snd_test_env[i]);
                    switch (status) {
                    case 1:
                    case GSSND_PLAYING:
                    case GSSND_PAUSED:
                        GSsndClose(snd_test_env[i]);
                        break;
                    }
                }
            }
            sndPlay(snd_test_env[sel], 0);
        }
    }
    menuClose(MENU_ID_NUMBER_INPUT);
    return -1;
}

s32 dbgMenuSoundTestBGM(void)
{
    s32 sel;
    s32 i;

    while ((sel = menuOpen(MENU_ID_NUMBER_INPUT, 1)) != -1) {
        if (sel < *snd_test_bgm_number) {
            for (i = 0; i < *snd_test_bgm_number; i++) {
                if (sel != i) {
                    GSsndGetStatus(snd_test_bgm[i]);
                }
            }
            sndPlay(snd_test_bgm[sel], 1);
        }
    }
    menuClose(MENU_ID_NUMBER_INPUT);
    return -1;
}

void sndPlay(s32 soundID, s32 bgm)
{
    switch (GSsndGetStatus(soundID)) {
    case 1:
    case GSSND_PLAYING:
    case GSSND_PAUSED:
        GSsndClose(soundID);
        break;
    case GSSND_CLOSED:
    default:
        if (bgm) {
            soundOpenPlay(soundID, 0, 127);
        } else {
            soundOpenPlayEnv(soundID, 0, 127);
        }
        break;
    }
}

#pragma peephole reset

#pragma force_active reset

