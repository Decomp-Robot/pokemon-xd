#ifndef GS_RES_H
#define GS_RES_H

#include <types.h>
#include <bool.h>

// Handle of a registered resource: the group and resource id pair the
// resource system resolves a resource with.
struct GSresID {
    unsigned long groupId;
    unsigned long resourceId;
#ifdef __cplusplus
    void set(unsigned long group, unsigned long resource);
    void clear();
#endif
};

// Release callback of an entry of the resource system: resFreeResource hands
// it the resource the entry has stored together with the group id and
// resource id the resource is registered under, and gives the entry up once
// the callback answers true.
typedef bool (*GSresFreeFunc)(void *resource, unsigned long group,
                              unsigned long res);

#ifdef __cplusplus
extern "C" {
#endif

void GSresInit(u32 entryCount);
void GSresIsUniqueID(u32 group, u32 resourceID, u8 unique);
void GSresDumpGroup(u32 group);
void GSresDumpAll(void);
void GSresChangeResourceAdress(void *address, u32 group, u32 resourceID, GSresFreeFunc freeFunc);
void *GSresAllocResource(u32 size, u32 group, u32 resourceID, GSresFreeFunc freeFunc);
void *GSresGetResource(unsigned long group, unsigned long resourceID);
void *GSresGetResourceAllGroup(unsigned long resourceID);
void GSresFreeResource(unsigned long group, unsigned long resourceID);
void GSresFreeResourceAllGroup(unsigned long resourceID);
void GSresFreeGroup(u32 groupID);
void GSresLockResource(u32 group, u32 res);
void GSresUnlockResource(u32 group, u32 res);
void *GSresAllocResourceAlign(u32 size, u16 align, u32 group, u32 res,
                              GSresFreeFunc freeFunc);
void GSresRegisterResource(void *resource, u32 group, u32 res,
                           GSresFreeFunc freeFunc);

#ifdef __cplusplus
}
#endif

#endif
