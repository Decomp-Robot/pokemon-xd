#ifndef SCRIPT_H
#define SCRIPT_H

#include <types.h>

struct Pokemon;
#ifdef __cplusplus
extern "C" {
#endif
void scriptLightSetDisp(unsigned long resourceId, int display);
void scriptPlayPokemonNakigoe(u16);
bool scriptSoundOpenPlayME(int, int, u8);
void scriptModelSetDisp(int, int);
void scriptModelSetMotion(int, int, int, int);
void scriptModelStopMotion(int);
int scriptModelWaitMotion(int, int);
int scriptModelAttach(int, int, int);
int scriptModelDetach(int);
void scriptModelSetParticle(int, int, u8);
void scriptModelDeleteParticle(int, bool);
void scriptModelSetMotionRate(int, int, int, float, int);
int scriptModelSetEndedMotion(int, int, bool);
void scriptModelPartPaticle(int, int, int);
void scriptModelSetFractionalFramesFlag(int, bool);
int controlTV(int);
int controlTVScreen(void);
int scriptGetRandom(int);
void scriptSetCol(int, bool);
u16 scriptGetEventColID(void);
void scriptSetEventCol(bool);
int scriptEncount(u16, bool, int);
int scriptModelOpen(int);
void scriptModelClose(int);
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
