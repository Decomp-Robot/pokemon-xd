#include <FightResult.h>
#include <GSthread.h>
#include <Pokemon.h>
#include <app/fight/fightKind.h>
#include <fight.h>
#include <fightAction.h>
#include <fightFloor.h>
#include <fightPokemon.h>
#include <fightPokemonBios.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTrainer.h>
#include <hero.h>
#include <heroBios.h>
#include <menuCBBios.h>
#include <pokemonEvolution.h>
#include <savedata.h>
#include <wazaSequence.h>
#include "app/tenkou/tenkouBios.h"

enum { FIGHT_KOUKA_ID_INITIAL_WEATHER = 1 };

u8 fightResultReadyFlag;
GSthread *_gSoundThread;
u8 fightMenuCancelFlag;
WazaSequence *fightEnemyTrainerWazaSeqHdl;
u8 fightNekoniKobanFlag;
u16 _fight_kaisi_pre_trainer_intr_wzx_data_id;

extern "C" {
void fightKoukaDoFightKoukaJoukenAndKouka(void *target, u16 fightKoukaId);
}

extern "C" u8 fightActionFlowSyuuryouPost(FIGHT_ACTION *action)
{
    u16 linkSideIndex;
    FIGHT_SIDE *linkSide;
    u16 linkTrainerIndex;
    FIGHT_TRAINER *linkTrainer;
    Pokemon *pokemon;
    Hero *menuHero;
    u16 trainerIndex;
    Hero *linkMenuHero;
    u16 slot;
    FIGHT_TRAINER *trainer;
    u16 sideIndex;
    FIGHT_SIDE *side;
    u8 meetDarkPokemonCount;
    Hero *savedHero;
    FightPokemon *fightPokemon;
    u32 trainerNum = fightFloor_GetFightTypeTrainerNum(0);
    if (fightGetFightResultId() == kFightResultCommunicationError) {
        return 1;
    }

    FIGHT_TRAINER *heroTrainer = fightFloorGetGcHeroFightTrainerPtr(0);
    if (heroTrainer != 0) {
        Hero *hero = fightTrainer_GetHeroPtr(heroTrainer);
        if (hero != 0) {
            fightTrainerBackFightPokemonToTemotiPokemon(heroTrainer, 0);
            if (fightFloorIsGcHeroWin(0, fightGetFightResultId()) == 1) {
                if (fightFloor_GetFightKindGetExpFlag(0) == 1 &&
                    fightTrainerCheckCanGetExp(heroTrainer) == 1) {
                    for (slot = 0; slot < HERO_PARTY_POKEMON_COUNT; ++slot) {
                        pokemon = (Pokemon *)heroGetStatus(hero, HERO_STATUS_POKEMON, slot);
                        if (!pokemonCheckFightOut(pokemon) || pokemonIsDarkPokemon(pokemon) == 1) {
                            continue;
                        }
                        fightPokemon = fightTrainerCheckTemotiPokemonFightEntry(heroTrainer, pokemon);
                        if (fightPokemon == 0 || !fightPokemonCheckFightOut(fightPokemon) ||
                            !fightPokemonBiosGetLevelUpFlag(fightPokemon)) {
                            continue;
                        }
                        u16 additionalSpecies;
                        POKEMON_EVOLUTION_INFO info;
                        u16 species = pokemonEvolutionCheck(pokemon, EVOLUTION_CHECK_LEVEL_UP, 0,
                                                           &additionalSpecies, &info);
                        if (species != 0) {
                            pokemonEvolutionAll(pokemon, species, additionalSpecies, &info, hero, 1, 1, 0);
                            fightMainWaitFrameEffect(10);
                        }
                    }
                }
                if (fightFloor_GetFightKindMonohiroiFlag(0) == 1) {
                    heroCheckSetMonohiroiAllTemotiPokemon(hero);
                }
                fightFloor_GetFightKindGetInfectPokerusFlag(0);
            }
            if (fightFloor_GetFightKindBackSaveDataFlag(0) == 1) {
                savedHero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
                if (savedHero != 0) {
                    meetDarkPokemonCount = heroBiosGetMeetDarkPokemonCount(savedHero);
                    heroBiosCopy(savedHero, hero);
                    heroBiosSetMeetDarkPokemonCount(savedHero, meetDarkPokemonCount);
                }
            }
        }
    }

    if (menuCBBios_CurrentlyWorkingToolBattle() == 1) {
        for (sideIndex = 0; sideIndex < FIGHT_FLOOR_SIDE_COUNT; ++sideIndex) {
            side = fightFloorGetValidFightSidePtr(0, sideIndex);
            if (side != 0) {
                for (trainerIndex = 0; trainerIndex < trainerNum; ++trainerIndex) {
                    trainer = fightSideGetValidFightTrainerPtr(side, trainerIndex);
                    if (trainer != 0) {
                        fightTrainerBackFightPokemonToTemotiPokemon(trainer, 0);
                        menuHero = menuCBBios_GetHeroImmediately((u8)((u32)trainerIndex + sideIndex * trainerNum));
                        if (menuHero != 0) {
                            Hero *trainerHero = fightTrainer_GetHeroPtr(trainer);
                            if (trainerHero != 0) {
                                heroBiosCopy(menuHero, trainerHero);
                            }
                        }
                    }
                }
            }
        }
    }

    if ((u8)fightFloor_GetFightKind(0) == FIGHT_KIND_ID_MENU_LINK_SINGLE ||
        (u8)fightFloor_GetFightKind(0) == FIGHT_KIND_ID_MENU_LINK_MULTI) {
        for (linkSideIndex = 0; linkSideIndex < FIGHT_FLOOR_SIDE_COUNT; ++linkSideIndex) {
            linkSide = fightFloorGetValidFightSidePtr(0, linkSideIndex);
            if (linkSide != 0) {
                for (linkTrainerIndex = 0; linkTrainerIndex < trainerNum; ++linkTrainerIndex) {
                    linkTrainer = fightSideGetValidFightTrainerPtr(linkSide, linkTrainerIndex);
                    if (linkTrainer != 0) {
                        if (((u8)fightFloor_GetFightKind(0) == FIGHT_KIND_ID_MENU_LINK_SINGLE ||
                             (u8)fightFloor_GetFightKind(0) == FIGHT_KIND_ID_MENU_LINK_MULTI) &&
                            !fightTrainerIsGcHero(linkTrainer)) {
                            continue;
                        }
                        fightTrainerBackFightPokemonToTemotiPokemon(linkTrainer, 0);
                        linkMenuHero = fightGetMenuCtrlHero();
                        if (linkMenuHero != 0) {
                            Hero *trainerHero = fightTrainer_GetHeroPtr(linkTrainer);
                            if (trainerHero != 0) {
                                heroBiosCopy(linkMenuHero, trainerHero);
                            }
                        }
                    }
                }
            }
        }
    }

    if (fightEnemyTrainerWazaSeqHdl != 0) {
        WazaSequence_Free(fightEnemyTrainerWazaSeqHdl);
    }
    return 1;
}


