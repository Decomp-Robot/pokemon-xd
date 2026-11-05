#include <menuFightStatus.h>
#include <winSprite.h>
#include <GSthread.h>
#include <GSsndPlayback.h>

// Window accessors used below; declared here because window.h pulls in the
// C++-only GSgfx.h, while this translation unit is compiled as C.
extern tagWINDOW_WORK *windowSearchID(s32 id);
extern struct WINDOW_FREE_WORK *windowGetFreeWork(tagWINDOW_WORK *window);
extern void *windowGetAllocPtr(tagWINDOW_WORK *window);

// Per-window state of the fight status window. The window keeps the values it
// is currently animating towards in the area windowGetAllocPtr returns; the
// HP and EXP entries sit at the end of that area.
struct MENU_FIGHT_STATUS_ALLOC {
    u8 _00[0x18];
    s16 maxHP;
    s16 curHP;
    u32 maxEXP;
    u32 curEXP;
};

// Sound played while the EXP gauge animates; the status controller stops
// this same sound when the EXP animation finishes.
enum { MENU_SE_EXP_GAUGE_ANIM = 36 };

// Animation timers kept in the window's free work area. Each gauge records
// the value it started from, how many frames the animation takes, and how
// many frames have elapsed since it started (advanced by the status
// controller and used to interpolate the gauge).
struct MENU_FIGHT_STATUS_ANIM {
    s16 hpStart;
    s16 hpFrames;
    s16 hpElapsed;
    s16 expDone;
    u32 expStart;
    s16 expFrames;
    s16 expElapsed;
};

s32 menuFightStatusWaitAnimeEXP(u32 menuId, u8 wait)
{
    tagWINDOW_WORK *window;
    struct MENU_FIGHT_STATUS_ANIM *anim;

    for (;;) {
        window = windowSearchID(menuId);
        if (window == NULL)
            return 0;
        anim = (struct MENU_FIGHT_STATUS_ANIM *)windowGetFreeWork(window);
        if (anim->expFrames == 0)
            return 0;
        if (wait != 0) {
            GSthreadSwitch();
            continue;
        }
        return 1;
    }
}

void menuFightStatusStartAnimEXP(u32 menuId, u32 exp)
{
    tagWINDOW_WORK *window;
    struct MENU_FIGHT_STATUS_ALLOC *status;
    struct MENU_FIGHT_STATUS_ANIM *anim;
    u32 diff;

    window = windowSearchID(menuId);
    if (window == NULL)
        return;
    status = (struct MENU_FIGHT_STATUS_ALLOC *)windowGetAllocPtr(window);
    anim = (struct MENU_FIGHT_STATUS_ANIM *)windowGetFreeWork(window);
    if (exp > status->curEXP)
        diff = exp - status->curEXP;
    else
        diff = status->curEXP - exp;
    anim->expFrames = (s16)((diff * 100) / status->maxEXP);
    if (anim->expFrames < 15)
        anim->expFrames = 15;
    anim->expStart = status->curEXP;
    status->curEXP = exp;
    anim->expElapsed = 0;
    if (exp == status->maxEXP)
        anim->expDone = 1;
    GSsndPlayNow(MENU_SE_EXP_GAUGE_ANIM);
}

s32 menuFightStatusWaitAnimeHP(u32 menuId, u8 wait)
{
    tagWINDOW_WORK *window;
    struct MENU_FIGHT_STATUS_ANIM *anim;

    for (;;) {
        window = windowSearchID(menuId);
        if (window == NULL)
            return 0;
        anim = (struct MENU_FIGHT_STATUS_ANIM *)windowGetFreeWork(window);
        if (anim->hpFrames == 0)
            return 0;
        if (wait != 0) {
            GSthreadSwitch();
            continue;
        }
        return 1;
    }
}

void menuFightStatusStartAnimHP(u32 menuId, s16 hp)
{
    tagWINDOW_WORK *window;
    struct MENU_FIGHT_STATUS_ALLOC *status;
    struct MENU_FIGHT_STATUS_ANIM *anim;
    s32 diff;

    window = windowSearchID(menuId);
    if (window == NULL)
        return;
    status = (struct MENU_FIGHT_STATUS_ALLOC *)windowGetAllocPtr(window);
    anim = (struct MENU_FIGHT_STATUS_ANIM *)windowGetFreeWork(window);
    diff = status->curHP - hp;
    if (diff < 0)
        diff = -diff;
    anim->hpFrames = (s16)((diff * 100) / status->maxHP);
    if (anim->hpFrames <= 0)
        anim->hpFrames = 1;
    anim->hpStart = status->curHP;
    status->curHP = hp;
    anim->hpElapsed = 0;
}
