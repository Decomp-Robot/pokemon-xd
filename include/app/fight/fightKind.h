#ifndef FIGHT_KIND_H
#define FIGHT_KIND_H

#include <types.h>

enum FightKindId {
    FIGHT_KIND_ID_UNK_0,
    FIGHT_KIND_ID_UNK_1,
    FIGHT_KIND_ID_UNK_2,
    FIGHT_KIND_ID_UNK_3,
    FIGHT_KIND_ID_UNK_4,
    FIGHT_KIND_ID_UNK_5,
    FIGHT_KIND_ID_MENU_LINK_SINGLE,
    FIGHT_KIND_ID_MENU_LINK_MULTI,
    FIGHT_KIND_ID_UNK_8,
    FIGHT_KIND_ID_UNK_9,
    FIGHT_KIND_ID_VS_DISTINCT_TRAINERS,
    FIGHT_KIND_ID_LINK,
    FIGHT_KIND_ID_WILD_POKEMON,
    FIGHT_KIND_ID_BATTLE_BINGO,
    FIGHT_KIND_ID_VIRTUAL_TUTORIAL,
    FIGHT_KIND_ID_BATTLE_DISK,
    FIGHT_KIND_ID_UNK_16,
    FIGHT_KIND_ID_VS_FIGHT
};

#ifdef __cplusplus
extern "C" {
#endif

// Reports whether the current fight belongs to the given fight kind.
u8 fightKind_CheckFightKind(u8 kind);

#ifdef __cplusplus
}
#endif

#endif
