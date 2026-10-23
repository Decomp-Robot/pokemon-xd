#ifndef ITEMBIOS_H
#define ITEMBIOS_H
#include <item.h>

#ifdef __cplusplus
extern "C" {
#endif
u16 itemBiosGetNum(ItemSlot *slot);
u16 itemBiosGetItemDataId(ItemSlot *slot);
#ifdef __cplusplus
}
#endif
#endif
