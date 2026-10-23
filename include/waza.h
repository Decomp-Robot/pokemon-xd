#ifndef WAZA_H
#define WAZA_H

#include <types.h>
typedef struct FIGHT_WAZA FIGHT_WAZA;

enum WAZA_FIELD {
    WAZA_FIELD_NAME = 1,
    WAZA_FIELD_PP = 2,
    WAZA_FIELD_TYPE = 3,
    WAZA_FIELD_ACCURACY = 6,
    WAZA_FIELD_POWER = 7,
    WAZA_FIELD_HIDEN_FLAG = 0x19,
    WAZA_FIELD_DESCRIPTION = 0x22,
};

enum WAZA_SHADOW {
    WAZA_SHADOW_FIRST = 0x164,
    WAZA_SHADOW_LAST = 0x175,
    WAZA_SHADOW_SPECIAL = 0x176,
    WAZA_SHADOW_SPECIAL_ICON = 0x7B,
};

enum {
    WAZA_STATUS_NAME_MESSAGE_ID = WAZA_FIELD_NAME,
    WAZA_STATUS_TYPE_ID = WAZA_FIELD_TYPE
};

enum WAZA_COMPATIBILITY {
    WAZA_COMPATIBILITY_UNDISPLAYED = 0,
    WAZA_COMPATIBILITY_TYPE_CHART = 1,
    WAZA_COMPATIBILITY_UNDISPLAYED_ALTERNATE = 2,
    WAZA_COMPATIBILITY_UNDISPLAYED_OTHER = 3,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GHOST = 4,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GRASS = 5,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_POISON_OR_STEEL = 6,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GROUND = 7,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_FLYING = 8,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_NORMAL = 9,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY = 10,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY_ALTERNATE = 11,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_DARK = 12,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_FIRE = 13,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY_OTHER = 14,
    WAZA_COMPATIBILITY_ALL_TYPES_NORMAL = 15,
};

enum { WAZA_ID_MASK = 0xFFFF };

enum { WAZA_STATUS_TYPE = 0x1A };

struct WazaData {
    u32 _00[0x20 / 4];
    u32 mNameID;
    u32 _24[(0x38 - 0x24) / 4];
};

#ifdef __cplusplus
extern "C" {
#endif

struct WazaData *wazaDataBiosGetPtr(u16 wazaID);
u32 wazaDataBiosGetName(struct WazaData *data);
u32 wazaGetWazaTypeIdName(u8 typeId);

s32 wazaGetStatus(void *data, u16 wazaID, u16 fieldID, s32 index);
void wazaSetStatus(void *data, u16 wazaID, u16 fieldID, s32 index, s32 value);

u8 wazaDB_GetCompatible(u16 wazaID);
s16 wazaDB_GetIryoku(u16 move);
s32 fightWaza_GetDamage(FIGHT_WAZA *waza);
void fightWaza_SetDamage(FIGHT_WAZA *waza, s32 damage);
u16 wazaDB_GetSeqId(u16 move);
u8 wazaDB_GetOujanosirusi(u16 move);
s32 wazaDB_GetName(u16 move);
#ifdef __cplusplus
}
#endif

#endif
