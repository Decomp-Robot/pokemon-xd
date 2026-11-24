#include <types.h>
#include <hero.h>
#include <GSflag.h>

#pragma force_active on

extern "C" s32 dbgMenuSetHeroStyleSnatch(void)
{
    Hero::getHeroPtr()->setHeroStyle(HERO_STYLE_SNATCH, true);
    return 0;
}

extern "C" s32 dbgMenuSetHeroStyleVisor(void)
{
    Hero::getHeroPtr()->setHeroStyle(HERO_STYLE_VISOR, true);
    return 0;
}

extern "C" s32 dbgMenuSetHeroStyleNormal(void)
{
    Hero::getHeroPtr()->setHeroStyle(HERO_STYLE_NORMAL, true);
    return 0;
}

extern "C" s32 dbgMenuScriptFlagAllClear(void)
{
    for (u32 flag = 1; flag < flag_data_number->count; flag++) {
        GSflagOff(flag);
    }
    return 0;
}

#pragma force_active reset
