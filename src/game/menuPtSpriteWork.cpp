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

tagMENU_ITEM_DATA *menuPtItemGetBySpriteWork(tagWINDOW_WORK *window, tagSPRITE_WORK *work)
{
    return menuItemBiosGetPtr(window->mMenuID, work->mItemID);
}

unsigned long menuPtSpriteWorkGetIndexFromId(tagWINDOW_WORK *window, tagSPRITE_WORK *work, unsigned long itemID)
{
    tagSPRITE_WORK *sprite = menuPtSpriteWorkGetByItemID(window, itemID, 0);
    unsigned long index = 0;

    while (1) {
        if (sprite == work) {
            break;
        }
        sprite = sprite->mNext;
        index++;
    }

    return index;
}

tagSPRITE_WORK *menuPtSpriteWorkGetByItemID(tagWINDOW_WORK *window, unsigned long itemID, long offset)
{
    tagSPRITE_WORK *work = window->mSpriteWork;

    while (work != 0) {
        if (work->mItemID == itemID) {
            break;
        }
        work = work->mNext;
    }

    while (offset > 0) {
        work = work->mNext;
        offset--;
    }

    return work;
}
