#include <types.h>
#include <floorBgmMask.h>
#include <GSres.h>

#pragma force_active on

// Debug menu entries for the people viewer, the floor sound volume display and
// the resource dump.

extern "C" s32 dbgMenuPeopleViewerRintoOnOff(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerMouthOnOff(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerEyeOnOff(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerAnim(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerPlaySpeed(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerMotion(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerChara(void)
{
    return 0;
}

extern "C" s32 dbgMenuSoundVolumeDisp(void)
{
    if (floorSound_getDispSoundVolume() == 1) {
        floorSound_setDispSoundVolume(0);
    } else {
        floorSound_setDispSoundVolume(1);
    }
    return 0;
}

extern "C" s32 dbgMenuGSresDumpGroup(void)
{
    GSresDumpAll();
    return 0;
}

#pragma force_active reset
