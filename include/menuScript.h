#ifndef MENU_SCRIPT_H
#define MENU_SCRIPT_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void menuScriptUsohachi2(void);
void menuScriptReadMail(long mailID);
void menuScriptReciveMail(void);
u8 menuScriptCheckMailOpen(void);

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
