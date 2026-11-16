#include <fade.h>
#include <fightEncountBios.h>
#include <types.h>

extern "C" {

/* Fade effect initialisers the battle wipes are drawn with; they live in the
 * fade effect module. */
void fadeEffectHookFunction_fadeout_in_Init(void);
void fadeEffectHookFunction_yoko_Init(void);
void fadeEffectHookFunction_tate_Init(void);
void fadeEffectHookFunction_ball_Init(void);
void fadeEffectHookFunction_yoko_or_tate_Init(void);
void fadeEffectHookFunction_yoko_or_tate_or_ball_Init(void);
void fadeEffectHookFunction_boss_Init(void);
void fadeEffectHookFunction_carde_Init(void);
void fadeEffectHookFunction_trainer_Init(void);
void fadeEffectHookFunction_fadein_Init(void);
void fadeEffectHookFunction_distortion_Init(void);
void fadeEffectHookFunction_tear_off_Init(void);
void fadeEffectHookFunction_boss_second_Init(void);
void fadeEffectHookFunction_bound_ball_Init(void);
void fadeEffectHookFunction_ball_second_Init(void);
void fadeEffectHookFunction_blur_ball_Init(void);
void fadeEffectHookFunction_dist_ball_Init(void);
void fadeEffectHookFunction_dist_zoom_in_Init(void);
void fadeEffectHookFunction_dist_zoom_out_Init(void);
void fadeEffectHookFunction_dist_yoko_Init(void);
void fadeEffectHookFunction_distortion_strip_tate_Init(void);
void fadeEffectHookFunction_distortion_strip_yoko_Init(void);
void fadeEffectHookFunction_four_ball_Init(void);
void fadeEffectHookFunction_slide_ball_Init(void);
void fadeEffectHookFunction_trainer_second_Init(void);

}

/* The battle wipes an encounter record selects by its wipe id, in wipe id
 * order: whether the wipe keeps the picture as a snapshot, the sound effect
 * played with it, how long it runs and the fade effect that draws it. */
FightEncountWipeData fight_encount_wipe_data[] = {
    /* FIGHT_WIPE_ID_FADEOUT_IN */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_NONE, 0.5f, fadeEffectHookFunction_fadeout_in_Init },
    /* FIGHT_WIPE_ID_YOKO */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_yoko_Init },
    /* FIGHT_WIPE_ID_TATE */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_tate_Init },
    /* FIGHT_WIPE_ID_BALL */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_ball_Init },
    /* FIGHT_WIPE_ID_BOSS */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_boss_Init },
    /* FIGHT_WIPE_ID_CARDE */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_carde_Init },
    /* FIGHT_WIPE_ID_YOKO_OR_TATE */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_yoko_or_tate_Init },
    /* FIGHT_WIPE_ID_YOKO_OR_TATE_OR_BALL */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_yoko_or_tate_or_ball_Init },
    /* FIGHT_WIPE_ID_FADEOUT_IN_2 */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_NONE, 0.5f, fadeEffectHookFunction_fadeout_in_Init },
    /* FIGHT_WIPE_ID_TRAINER */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_trainer_Init },
    /* FIGHT_WIPE_ID_FADEIN */
    { 0, FIGHT_ENCOUNT_WIPE_SND_ID_NONE, 0.1f, fadeEffectHookFunction_fadein_Init },
    /* FIGHT_WIPE_ID_DISTORTION */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_distortion_Init },
    /* FIGHT_WIPE_ID_TEAR_OFF */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_tear_off_Init },
    /* FIGHT_WIPE_ID_BOSS_SECOND */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_boss_second_Init },
    /* FIGHT_WIPE_ID_BOUND_BALL */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_bound_ball_Init },
    /* FIGHT_WIPE_ID_BALL_SECOND */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_ball_second_Init },
    /* FIGHT_WIPE_ID_BLUR_BALL */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_blur_ball_Init },
    /* FIGHT_WIPE_ID_DIST_BALL */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_dist_ball_Init },
    /* FIGHT_WIPE_ID_DIST_ZOOM_IN */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_dist_zoom_in_Init },
    /* FIGHT_WIPE_ID_DIST_ZOOM_OUT */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_dist_zoom_out_Init },
    /* FIGHT_WIPE_ID_DIST_YOKO */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_dist_yoko_Init },
    /* FIGHT_WIPE_ID_DISTORTION_STRIP_TATE */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_distortion_strip_tate_Init },
    /* FIGHT_WIPE_ID_DISTORTION_STRIP_YOKO */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_distortion_strip_yoko_Init },
    /* FIGHT_WIPE_ID_FOUR_BALL */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_four_ball_Init },
    /* FIGHT_WIPE_ID_SLIDE_BALL */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_slide_ball_Init },
    /* FIGHT_WIPE_ID_TRAINER_SECOND */
    { 1, FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT, 2.5f, fadeEffectHookFunction_trainer_second_Init },
};

u32 fight_encount_wipe_data_number = 26;