extern "C" u8 fightActionFlowHeijou(FIGHT_ACTION *action)
{
    return 1;
}

u8 _fightActionFlowTenkouInitSubGetSeqFightOutPokemonPtr(void *pokemon, u16, void *result)
{
    if (fightOutPokemon_GetSequencePtr((FightOutPokemon *)pokemon) != 0) {
        if (result != 0) {
            *(FightOutPokemon **)result = (FightOutPokemon *)pokemon;
        }
        return 0;
    }
    return 1;
}

extern "C" u8 fightActionFlowTenkouInit(FIGHT_ACTION *action)
{
    fightKoukaDoFightKoukaJoukenAndKouka(0, FIGHT_KOUKA_ID_INITIAL_WEATHER);
    u32 weatherId = fightFloorGetNowTenkouDataId(0, 0);
    FightOutPokemon *pokemon = 0;
    fightFloorLoopValidFightOutPokemon(0,
        _fightActionFlowTenkouInitSubGetSeqFightOutPokemonPtr, &pokemon, 0);
    if (weatherId != 0) {
        fightFloor_SetAttackPokemonPtr(0, pokemon);
        u32 messageId = tenkouDataBiosGetFightInitMsgId(weatherId);
        fightFloor_SetAppointMsgPtr(0, messageId);
        fightSeqExec(action, (u8 *)fightActionBiosGetBuffDataPtr(action), 0);
    }
    return 1;
}

extern "C" u8 fightActionFlowWazaKiaipantiPre(FIGHT_ACTION *action)
{
    fightFloor_SetAttackPokemonPtr(0,
        (FightOutPokemon *)fightActionBiosGetActorFightTargetPtr(action));
    fightSeqExec(action, (u8 *)fightActionBiosGetBuffDataPtr(action), 0);
    return 1;
}
