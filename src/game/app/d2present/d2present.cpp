#include <app/d2present/d2present.h>
#include <floor.h>
#include <floorManager.h>

namespace {
const u32 D2PRESENT_FLOOR_ID = 22;
const u32 COMMON_SCRIPT_GROUP_ID = 1430;
const u32 D2PRESENT_FADE_OUT_FUNCTION = 9;
const s32 D2PRESENT_FADE_OUT_SCRIPT =
    (COMMON_SCRIPT_GROUP_ID << 16) | D2PRESENT_FADE_OUT_FUNCTION;
}

// Presentation mode selected by the debug menu.
static D2PRESENT_MODE Parameter;

extern "C" void d2presentOpen(D2PRESENT_MODE mode)
{
    Parameter = mode;
    GSfloorPush(D2PRESENT_FLOOR_ID);
    floorSetFadeScript(D2PRESENT_FADE_OUT_SCRIPT, 0);
    floorWaitForChange();
}
