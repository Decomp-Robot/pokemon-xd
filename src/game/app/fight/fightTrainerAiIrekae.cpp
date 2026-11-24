#include <types.h>
#include <fightAction.h>
#include <fightSeq.h>
#include <fightTrainer.h>
#include <fightTrainerAi.h>
#include <fightTrainerAiCombo.h>
#include <hsdRandom.h>
#include <fightTrainerDB.h>
#include <pokemonDB.h>
#include <waza.h>
#include <zokusei.h>
#include <deck.h>
#include <fightFloor.h>
#include <fightPokemon.h>
#include <fightPokemonBios.h>
#include <dbgMenuFight.h>

enum { FIGHT_AI_IREKAE_CANDIDATE_COUNT = 6 };
enum { FIGHT_AI_FIGHT_POKEMON_ROSTER_COUNT = 24 };
enum { FIGHT_AI_COMBO_SLOT_COUNT = 4 };
enum { FIGHT_AI_ZOKUSEI_REVISE_COUNT = 2 };

enum { FIGHT_AI_TOKUSEI_DATA_ID_COUNT = 10 };
enum { FIGHT_AI_WAZA_LIST_CAPACITY = 8 };
enum { MIN_AISYOU_VALUE = -65535 };
enum { MAX_POKEMON_DATA_ID = 65535 };

u8 fightTrainerAiIrekae_SelComboPartner(FIGHT_TRAINER *trainer, FightPokemon *pokemon, FightOutPokemon *partner) {
    Pokemon *motoPokemon = fightPokemonGetPokemonPtr(pokemon);
    DeckTrainer *deckTrainer = DeckTrainer::getDeckTrainerObj(fightTrainer_GetFightTrainerDataId(trainer));
    u8 i;
    for (i = 0; i < FIGHT_AI_COMBO_SLOT_COUNT; i++) {
        u16 comboId = deckTrainer->getComboID(i);
        if (comboId == FIGHT_AI_COMBO_ID_NONE) {
            continue;
        }
        FightAiComboData *combo = &comboid[comboId];
        u8 order = combo->mOrder;
        if (combo->mKind == FIGHT_AI_COMBO_KIND_TWO_POKEMON) {
            if (partner == 0) {
                continue;
            }
            Pokemon *partnerPokemon = fightOutPokemonGetPokemonPtr(partner);
            u8 partnerFirstRole = pokemon_GetComboPartner(partnerPokemon, i, FIGHT_AI_COMBO_ROLE_FIRST);
            u8 partnerSecondRole = pokemon_GetComboPartner(partnerPokemon, i, FIGHT_AI_COMBO_ROLE_SECOND);
            u8 pokemonFirstRole = pokemon_GetComboPartner(motoPokemon, i, FIGHT_AI_COMBO_ROLE_FIRST);
            u8 pokemonSecondRole = pokemon_GetComboPartner(motoPokemon, i, FIGHT_AI_COMBO_ROLE_SECOND);
            if (order == FIGHT_AI_COMBO_ORDER_EITHER) {
                if ((partnerFirstRole == 1 && pokemonSecondRole == 1) || (partnerSecondRole == 1 && pokemonFirstRole == 1)) {
                    return FIGHT_AI_COMBO_PARTNER_MATCH;
                }
            }
            if (order == FIGHT_AI_COMBO_ORDER_RESTRICTED) {
                if (partnerFirstRole == 1 && pokemonSecondRole == 1) {
                    return FIGHT_AI_COMBO_PARTNER_RESTRICTED;
                }
            }
        }
        if (combo->mKind == FIGHT_AI_COMBO_KIND_ONE_POKEMON) {
            u8 pokemonFirstRole = pokemon_GetComboPartner(motoPokemon, i, FIGHT_AI_COMBO_ROLE_FIRST);
            u8 pokemonSecondRole = pokemon_GetComboPartner(motoPokemon, i, FIGHT_AI_COMBO_ROLE_SECOND);
            if (pokemonFirstRole == 1 || pokemonSecondRole == 1) {
                if (order == FIGHT_AI_COMBO_ORDER_EITHER) {
                    return FIGHT_AI_COMBO_PARTNER_MATCH;
                }
                if (order == FIGHT_AI_COMBO_ORDER_RESTRICTED) {
                    return FIGHT_AI_COMBO_PARTNER_NONE;
                }
            }
        }
    }
    return FIGHT_AI_COMBO_PARTNER_NONE;
}

