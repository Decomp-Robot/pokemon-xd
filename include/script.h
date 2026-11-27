#ifndef SCRIPT_H
#define SCRIPT_H

#include <types.h>
#include <scriptModel.h>

struct Pokemon;
#ifdef __cplusplus
extern "C" {
#endif
void scriptOpenItemMenu(void);
void scriptLightSetDisp(unsigned long resourceId, int display);
void scriptPlayPokemonNakigoe(u16);
long scriptContinueSave(void);
long scriptBattleMountainEnd(void);
bool scriptSoundOpenPlayME(int, int, u8);
int controlTV(int);
int controlTVScreen(void);
int scriptGetRandom(int);
void scriptSetCol(int, bool);
int scriptGetEventColID(void);
int scriptSetEventColID(int colID);
void scriptSetEventCol(bool);
int scriptEncount(u16, bool, int);
int scriptPokemonRememberWaza(struct Pokemon*, int);
int scriptPokemonTeachWazaIndex(int);
void scriptWaitTime(float);
bool scriptSetMemoFlag(void);
int scriptIsButtonPush(void);
int scriptIsMoveButtonPush(void);
int scriptIsTrigerPush(void);
u16 *scriptGetPokemonNickName(int slot);
u16 scriptGetDarkPokemonNum(void);
u16 scriptGetPokemonNum(void);
int scriptFloorCharSetPos(unsigned long index, float x, float y, float z);
int scriptFloorCharSetDisp(unsigned long index, unsigned long visible);
void scriptGlowStart(float, u8, u8, u8, u8, u8);
void scriptGlowEnd(void);
void scriptSetLensFlare(int, u8, float, float, float);
int scriptGetDarkPointZeroPokemonNum(void);
int scriptSetEndedMotion(unsigned long group, unsigned long id, long motion, u8 wait);
#ifdef __cplusplus
}
#endif

#endif
