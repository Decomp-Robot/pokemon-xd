#include <GSflag.h>
#include <GSmsg.h>
#include <app/fight/fightTrainerBios.h>
#include <charNameBios.h>
#include <fightEncountBios.h>
#include <fightKindDataBios.h>
#include <fightMenu.h>
#include <fightSeq.h>
#include <fightPokemonBios.h>
#include <fightTrainer.h>
#include <fightTrainerDB.h>
#include <heroItem.h>
#include <item.h>
#include <msgctrl.h>

enum {
    HERO_ITEM_KIND_NORMAL = 2,
    FIGHT_ACTION_ITEM_KIND_MATCH = 1,
    FIGHT_ACTION_ITEM_KIND_MISMATCH = 0
};

extern "C" {
u8 fightTrainerCheckValid(FIGHT_TRAINER *trainer);
u16 fightTrainerGetSelectedItemDataId(FIGHT_TRAINER *trainer, u16 *itemDataIdAry);
FightPokemon *fightTrainerCheckTemotiPokemonFightEntry(FIGHT_TRAINER *trainer,
                                                       Pokemon *pokemon);
}

// Record the heard-name flag for the trainer in this encounter.
void fightTrainerSetNameHearFlag(u16 encountNo)
{
    u8 *encountData = fightEncountDataBiosGetPtr(encountNo);
    u8 fightKind = (u8)fightEncountDataBiosGetFightKind(encountData);
    void *fightKindData = fightKindDataBiosGetPtr(fightKind);

    if (fightKindData != 0 && fightKindDataBiosGetHostEnemyMsgFlag(fightKindData) != 0) {
        u32 trainerDataId = fightEncountDataBiosGetFightTrainerDataId(encountData, 1);

        if (fightTrainerDataBiosGetPtr(trainerDataId) != 0) {
            u16 trainerName = fightTrainerDB_GetName(trainerDataId);
            u16 nameIndex = charNameBiosSearchIndex(trainerName);

            if (nameIndex != 0) {
                u32 hearFlag = charNameBiosGetHearFlag(nameIndex);

                if (hearFlag != 0) {
                    GSflagOn(hearFlag);
                }
            }
            fightMsgctrlSetValue(MSG_VAR_CHARACTER_NAME, trainerName);
        }
    }
}

void fightTrainerToMenuBallStatus(FIGHT_TRAINER *trainer, u8 *ballStatus)
{
    Hero *hero;
    u16 slot;

    if (trainer == 0) {
        return;
    }
    if (fightTrainerCheckValid(trainer) == 0) {
        return;
    }

    hero = fightTrainer_GetHeroPtr(trainer);
    for (slot = 0; slot < 6; ++slot) {
        ballStatus[slot] = FIGHT_TRAINER_MENU_BALL_STATUS_NONE;
    }

    for (slot = 0; slot < 6; ++slot) {
        Pokemon *pokemon = (Pokemon *)heroGetStatus(hero, HERO_STATUS_POKEMON, slot);
        FightPokemon *fightPokemon;

        if (pokemonCheckValid(pokemon) == 0) {
            continue;
        }
        fightPokemon = fightTrainerCheckTemotiPokemonFightEntry(trainer, pokemon);
        if (fightPokemon == 0 || fightPokemonCheckValid(fightPokemon) == 0 ||
            fightPokemonBiosGetHokakuFlag(fightPokemon) == 1) {
            continue;
        }

        if (fightPokemonCheckFightOut(fightPokemon) == 0) {
            ballStatus[slot] = FIGHT_TRAINER_MENU_BALL_STATUS_FIGHT_OUT;
        } else if (pokemonIsJoutaiNormal(fightPokemonGetPokemonPtr(fightPokemon)) == 0) {
            ballStatus[slot] = FIGHT_TRAINER_MENU_BALL_STATUS_ABNORMAL;
        } else {
            ballStatus[slot] = FIGHT_TRAINER_MENU_BALL_STATUS_NORMAL;
        }
    }
}