s32 fightTrainerAiSelectFightActionIrekae(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 fightType) {
    u16 aiDataId = fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));
    FIGHT_TRAINER *ownerTrainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, pokemon);
    if (fightTrainerAiGetFightPokemonIrekaeModosuValue(trainer, pokemon, fightType) <= 0) {
        return 0;
    }
    s16 entryId = fightTrainerAiSelectIrekaeDasuFightPokemon(trainer, fightType, 1, pokemon);
    if (entryId < 0) {
        return 0;
    }
    fightOutPokemonCreateFightAction(pokemon, 0, FIGHT_ACTION_KIND_FIGHT_OUT_POKEMON_IREKAE, 0, fight_action_data_turn, entryId);
    return 1;
}

s16 fightTrainerAiSelectIrekaeDasuFightPokemon(FIGHT_TRAINER *trainer, u16 fightType, s32 selectType, FightOutPokemon *pokemon)
{
    u16 aiDataId;
    u16 wazaDataId;
    u8 i;
    u16 k;
    u8 partyIndex;
    u16 typeIndex;
    u8 j;
    FightPokemon *candidatePokemon;
    FightPokemon *other;
    FIGHT_TRAINER *otherTrainer;
    FightOutPokemon *enemy;
    FightPokemon *selectedPokemon;
    u16 wazaDataIdAry[FIGHT_AI_WAZA_LIST_CAPACITY];
    FightPokemon *irekaePokemonAry[FIGHT_AI_IREKAE_CANDIDATE_COUNT];
    s32 valueAry[FIGHT_AI_IREKAE_CANDIDATE_COUNT];
    FightOutPokemon *enemies[FIGHT_OUT_POKEMON_COUNT];
    FightOutPokemon *allies[FIGHT_OUT_POKEMON_COUNT];
    FightPokemon *fightPokemonAry[FIGHT_AI_FIGHT_POKEMON_ROSTER_COUNT];
    FightOutPokemon bios;
    u16 irekaeNum;
    u32 maxSubayasa;
    u16 enemyNum;
    u16 fightWazaNum;
    u8 tenkou;
    u8 lastValueRevise;
    u16 fightPokemonNum;
    s32 maxAisyouValue;
    u16 minPokemonDataId;
    u32 subayasa;
    u16 pokemonDataId;
    u16 wazaNum;
    u8 hitFlag;
    u8 batonPassOut;
    u8 comboPartner;
    u8 keyPlayerFlag;
    u8 reviseIndex;
    u8 reviseZokuseiDataId;
    u16 zokuseiDataId;

    aiDataId = fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));
    lastValueRevise = fightTrainerAiDB_GetLastValueRevise(aiDataId);
    tenkou = fightTrainerAiGetFightFloorNowTenkouDataId(trainer, 0, 1);
    for (i = 0; i < FIGHT_AI_IREKAE_CANDIDATE_COUNT; i++) valueAry[i] = 0;
    fightOutPokemonBiosCopy(&bios, pokemon);
    if (fightOutPokemonCheckFightOut(pokemon) == 1) {
        batonPassOut = fightOutPokemonIsFightActionAttackWazaOut(pokemon, WAZA_DATA_ID_BATON_PASS, 0);
    }
    irekaeNum = fightTrainerGetIrekaeFightPokemonAry(trainer, irekaePokemonAry);
    enemyNum = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, enemies, 0, 1);
    fightPokemonNum = fightFloorGetFightTrainerFightPokemonPtrAry(0, trainer, fightPokemonAry, 1, 1);
    fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, allies, 1, 1);
    if (irekaeNum == 0) return -1;
    if (fightTrainerAiDB_GetPokemonSelectRandomFlag(aiDataId) == 1) {
        u16 randomValue = HSD_Rand();
        u16 randomIndex = randomValue % irekaeNum;
        candidatePokemon = irekaePokemonAry[randomIndex];
        if (candidatePokemon != 0) {
            otherTrainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, candidatePokemon);
            return fightPokemon_GetEntryId(candidatePokemon);
        }
    }
    maxSubayasa = 0;
    maxAisyouValue = MIN_AISYOU_VALUE;
    minPokemonDataId = MAX_POKEMON_DATA_ID;
    for (i = 0; i < irekaeNum; i++) {
        candidatePokemon = irekaePokemonAry[i];
        if (candidatePokemon == 0) continue;
        if (fightPokemon_GetEntryId(candidatePokemon) < 0) continue;
        subayasa = fightTrainerAiGetFightPokemonSubayasa(trainer, candidatePokemon);
        s32 aisyouValue = fightTrainerAiGetFightPokemonZokuseiAisyouValue(trainer, candidatePokemon, FIGHT_AI_ZOKUSEI_AISYOU_MODE_WEAKNESS);
        pokemonDataId = pokemon_GetFightTrainerPokemonDataId(fightPokemonGetPokemonPtr(candidatePokemon));
        if (maxSubayasa < subayasa) maxSubayasa = subayasa;
        if (maxAisyouValue < aisyouValue) maxAisyouValue = aisyouValue;
        if (minPokemonDataId > pokemonDataId) minPokemonDataId = pokemonDataId;
    }
    if (fightTrainerAiDB_GetPokemonDataOrderOutFlag(aiDataId) == 1) {
        for (i = 0; i < irekaeNum; i++) {
            candidatePokemon = irekaePokemonAry[i];
            if (candidatePokemon == 0) continue;
            if (fightPokemon_GetEntryId(candidatePokemon) < 0) continue;
            otherTrainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, candidatePokemon);
            if (minPokemonDataId < pokemon_GetFightTrainerPokemonDataId(fightPokemonGetPokemonPtr(candidatePokemon))) continue;
            otherTrainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, candidatePokemon);
            return fightPokemon_GetEntryId(candidatePokemon);
        }
    }
    for (i = 0; i < irekaeNum; i++) {
        candidatePokemon = irekaePokemonAry[i];
        if (candidatePokemon == 0) continue;
        if (fightPokemon_GetEntryId(candidatePokemon) < 0) continue;
        otherTrainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, candidatePokemon);
        fightOutPokemonCreate(pokemon, candidatePokemon, 0);
        wazaNum = fightTrainerAiGetFightOutPokemonWazaDataIdAry(trainer, pokemon, wazaDataIdAry, 0, 1);
        hitFlag = 0;
        for (j = 0; j < enemyNum; j++) {
            enemy = enemies[j];
            if (enemy == 0) continue;
            for (k = 0; k < wazaNum; k++) {
                wazaDataId = wazaDataIdAry[k];
                if (wazaDataId == WAZA_DATA_ID_NONE) continue;
                if (wazaDataId == WAZA_SHADOW_SPECIAL) continue;
                if (fightTrainerAiWazaDoWazaHitFunc(trainer, pokemon, wazaDataId, enemy) == 1) {
                    hitFlag = 1;
                    break;
                }
            }
            if (hitFlag == 1) break;
        }
        fightOutPokemonBiosCopy(pokemon, &bios);
        fightWazaNum = fightTrainerAiGetFightPokemonWazaDataIdAry(trainer, candidatePokemon, wazaDataIdAry, 0, 1);
        comboPartner = fightTrainerAiIrekae_SelComboPartner(trainer, candidatePokemon, allies[0]);
        if (comboPartner == FIGHT_AI_COMBO_PARTNER_MATCH) valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_COMBO_PARTNER);
        if (comboPartner == FIGHT_AI_COMBO_PARTNER_RESTRICTED) valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_COMBO_PARTNER_KATAOMOTE);
        keyPlayerFlag = fightTrainerAiGetFightPokemonKeyPlayerFlag(trainer, candidatePokemon);
        if (keyPlayerFlag == FIGHT_AI_KEY_PLAYER_STANDARD) valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_IREKAE_KEY_PLAYER);
        if (keyPlayerFlag == FIGHT_AI_KEY_PLAYER_ACE) valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_ACE);
        if (keyPlayerFlag == FIGHT_AI_KEY_PLAYER_BOSS) valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_BOSS);
        for (reviseIndex = 0; reviseIndex < FIGHT_AI_ZOKUSEI_REVISE_COUNT; reviseIndex++) {
            reviseZokuseiDataId = fightTrainerAiDB_GetZokuseiReviseZokuseiDataId(aiDataId, reviseIndex);
            if (reviseZokuseiDataId == ZOKUSEI_DATA_ID_MYSTERY) continue;
            if (fightTrainerAiIsFightPokemonZokuseiDataId(trainer, candidatePokemon, reviseZokuseiDataId) == 1) {
                valueAry[i] = fightTrainerAiReviseValueByAddsubDataId(valueAry[i],
                    fightTrainerAiDB_GetZokuseiReviseValue(aiDataId, reviseIndex), trainer, AI_ADDSUB_ZOKUSEI_REVISE);
            }
        }
        if (fightTrainerAiIsFightPokemonNokoriHpHikui(trainer, candidatePokemon) == 1) {
            valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_NOKORI_HP_HIKUI);
        }
        if (hitFlag == 0) {
            valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_WAZA_ATARANAI);
        }
        if (maxSubayasa <= fightTrainerAiGetFightPokemonSubayasa(trainer, candidatePokemon)) {
            valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_SUBAYASA_ITIBAN_TAKAI);
        }
        if (maxAisyouValue <= fightTrainerAiGetFightPokemonZokuseiAisyouValue(trainer, candidatePokemon, FIGHT_AI_ZOKUSEI_AISYOU_MODE_WEAKNESS)) {
            valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_ZOKUSEI_AISYOU_YOI);
        }
        for (j = 0; j < enemyNum; j++) {
            enemy = enemies[j];
            if (enemy == 0) continue;
            for (k = 0; k < fightWazaNum; k++) {
                wazaDataId = wazaDataIdAry[k];
                if (wazaDataId == WAZA_DATA_ID_NONE) continue;
                zokuseiDataId = fightTrainerAiGetWazaZokuseiDataId(trainer, wazaDataId, pokemon);
                if (zokuseiDataId == ZOKUSEI_DATA_ID_MYSTERY) continue;
                if (fightTrainerAiIsWazaTypeId(trainer, wazaDataId, WAZA_TYPE_ID_IRYOKU) == 0) continue;
                if (fightTrainerAiGetFightOutPokemonZokuseiDataIdWazaJoutaiDataId(trainer, enemy,
                        zokuseiDataId, fightTrainerAiGetWazaIryoku(trainer, wazaDataId)) == JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE) {
                    valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_KOUKANOMI_WAZA_AISYOU_YOI);
                }
            }
        }
        for (j = 0; j < enemyNum; j++) {
            enemy = enemies[j];
            if (enemy == 0) continue;
            u16 enemyWazaNum = fightTrainerAiGetFightOutPokemonWazaDataIdAry(trainer, enemy, wazaDataIdAry, 0, 0);
            for (k = 0; k < enemyWazaNum; k++) {
                wazaDataId = wazaDataIdAry[k];
                if (wazaDataId == WAZA_DATA_ID_NONE) continue;
                zokuseiDataId = fightTrainerAiGetWazaZokuseiDataId(trainer, wazaDataId, enemy);
                if (zokuseiDataId == ZOKUSEI_DATA_ID_MYSTERY) continue;
                if (fightTrainerAiIsWazaTypeId(trainer, wazaDataId, WAZA_TYPE_ID_IRYOKU) == 0) continue;
                if (fightTrainerAiGetFightPokemonZokuseiDataIdWazaJoutaiDataId(trainer, candidatePokemon,
                        zokuseiDataId, fightTrainerAiGetWazaIryoku(trainer, wazaDataId)) == JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE) {
                    valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_TEKI_WAZA_AISYOU_WARUI);
                }
            }
        }
        if (fightTrainerAiDB_GetPokemonSelectWeakPointFlag(aiDataId) == 1) {
            for (typeIndex = 0; typeIndex < *zokusei_data_number; typeIndex++) {
                if (typeIndex == ZOKUSEI_DATA_ID_MYSTERY) continue;
                if (fightTrainerAiGetFightPokemonZokuseiDataIdWazaJoutaiDataId(trainer, candidatePokemon, typeIndex, 1) != JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE) continue;
                for (partyIndex = 0; partyIndex < fightPokemonNum; partyIndex++) {
                    other = fightPokemonAry[partyIndex];
                    if (other == 0) continue;
                    otherTrainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, other);
                    if (otherTrainer == 0) continue;
                    u8 irekaeState = fightTrainerCheckCanIrekaeFightPokemon(otherTrainer, other);
                    if (irekaeState == FIGHT_TRAINER_IREKAE_CANNOT_FIGHT) continue;
                    if (irekaeState != FIGHT_TRAINER_IREKAE_FIGHT_OUT && irekaeState != FIGHT_TRAINER_IREKAE_REFUSED) continue;
                    if (fightTrainerAiGetFightPokemonZokuseiDataIdWazaJoutaiDataId(trainer, other, typeIndex, 1) == JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE) {
                        valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_MIKATA_TO_ONAZI_YOWAI_ZOKUSEI);
                    }
                }
            }
        }
        u16 tokuseiAddsubDataId = fightTrainerAiGetFightPokemonIrekaeDasuTokuseiAddsubDataId(trainer, candidatePokemon);
        if (tokuseiAddsubDataId != AI_ADDSUB_NONE) {
            valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, tokuseiAddsubDataId);
        }
        if (tenkou == FIGHT_WEATHER_RAIN) {
            if (fightTrainerAiIsFightPokemonTokuseiDataId(trainer, candidatePokemon, TOKUSEI_DATA_ID_SWIFT_SWIM) == 1 || fightTrainerAiIsFightPokemonTokuseiDataId(trainer, candidatePokemon, TOKUSEI_DATA_ID_RAIN_DISH) == 1) {
                valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_TENKOU_AME);
            }
        } else if (tenkou == FIGHT_WEATHER_SUNLIGHT) {
            if (fightTrainerAiIsFightPokemonTokuseiDataId(trainer, candidatePokemon, TOKUSEI_DATA_ID_CHLOROPHYLL) == 1) {
                valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_TENKOU_HARE);
            }
        } else if (tenkou == FIGHT_WEATHER_SANDSTORM) {
            u8 tenkouFlag = 0;
            if (fightTrainerAiIsFightPokemonTokuseiDataId(trainer, candidatePokemon, TOKUSEI_DATA_ID_SAND_VEIL) == 1) tenkouFlag = 1;
            if (fightTrainerAiIsFightPokemonZokuseiDataId(trainer, candidatePokemon, ZOKUSEI_DATA_ID_STEEL) == 1 || fightTrainerAiIsFightPokemonZokuseiDataId(trainer, candidatePokemon, ZOKUSEI_DATA_ID_ROCK) == 1
                || fightTrainerAiIsFightPokemonZokuseiDataId(trainer, candidatePokemon, ZOKUSEI_DATA_ID_GROUND) == 1) {
                tenkouFlag = 1;
            }
            if (tenkouFlag == 1) {
                valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_TENKOU_SUNAARASI);
            }
        } else if (tenkou == FIGHT_WEATHER_HAIL) {
            if (fightTrainerAiIsFightPokemonZokuseiDataId(trainer, candidatePokemon, ZOKUSEI_DATA_ID_ICE) == 1) {
                valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_TENKOU_ARARE);
            }
        }
        for (j = 0; j < enemyNum; j++) {
            enemy = enemies[j];
            if (enemy == 0) continue;
            if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, enemy, JOUTAI_CONTINUING_MOVE) == 1) {
                u16 motoWazaDataId = fightOutPokemon_GetMotoWazaDataId(enemy);
                s32 damage = fightTrainerAiWazaDoWazaDamageFightPokemonFunc(trainer, enemy,
                    motoWazaDataId, candidatePokemon, 0);
                if (damage >= pokemon_GetHp(fightPokemon_GetPokemonBuffPtr(candidatePokemon))) {
                    valueAry[i] = fightTrainerAiAddValueByAddsubDataId(valueAry[i], trainer, AI_ADDSUB_IREKAE_TEKI_CONTINUING_MOVE_DAMAGE);
                }
            }
        }
        if (dbgMenuFightGetFightTrainerAiLastValueReviseFlag() == 1) {
            u16 randomValue = HSD_Rand();
            valueAry[i] = fightTrainerAiAddValue(valueAry[i], randomValue % (lastValueRevise * 2 + 1) - lastValueRevise);
        }
    }
    s32 selectedIndex = fightTrainerAiGetValueAryMaxBanme(valueAry, irekaeNum, 1);
    if (selectedIndex < 0) return -1;
    selectedPokemon = irekaePokemonAry[selectedIndex];
    if (selectedPokemon == 0) return -1;
    otherTrainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, selectedPokemon);
    return fightPokemon_GetEntryId(selectedPokemon);
}

