#ifndef MENU_CB_SUB1_H
#define MENU_CB_SUB1_H

#include <types.h>
#include <Pokemon.h>
#include <winSprite.h>
#include <window.h>

#ifdef __cplusplus
long _menuCB_NotDarkWaza(long wazaID);
void _menuCB_ErrorTAMAGOCheck();
void _menuCBGCDelPokemon();
extern "C" {
void menuCB2DItemMsgDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
u32 menuCBRegionMsgID(u32 msgID);
}

void _menuCB2DPokemonNewSet2(unsigned long bank, unsigned long page, unsigned long count, unsigned char mode);
void menuCB2DPokemonPageScroll(unsigned char key);
void _menuCB_errorMsgDisp2(unsigned long msgID);
void _menuCB_errorMsgDisp(unsigned long msgID);
void menuCBBattleMenuOpen(unsigned long load);
void menuCBBattleMenuClose(unsigned long unload);
extern "C" {
}

u8 menuCBPokemonMsgCheck(unsigned long bank, unsigned long index);
u8 _menuCBPokemonEntryGetlevel(Pokemon *pokemon);
unsigned long menuCBPokemonGetIndex(unsigned long menuID, unsigned long index);
extern "C" {
Pokemon *menuCBPokemonGetPtr(unsigned long bank, unsigned long index);
void menuCBDrawStringCenter(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 *str, long x, long y, unsigned long msgID);
}

void _drawStrings(tagWINDOW_WORK *window, u16 *str, long x, long y, unsigned long msgID);
extern "C" {
void menuCB_SE_Stop(unsigned long seID);
void menuCBSEPlay(unsigned long seID);
void menuCBSpriteAlphaSet(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned char flag);
void menuCBWindowColorSave(tagWINDOW_WORK *window);
void menuCBAccessFlashDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned char flag);
void menuCB_GCPokemonHPandWAZA_MAX(void);
void menuCBTrainerNameDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 trainer);
void menuCBSpriteColorSet(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned char flag);
void menuCBSpriteColorIfMatch(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 value);
void menuCBTrainerMarksDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuCBRegulationItemDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuCBStatusNumDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 kind);
void menuCBMessageYCenterDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long msgID);
void menuCBRegionMsgDrawOrSet(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long msgID);
void menuCBMsgDrawOnSprite(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long msgID);
void menuCBPokemonEntryOpen(u8 load);
void menuCBPokemonEntryClose(u8 unload);
void menuCBRuleCursorWindowColor(tagWINDOW_WORK *window, s32 index);
void menuCBEntryCursorWindowColor(tagWINDOW_WORK *window, s32 index);
void menuCB_MenuDispCheckONorOFF(tagWINDOW_WORK *window, bool on);
void menuCB_SpriteDispCheckONorOFF(tagSPRITE_WORK *sprite, bool on);
void menuCBPokemonEntryTakeItem(void);
void menuCBPokemonEntryGiveItem(void);
void menuCBRuleWindowDisp(tagWINDOW_WORK *window, s32 trainer);
void menuCBPortTrainerNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 trainer);
void menuCBPokemonEntryStatusOpen(void);
s32 menuCBPokemonEntryActionSelect(void);
void menuCBRuleSelect(void);
void menuCBPokemonEntryCheck(void);
void menuCB_WaitGCControler1(void);
void menuCB_GBADisconnectedErrorJob(void);
void menuCBPokemonEntryGCControlDisp(tagSPRITE_WORK *sprite, s32 slot);
void menuCBPokemonEntryFacePlace(tagSPRITE_WORK *sprite, s32 port, u32 slot);
void menuCBPokemonEntryFaceDisp(tagSPRITE_WORK *sprite, s32 port, u32 slot);
u8 menuCBPokemonEntryDispPokemonFace_AFTER(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 arg, s32 slot);
u8 menuCBPokemonEntryDispPokemonFace_BEFORE(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 arg, s32 slot);
void menuCB_BattleEntryPokemonSelectDisp(tagSPRITE_WORK *sprite, s32 bank, s32 index);
s32 menuCB_GCStatusDispMode(tagSPRITE_WORK *sprite, s32 mode);
u8 menuCBPokemonEntryDisp(tagSPRITE_WORK *sprite);
void menuCB2DPokemonSetSource(void);
void menuCBPokemonReturnSelected(void);
void menuCBPokemonEntryAdd(void);
void menuCB2DPokemonSetPage(void);
void menuCBPCBOXTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 box, s32 x);
void menuCBReliveTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID, s32 x);
void menuCBPokemonEntryTitleNameDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID, s32 x);
u8 menuCBPokemonCheck(unsigned long bank, unsigned long index);
void menuCBPokemonItemName(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode);
void menuCBPokemonWazaNAME(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long moveSlot);
void menuCBPokemonWazaTYPE(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long moveSlot);
void menuCBPokemonTYPE2(tagWINDOW_WORK *window, unsigned long bank, unsigned long index);
void menuCBPokemonTYPE1(tagWINDOW_WORK *window, unsigned long bank, unsigned long index);
void menuCBPokemonStatusDisp(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode, unsigned long stat);
void menuCBPokemonTOKUSEI(tagWINDOW_WORK *window, unsigned long bank, unsigned long index);
void menuCBPokemonHP(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode);
void menuCBPokemonLevel(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode);
void menuCBPokemonNameOrg(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode);
void menuCBPokemonSexOnly(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode);
void menuCBPokemonNameOnly(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode);
}

void _menuCBPokemonName(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode, bool showNidoranSex);
extern "C" {
void menuCBPokemonName(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode);
void menuCBPokemonName_nido(tagWINDOW_WORK *window, unsigned long bank, unsigned long index, unsigned long mode);
void menuCB2DPokemonItemDisp(tagSPRITE_WORK *sprite, unsigned long bank, unsigned long index);
void menuCB2DPokemonClearSHIFT(unsigned long direction);
void menuCB2DPokemonALLClear();
void menuCB2DPokemonDisp2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long bank, unsigned long index);
void menuCB2DPokemonDisp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long bank, unsigned long index);
u8 menuCBBattleBANextMenu(tagWINDOW_WORK *window);
void menuCBBattleBABatuMark(tagSPRITE_WORK *sprite, s32 port, s32 slot);
void menuCBBattleEndSetPokemonHP();
void menuCB_BattleBA_HPBarDispSet(tagSPRITE_WORK *sprite, s32 trainer, s32 slot);
void menuCB_BattleBA_FacePanelSet(tagSPRITE_WORK *sprite, s32 trainer, s32 slot);
void menuCB_DispEffect(tagWINDOW_WORK *window);
u8 menuCB_BattleBA_PokemonEntryNum();
void menuCB_PrintXP(tagSPRITE_WORK *sprite, s32 trainer);
void menuCBBattleStartDispTrainerFaceSub(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 trainer);
}

#endif

#endif
