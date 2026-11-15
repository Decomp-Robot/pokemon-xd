#ifndef ITEMBIOS_H
#define ITEMBIOS_H

#include <item.h>
#include <itemData.h>

typedef struct TasteData TasteData;
struct TasteData {
    s32 mNigateMsgDataId;
    s8 mAisyou[25];
};

typedef struct ItemSoubiData ItemSoubiData;
struct ItemSoubiData {
    u16 mFightKoukaDataId;
};

typedef struct ItemBallData ItemBallData;
struct ItemBallData {
    u16 mFightKoukaDataId;
    u32 mInWzxDataId;
    u32 mOpenWzxDataId;
    u32 mOutWzxDataId;
    u32 mDowninWzxDataId;
    u32 mThrowWzxDataId;
    u32 mSnatchAttackWzxDataId;
    u32 mSnatchBalllandWzxDataId;
    u32 mSnatchMissWzxDataId;
    u32 mSnatchPokeoutWzxDataId;
    u32 mSnatchShakeWzxDataId;
    u32 mSnatchSnatchWzxDataId;
    u32 mYaseiBallLandWzxDataId;
    u32 mYaseiShakeWzxDataId;
    u32 mYaseiGetWzxDataId;
    u32 mYaseiPokeoutWzxDataId;
};

#ifdef __cplusplus
extern "C" {
#endif
s32 tasteDataGetAisyou(TasteData *data, u16 nature);
s32 tasteDataGetNigateMsgDataId(TasteData *data);
TasteData *tasteDataGetPtr(u16 taste);
u16 itemSoubiDataBiosGetFightKoukaDataId(ItemSoubiData *data);
ItemSoubiData *itemSoubiDataBiosGetPtr(u16 soubiID);
void itemBiosSetNum(ItemSlot *data, u16 value);
void itemBiosSetItemDataId(ItemSlot *data, u16 value);
void itemBallDataBiosSetFightKoukaDataId(ItemBallData *data, u16 value);
u16 itemBiosGetNum(ItemSlot *data);
u16 itemBiosGetItemDataId(ItemSlot *data);
u32 itemBallDataBiosGetSnatchSnatchWzxDataId(ItemBallData *data);
u32 itemBallDataBiosGetSnatchShakeWzxDataId(ItemBallData *data);
u32 itemBallDataBiosGetSnatchPokeoutWzxDataId(ItemBallData *data);
u32 itemBallDataBiosGetSnatchMissWzxDataId(ItemBallData *data);
u32 itemBallDataBiosGetSnatchBalllandWzxDataId(ItemBallData *data);
u32 itemBallDataBiosGetSnatchAttackWzxDataId(ItemBallData *data);
u32 itemBallDataBiosGetThrowWzxDataId(ItemBallData *data);
u32 itemBallDataBiosGetDowninWzxDataId(ItemBallData *data);
u32 itemBallDataBiosGetOutWzxDataId(ItemBallData *data);
u32 itemBallDataBiosGetOpenWzxDataId(ItemBallData *data);
u32 itemBallDataBiosGetInWzxDataId(ItemBallData *data);
u16 itemBallDataBiosGetFightKoukaDataId(ItemBallData *data);
ItemBallData *itemBallDataBiosGetPtr(u16 ballID);
#ifdef __cplusplus
}
#endif

#endif
