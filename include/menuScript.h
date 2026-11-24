#ifndef MENU_SCRIPT_H
#define MENU_SCRIPT_H

#include <types.h>
#include <Pokemon.h>
#include <winSprite.h>

#ifdef __cplusplus
extern "C" {
#endif

void menuScriptInfoRelive(Pokemon *work, u8 mode);
void menuScriptInfoEsa(void);
void menuScriptUsohachi2(void);
void menuScriptReadMail(s32 mailID);
void menuScriptReciveMail(void);
void menuScriptSetMailOpen(u8 open);
u8 menuScriptCheckMailOpen(void);

// Menu interfaces script commands call into. The tutorial, Eevee and
// move-tutor routines report a result to the calling script.
s32 menuScriptTutorial(void);
s32 menuScriptEevee(void);
void menuScriptOpenWorldMap(u16 mapID);
s32 menuScriptWazaMeijin(s32 partySlot);
s32 menuScriptWazaBasan(bool* tutorFlag);
u8 menuScriptWazaBasanCheck(void);
void menuScriptSetWazaOboetaFlag(u16 wazaID);
void menuScriptMoneyOpen(s16 x, s16 y);
void menuScriptMoneyClose(void);
void menuScriptUsohachi(void);
void menuScriptPdaAddFunc(u32 func);
void menuScriptCouponOpen(s16 x, s16 y);
void menuScriptCouponClose(void);

void menuScriptSetOpenWorldMapFlag(u8 flag);
u8 menuScriptGetOpenWorldMapFlag(void);
u16 menuScriptOpenWorldMapGetMapNumber(void);

void menuScriptMailOpenInit(void);
void menuScriptMailOpenMain(void);
void menuScriptMailOpenExit(void);
void menuScriptPdaAddFuncInit(void);
void menuScriptPdaAddFuncMain(void);
void menuScriptPdaAddFuncExit(void);

void menuScriptTutorialHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
s32 menuScriptWazaOshieCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuScriptWazaOshieHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuScriptWazaOshieCtrl(tagWINDOW_WORK *window);
void menuScriptTutorialCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuScriptTutorialCtrl(tagWINDOW_WORK *window);
void menuScriptEeveeHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuScriptEeveeCtrl(tagWINDOW_WORK *window);
void menuScriptUsohachiCtrl(tagWINDOW_WORK *window);
void menuUsohachiHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
s32 menuScriptMoneyDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
struct WAZAOSHIEDEF {
    u16 wazaID;
    u32 nameID;
    u8 pp;
    u8 zokuseiDataID;
    s16 iryoku;
    u8 avg;
    u32 docID;
    u8 tutorFlagSet;
};

void _getWazaData(u16 wazaID, WAZAOSHIEDEF *work);
#endif

#endif
