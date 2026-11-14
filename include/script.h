#ifndef SCRIPT_H
#define SCRIPT_H

#include <types.h>
#include <scriptModel.h>

struct Pokemon;
#ifdef __cplusplus
extern "C" {
#endif
void scriptLightSetDisp(unsigned long resourceId, int display);
void scriptPlayPokemonNakigoe(u16);
bool scriptSoundOpenPlayME(int, int, u8);
int controlTV(int);
int controlTVScreen(void);
int scriptGetRandom(int);
void scriptSetCol(int, bool);
u16 scriptGetEventColID(void);
void scriptSetEventCol(bool);
int scriptEncount(u16, bool, int);
int scriptPokemonRememberWaza(struct Pokemon*, int);
int scriptPokemonTeachWazaIndex(int);
void scriptWaitTime(float);
bool scriptSetMemoFlag(void);
int scriptIsButtonPush(void);
int scriptIsMoveButtonPush(void);
int scriptIsTrigerPush(void);
void scriptGlowStart(float, u8, u8, u8, u8, u8);
void scriptGlowEnd(void);
void scriptSetLensFlare(int, u8, float, float, float);
#ifdef __cplusplus
}
#endif

#endif
