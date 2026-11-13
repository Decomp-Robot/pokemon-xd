#include <fightTarget.h>

#include <fightFloor.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTrainer.h>

#include <app/fight/fightType.h>

// Private helper: the side of the fight the given target pointer belongs to.
void *_fightTargetGetTargetPtrToFightSidePtr(void *target, u16 fightType);

// Whether the target belongs to the host side of the fight.
bool fightTargetIsHostSide(void *target, u16 fightType) {
    void *hostSide = fightTargetGetPtr(FIGHT_TARGET_ID_HOST_SIDE, 0, fightType);
    if (hostSide == 0) {
        return 0;
    }
    void *ownSide = fightTargetGetPtr(FIGHT_TARGET_ID_OWN_SIDE, target, fightType);
    if (ownSide == 0) {
        return 0;
    }
    return ownSide == hostSide;
}

// Turns a target id relative to the host side into the target it names in the
// fight the fight type describes.
void *fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(u16 targetId, u16 fightType) {
    void *hostSide = fightTargetGetPtr(FIGHT_TARGET_ID_HOST_SIDE, 0, fightType);
    if (hostSide == 0) {
        return 0;
    }
    void *target = fightTargetGetPtr(targetId, hostSide, fightType);
    if (target == 0) {
        return 0;
    }
    return target;
}

// Turns a target of the fight into the target id the host side knows it by.
u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(void *target, u16 fightType) {
    void *hostSide = fightTargetGetPtr(FIGHT_TARGET_ID_HOST_SIDE, 0, fightType);
    if (hostSide == 0) {
        return 0;
    }
    return fightTargetGetTragetPtrToRelativeFightTargetId(hostSide, target, fightType);
}

// Turns a target of the fight into the target id relative to the given side.
u16 fightTargetGetTragetPtrToRelativeFightTargetId(void *base, void *target, u16 fightType) {
    for (u16 targetId = 0; targetId < fight_target_data_number; targetId++) {
        if (targetId == 0) {
            continue;
        }
        FIGHT_TARGET_DATA *targetData = fightTargetDataBiosGetPtr(targetId);
        if (targetData == 0) {
            continue;
        }
        if (fightTargetDataBiosGetRelativeFlag(targetData) == 0) {
            continue;
        }
        if (fightTargetGetPtr(targetId, base, fightType) == target) {
            return targetId;
        }
    }
    return 0;
}

u8 fightTargetDataBiosGetRelativeFlag(FIGHT_TARGET_DATA *targetData) {
    if (targetData == 0) {
        return 0;
    }
    return targetData->mRelativeFlag;
}

s32 fightTargetDataBiosGetBuff(FIGHT_TARGET_DATA *targetData) {
    if (targetData == 0) {
        return 0;
    }
    return targetData->mBuff;
}

u8 fightTargetDataBiosGetStatusKid(FIGHT_TARGET_DATA *targetData) {
    if (targetData == 0) {
        return 0;
    }
    return targetData->mStatusKid;
}

FIGHT_TARGET_DATA *fightTargetDataBiosGetPtr(u16 target) {
    if (target >= fight_target_data_number) {
        return 0;
    }
    return &fight_target_data[target];
}

// Resolves a target id against the fight type that is running right now.
void *fightTargetGetPtrAsNowFightType(u16 kind, void *target) {
    return fightTargetGetPtr(kind, target, fightFloor_GetFightType(0));
}

