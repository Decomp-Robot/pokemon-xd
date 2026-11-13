#include <types.h>
#include <floorBgmMask.h>
#include <GSres.h>
#include <gamedata.h>
#include <hero.h>
#include <itemData.h>
#include <memo.h>
#include <Pokemon.h>
#include <savedata.h>

#pragma force_active on

// Debug menu entries for the people viewer, the floor sound volume display,
// the resource dump and the starter kit.

extern "C" s32 dbgMenuPeopleViewerRintoOnOff(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerMouthOnOff(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerEyeOnOff(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerAnim(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerPlaySpeed(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerMotion(void)
{
    return 0;
}

extern "C" s32 dbgMenuPeopleViewerChara(void)
{
    return 0;
}

extern "C" s32 dbgMenuSoundVolumeDisp(void)
{
    if (floorSound_getDispSoundVolume() == 1) {
        floorSound_setDispSoundVolume(0);
    } else {
        floorSound_setDispSoundVolume(1);
    }
    return 0;
}

extern "C" s32 dbgMenuGSresDumpGroup(void)
{
    GSresDumpAll();
    return 0;
}

// Starter kit for the current save: two party pokemon, four shadow pokemon and
// the item stacks below. The first two pokemon are also registered with the
// memo; the shadow ones are only caught, each tied to one dark pokemon record
// of the save.
extern "C" s32 dbgMenuDbgPokemonGet(void)
{
    enum {
        DEBUG_KIT_PLUSLE_SHADOW_RECORD = 64,
        DEBUG_KIT_MINUN_SHADOW_RECORD = 77,
        DEBUG_KIT_PICHU_SHADOW_RECORD = 76,
        DEBUG_KIT_HOUOU_SHADOW_RECORD = 29
    };
    Pokemon pokemon;
    Hero *hero = (Hero *)savedataGetStatus(0, TRAINER_DATA_HERO);

    pokemonCreate(&pokemon, POKEMON_SPECIES_JOLTEON, 40, gamedataGetStatus(0, GAMEDATA_STATUS_ATTEST));
    heroCatchPokemon(hero, &pokemon, 0, POKEMON_CATCH_BALL_POKE_BALL, true);
    memoDataSet(0, &pokemon);

    pokemonCreate(&pokemon, POKEMON_SPECIES_VAPOREON, 40, gamedataGetStatus(0, GAMEDATA_STATUS_ATTEST));
    heroCatchPokemon(hero, &pokemon, 0, POKEMON_CATCH_BALL_POKE_BALL, true);
    memoDataSet(0, &pokemon);

    pokemonCreate(&pokemon, POKEMON_SPECIES_PLUSLE, 100, gamedataGetStatus(0, GAMEDATA_STATUS_ATTEST));
    pokemonSetDarkPokemonStatus(&pokemon, DEBUG_KIT_PLUSLE_SHADOW_RECORD);
    heroCatchPokemon(hero, &pokemon, 0, POKEMON_CATCH_BALL_POKE_BALL, true);

    pokemonCreate(&pokemon, POKEMON_SPECIES_MINUN, 100, gamedataGetStatus(0, GAMEDATA_STATUS_ATTEST));
    pokemonSetDarkPokemonStatus(&pokemon, DEBUG_KIT_MINUN_SHADOW_RECORD);
    heroCatchPokemon(hero, &pokemon, 0, POKEMON_CATCH_BALL_POKE_BALL, true);

    pokemonCreate(&pokemon, POKEMON_SPECIES_PICHU, 100, gamedataGetStatus(0, GAMEDATA_STATUS_ATTEST));
    pokemonSetDarkPokemonStatus(&pokemon, DEBUG_KIT_PICHU_SHADOW_RECORD);
    heroCatchPokemon(hero, &pokemon, 0, POKEMON_CATCH_BALL_POKE_BALL, true);

    pokemonCreate(&pokemon, POKEMON_SPECIES_HOUOU, 100, gamedataGetStatus(0, GAMEDATA_STATUS_ATTEST));
    pokemonSetDarkPokemonStatus(&pokemon, DEBUG_KIT_HOUOU_SHADOW_RECORD);
    heroCatchPokemon(hero, &pokemon, 0, POKEMON_CATCH_BALL_POKE_BALL, true);

    heroItemAddItemDataId(hero, ITEM_MASTER_BALL, 99, -1);
    heroItemAddItemDataId(hero, ITEM_ULTRA_BALL, 99, -1);
    heroItemAddItemDataId(hero, ITEM_GREAT_BALL, 99, -1);
    heroItemAddItemDataId(hero, ITEM_POKE_BALL, 99, -1);
    heroItemAddItemDataId(hero, ITEM_SAFARI_BALL, 99, -1);
    heroItemAddItemDataId(hero, ITEM_NET_BALL, 99, -1);
    heroItemAddItemDataId(hero, ITEM_DIVE_BALL, 99, -1);
    heroItemAddItemDataId(hero, ITEM_NEST_BALL, 99, -1);
    heroItemAddItemDataId(hero, ITEM_REPEAT_BALL, 99, -1);
    heroItemAddItemDataId(hero, ITEM_TIMER_BALL, 99, -1);
    heroItemAddItemDataId(hero, ITEM_LUXURY_BALL, 99, -1);
    heroItemAddItemDataId(hero, ITEM_PREMIER_BALL, 99, -1);

    heroItemAddItemDataId(hero, ITEM_HYPER_POTION, 99, -1);
    heroItemAddItemDataId(hero, ITEM_POTION, 300, -1);

    heroItemAddItemDataId(hero, ITEM_TM02, 1, -1);
    heroItemAddItemDataId(hero, ITEM_TM01, 3, -1);
    heroItemAddItemDataId(hero, ITEM_RAWST_BERRY, 3, -1);
    heroItemAddItemDataId(hero, ITEM_CHESTO_BERRY, 1, -1);
    heroItemAddItemDataId(hero, ITEM_PECHA_BERRY, 2, -1);

    heroItemAddItemDataId(hero, ITEM_COLOGNE_CASE, 1, -1);
    heroItemAddItemDataId(hero, ITEM_EXCITE_SCENT, 1, -1);
    heroItemAddItemDataId(hero, ITEM_VIVID_SCENT, 1, -1);
    heroItemAddItemDataId(hero, ITEM_JOY_SCENT, 1, -1);

    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_CASE, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_01, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_02, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_03, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_04, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_05, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_06, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_07, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_08, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_09, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_10, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_11, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_12, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_13, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_14, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_15, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_16, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_17, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_18, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_19, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_20, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_21, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_22, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_23, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_24, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_25, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_26, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_27, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_28, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_29, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_30, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_31, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_32, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_33, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_34, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_35, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_36, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_37, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_38, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_39, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_40, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_41, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_42, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_43, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_44, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_45, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_46, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_47, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_48, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_49, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_50, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_51, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_52, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_53, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_54, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_55, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_56, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_57, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_58, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_59, 1, -1);
    heroItemAddItemDataId(hero, ITEM_BATTLE_DISK_60, 1, -1);

    return 0;
}

#pragma force_active reset