s32 fightTrainerAiGetFightPokemonIrekaeModosuValue(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 fightType)
{
    FightOutPokemon *pokemonAry[FIGHT_OUT_POKEMON_COUNT];
    u16 wazaDataIdAry[FIGHT_AI_WAZA_LIST_CAPACITY];
    u16 wazaCount;
    u16 pokemonCount;
    u16 basisSubayasa;
    u16 nowHp;
    u16 addsubDataId;
    u16 i;
    u16 wazaDataId;
    u16 j;
    u16 k;
    FightOutPokemon *fightOutPokemon;
    u8 hitResult;
    s32 value;
    u8 canDefeatEnemy;
    u8 skipNoHitPenalty;
    s32 damage;
    u16 aiDataId;
    FIGHT_TRAINER *ownerTrainer;

    aiDataId = fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));
    value = 0;
    ownerTrainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, pokemon);
    basisSubayasa = fightTrainerAiGetFightOutPokemonBasisSubayasa(trainer, pokemon);
    wazaCount = fightTrainerAiGetFightOutPokemonWazaDataIdAry(trainer, pokemon, wazaDataIdAry, 0, 1);
    pokemonCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, pokemonAry, 0, 1);
    canDefeatEnemy = 0;
    skipNoHitPenalty = 0;
    for (i = 0; i < pokemonCount; i++) {
        fightOutPokemon = pokemonAry[i];
        if (fightOutPokemon == 0) {
            continue;
        }
        nowHp = fightTrainerAiGetFightOutPokemonNowHp(trainer, fightOutPokemon);
        for (j = 0; j < wazaCount; j++) {
            wazaDataId = wazaDataIdAry[j];
            if (wazaDataId == WAZA_DATA_ID_NONE || wazaDataId == WAZA_SHADOW_SPECIAL) {
                continue;
            }
            hitResult = fightTrainerAiWazaDoWazaHitFunc(trainer, pokemon, wazaDataId, fightOutPokemon);
            if (hitResult == 1) {
                skipNoHitPenalty = 1;
            }
            if (fightTrainerAiIsFightOutPokemonJoutaiKie(trainer, fightOutPokemon) == 1) {
                skipNoHitPenalty = 1;
            }
            if (fightTrainerAiIsWazaTypeId(trainer, wazaDataId, WAZA_TYPE_ID_IRYOKU)) {
                if (nowHp < fightTrainerAiWazaDoWazaDamageFunc(trainer, pokemon, wazaDataId,
                        fightOutPokemon, 0)) {
                    if (hitResult == 1) {
                        canDefeatEnemy = 1;
                    }
                }
            }
        }
    }
    if (fightTrainerAiGetFightOutPokemonKeyPlayerFlag(trainer, pokemon)) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_KEY_PLAYER);
    }
    if (fightTrainerAiIsFightOutPokemonNokoriHpHikui(trainer, pokemon) == 0) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_NOT_NOKORI_HP_HIKUI);
    } else if (fightTrainerAiGetFightOutPokemonKeyPlayerFlag(trainer, pokemon)) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_KEY_PLAYER_NOKORI_HP_HIKUI);
    }
    if (fightTrainerAiIsFightOutPokemonAllAbiCntTakai(trainer, pokemon) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_ALL_ABI_CNT_TAKAI);
    }
    if (fightTrainerAiIsFightOutPokemonAllAbiCntHikui(trainer, pokemon) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_ALL_ABI_CNT_HIKUI);
    }
    if (!skipNoHitPenalty) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_WAZA_HIT_NASI);
    }
    if (canDefeatEnemy == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_TEKI_TAOSERU);
    }
    if (fightTrainerAiIsFightOutPokemonNokoriHpHikui(trainer, pokemon) == 1) {
        for (k = 0; k < pokemonCount; k++) {
            if (pokemonAry[k] == 0) {
                continue;
            }
            if (fightTrainerAiGetFightOutPokemonBasisSubayasa(trainer, pokemonAry[k]) > basisSubayasa) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_TEKI_SUBAYASA_TAKAI);
            }
        }
    }
    addsubDataId = fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId(trainer, pokemon);
    if (addsubDataId != AI_ADDSUB_NONE) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, addsubDataId);
    }
    for (i = 0; i < pokemonCount; i++) {
        fightOutPokemon = pokemonAry[i];
        if (fightOutPokemon == 0) {
            continue;
        }
        if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, fightOutPokemon, JOUTAI_CONTINUING_MOVE) == 1) {
            damage = fightTrainerAiWazaDoWazaDamageFunc(trainer, pokemon,
                fightOutPokemon_GetMotoWazaDataId(fightOutPokemon), fightOutPokemon, 0);
            if (damage >= fightOutPokemonNowHpWaruValue(pokemon, 1)) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_TEKI_CONTINUING_MOVE_DAMAGE);
            }
        }
    }
    return value;
}