// Resolves a target id of the given fight type to the target it names.
void *fightTargetGetPtr(u16 kind, void *target, u16 fightType) {
    FIGHT_SIDE *sides[FIGHT_FLOOR_SIDE_COUNT];
    FIGHT_FLOOR *floor;
    FightOutPokemon *pokemons[FIGHT_OUT_POKEMON_COUNT];
    FIGHT_TRAINER *trainers[FIGHT_FLOOR_SIDE_COUNT * FIGHT_SIDE_TRAINER_COUNT];
    void *fightTypeData;
    u16 trainer;
    u16 side;
    u16 trainerCount;
    u8 fightoutPokemonNum;
    u8 isTargetSide;
    u16 pokemon;
    u16 pokemonCount;
    u16 pokemonIndex;
    void *targetPtr;
    u8 trainerNum;

    if (kind == 0) {
        return 0;
    }
    floor = (FIGHT_FLOOR *)fightFloorGetNowPtr();
    if (kind == FIGHT_TARGET_ID_FLOOR) {
        return floor;
    }
    switch (kind) {
    case FIGHT_TARGET_ID_ATTACKER:
        return fightFloor_GetAttackPokemonPtr(floor);
    case FIGHT_TARGET_ID_DEFENDER:
        return fightFloor_GetDefensePokemonPtr(floor);
    case FIGHT_TARGET_ID_ESCAPE_POKEMON:
        return fightFloor_GetEscapePokemonPtr(floor);
    case FIGHT_TARGET_ID_IREKAE_POKEMON:
        return fightFloor_GetIrekaePokemonPtr(floor);
    case FIGHT_TARGET_ID_KIZETU_POKEMON:
        return fightFloor_GetKizetuPokemonPtr(floor);
    case FIGHT_TARGET_ID_TUIKAKOUKA_POKEMON:
        return fightFloor_GetTuikakoukaPokemonPtr(floor);
    case FIGHT_TARGET_ID_TOKUSEI_POKEMON:
        return fightFloor_GetTokuseiPokemonPtr(floor);
    case FIGHT_TARGET_ID_ITEM_POKEMON:
        return fightFloor_GetItemPokemonPtr(floor);
    case FIGHT_TARGET_ID_APPOINT_POKEMON:
        return fightFloor_GetAppointPokemonPtr(floor);
    case FIGHT_TARGET_ID_APPOINT_TRAINER:
        return fightFloor_GetAppointTrainerPtr(floor);
    case FIGHT_TARGET_ID_APPOINT_WAZA:
        return fightFloor_GetAppointWazaPtr(floor);
    case FIGHT_TARGET_ID_APPOINT_SIDE:
        return fightFloor_GetAppointSidePtr(floor);
    case FIGHT_TARGET_ID_APPOINT_ITEM:
        return fightFloor_GetAppointItemPtr(floor);
    }
    fightTypeData = fightTypeDataBiosGetPtr(fightType);
    if (fightTypeData == 0) {
        return 0;
    }
    if (target != 0) {
        targetPtr = target;
        target = _fightTargetGetTargetPtrToFightSidePtr(target, fightType);
        if (target == 0) {
            return 0;
        }
    }
    trainerCount = 0;
    pokemonCount = 0;
    trainerNum = fightTypeDataBiosGetTrainerNum(fightTypeData);
    fightoutPokemonNum = fightTypeDataBiosGetFightoutPokemonNum(fightTypeData);
    for (side = 0; side < FIGHT_FLOOR_SIDE_COUNT; side++) {
        sides[side] = fightFloor_GetFightSidePtr(floor, side);
        if (kind == FIGHT_TARGET_ID_HOST_SIDE && side == 0) {
            return sides[side];
        }
        if (kind == FIGHT_TARGET_ID_HOST_ENEMY_SIDE && side == 1) {
            return sides[side];
        }
        if (sides[side] == target) {
            isTargetSide = 1;
            if (kind == FIGHT_TARGET_ID_OWN_SIDE) {
                return sides[side];
            }
        } else {
            isTargetSide = 0;
            if (kind == FIGHT_TARGET_ID_OPPOSING_SIDE) {
                return sides[side];
            }
        }
        pokemonIndex = 0;
        for (trainer = 0; trainer < trainerNum; trainer++) {
            trainers[trainerCount] = fightSide_GetFightTrainerPtr(sides[side], trainer);
            if (kind == FIGHT_TARGET_ID_HOST_TRAINER_1 && side == 0 && trainer == 0) {
                return trainers[trainerCount];
            }
            if (isTargetSide == 1) {
                if (kind == FIGHT_TARGET_ID_OWN_SIDE_TRAINER_1 && trainer == 0) {
                    return trainers[trainerCount];
                }
                if (kind == FIGHT_TARGET_ID_OWN_SIDE_TRAINER_2 && trainer == 1) {
                    return trainers[trainerCount];
                }
                if (kind == FIGHT_TARGET_ID_OTHER_TRAINER_ON_SIDE && targetPtr != trainers[trainerCount]) {
                    return trainers[trainerCount];
                }
            } else {
                if (kind == FIGHT_TARGET_ID_OPPONENT_TRAINER_1 && trainer == 0) {
                    return trainers[trainerCount];
                }
                if (kind == FIGHT_TARGET_ID_OPPONENT_TRAINER_2 && trainer == 1) {
                    return trainers[trainerCount];
                }
            }
            for (pokemon = 0; pokemon < fightoutPokemonNum; pokemon++) {
                pokemons[pokemonCount] =
                    fightTrainer_GetFightoutPokemonPtr(trainers[trainerCount], pokemon);
                if (isTargetSide == 1) {
                    if (kind == FIGHT_TARGET_ID_OWN_SIDE_FIRST_POKEMON && pokemonIndex == 0) {
                        return pokemons[pokemonCount];
                    }
                    if (kind == FIGHT_TARGET_ID_OWN_SIDE_SECOND_POKEMON && pokemonIndex == 1) {
                        return pokemons[pokemonCount];
                    }
                    if (kind == FIGHT_TARGET_ID_OTHER_POKEMON_ON_SIDE &&
                        targetPtr != pokemons[pokemonCount]) {
                        return pokemons[pokemonCount];
                    }
                } else {
                    if (kind == FIGHT_TARGET_ID_OPPOSING_SIDE_FIRST_POKEMON && pokemonIndex == 0) {
                        return pokemons[pokemonCount];
                    }
                    if (kind == FIGHT_TARGET_ID_OPPOSING_SIDE_SECOND_POKEMON && pokemonIndex == 1) {
                        return pokemons[pokemonCount];
                    }
                }
                pokemonCount++;
                pokemonIndex++;
            }
            trainerCount++;
        }
    }
    return 0;
}

