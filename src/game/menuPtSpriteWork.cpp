#include <menuPtSpriteWork.h>

tagSPRITE_WORK *menuPtGetSpriteWorkByHookFunction(tagWINDOW_WORK *window, void *hook, unsigned long index, long offset)
{
    tagSPRITE_WORK *work = window->mSpriteWork;

    while (work != 0) {
        if (work->mHookFunction == hook && index-- == 0) {
            break;
        }
        work = work->mNext;
    }

    while (offset != 0 && work != 0) {
        offset--;
        work = work->mNext;
    }

    return work;
}
