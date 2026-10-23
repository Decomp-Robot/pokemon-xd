#ifndef FLOOR_H
#define FLOOR_H
#include <GSgfxTypes.h>
#include <types.h>
#include <GSthread.h>
#include <roomData.h>
#ifdef __cplusplus
extern "C" {
#endif
void floorMapSetDisp(long on);
u32 floorReadMakeModelResID(u32 resource);
u32 floorReadMakeLightResID(u32 resource);
void floorSetLightInfo(u32 resource, long count);

GSgfxLayerID floorGetMenuLayerID(void);
s32 floorGetNextFloorID();
s32 floorGetPrevFloorID();
s32 floorGetCurrentGroupID();
void floorLink(s32 floorID, s32 mode);
void floorSetFadeScript(s32 scriptID, s32 fadeID);
RoomData* floorDataBiosGetPtr(s32 floorID);
void GSfloorPop();

void floorWaitForChange(void);
u8 GSfloorIsPop(void);

void floorExecScriptInteger(int scriptID, int parameter1, int parameter2, int parameter3, int parameter4);
void floorExecScriptIntegerThread(int scriptID, int parameter1, int parameter2, int parameter3, int parameter4);
void floorExecScriptIntegerByName(const char *name, int parameter1, int parameter2, int parameter3, int parameter4);
void floorExecScriptIntegerByNameThread(const char *name, int parameter1, int parameter2, int parameter3, int parameter4);
int floorGetScriptResult(void);
void floorSetDispScriptInfo(u8 state);
void floorSetHumanColision(u8 enabled);
#ifdef __cplusplus
bool floorGetHumanColision(void);
#else
u8 floorGetHumanColision(void);
#endif
int floorTresureGetResID(u16 treasureID);
GSthread *floorCreateThread(u32 priority, u32 unk1, u32 flags, void (*entry)());
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace GSscript { class Tiga; }
GSscript::Tiga *floorGetCommonScript(void);
#endif

#endif
