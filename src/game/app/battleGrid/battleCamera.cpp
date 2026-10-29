/*
 * battleCamera.cpp
 *
 * State for the battle camera and the switches the battle code uses to pick
 * between the full and the simplified camera.
 */

#include <types.h>
#include <wazaSequence.h>

/* Waza sequence played by the waza camera, null while no waza camera runs. */
WazaSequence* gActiveWazaSequenceCamera;

/* Zero while the battle camera runs the full camera, one for the simplified one. */
u8 gBattleCameraSimpleMode;

/* Camera state the update, stop and start paths switch on. */
u8 gBattleCameraState;

/* Whether the battle camera runs on the current floor. */
u8 gBattleCameraEnabled;

extern "C" void battleCameraDisable(void)
{
    gBattleCameraEnabled = 0;
}

extern "C" u8 battleCameraIsSimple(void)
{
    return gBattleCameraSimpleMode;
}

extern "C" void battleCameraDoFull(void)
{
    gBattleCameraSimpleMode = 0;
}

extern "C" void battleCameraDoSimple(void)
{
    gBattleCameraSimpleMode = 1;
}