u16 fightTrainerAiGetFightPokemonIrekaeDasuTokuseiAddsubDataId(FIGHT_TRAINER *trainer,
    FightPokemon *pokemon)
{
    FightOutPokemon *pokemonAry[FIGHT_OUT_POKEMON_COUNT];
    u8 weather = fightTrainerAiGetFightFloorNowTenkouDataId(trainer, 0, 1);
    u16 tokuseiDataIdAry[FIGHT_AI_TOKUSEI_DATA_ID_COUNT] = {
        TOKUSEI_DATA_ID_STENCH,
        TOKUSEI_DATA_ID_DRIZZLE,
        TOKUSEI_DATA_ID_ILLUMINATE,
        TOKUSEI_DATA_ID_TRACE,
        TOKUSEI_DATA_ID_SAND_STREAM,
        TOKUSEI_DATA_ID_RUN_AWAY,
        TOKUSEI_DATA_ID_PICKUP,
        TOKUSEI_DATA_ID_TRUANT,
        TOKUSEI_DATA_ID_FORECAST,
        TOKUSEI_DATA_ID_DROUGHT
    };
    u16 pokemonCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, pokemonAry, 0, 1);
    u16 i;
    u16 j;
    u16 phyAtk;
    u16 speAtk;
    u8 hasExcludedAbility;

    if (fightTrainerAiIsFightPokemonTokuseiDataId(trainer, pokemon, TOKUSEI_DATA_ID_INTIMIDATE) == 1) {
        for (i = 0; i < pokemonCount; i++) {
            phyAtk = fightTrainerAiGetFightOutPokemonBasisPhyAtk(trainer, pokemonAry[i]);
            speAtk = fightTrainerAiGetFightOutPokemonBasisSpeAtk(trainer, pokemonAry[i]);
            if (phyAtk >= speAtk) {
                return AI_ADDSUB_TOKUSEI_IKAKU;
            }
        }
    }
    if (fightTrainerAiIsFightPokemonTokuseiDataId(trainer, pokemon, TOKUSEI_DATA_ID_TRACE) == 1) {
        for (i = 0; i < pokemonCount; i++) {
            hasExcludedAbility = 0;
            for (j = 0; j < FIGHT_AI_TOKUSEI_DATA_ID_COUNT; j++) {
                if (fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, pokemonAry[i],
                        tokuseiDataIdAry[j]) == 1) {
                    hasExcludedAbility = 1;
                    break;
                }
            }
            if (hasExcludedAbility != 1) {
                return AI_ADDSUB_TOKUSEI_TRACE;
            }
        }
    }
    if (weather != FIGHT_WEATHER_NONE) {
        if (fightTrainerAiIsFightPokemonTokuseiDataId(trainer, pokemon, TOKUSEI_DATA_ID_AIR_LOCK) == 1) {
            return AI_ADDSUB_TOKUSEI_AIR_LOCK;
        }
        if (fightTrainerAiIsFightPokemonTokuseiDataId(trainer, pokemon, TOKUSEI_DATA_ID_CLOUD_NINE) == 1) {
            return AI_ADDSUB_TOKUSEI_CLOUD_NINE;
        }
    }
    return AI_ADDSUB_NONE;
}

