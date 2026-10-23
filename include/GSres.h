#ifndef GS_RES_H
#define GS_RES_H

#include <types.h>

// Release callback of an entry of the resource system: resFreeResource hands
// it the resource the entry has stored together with the group id and
// resource id the resource is registered under, and gives the entry up once
// the callback answers true.
// (Spelled with unsigned long so the type is identical in every TU: u32
// resolves to unsigned long under the SDK headers but to unsigned int
// under the game header alone.)
typedef bool (*GSresFreeFunc)(void *resource, unsigned long group,
                              unsigned long res);

#ifdef __cplusplus
extern "C" {
#endif

void *GSresGetResource(unsigned long group, unsigned long resourceID);
void *GSresGetResourceAllGroup(unsigned long resourceID);
void GSresFreeResource(unsigned long group, unsigned long resourceID);
void GSresFreeResourceAllGroup(unsigned long resourceID);
void GSresLockResource(u32 group, u32 res);
void GSresUnlockResource(u32 group, u32 res);
void *GSresAllocResourceAlign(u32 size, u32 align, u32 group, u32 res,
                              GSresFreeFunc freeFunc);
void GSresRegisterResource(void *resource, u32 group, u32 res,
                           GSresFreeFunc freeFunc);

#ifdef __cplusplus
}
#endif

#endif
