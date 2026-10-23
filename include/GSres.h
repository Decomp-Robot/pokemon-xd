#ifndef GS_RES_H
#define GS_RES_H

#ifdef __cplusplus
extern "C" {
#endif

void *GSresGetResource(unsigned long group, unsigned long resourceID);
void *GSresGetResourceAllGroup(unsigned long resourceID);
void GSresFreeResource(unsigned long group, unsigned long resourceID);
void GSresFreeResourceAllGroup(unsigned long resourceID);

#ifdef __cplusplus
}
#endif

#endif