// The side of the fight the given target pointer belongs to, or null when the
// pointer is not part of the fight.
void *_fightTargetGetTargetPtrToFightSidePtr(void *target, u16 fightType) {
    FIGHT_FLOOR *floor;
    void *fightTypeData;
    u8 trainerNum;
    u8 fightoutPokemonNum;
    u8 entryPokemonNum;
    u16 side;
    u16 trainer;
    u16 pokemon;

    floor = (FIGHT_FLOOR *)fightFloorGetStatus(0, 0, FIGHT_FLOOR_STATUS_FIGHT_FLOOR_PTR, 0);
    fightTypeData = fightTypeDataBiosGetPtr(fightType);
    if (fightTypeData == 0) {
        return 0;
    }
    trainerNum = fightTypeDataBiosGetTrainerNum(fightTypeData);
    fightoutPokemonNum = fightTypeDataBiosGetFightoutPokemonNum(fightTypeData);
    entryPokemonNum = fightTypeDataBiosGetEntryPokemonNum(fightTypeData);
    for (side = 0; side < FIGHT_FLOOR_SIDE_COUNT; side++) {
        FIGHT_SIDE *sidePtr = fightFloor_GetFightSidePtr(floor, side);
        if (sidePtr == target) {
            return sidePtr;
        }
        for (trainer = 0; trainer < trainerNum; trainer++) {
            FIGHT_TRAINER *trainerPtr = fightSide_GetFightTrainerPtr(sidePtr, trainer);
            if (trainerPtr == target) {
                return sidePtr;
            }
            for (pokemon = 0; pokemon < entryPokemonNum; pokemon++) {
                if (fightTrainer_GetFightPokemonPtr(trainerPtr, pokemon) == target) {
                    return sidePtr;
                }
            }
            for (pokemon = 0; pokemon < fightoutPokemonNum; pokemon++) {
                if (fightTrainer_GetFightoutPokemonPtr(trainerPtr, pokemon) == target) {
                    return sidePtr;
                }
            }
        }
    }
    return 0;
}
