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

s32 menuUseItemEventUsohachiPhoto(unsigned long itemID, s32 *consumed);
s32 menuUseItemEventMirraboradar(unsigned long itemID, s32 *consumed);
s32 menuUseItemEventCraneMemo(unsigned long itemID, s32 *consumed);
s32 menuUseItemEventNotConsum(unsigned long itemID, s32 *consumed);
s32 menuUseItemBattleNazonomi(unsigned long itemID, s32 *consumed);
s32 menuUseItemBattleBall(unsigned long itemID, s32 *consumed);
s32 menuUseItemStatusUp(unsigned long itemID, s32 *consumed);
s32 menuUseItemFieldNazonomi(unsigned long itemID, s32 *consumed);
s32 menuUseItemSkill(unsigned long itemID, s32 *consumed);
s32 menuUseItemCologn(unsigned long itemID, s32 *consumed);
s32 menuUseItemEvolution(unsigned long itemID, s32 *consumed);
s32 menuUseItemWPointUp(unsigned long itemID, s32 *consumed);
s32 menuUseItemListLVUp(unsigned long itemID, s32 *consumed);
s32 menuUseItemAlldeathRcv(unsigned long itemID, s32 *consumed);
s32 menuUseItemPPRecovery(unsigned long itemID, s32 *consumed);
s32 menuUseItemRecovery(unsigned long itemID, s32 *consumed);
s32 menuUseItemNoPokecolo(unsigned long itemID, s32 *consumed);
s32 menuUseItemNoEffects(unsigned long itemID, s32 *consumed);
s32 menuUseItemDummy(unsigned long itemID, s32 *consumed);

u32 menuUseItemScriptCologn(u32 itemID, s32 slot);

#ifdef __cplusplus
}
#endif

#endif
