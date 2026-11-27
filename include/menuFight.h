#ifndef MENU_FIGHT_H
#define MENU_FIGHT_H

#include <types.h>

enum MenuFightMessage {
    MSG_FIGHT_COMMAND = 17055,
    MSG_FIGHT_COMMAND_BINGO = 5001,
    MSG_FIGHT_COMMAND_BATTLE_DISK = 17468,
    MSG_FIGHT_COMMAND_TRAINER = 17053,
    MSG_FIGHT_COMMAND_LINK = 17056,
    MSG_NO_VALUE_TEXT = 15094,
    MSG_SLASH_TEXT = 17092,
    MSG_PP_LABEL = 17105,
    MSG_LEVEL_LABEL = 17108,
    MSG_HP_LABEL = 17109,
    MSG_ABILITY_LABEL = 17242,
    MSG_KANE = 17230,
    MSG_PP_NUMBER = 17240,
    MSG_PP_NO_VALUE = 15093,
    MSG_PP_SPECIAL = 15057,
    MSG_FIGHT_BINGO_BALL_EMPTY = 20498,
    MSG_FIGHT_SNATCH_USED = 20485,
    MSG_FIGHT_STATUS_NAME = 17091,
    MSG_FIGHT_STATUS_NAME_2 = 17094,
    MSG_FIGHT_WAZA_BAN_NO_PP = 20198,
    MSG_FIGHT_WAZA_BAN_SHADOW_TAG = 20199,
    MSG_FIGHT_WAZA_BAN_MAGNET_PULL = 20200,
    MSG_FIGHT_WAZA_BAN_ARENA_TRAP = 20201,
    MSG_FIGHT_CHANGE_REPLACE_SET = 20232,
    MSG_FIGHT_WAZA_BAN_SOUUBI_ITEM = 20290,
    MSG_FIGHT_WAZA_BAN_FORCED_WAZA = 20378,
    MSG_FIGHT_CHANGE_OUT_BLOCKED = 20424,
    MSG_FIGHT_CHANGE_FIGHT_OUT = 20425,
    MSG_FIGHT_CHANGE_REFUSED = 20426,
    MSG_FIGHT_CHANGE_CANNOT_FIGHT = 20427,
};

struct FIGHT_TRAINER;
struct FightOutPokemon;
struct MENU_WAZA_STATUS;
struct tagWINDOW_WORK;
struct tagSPRITE_WORK;

struct MENU_FIGHT_TARGET_SLOT {
    u32 mUnused0;
    s32 mMenuId;
    u16 *mNickname;
};

enum MenuFightTargetMode {
    MENU_FIGHT_TARGET_MODE_NORMAL,
    MENU_FIGHT_TARGET_MODE_SUB
};

struct FightTargetSelectParam {
    MENU_FIGHT_TARGET_SLOT mTargets[4];
    u8 mUnused30;
    u8 mMode;
    u16 mPadding32;
    FightOutPokemon *mOutgoing;
};

enum MenuFightCommandKind {
    MENU_FIGHT_COMMAND_KIND_NORMAL,
    MENU_FIGHT_COMMAND_KIND_GBA
};

struct FightMenuTopParam {
    u16 mNickname[11];
    u8 mCanPlaySeq;
    u8 mMenuKind;
};

#ifdef __cplusplus
extern "C" {
#endif

void menuFightCloseAll(void);

enum MenuFightStatusSequence {
    MENU_SEQ_FIGHT_STATUS_LEFT_DOWN_OUT = 413,
    MENU_SEQ_FIGHT_STATUS_UP_RIGHT_OUT = 417,
    MENU_SEQ_FIGHT_STATUS_LEFT_DOWN_IN = 421,
    MENU_SEQ_FIGHT_STATUS_UP_RIGHT_IN = 425
};
s32 menuFightGetStatus(void);
void menuFightSetStatus(u32 status);
s32 menuFightGetKeyWaitMenuId(FIGHT_TRAINER *trainer, u16 fightType);
void menuFightSetSeq(void);
s32 menuFightChkOpenBall(s32 menuId);
s32 menuFightChkSeq(void);
void menuFightOpenTimeup(void);

s32 menuFightCloseTarget(bool wait);
s32 menuFightCloseTop(bool wait);
s32 menuFightCloseWaza(bool wait);
s32 menuFightMainCtrl(tagWINDOW_WORK *window);
s32 menuFightWazaCtrl(tagWINDOW_WORK *window);
s32 menuFightWazaButton(tagWINDOW_WORK *window);
u8 menuPokemonCheckPokemonChange(FightOutPokemon *outgoing, FIGHT_TRAINER *trainer, s32 index);

s32 menuFightOpenTarget(FightTargetSelectParam *param, void *result, bool wait);
s32 menuFightOpenTop(FIGHT_TRAINER *trainer, FightMenuTopParam *param, void *result, bool wait);
s32 menuFightOpenWaza(void *target, MENU_WAZA_STATUS *status, void *result, bool wait);
s32 menuFightOpenPokemon(FIGHT_TRAINER *trainer, FightOutPokemon *outgoing, u16 fightType, u8 battleSelection, u8 flag);
long menuFightOpenGBAIrekae(u32 channel, FIGHT_TRAINER *trainer, u32 slot, u16 fightType);
long menuFightOpenGBAMain(u32 channel, FIGHT_TRAINER *trainer, u32 slot, u16 fightType);

void menuFightDrawTurn(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawTimer(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
s32 menuFightCtrlTimer(tagWINDOW_WORK *window);
void menuFightButtonTimeOutCheck(tagWINDOW_WORK *window);
u8 menuFightCloseCheckTotalTimer(void);
void menuFightCloseTotalTimer(void);
void menuFightOpenTotalTimer(void);
u8 menuFightCloseCheckCountDown(void);
void menuFightCloseCountDown(void);
void menuFightOpenCountDown(void);

void menuFightDrawTargetSecret(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightButtonTargetSecret(tagWINDOW_WORK *window);
void menuFightDrawTargetCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawTargetI(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawBall(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawCmdMsg(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawIPokemonStatus(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawIwaitStatus(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawKane(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawNew(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawPP(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawSecretPokemon(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawSecretPokemonStatus(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawSecretSelect(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawSecretWazaDoc(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawSecretWazaSelect(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawType(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightDrawWaza(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuFightButtonIwaza1p(tagWINDOW_WORK *window);
void menuFightButtonSecretKousan(tagWINDOW_WORK *window);
void menuFightButtonSecretMain(tagWINDOW_WORK *window);
void menuFightButtonSecretPokemonTop(tagWINDOW_WORK *window);
void menuFightButtonSecretWazaTop(tagWINDOW_WORK *window);
void menuFightCurIkousan(tagWINDOW_WORK *window);
s32 menuFightCtrlBall(tagWINDOW_WORK *window);
s32 menuFightCtrlSecretMain(tagWINDOW_WORK *window);
s32 menuFightCtrlSecretPokemonTop(tagWINDOW_WORK *window);
s32 menuFightCtrlSecretWazaTop(tagWINDOW_WORK *window);
void menuFightCtrlTarget(tagWINDOW_WORK *window);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
u16 _menuFightGetRightStickInfo(long stickNo, bool trigger);
u16 _menuFightGetLeftKeyInfo(long stickNo);
bool _menuFightCheckTimeOut(void);
s32 _menuFightIsUse(MENU_WAZA_STATUS *status, u16 wazaIndex);
#endif

#endif
