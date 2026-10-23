#ifndef PEOPLE_SAVE_H
#define PEOPLE_SAVE_H
#include <people.h>
#include <GSmemory.h>
#include <hero.h>
#include <peopleCollision.h>
#include <GSlog.h>
#include "GSBinStream.h"
extern "C" {
unsigned long peopleGetAnywaySaveSize();
void peopleRestoreAnywaySaveData(void*, unsigned long);
void peopleMakeAnywaySaveData(void*, unsigned long);
int peopleOpen(unsigned long, unsigned long, unsigned long);
void peopleClose(unsigned long, unsigned long, bool);
void peopleAddCollision(unsigned long, unsigned long);
void peopleSetDisp(unsigned long, unsigned long, bool);
}
void _peopleRestoreAnywaySaveData(GSBinReadStream*);
void _makeAnywaySaveData(GSBinWriteStream*);
#endif
