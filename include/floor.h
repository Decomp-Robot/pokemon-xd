#ifndef FLOOR_H
#define FLOOR_H
#include <GSgfxTypes.h>
struct GStexture;
#include <types.h>
#include <GSthread.h>
#include <roomData.h>
#include <floorManager.h>

// Alignment of the resource allocations the reader makes for the archives of a
// stage; the length of an archive is rounded up to it.
#define FLOOR_READ_ALIGN 0x20

enum FloorMapResourceKind {
    FLOOR_MAP_RESOURCE_MODEL = 1,
    FLOOR_MAP_RESOURCE_LIGHT = 2
};

struct GSfloor_dd_;
struct _GSmodel;

#ifdef __cplusplus
extern "C" {
#endif
struct _GSmodel *floorOpenObject(long id);
struct _GSmodel *floorGetResource(u32 floorID, u32 resourceID);
void floorChangePos(u32 floorID, s16 rotY, f32 x, f32 y, f32 z);
void floorMapSetDisp(long on);
u32 floorReadMakeFogResID(u32 resource);
u32 floorReadMakeCameraResID(u32 resource);
u32 floorReadMakeModelResID(u32 resource);
u32 floorReadMakeLightResID(u32 resource);
u32 floorReadMakeObjResID(u32 resource);
void floorSetLightInfo(u32 resource, long count);
void floorInitMap(u32 group, u32 res);
void floorExecSoundScript(void);
s32 floorEventExecScript(s8 kind, s32 scriptID);

GSgfxLayerID floorGetMenuLayerID(void);
s32 floorGetNextFloorID(void);
s32 floorGetPrevFloorID(void);
u32 floorGetCurrentGroupID(void);
void floorLink(s32 floorID, s32 mode);
void floorSetFadeScript(s32 scriptID, s32 fadeID);
GSfloor_dd_* floorDataBiosGetPtr(u32 floorID);
void GSfloorPop(void);
void floorSetFloorChangeInfo(u32 floorID, u32 index);
void floorPopAndChange(u32 floorID);
void floorLinkWithSE(u32 floorID, u32 index, u32 soundID);
void floorSetPrevFloorID(u32 floorID);
void floorClearReturnPoint(void);
void floorWaitForChange(void);
void floorUnblockMainScript(void);
void floorBlockMainScript(void);
u8 GSfloorIsPop(void);
int floorExecScriptInteger(int scriptID, int parameter1, int parameter2, int parameter3, int parameter4);
long floorExecScriptIntegerThread(int scriptID, int parameter1, int parameter2, int parameter3, int parameter4);
int floorExecScriptIntegerByName(const char *name, int parameter1, int parameter2, int parameter3, int parameter4);
long floorExecScriptIntegerByNameThread(const char *name, int parameter1, int parameter2, int parameter3, int parameter4);
int floorExecScriptRes(int scriptID, unsigned long groupID, unsigned long resourceID);
long floorExecScriptResThread(int scriptID, unsigned long groupID, unsigned long resourceID);
int floorGetScriptResult(void);
#ifdef __cplusplus
bool floorIsRunningThread(long threadHandle);
#else
u8 floorIsRunningThread(long threadHandle);
#endif
void floorBlockScriptThread(long threadHandle);
void floorUnblockScriptThread(long threadHandle);
int floorGetScriptMode(int scriptID);
int floorGetCurentScriptMode(void);
void floorSetDispScriptInfo(u8 state);
u8 floorIsDispScriptInfo(void);
s32 floorGetLocalHeapHandle(void);
struct GStexture *floorGetActiveOffscreenTexture(void);
struct _GSmodel *floorOpenModel(u32 groupID, u32 resourceID);
s32 floorMoviePlay(u32 movieID, s32 flags, s32 loop, u32 targetModelResourceID, u16 width, u16 height);
void floorOffscreenPlayWait(void);
s32 floorSetOffscreenIndex(s32 index);
void floorReleaseOffscreen(void);
void floorUpdateOffscreen(void);
void floorInitOffscreen(struct GSfloor_dd_ *floor);
void floorOffscreenMainInit(void);
void floorSetHumanColision(u8 enabled);
#ifdef __cplusplus
bool floorGetHumanColision(void);
#else
u8 floorGetHumanColision(void);
#endif
u32 floorTresureGetResID(u16 tableIndex);
GSthread *floorCreateThread(s32 priority, u32 stackSize, s32 terminateOnExit, GSthreadEntry entry);
GSthread *floorCreateThreadLocal(s32 priority, u32 stackSize, s32 terminateOnExit, GSthreadEntry entry);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace GSscript { class Tiga; }
GSscript::Tiga *floorGetCommonScript(void);
GSscript::Tiga *floorGetCurentScript(void);
void EvlogSet(s8 kind, u32 scriptID);
#endif

#endif
