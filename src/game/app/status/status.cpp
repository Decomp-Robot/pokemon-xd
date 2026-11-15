#include <status.h>
#include <gamedata.h>
#include <app/pcbox/pcbox.h>
#include <hero.h>
#include <item.h>
#include <Pokemon.h>
#include <waza.h>
#include <fightFloor.h>
#include <fightSide.h>
#include <fightTrainer.h>

s32 statusSetStatus(u8 statusKind, void* status, u16 dataId, u16 fieldId, u16 index, s32 value) {
    s32 result = 0;
    switch (statusKind) {
    case STATUS_KIND_GAMEDATA:
        gamedataSetStatus(status, fieldId, value);
        break;
    case STATUS_KIND_PCBOX:
        pcboxSetStatus(status, fieldId, value);
        break;
    case STATUS_KIND_HERO:
        heroSetStatus((Hero*)status, fieldId, value);
        break;
    case STATUS_KIND_ITEM:
        itemSetStatus((ItemSlot*)status, dataId, fieldId, index, value);
        break;
    case STATUS_KIND_POKEMON:
        pokemonSetStatus(status, dataId, fieldId, index, value);
        break;
    case STATUS_KIND_WAZA:
        wazaSetStatus(status, dataId, fieldId, index, value);
        break;
    case STATUS_KIND_FIGHT_FLOOR:
        result = fightFloorSetStatus((FIGHT_FLOOR*)status, dataId, fieldId, index, value);
        break;
    case STATUS_KIND_FIGHT_SIDE:
        fightSideSetStatus((FIGHT_SIDE*)status, dataId, fieldId, index, value);
        break;
    case STATUS_KIND_FIGHT_TRAINER:
        fightTrainerSetStatus((FIGHT_TRAINER*)status, dataId, fieldId, index, value);
        break;
    }
    return result;
}

s32 statusGetStatus(u8 statusKind, void* status, u16 dataId, u16 fieldId, u16 index) {
    switch (statusKind) {
    case STATUS_KIND_NONE:
        return 0;
    case STATUS_KIND_GAMEDATA:
        return (s32)gamedataGetStatus(status, fieldId);
    case STATUS_KIND_PCBOX:
        return pcboxGetStatus(status, fieldId, index);
    case STATUS_KIND_HERO:
        return heroGetStatus((Hero*)status, fieldId, index);
    case STATUS_KIND_ITEM:
        return itemGetStatus((ItemSlot*)status, dataId, fieldId, index);
    case STATUS_KIND_POKEMON:
        return pokemonGetStatus(status, dataId, fieldId, index);
    case STATUS_KIND_WAZA:
        return wazaGetStatus(status, dataId, fieldId, index);
    case STATUS_KIND_FIGHT_FLOOR:
        return fightFloorGetStatus((FIGHT_FLOOR*)status, dataId, fieldId, index);
    case STATUS_KIND_FIGHT_SIDE:
        return fightSideGetStatus((FIGHT_SIDE*)status, dataId, fieldId, index);
    case STATUS_KIND_FIGHT_TRAINER:
        return fightTrainerGetStatus((FIGHT_TRAINER*)status, dataId, fieldId, index);
    }
    return 0;
}
