#ifndef ITEMBIOS_H
#define ITEMBIOS_H
#include <item.h>

typedef struct TasteData TasteData;

#ifdef __cplusplus
extern "C" {
#endif
u16 itemBiosGetNum(ItemSlot *slot);
u16 itemBiosGetItemDataId(ItemSlot *slot);
TasteData *tasteDataGetPtr(u16 taste);
s32 tasteDataGetNigateMsgDataId(TasteData *taste);
#ifdef __cplusplus
}
#endif
#endif