u16 fightTrainerGetTemotiNormalItemDataIdAry(FIGHT_TRAINER *trainer,
                                             u16 *itemDataIdAry,
                                             u16 capacity,
                                             u8 excludeSelected)
{
    Hero *hero = fightTrainer_GetHeroPtr(trainer);
    u16 itemSlotCount;
    ItemSlot *itemSlots;
    u16 selectedItemDataIds[2];
    u16 selectedCount;
    u16 slot;
    u16 itemCount;

    if (hero == 0) {
        return 0;
    }

    itemSlots = heroItemGetItemKindToItemAryPtr(hero, HERO_ITEM_KIND_NORMAL, &itemSlotCount, 0, 0, 0);
    if (itemSlots == 0) {
        return 0;
    }

    selectedCount = fightTrainerGetSelectedItemDataId(trainer, selectedItemDataIds);
    for (slot = 0; slot < capacity; ++slot) {
        itemDataIdAry[slot] = 0;
    }

    itemCount = 0;
    for (slot = 0; slot < itemSlotCount; ++slot) {
        ItemSlot *itemSlot = &itemSlots[slot];
        u16 itemDataId;
        u16 quantity;

        if (itemCheckValid(itemSlot) == 0) {
            continue;
        }
        itemDataId = item_GetItemDataId(itemSlot);
        if (itemDataId == 0) {
            continue;
        }
        quantity = item_GetNum(itemSlot);
        if (quantity == 0) {
            continue;
        }
        if (excludeSelected == 1) {
            u16 selectedIndex;

            for (selectedIndex = 0; selectedIndex < selectedCount; ++selectedIndex) {
                if (itemDataId == selectedItemDataIds[selectedIndex] && quantity != 0) {
                    --quantity;
                }
            }
            if (quantity == 0) {
                continue;
            }
        }
        if (itemCount < capacity) {
            itemDataIdAry[itemCount] = itemDataId;
            ++itemCount;
        }
    }
    return itemCount;
}

u16 fightTrainerGetSelectedItemDataId(FIGHT_TRAINER *trainer, u16 *itemDataIdAry)
{
    FightOutPokemon *pokemon;
    u16 slot;
    u16 itemCount;

    for (slot = 0; slot < 2; ++slot) {
        itemDataIdAry[slot] = 0;
    }
    itemCount = 0;
    for (slot = 0; slot < 2; ++slot) {
        pokemon = fightTrainer_GetFightoutPokemonPtr(trainer, slot);
        u16 itemDataId;

        if (fightOutPokemonCheckFightOut(pokemon) == 0) {
            continue;
        }
        itemDataId = fightOutPokemonGetFightActionUseItemDataId(pokemon);
        if (itemDataId != 0) {
            itemDataIdAry[itemCount] = itemDataId;
            ++itemCount;
        }
    }
    return itemCount;
}

u8 fightTrainerIsUsedItem(FIGHT_TRAINER *trainer)
{
    FightOutPokemon *pokemon;
    u16 slot;

    for (slot = 0; slot < 2; ++slot) {
        pokemon = fightTrainer_GetFightoutPokemonPtr(trainer, slot);

        if (fightOutPokemonCheckFightOut(pokemon) != 0 &&
            fightOutPokemonIsFightActionUseItemKind(
                pokemon, ITEM_KIND_POKEMON_BALL, FIGHT_ACTION_ITEM_KIND_MISMATCH, 0) == 1) {
            return 1;
        }
    }
    return 0;
}

u8 fightTrainerIsSelectedItemBall(FIGHT_TRAINER *trainer)
{
    FightOutPokemon *pokemon;
    u16 slot;

    for (slot = 0; slot < 2; ++slot) {
        pokemon = fightTrainer_GetFightoutPokemonPtr(trainer, slot);

        if (fightOutPokemonCheckFightOut(pokemon) != 0 &&
            fightOutPokemonIsFightActionUseItemKind(
                pokemon, ITEM_KIND_POKEMON_BALL, FIGHT_ACTION_ITEM_KIND_MATCH, 0) == 1) {
            return 1;
        }
    }
    return 0;
}

FightPokemon *fightTrainerSearchInvalidFightPokemonPtr(FIGHT_TRAINER *trainer,
                                                       u16 entryPokemonNum)
{
    u16 slot;

    if (trainer == 0) {
        return 0;
    }
    for (slot = 0; slot < entryPokemonNum; ++slot) {
        FightPokemon *pokemon = fightTrainer_GetFightPokemonPtr(trainer, slot);

        if (fightPokemonCheckValid(pokemon) == 0) {
            return pokemon;
        }
    }
    return 0;
}

u16 *fightTrainerGetPrefixNamePtr(FIGHT_TRAINER *trainer)
{
    u16 trainerDataId;
    u8 trainerKind;
    void *trainerKindData;

    if (trainer == 0) {
        return 0;
    }
    if (fightTrainerCheckValid(trainer) == 0) {
        return 0;
    }
    trainerDataId = fightTrainer_GetFightTrainerDataId(trainer);
    trainerKind = fightTrainerDB_GetKindDataId(trainerDataId);
    trainerKindData = fightTrainerKindDataBiosGetPtr(trainerKind);
    return GSmsgGetStringPtr(fightTrainerKindDataBiosGetPrefixName(trainerKindData));
}

u16 *fightTrainerGetNamePtr(FIGHT_TRAINER *trainer)
{
    Hero *hero;

    if (trainer == 0) {
        return 0;
    }
    if (fightTrainerCheckValid(trainer) == 0) {
        return 0;
    }
    hero = fightTrainer_GetHeroPtr(trainer);
    if (hero == 0) {
        return 0;
    }
    return (u16 *)heroGetStatus(hero, HERO_STATUS_NAME, 0);
}
