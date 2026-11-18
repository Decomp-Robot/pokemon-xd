#include <GSflag.h>
#include <GSmsg.h>
#include <app/fight/fightTrainer.h>
#include <app/fight/fightTrainerBios.h>
#include <app/fight/fightTrainerEnemy.h>
#include <charNameBios.h>
#include <fightEncountBios.h>
#include <fightKindDataBios.h>
#include <fightMenu.h>
#include <fightSeq.h>
#include <fightTarget.h>
#include <fightPokemonBios.h>
#include <fightTrainer.h>
#include <fightTrainerDB.h>
#include <heroItem.h>
#include <item.h>
#include <msgctrl.h>
#include <pokemonDB.h>

enum {
    HERO_ITEM_KIND_NORMAL = 2,
    FIGHT_ACTION_ITEM_KIND_MATCH = 1,
    FIGHT_ACTION_ITEM_KIND_MISMATCH = 0,
};

static const u16 FIGHT_TRAINER_FIGHT_POKEMON_NUM = 6;
static const u16 FIGHT_TRAINER_FIGHTOUT_POKEMON_NUM = 2;
static const u8 FIGHT_TRAINER_ENEMY_POKEMON_NUM = 12;

extern "C" {
u8 fightTrainerCheckValid(FIGHT_TRAINER *trainer);
u16 fightTrainerGetSelectedItemDataId(FIGHT_TRAINER *trainer, u16 *itemDataIdAry);
FightPokemon *fightTrainerCheckTemotiPokemonFightEntry(FIGHT_TRAINER *trainer,
                                                       Pokemon *pokemon);
}

