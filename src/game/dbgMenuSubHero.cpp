#include <types.h>
#include <hero.h>

#pragma force_active on

// Debug menu entries that switch the hero model style.

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

#pragma force_active reset
