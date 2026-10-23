#include <menu.h>
#include <sound.h>
#include <GSsnd.h>
#include <types.h>

/* Debug sound test menus (SE / ENV / BGM) and the sound request helper.  Each
 * table has a count pointer and a sound id list loaded from its resource. */
extern u32 *snd_test_se_number;
extern u16 *snd_test_se;
extern u32 *snd_test_nakigoe_number;
extern u16 *snd_test_nakigoe;
extern u32 *snd_test_env_number;
extern u16 *snd_test_env;
extern u32 *snd_test_bgm_number;
extern u16 *snd_test_bgm;

void GSsndSetMasterVolume(s32 volume, s32 a, s32 b, s32 c);
void sndPlay(s32 soundID, s32 bgm);

#pragma force_active on

#pragma peephole off

s32 dbgMenuSoundTestChangeSE(s32 item)
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

s32 dbgMenuSoundTestChangeBGM(s32 item)
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

s32 dbgMenuSoundTestNAKIGOE(void)
{
    s32 sel;

    while ((sel = menuOpen(2, 1)) != -1) {
        if (sel < *snd_test_nakigoe_number) {
            sndPlay(snd_test_nakigoe[sel], 0);
        }
    }
    menuClose(2);
    return -1;
}

s32 dbgMenuSoundTestSE(void)
{
    s32 sel;

    while ((sel = menuOpen(2, 1)) != -1) {
        if (sel < *snd_test_se_number) {
            sndPlay(snd_test_se[sel], 0);
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
        if (sel < *snd_test_env_number) {
            for (i = 0; i < *snd_test_env_number; i++) {
                if (sel != i) {
                    status = GSsndGetStatus(snd_test_env[i]);
                    switch (status) {
                    case 1:
                    case 2:
                    case 3:
                        GSsndClose(snd_test_env[i]);
                        break;
                    }
                }
            }
            sndPlay(snd_test_env[sel], 0);
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
        if (sel < *snd_test_bgm_number) {
            for (i = 0; i < *snd_test_bgm_number; i++) {
                if (sel != i) {
                    GSsndGetStatus(snd_test_bgm[i]);
                }
            }
            sndPlay(snd_test_bgm[sel], 1);
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