// Record the heard-name flag for the trainer in this encounter.
void fightTrainerSetNameHearFlag(u16 encountNo)
{
    FightEncountRecord *encountData = fightEncountDataBiosGetPtr(encountNo);
    u8 fightKind = (u8)fightEncountDataBiosGetFightKind(encountData);
    FIGHT_KIND *fightKindData = fightKindDataBiosGetPtr(fightKind);

    if (fightKindData != 0 && fightKindDataBiosGetHostEnemyMsgFlag(fightKindData) != 0) {
        u16 trainerDataId = fightEncountDataBiosGetFightTrainerDataId(encountData, 1);

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

// Reserve Pokemon left after accounting for the number the fight wants on the
// field; Pokemon that cannot fight are ignored. A reserve is spent for each
// missing fight-out Pokemon.
s32 fightTrainerGetHikaeFightPokemonNum(
    FIGHT_TRAINER *trainer, u16 pokemonNum, u16 fightoutPokemonNum)
{
    u16 slot;
    s32 hikaeNum;
    u16 fightoutNum;

    if (trainer == 0) {
        return 0;
    }
    hikaeNum = 0;
    fightoutNum = 0;
    for (slot = 0; slot < pokemonNum; slot++) {
        FightPokemon *pokemon = fightTrainerGetValidFightPokemonPtr(trainer, slot);
        u8 state;

        if (pokemon == 0) {
            continue;
        }
        state = fightTrainerCheckCanIrekaeFightPokemon(trainer, pokemon);
        if (state == FIGHT_TRAINER_IREKAE_CANNOT_FIGHT) {
            continue;
        }
        if (state == FIGHT_TRAINER_IREKAE_FIGHT_OUT) {
            fightoutNum++;
        } else {
            hikaeNum++;
        }
    }
    for (;;) {
        if (fightoutNum >= fightoutPokemonNum || (u16)hikaeNum < 1) {
            break;
        }
        hikaeNum--;
        fightoutNum++;
    }
    return hikaeNum;
}

// Whether the target stands on the same side as the trainer.
u8 fightTrainerIsAllyFightTargetPtr(FIGHT_TRAINER *trainer, FightOutPokemon *target, u16 fightType)
{
    void *trainerSide;
    void *targetSide;

    if (trainer == 0) {
        return 0;
    }
    if (target == 0) {
        return 0;
    }
    trainerSide = fightTargetGetPtr(FIGHT_TARGET_ID_OWN_SIDE, trainer, fightType);
    targetSide = fightTargetGetPtr(FIGHT_TARGET_ID_OWN_SIDE, target, fightType);
    return trainerSide == targetSide;
}

// Resets the fight-out status of the trainer's enemy-status records; when
// validOnly is set, records without a Pokemon entry are left alone.
void fightTrainerInitEnemyPokemonFightOutStatus(FIGHT_TRAINER *trainer, u8 validOnly)
{
    FightTrainerEnemyPokemon *enemyPokemon;
    u8 index;

    if (trainer == 0) {
        return;
    }
    for (index = 0; index < FIGHT_TRAINER_ENEMY_POKEMON_NUM; index++) {
        enemyPokemon = fightTrainer_GetFightTrainerEnemyPokemonAryPtr(trainer, index);

        if (validOnly == 1 && fightTrainerEnemyPokemonCheckValid(enemyPokemon) == 0) {
            continue;
        }
        fightTrainerEnemyPokemonInitFightOutStatus(enemyPokemon);
    }
}

// The fight-out Pokemon in a slot with an action buffer but no valid action.
FightOutPokemon *fightTrainerGetNoActionFightOutPokemonPtr(FIGHT_TRAINER *trainer, u16 slot)
{
    FightOutPokemon *pokemon = fightTrainerGetValidFightOutPokemonPtr(trainer, slot);
    FIGHT_ACTION *action;

    if (pokemon == 0) {
        return 0;
    }
    action = fightOutPokemon_GetFightActionBuffPtr(pokemon);
    if (action == 0) {
        return 0;
    }
    if (fightActionCheckValid(action) == 1) {
        return 0;
    }
    return pokemon;
}

// Fight-out slot holding the given Pokemon, or -1 if it is not one of this
// trainer's fight-out Pokemon.
s16 fightTrainerGetFightOutPokemonToTemotiBanme(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon)
{
    s16 slot;

    if (trainer == 0) {
        return -1;
    }
    if (pokemon == 0) {
        return -1;
    }
    for (slot = 0; slot < FIGHT_TRAINER_FIGHTOUT_POKEMON_NUM; slot++) {
        if (pokemon == fightTrainer_GetFightoutPokemonPtr(trainer, slot)) {
            return slot;
        }
    }
    return -1;
}

// Party slot holding the given fight Pokemon, or -1 if the trainer does not
// hold it.
s16 fightTrainerGetFightPokemonToTemotiBanme(FIGHT_TRAINER *trainer, FightPokemon *pokemon)
{
    s16 slot;

    if (trainer == 0) {
        return -1;
    }
    if (pokemon == 0) {
        return -1;
    }
    for (slot = 0; slot < FIGHT_TRAINER_FIGHT_POKEMON_NUM; slot++) {
        if (pokemon == fightTrainer_GetFightPokemonPtr(trainer, slot)) {
            return slot;
        }
    }
    return -1;
}

// Copies the working fight copy of every party Pokemon back over the original
// Pokemon; when setItem is set, the held item is written back explicitly.
void fightTrainerBackFightPokemonToTemotiPokemon(FIGHT_TRAINER *trainer, u8 setItem)
{
    u16 slot;

    if (trainer == 0) {
        return;
    }
    for (slot = 0; slot < FIGHT_TRAINER_FIGHT_POKEMON_NUM; slot++) {
        FightPokemon *fightPokemon = fightTrainerGetValidFightPokemonPtr(trainer, slot);
        Pokemon *pokemon;
        u16 itemDataId;
        Pokemon *motoPokemon;

        if (fightPokemon == 0) {
            continue;
        }
        pokemon = fightPokemon_GetPokemonBuffPtr(fightPokemon);
        if (pokemon == 0) {
            continue;
        }
        itemDataId = pokemonGetSoubiItemDataId(pokemon);
        motoPokemon = fightPokemon_GetMotoPokemonPtr(fightPokemon);
        if (motoPokemon == 0) {
            continue;
        }
        pokemonBiosCopy(motoPokemon, pokemon);
        if (pokemonIsJoutaiDataId(motoPokemon, JOUTAI_BAD_POISON) == 1) {
            pokemonSetCountAsJoutaiDataId(motoPokemon, JOUTAI_BAD_POISON, 1);
        }
        if (setItem == 1) {
            pokemon_SetItemDataId(motoPokemon, itemDataId);
        }
    }
}

// Swappable party Pokemon in party order; returns how many were stored.
u16 fightTrainerGetIrekaeFightPokemonAry(FIGHT_TRAINER *trainer, FightPokemon **pokemonAry)
{
    u16 index;
    u16 count;
    FightPokemon *pokemon;

    for (index = 0; index < FIGHT_TRAINER_FIGHT_POKEMON_NUM; index++) {
        pokemonAry[index] = 0;
    }
    count = 0;
    index = 0;
    while (1) {
        pokemon = fightTrainerSearchIrekaeFightPokemon(trainer, &index);
        if (pokemon == 0) {
            break;
        }
        pokemonAry[count++] = pokemon;
        index++;
    }
    return count;
}

// First swappable party Pokemon at or after the search cursor; the cursor is
// left on the Pokemon found, so the caller advances it before searching again.
FightPokemon *fightTrainerSearchIrekaeFightPokemon(FIGHT_TRAINER *trainer, u16 *index)
{
    FightPokemon *pokemon;

    if (trainer == 0) {
        return 0;
    }
    if (index == 0) {
        return 0;
    }
    while (*index < FIGHT_TRAINER_FIGHT_POKEMON_NUM) {
        pokemon = fightTrainer_GetFightPokemonPtr(trainer, *index);
        if (fightTrainerCheckCanIrekaeFightPokemon(trainer, pokemon) == FIGHT_TRAINER_IREKAE_SWAPPABLE) {
            break;
        }
        (*index)++;
    }
    if (*index >= FIGHT_TRAINER_FIGHT_POKEMON_NUM) {
        return 0;
    }
    return pokemon;
}

// Whether this party Pokemon can be selected to switch in: cannot fight, already
// out, held back by a reservation, or available.
u8 fightTrainerCheckCanIrekaeFightPokemon(FIGHT_TRAINER *trainer, FightPokemon *pokemon)
{
    if (fightPokemonCheckFightOut(pokemon) == 0) {
        return FIGHT_TRAINER_IREKAE_CANNOT_FIGHT;
    }
    if (fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(trainer, pokemon) != 0) {
        return FIGHT_TRAINER_IREKAE_FIGHT_OUT;
    }
    if (fightTrainerCheckIrekaeReserveFightPokemon(trainer, pokemon) == 1) {
        return FIGHT_TRAINER_IREKAE_REFUSED;
    }
    return FIGHT_TRAINER_IREKAE_SWAPPABLE;
}

// Whether a fight-out Pokemon has reserved this party Pokemon as its
// scheduled switch-in.
u8 fightTrainerCheckIrekaeReserveFightPokemon(FIGHT_TRAINER *trainer, FightPokemon *pokemon)
{
    u16 slot;

    if (trainer == 0) {
        return 0;
    }
    if (pokemon == 0) {
        return 0;
    }
    for (slot = 0; slot < FIGHT_TRAINER_FIGHTOUT_POKEMON_NUM; slot++) {
        FightOutPokemon *fightoutPokemon = fightTrainerGetValidFightOutPokemonPtr(trainer, slot);

        if (fightoutPokemon != 0 && fightOutPokemonCheckIrekaeReserveFightPokemon(fightoutPokemon, pokemon) == 1) {
            return 1;
        }
    }
    return 0;
}

// Fight-out Pokemon that the party Pokemon was sent out as.
FightOutPokemon *fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(FIGHT_TRAINER *trainer,
                                                                  FightPokemon *pokemon)
{
    u16 slot;

    if (trainer == 0) {
        return 0;
    }
    if (pokemon == 0) {
        return 0;
    }
    for (slot = 0; slot < FIGHT_TRAINER_FIGHTOUT_POKEMON_NUM; slot++) {
        FightOutPokemon *fightoutPokemon = fightTrainerGetValidFightOutPokemonPtr(trainer, slot);

        if (fightoutPokemon != 0 && fightPokemonCheckMotoFightPokemon(pokemon, fightoutPokemon) == 1) {
            return fightoutPokemon;
        }
    }
    return 0;
}

// Fight-out Pokemon currently on the field for this party Pokemon.
FightOutPokemon *fightTrainerCheckFightPokemonFightOut(FIGHT_TRAINER *trainer, FightPokemon *pokemon)
{
    FightOutPokemon *fightoutPokemon;

    if (trainer == 0) {
        return 0;
    }
    if (pokemon == 0) {
        return 0;
    }
    fightoutPokemon = fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(trainer, pokemon);
    if (fightoutPokemon != 0 && fightOutPokemonCheckFightOut(fightoutPokemon) == 0) {
        fightoutPokemon = 0;
    }
    return fightoutPokemon;
}

// Party Pokemon booked with the given fight entry, or 0 when none matches.
FightPokemon *fightTrainerGetEntryIdToFightPokemonPtr(FIGHT_TRAINER *trainer, s16 entryId)
{
    u16 slot;

    if (entryId < 0) {
        return 0;
    }
    for (slot = 0; slot < FIGHT_TRAINER_FIGHT_POKEMON_NUM; slot++) {
        FightPokemon *pokemon = fightTrainerGetValidFightPokemonPtr(trainer, slot);

        if (pokemon != 0 && fightPokemon_GetEntryId(pokemon) == entryId) {
            return pokemon;
        }
    }
    return 0;
}
