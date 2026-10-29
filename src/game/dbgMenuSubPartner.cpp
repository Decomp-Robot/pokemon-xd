#include <hero.h>
#include <light.h>
#include <types.h>

#pragma force_active on

// Debug menu entries that bring the partner trainers in and out, and the
// debug light view toggle.

extern "C" s32 dbgMenuKleinPTin(void)
{
    heroSetFollowerWithID(3, 0);
    return 0;
}

extern "C" s32 dbgMenuGirlPTin(void)
{
    heroSetFollowerWithID(2, 0);
    return 0;
}

extern "C" s32 dbgMenuManaPTin(void)
{
    heroSetFollowerWithID(1, 0);
    return 0;
}

extern "C" s32 dbgMenuPTout(void)
{
    heroSetFollowerWithID(0, 0);
    return 0;
}

extern "C" s32 dbgMenuScriptLightEdt(void)
{
    if (lightIsDebugMode() == 1) {
        lightSetDebugMode(0);
    } else {
        lightSetDebugMode(1);
    }
    return 0;
}

#pragma force_active reset
