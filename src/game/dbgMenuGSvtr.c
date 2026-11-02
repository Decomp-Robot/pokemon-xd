#include <types.h>
#include <GSinput.h>
#include <GSAPI/GSvtr/GSvtr.h>
#include <GSgfxC.h>
#include <sound.h>

#pragma force_active on

enum {
    /* Z trigger press toggles the VTR movie capture. */
    GSVTR_PAD_TRIGGER_Z = 0x10
};

static u8 sCaptureActive; /* VTR movie capture recording state */
static u8 sInfoFrame;     /* GSvtr info-display frame value; capture arms while set */

void dbgMenuGSvtrCaptureUpdate(void)
{
    if ((GSinputButtonsPressed(GS_INPUT_PORT_1) & GSinputButtonsChanged(GS_INPUT_PORT_1)) & GSVTR_PAD_TRIGGER_Z) {
        switch (sCaptureActive) {
        case 0:
            if (sInfoFrame != 0) {
                soundOpenPlay(SOUND_SILENT_BGM, 0, 0);
                GSgfxCaptureMovieStart(18000);
                sInfoFrame = 0;
                sCaptureActive = 1;
            }
            break;

        case 1:
            GSgfxCaptureMovieStop();
            sCaptureActive = 0;
            break;
        }
    }
}

s32 dbgMenuGSvtrCaptureSD(void)
{
    GSgfxCaptureSetTarget(GSGFX_CAPTURE_TARGET_SD);
    sInfoFrame = 1;
    GSvtrSetInfoDisp(GSvtrGetInfoDisp(), 1);

    return 0;
}

s32 dbgMenuGSvtrCaptureBBA(void)
{
    GSgfxCaptureSetTarget(GSGFX_CAPTURE_TARGET_BBA);
    sInfoFrame = 1;
    GSvtrSetInfoDisp(GSvtrGetInfoDisp(), 1);

    return 0;
}

s32 dbgMenuGSvtrSlow(void)
{
    GSvtrSlowPlay();

    return 0;
}

s32 dbgMenuGSvtrStep(void)
{
    GSvtrSingleStep();

    return 0;
}

s32 dbgMenuGSvtrPause(void)
{
    GSvtrPause();

    return 0;
}

s32 dbgMenuGSvtrPlay(void)
{
    GSvtrPlay();

    return 0;
}

s32 dbgMenuGSvtrChangeDisp(void)
{
    u8 disp = GSvtrGetInfoDisp();

    GSvtrSetInfoDisp(!disp, sInfoFrame);

    return 0;
}

s32 dbgMenuGSvtrChangeActive(void)
{
    u8 active = GSvtrIsActive();

    if (active == 0) {
        GSvtrEnable();
    } else {
        GSvtrDisable();
    }

    return 0;
}

#pragma force_active reset
