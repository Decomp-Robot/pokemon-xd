#ifndef ITEMBIOS_H
#define ITEMBIOS_H

#include <item.h>

typedef struct TasteData TasteData;
typedef struct ItemBallData ItemBallData;
typedef struct ItemSoubiData ItemSoubiData;

#ifdef __cplusplus
extern "C" {
#endif

u16 itemBiosGetNum(ItemSlot *slot);
u16 itemBiosGetItemDataId(ItemSlot *slot);
TasteData *tasteDataGetPtr(u16 taste);
s32 tasteDataGetNigateMsgDataId(TasteData *taste);
ItemBallData* itemBallDataBiosGetPtr(u16 itemID);
u16 itemBallDataBiosGetFightKoukaDataId(ItemBallData* data);
u32 itemBallDataBiosGetInWzxDataId(ItemBallData* data);
u32 itemBallDataBiosGetOpenWzxDataId(ItemBallData* data);
u32 itemBallDataBiosGetOutWzxDataId(ItemBallData* data);
u32 itemBallDataBiosGetDowninWzxDataId(ItemBallData* data);
u32 itemBallDataBiosGetThrowWzxDataId(ItemBallData* data);
u32 itemBallDataBiosGetSnatchAttackWzxDataId(ItemBallData* data);
u32 itemBallDataBiosGetSnatchBalllandWzxDataId(ItemBallData* data);
u32 itemBallDataBiosGetSnatchMissWzxDataId(ItemBallData* data);
u32 itemBallDataBiosGetSnatchPokeoutWzxDataId(ItemBallData* data);
u32 itemBallDataBiosGetSnatchShakeWzxDataId(ItemBallData* data);
u32 itemBallDataBiosGetSnatchSnatchWzxDataId(ItemBallData* data);
ItemSoubiData* itemSoubiDataBiosGetPtr(u16 itemID);
u16 itemSoubiDataBiosGetFightKoukaDataId(ItemSoubiData* data);

#ifdef __cplusplus
}
#endif

#endif
