#ifndef MENU_USE_ITEM_H
#define MENU_USE_ITEM_H

#include <types.h>

#include <itemData.h>

// Item dispatch results; the consumed count is returned separately.
enum MenuItemUseResult {
    ITEM_USE_APPLIED = 0,
    ITEM_USE_REFUSED = 1,
    ITEM_USE_EVENT = 2 // event use: the pocket decides whether to stay open.
};

// Routine selected by the item data; reports the number of items consumed.
typedef PocketItemUseFunc MenuItemUseFunc;

#ifdef __cplusplus
extern "C" {
#endif

int menuUseItemEventUsohachiPhoto(unsigned long itemID, int *consumed);
int menuUseItemEventMirraboradar(unsigned long itemID, int *consumed);
int menuUseItemEventCraneMemo(unsigned long itemID, int *consumed);
int menuUseItemEventNotConsum(unsigned long itemID, int *consumed);
int menuUseItemBattleNazonomi(unsigned long itemID, int *consumed);
int menuUseItemBattleBall(unsigned long itemID, int *consumed);
int menuUseItemStatusUp(unsigned long itemID, int *consumed);
int menuUseItemFieldNazonomi(unsigned long itemID, int *consumed);
int menuUseItemSkill(unsigned long itemID, int *consumed);
int menuUseItemCologn(unsigned long itemID, int *consumed);
int menuUseItemEvolution(unsigned long itemID, int *consumed);
int menuUseItemWPointUp(unsigned long itemID, int *consumed);
int menuUseItemListLVUp(unsigned long itemID, int *consumed);
int menuUseItemAlldeathRcv(unsigned long itemID, int *consumed);
int menuUseItemPPRecovery(unsigned long itemID, int *consumed);
int menuUseItemRecovery(unsigned long itemID, int *consumed);
int menuUseItemNoPokecolo(unsigned long itemID, int *consumed);
int menuUseItemNoEffects(unsigned long itemID, int *consumed);
int menuUseItemDummy(unsigned long itemID, int *consumed);

u32 menuUseItemScriptCologn(u32 itemID, s32 slot);

#ifdef __cplusplus
}
#endif

#endif
