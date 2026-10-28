#include <GSres.h>
#include <GSmemman.h>
#include <GSmem.h>
#include <GSlog.h>

struct _resEntry {
    u8 owned;
    u8 locked;
    u8 unique;
    void* address;
    u32 group;
    u32 resourceID;
    GSresFreeFunc freeFunc;
    _resEntry* next;
    _resEntry* prev;
};

_resEntry* _resStructList;
GSmemPoolAllocator* _resEntryAllocator;

_resEntry* resFindResource(unsigned long group, unsigned long resourceID);
_resEntry* resFreeResource(_resEntry* entry, bool remove);

extern "C" void GSresIsUniqueID(u32 group, u32 resourceID, u8 unique)
{
    _resEntry* entry = resFindResource(group, resourceID);
    if (entry) entry->unique = unique;
}
extern "C" void GSresUnlockResource(u32 group, u32 resourceID)
{
    _resEntry* entry = resFindResource(group, resourceID);
    if (entry) entry->locked = 0;
}
extern "C" void GSresLockResource(u32 group, u32 resourceID)
{
    _resEntry* entry = resFindResource(group, resourceID);
    if (entry) entry->locked = 1;
}
extern "C" void GSresDumpGroup(u32 group)
{
    _resEntry* entry = _resStructList;
    GSmemDumpPool(_resEntryAllocator);
    GSlogWrite("alloc registMem   grpID resID      freeFunc\n");
    GSlogWrite("-----------------------------------------------\n");
    while (entry) {
        if (entry->group == group || group == 0xffffffffUL) {
            char mark = entry->owned ? 'o' : ' ';
            GSlogWrite("  %c   0x%08x  %04d  0x%08x 0x%08x\n", mark, entry->address, entry->group, entry->resourceID, entry->freeFunc);
        }
        entry = entry->next;
    }
}
extern "C" void GSresDumpAll(void) { GSresDumpGroup(0xffffffffUL); }
extern "C" void GSresFreeGroup(u32 group)
{
    _resEntry* entry = _resStructList;
    while (entry) {
        entry = entry->group == group ? resFreeResource(entry, true) : entry->next;
    }
}
extern "C" void GSresFreeResource(unsigned long group, unsigned long resourceID)
{
    _resEntry* entry = resFindResource(group, resourceID);
    if (entry) resFreeResource(entry, true);
}
extern "C" void GSresFreeResourceAllGroup(unsigned long resourceID)
{
    _resEntry* entry = _resStructList;
    while (entry) {
        if (entry->unique && entry->resourceID == resourceID) {
            resFreeResource(entry, true);
            break;
        }
        entry = entry->next;
    }
}
extern "C" void* GSresGetResourceAllGroup(unsigned long resourceID)
{
    _resEntry* entry = _resStructList;
    while (entry) {
        if (entry->unique && entry->resourceID == resourceID && !entry->locked)
            return entry->address;
        entry = entry->next;
    }
    return 0;
}
extern "C" void* GSresGetResource(unsigned long group, unsigned long resourceID)
{
    _resEntry* entry = resFindResource(group, resourceID);
    if (!entry || entry->locked) return 0;
    return entry->address;
}
extern "C" void GSresChangeResourceAdress(void* address, u32 group, u32 resourceID, GSresFreeFunc freeFunc)
{
    _resEntry* entry = resFindResource(group, resourceID);
    if (!entry) {
        if (address) GSresRegisterResource(address, group, resourceID, freeFunc);
    } else if (!address) {
        resFreeResource(entry, true);
    } else {
        resFreeResource(entry, false);
        entry->owned = 0;
        entry->address = address;
        entry->freeFunc = freeFunc;
    }
}
extern "C" void GSresRegisterResource(void* address, u32 group, u32 resourceID, GSresFreeFunc freeFunc)
{
    _resEntry* entry = resFindResource(group, resourceID);
    if (!address) {
        if (entry) resFreeResource(entry, true);
        return;
    }
    if (entry) {
        if (entry->owned) return;
        if (entry->address != address) {
            if (entry->freeFunc) entry->freeFunc(entry->address, group, resourceID);
            entry->address = address;
        }
        entry->freeFunc = freeFunc;
        return;
    }
    entry = (_resEntry*)_resEntryAllocator->allocCell();
    if (!entry) {
        GSlogWrite("GSres: error -- no free pool\n");
        return;
    }
    if (_resStructList) _resStructList->prev = entry;
    entry->next = _resStructList;
    entry->prev = 0;
    _resStructList = entry;
    entry->owned = 0;
    entry->locked = 0;
    entry->unique = 1;
    entry->address = address;
    entry->group = group;
    entry->resourceID = resourceID;
    entry->freeFunc = freeFunc;
}
extern "C" void* GSresAllocResourceAlign(u32 size, u16 align, u32 group, u32 resourceID, GSresFreeFunc freeFunc)
{
    if (resFindResource(group, resourceID)) return 0;
    _resEntry* entry = (_resEntry*)_resEntryAllocator->allocCell();
    if (!entry) { GSlogWrite("GSres: error -- no free pool\n"); return 0; }
    entry->address = GSmemAllocAlign(size, align);
    if (!entry->address) { _resEntryAllocator->freeCell(entry); return 0; }
    if (_resStructList) _resStructList->prev = entry;
    entry->next = _resStructList;
    entry->prev = 0;
    _resStructList = entry;
    entry->owned = 1;
    entry->locked = 0;
    entry->unique = 1;
    entry->group = group;
    entry->resourceID = resourceID;
    entry->freeFunc = freeFunc;
    return entry->address;
}
extern "C" void* GSresAllocResource(u32 size, u32 group, u32 resourceID, GSresFreeFunc freeFunc)
{
    if (resFindResource(group, resourceID)) return 0;
    _resEntry* entry = (_resEntry*)_resEntryAllocator->allocCell();
    if (!entry) { GSlogWrite("GSres: error -- no free pool\n"); return 0; }
    entry->address = GSmemAlloc(size);
    if (!entry->address) { _resEntryAllocator->freeCell(entry); return 0; }
    if (_resStructList) _resStructList->prev = entry;
    entry->next = _resStructList;
    entry->prev = 0;
    _resStructList = entry;
    entry->owned = 1;
    entry->locked = 0;
    entry->unique = 1;
    entry->group = group;
    entry->resourceID = resourceID;
    entry->freeFunc = freeFunc;
    return entry->address;
}
extern "C" void GSresInit(u32 entryCount)
{
    _resEntryAllocator = GSmemCreatePool(entryCount, sizeof(_resEntry));
}
_resEntry* resFreeResource(_resEntry* entry, bool remove)
{
    if (entry->freeFunc && !entry->freeFunc(entry->address, entry->group, entry->resourceID))
        return entry->next;
    if (entry->owned) {
        if (entry->address) {
            GSmemFree(entry->address);
            entry->address = 0;
        }
        entry->owned = 0;
    }
    entry->address = 0;
    _resEntry* next = entry->next;
    if (remove) {
        if (entry->prev) entry->prev->next = entry->next;
        else _resStructList = entry->next;
        if (next) next->prev = entry->prev;
        _resEntryAllocator->freeCell(entry);
    }
    return next;
}
_resEntry* resFindResource(unsigned long group, unsigned long resourceID)
{
    _resEntry* entry = _resStructList;
    while (entry) {
        if (entry->group == group && entry->resourceID == resourceID) return entry;
        entry = entry->next;
    }
    return 0;
}
