#ifndef FIGHT_SEQ_H
#define FIGHT_SEQ_H

#include <types.h>
#include <fightPokemon.h>

typedef FightOutPokemon *(*FightPokemonSelectCallback)(FightOutPokemon *pokemon,
    u16 waza, u16 fightType);

typedef struct FIGHT_ACTION FIGHT_ACTION;

#ifdef __cplusplus
extern "C" {
#endif

enum { FIGHT_STAT_ATTACK=1, FIGHT_STAT_DEFENSE=2, FIGHT_STAT_SPEED=3, FIGHT_STAT_SPECIAL_ATTACK=4, FIGHT_STAT_SPECIAL_DEFENSE=5 };
enum { JOUTAI_POISON=3, JOUTAI_BAD_POISON=4, JOUTAI_PARALYSIS=5, JOUTAI_BURN=6, JOUTAI_FREEZE=7, JOUTAI_SLEEP=8, JOUTAI_CONFUSION=9, JOUTAI_INFATUATION=10, JOUTAI_FOCUS_ENERGY=15, JOUTAI_AKUMU=23, JOUTAI_LEECH_SEED=0x1C, JOUTAI_GUARD=0x2B, JOUTAI_ENDURE=0x2C, JOUTAI_MOVE_FAILED=0x40, JOUTAI_REFLECT=0x48 };
enum { SEQ_STATUS_PARAMETER=3, SEQ_MULTIPLE_STATUS=5, SEQ_ATTACK_MESSAGE_SHOWN=0x400 };
extern u8 fightSeqWazaWork[8];
extern u8 *UserWork;
extern u8 *_wazaSeqAdrs;
extern u32 ServerStatusFlag;
extern FIGHT_ACTION *fight_seq_now_fight_action_ptr;
u8 *fightSeq_GetWazaSeq(void);
void fightSeq_CallWazaSeq(u8 *sequence);
void fightSeqExec(FIGHT_ACTION *action, u8 *sequence, u8 mode);
void fightSeqPpdecMonomane(FightOutPokemon *pokemon, u8 slot, u8 mode);
s32 fightSeqCondChgActTypeToPokemonStatusId(u8 type);
void fightSeq_NextWazaSeq(s32 count);
void fightSeq_JumpWazaSeq(u8 *sequence);
void fightSeqCheckSetHensinBuffToMotoPokemon(FightOutPokemon *pokemon, u32 field);
void fightSeqTuikaAct(u8 action, u8 mode);
u8 fightSeqGetEffectAminFlag(void);
void fightSeqSetEffectAminFlag(u8 enabled);
u8 fightSeqCheckYubiwohuruWazaDataId(u16 wazaId);

FightOutPokemon *fightSeqSelectDefensePokemonPtr(FightOutPokemon *pokemon,
    u16 waza, long fightType, FightPokemonSelectCallback selectCallback,
    u8 selectPokemon, u8 checkRedirect, s8 targetType);

#ifdef __cplusplus
}
#endif

#endif
