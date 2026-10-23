#ifndef DBG_MENU_FIGHT_H
#define DBG_MENU_FIGHT_H

#include <fightPokemon.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tagWINDOW_WORK;
void dbmMenuFightButtonNormal(struct tagWINDOW_WORK *window);

s32 dbgMenuFightFightTrainerSerihuTestHaiboku2(void);
s32 dbgMenuFightFightTrainerSerihuTestHaiboku(void);
s32 dbgMenuFightFightTrainerSerihuTestSyouri(void);
s32 dbgMenuFightFightTrainerSerihuTestToujou(void);
s32 _dbgMenuFightFightTrainerSerihuTestSub(u16 trainerDataId, u8 kind);
s32 dbgMenuFightFightTrainerSerihuTest(void);
s32 dbgMenuFightEncountDataEditSub(u16 encountNo);
s32 dbgMenuFightStopTimeOut(void);
s32 dbgMenuFightStopHostLose(void);
s32 dbgMenuFightStopHostWin(void);
void _dbgMenuFightStopSub(u16 result);
s32 dbgMenuFightFightPokemonSelect5(void);
s32 dbgMenuFightFightPokemonSelect4(void);
s32 dbgMenuFightFightPokemonSelect3(void);
s32 dbgMenuFightFightPokemonSelect2(void);
s32 dbgMenuFightFightPokemonSelect1(void);
s32 dbgMenuFightFightPokemonSelect0(void);
s32 _dbgMenuFightFightPokemonSelectSub(FightPokemon *fightPokemon);
s32 dbgMenuFightFightTrainerPokemonPartDataEdit(void);
s32 dbgMenuFightFightTrainerAiDataEdit(void);
s32 dbgMenuFightFightTrainerAiAddsubCombo(void);
s32 dbgMenuFightFightTrainerAiAddsubItem(void);
s32 dbgMenuFightFightTrainerAiAddsubWazaUsewaza(void);
s32 dbgMenuFightFightTrainerAiAddsubWazaDefpokemon(void);
s32 dbgMenuFightFightTrainerAiAddsubWaza(void);
s32 dbgMenuFightFightTrainerAiAddsubIrekaeDasu(void);
s32 dbgMenuFightFightTrainerAiAddsubIrekaeModosu(void);
s32 dbgMenuFightFightTrainerAiAddsubIrekae(void);
s32 dbgMenuFightFightTrainerAiAddsubDataEdit(void);
s32 dbgMenuFightFightTrainerSelect1(void);
s32 dbgMenuFightFightTrainerSelect0(void);
s32 dbgMenuFightFightSideSelectHostEnemy(void);
s32 dbgMenuFightFightSideSelectHost(void);
s32 dbgMenuFightFightFloorEditTenkou(void);
s32 dbgMenuFightWazaEdit(void);
s32 dbgMenuFightFlagEdit(void);
s32 dbgMenuFightStop(void);
s32 dbgMenuFightFightFloorEdit(void);
s32 dbgMenuFightEncountSelect(void);
s32 dbgMenuFightEncount(u16 encount);
s32 dbgMenuFightMain(void);
void dbgMenuFightSetYubiwohuruWazaDataId(u16 id);
void dbgMenuFightSetSoubikoukaFlag(u8 value);
u16 dbgMenuFightGetYubiwohuruWazaDataId(void);
u8 dbgMenuFightGetSoubikoukaFlag(void);
void dbgMenuFightSetFightPokeStMenuOpenFlag(u8 value);
u8 dbgMenuFightGetFightPokeStMenuOpenFlag(void);
void dbgMenuFightSetFightTrainerAiLastValueReviseFlag(u8 value);
u8 dbgMenuFightGetFightTrainerAiLastValueReviseFlag(void);
void dbgMenuFightSetFightTrainerAiFlowDispFlag(u8 value);
u8 dbgMenuFightGetFightTrainerAiFlowDispFlag(void);
void dbgMenuFightSetFightDebugMenuOpenFlag(u8 value);
u8 dbgMenuFightGetFightDebugMenuOpenFlag(void);
void dbgMenuFightSetAllFightTrainerOperateFlag(u8 value);
u8 dbgMenuFightGetAllFightTrainerOperateFlag(void);
void dbgMenuFightSetMsgKeyWaitFlag(u8 value);
u8 dbgMenuFightGetMsgKeyWaitFlag(void);
void dbgMenuFightSetTuikakoukaFlag(u8 value);
u8 dbgMenuFightGetTuikakoukaFlag(void);
void dbgMenuFightSetMsgSpeed(u8 speed);
u8 dbgMenuFightGetMsgSpeed(void);
u32 dbgMenuFightGetMsgSpeedToFrame(u32 frame);
s32 dbgMenuFightGetFightTrainerAiAddsubValueDataId(u16 id, u8 close);
u16 *_dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub(u32 id);
s32 dbgMenuFightGetFightTrainerPokemonPartDataId(u16 id);
u16 *_dbgMenuFightGetFightTrainerPokemonPartDataIdSub(u32 id);
s32 dbgMenuFightGetWazaDataId(u16 id);
u16 *_dbgMenuFightGetWazaDataIdSub(u32 id);
s32 dbgMenuFightGetWazaTypeId(u8 id);
u16 *_dbgMenuFightGetWazaTypeIdSub(u32 id);
s32 dbgMenuFightGetZokuseiDataId(u16 id);
u16 *_dbgMenuFightGetZokuseiDataIdSub(u32 id);
s32 dbgMenuFightGetGsinputDevId(u16 id);
u16 *_dbgMenuFightGetGsinputDevIdSub(u32 id);
s32 dbgMenuFightGetFightTrainerDataId(u16 id);
u16 *_dbgMenuFightGetFightTrainerDataIdSub(u32 id);
s32 dbgMenuFightGetPokemonEntry(u8 id);
u16 *_dbgMenuFightGetPokemonEntrySub(u32 id);
s32 dbgMenuFightGetPokemonFightType(u8 id);
u16 *_dbgMenuFightGetPokemonFightTypeSub(u32 id);
s32 dbgMenuFightGetFightTypeDataId(u8 id);
u16 *_dbgMenuFightGetFightTypeDataIdSub(u32 id);
s32 dbgMenuFightGetFightKindDataId(u8 id);
u16 *_dbgMenuFightGetFightKindDataIdSub(u32 id);
s32 dbgMenuFightGetFightFloorDataId(u16 id);
u16 *_dbgMenuFightGetFightFloorDataIdSub(u32 id);
s32 dbgMenuFightGetFightDeckDataId(u16 id);
u16 *_dbgMenuFightGetFightDeckDataIdSub(u32 id);
s32 dbgMenuFightGetFightEncountDataId(u16 id);
u16 *_dbgMenuFightGetFightEncountDataIdSub(u32 id);
s32 dbgMenuFightTrainerAiAddsubDataComboEditSub(void);
s32 dbgMenuFightTrainerAiAddsubDataItemEditSub(void);
s32 dbgMenuFightTrainerAiAddsubDataWazaUsewazaEditSub(void);
s32 dbgMenuFightTrainerAiAddsubDataWazaDefpokemonEditSub(void);
s32 dbgMenuFightTrainerAiAddsubDataIrekaeDasuEditSub(void);
s32 dbgMenuFightTrainerAiAddsubDataIrekaeModosuEditSub(void);
s32 dbgMenuFightTrainerPokemonPartDataEditSub(u16 index);
s32 dbgMenuFightTrainerAiDataEditSub(u16 index);
void dbgMenuFightTrainerDataStatusInputDigit(u16 id, u16 field, u16 sub, s32 max, s32 min);
void dbgMenuFightTrainerDataStatusInputFlag(u16 id, u16 field);
s32 dbgMenuFightWazaEditSub(u16 dataId);

#ifdef __cplusplus
}
#endif

#endif
