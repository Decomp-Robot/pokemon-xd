#ifndef PEOPLE_INFO_BIOS_H
#define PEOPLE_INFO_BIOS_H

#include <people.h>

typedef tagPeopleInfoBios PeopleInfo;

extern "C" {
s8 peopleInfoBiosGetNeckIndex(PeopleInfo *info);
PeopleInfo *peopleInfoBiosGetPtr(unsigned long id);
void peopleInfoBiosGetMotion(PeopleInfo *info, int motionType, int *motionIndex, u8 *looping);
}

#endif
