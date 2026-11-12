#ifndef MENU_RELIVE_HALL_H
#define MENU_RELIVE_HALL_H

#include <GSgfxTypes.h>

class CMenuGlobalState;
struct Pokemon;

void relivehallDraw9StageInfo(int stage);
void relivehallDraw9StageNumber(int stage);
void relivehallDrawStageInfoCursor(long stage, int tempo, int dancers, Pokemon *visitor, bool showCursor);

extern long pcReliveHallSoundTbl[];
extern GSgfxColAu8 relivehallAisyou2ColorTbl[];

void menuReliveHallExecFromPcBox(CMenuGlobalState &state);
long relivehallGetLastViewStage();
void relivehallSetLastViewStage(long stage);

#endif
