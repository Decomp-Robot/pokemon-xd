#ifndef FIGHTGSFLOOR_H
#define FIGHTGSFLOOR_H

#ifdef __cplusplus
extern "C" {
#endif

void fightGSfloorFree(void);
unsigned long fightGSfloorGetPushDataSize(void);
void fightGSfloorPushData(void *data, unsigned long size);
void fightGSfloorPopData(void *data, unsigned long size);

#ifdef __cplusplus
}
#endif

#endif
