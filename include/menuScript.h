#ifndef MENU_SCRIPT_H
#define MENU_SCRIPT_H

#include <types.h>
#include <Pokemon.h>

#ifdef __cplusplus
extern "C" {
#endif

void menuScriptInfoRelive(Pokemon *pokemon, u8 mode);
void menuScriptInfoEsa(void);
void menuScriptUsohachi2(void);
void menuScriptReadMail(long mailID);
void menuScriptReciveMail(void);
u8 menuScriptCheckMailOpen(void);

// Menu interfaces script commands call into. The tutorial, Eevee and
// move-tutor routines report a result to the calling script.
long menuScriptTutorial(void);
long menuScriptEevee(void);
void menuScriptOpenWorldMap(u16 mapID);
long menuScriptWazaMeijin(long wazaID);
long menuScriptWazaBasan(bool* tutorFlag);
u8 menuScriptWazaBasanCheck(void);
void menuScriptSetWazaOboetaFlag(u16 wazaID);
void menuScriptMoneyOpen(s16 x, s16 y);
void menuScriptMoneyClose(void);
void menuScriptUsohachi(void);
void menuScriptPdaAddFunc(u32 func);
void menuScriptCouponOpen(s16 x, s16 y);
void menuScriptCouponClose(void);

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
