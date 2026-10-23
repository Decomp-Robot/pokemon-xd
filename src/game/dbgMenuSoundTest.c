#include <types.h>

/* Debug sound test menus (SE / ENV / BGM) and the sound request helper. */
extern u32 *lbl_804E8820;
extern u16 *lbl_804E8824;
extern u32 *lbl_804E8828;
extern u16 *lbl_804E882C;
extern u32 *lbl_804E8830;
extern u16 *lbl_804E8834;
extern u32 *lbl_804E8838;
extern u16 *lbl_804E883C;

s32 menuOpen(s32 menuID, s32 mode);
void menuClose(s32 menuID);
void GSsndSetMasterVolume(s32 volume, s32 a, s32 b, s32 c);
s32 GSsndGetStatus(u16 soundID);
void sndPlay(u16 soundID, s32 bgm);
void GSsndClose(u16 soundID);
void soundOpenPlay(u16 soundID, s32 a, s32 b);
void soundOpenPlayEnv(u16 soundID, s32 a, s32 b);

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

#pragma peephole off


s32 zz_000bd20_(s32 item)
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

s32 zz_000bd84_(s32 item)
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
        if (sel < *lbl_804E8828) {
            sndPlay(lbl_804E882C[sel], 0);
        }
    }
    menuClose(2);
    return -1;
}

s32 zz_000be4c_(void)
{
    s32 sel;

    while ((sel = menuOpen(2, 1)) != -1) {
        if (sel < *lbl_804E8820) {
            sndPlay(lbl_804E8824[sel], 0);
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
        if (sel < *lbl_804E8830) {
            for (i = 0; i < *lbl_804E8830; i++) {
                if (sel != i) {
                    status = GSsndGetStatus(lbl_804E8834[i]);
                    switch (status) {
                    case 1:
                    case 2:
                    case 3:
                        GSsndClose(lbl_804E8834[i]);
                        break;
                    }
                }
            }
            sndPlay(lbl_804E8834[sel], 0);
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
        if (sel < *lbl_804E8838) {
            for (i = 0; i < *lbl_804E8838; i++) {
                if (sel != i) {
                    GSsndGetStatus(lbl_804E883C[i]);
                }
            }
            sndPlay(lbl_804E883C[sel], 1);
        }
    }
    menuClose(2);
    return -1;
}

void sndPlay(u16 soundID, s32 bgm)
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

