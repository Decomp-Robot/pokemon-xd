#include <fightAction.h>
#include <fightSeq.h>

// Action sequences used while a battle opens. Where an entry needs a target,
// the payload selects one of the two fixed sides of the battle floor.
FIGHT_ACTION_DATA_dd_ fight_action_data_kaisi[] = {
    {FIGHT_ACTION_KIND_KAIJOU, 0},
    {FIGHT_ACTION_KIND_KAISI_NYUUJOU_TRAINER, FIGHT_TARGET_ID_HOST_SIDE},
    {FIGHT_ACTION_KIND_KAISI_NYUUJOU_TRAINER, FIGHT_TARGET_ID_HOST_ENEMY_SIDE},
    {FIGHT_ACTION_KIND_KAISI_NYUUJOU_POKEMON, FIGHT_TARGET_ID_HOST_SIDE},
    {FIGHT_ACTION_KIND_KAISI_NYUUJOU_POKEMON, FIGHT_TARGET_ID_HOST_ENEMY_SIDE},
    {FIGHT_ACTION_KIND_KAISI_PRE, 0},
    {FIGHT_ACTION_KIND_KAISI_POST, 0},
    {FIGHT_ACTION_KIND_TENKOU_INIT, 0},
    {0, 0},
};