u16 fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId(FIGHT_TRAINER *trainer,
    FightOutPokemon *pokemon)
{
    s8 kaisuu;
    s8 nowKaisuu;

    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_CONFUSION) == 1) {
        return AI_ADDSUB_KONRAN;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_INFATUATION) == 1) {
        return AI_ADDSUB_MERO;
    }
    kaisuu = fightOutPokemonGetKaisuuJoutaiDataId(pokemon, JOUTAI_PERISH_SONG);
    nowKaisuu = fightOutPokemonGetNowKaisuuJoutaiDataId(pokemon, JOUTAI_PERISH_SONG);
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_PERISH_SONG) == 1
        && fightTrainerAiIsFightOutPokemonNokoriHpHikui(trainer, pokemon) == 0
        && nowKaisuu + 1 >= kaisuu) {
        return AI_ADDSUB_PERISH_SONG;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_PARTIAL_TRAPPING) == 1) {
        return AI_ADDSUB_PARTIAL_TRAPPING;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_AKUMU) == 1) {
        return AI_ADDSUB_AKUMU;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_CURSE) == 1) {
        return AI_ADDSUB_NOROI;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_FORESIGHT) == 1) {
        return AI_ADDSUB_MIYABURU;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_TORMENT) == 1) {
        return AI_ADDSUB_TORMENT;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_LEECH_SEED) == 1) {
        return AI_ADDSUB_YADORIGI;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_LOCK_ON) == 1) {
        return AI_ADDSUB_LOCK_ON;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_DROWSY) == 1) {
        return AI_ADDSUB_AKUBI;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_IMPRISON) == 1) {
        return AI_ADDSUB_HUUIN;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_GRUDGE) == 1) {
        return AI_ADDSUB_ONNEN;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_DISABLE) == 1) {
        return AI_ADDSUB_DISABLE;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_ENCORE) == 1) {
        return AI_ADDSUB_ENCORE;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_TYOUHATU) == 1) {
        return AI_ADDSUB_TYOUHATU;
    }
    return AI_ADDSUB_NONE;
}
