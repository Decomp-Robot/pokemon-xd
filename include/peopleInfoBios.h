#ifndef PEOPLE_INFO_BIOS_H
#define PEOPLE_INFO_BIOS_H

#include <types.h>

struct PeopleInfo;

extern "C" {
PeopleInfo *peopleInfoBiosGetPtr(unsigned long id);
void peopleInfoBiosGetMotion(PeopleInfo *info, int motionType, int *motionIndex, u8 *looping);
}

#endif
