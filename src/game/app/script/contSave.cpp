#include <script.h>
#include <menuSub.h>
#include <floorManager.h>
#include <winMsg.h>

enum ContSaveStep {
    CONT_SAVE_STEP_PROMPT,
    CONT_SAVE_STEP_SAVE,
    CONT_SAVE_STEP_RESUME,
    CONT_SAVE_STEP_EXIT_FLOOR,
    CONT_SAVE_STEP_FINISHED
};

static s32 _coutineSave(void)
{
    ContSaveStep step = CONT_SAVE_STEP_PROMPT;
    s32 finished = 0;

    while (!finished) {
        switch (step) {
        case CONT_SAVE_STEP_PROMPT: {
            winMsgOpen(WINMSG_WINDOW_MENU, WINMSG_MESSAGE_CONTINUE_SAVE, true, true);
            s32 answer = (s8)menuSubOpenYesNoField();
            winMsgClose(true);
            if (answer != 0) {
                finished = 1;
            } else {
                step = CONT_SAVE_STEP_RESUME;
            }
            break;
        }
        case CONT_SAVE_STEP_RESUME:
            break;
        case CONT_SAVE_STEP_EXIT_FLOOR:
            GSfloorChange(FLOOR_ID_VS_LOBBY);
            step = CONT_SAVE_STEP_FINISHED;
            break;
        case CONT_SAVE_STEP_FINISHED:
            finished = 1;
            break;
        default:
            break;
        }
    }

    return 0;
}

long scriptBattleMountainEnd(void)
{
    GSfloorChange(FLOOR_ID_VS_LOBBY);
    return 0;
}

long scriptContinueSave(void)
{
    return _coutineSave();
}
