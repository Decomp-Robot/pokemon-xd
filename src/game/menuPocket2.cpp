#include <menuPocket2.h>

// MSL memory routine used to clear the item-script queue.
extern "C" void *memset(void *dest, int ch, unsigned long count);

// menuPocket2GetLastSlotID -- return the last selected slot ID kept in
// menuPocket2Work. Groups with the other menuPocket2 helpers sharing
// menuPocket2Work (see menuPocket2Pokemon.cpp for the temochi pokemon
// slot helpers).
u8 menuPocket2GetLastSlotID(void)
{
    return menuPocket2Work.mLastSlotID;
}

// menuPocketClearItemScript -- drop every queued item script.
void menuPocketClearItemScript(void)
{
    memset(menuPocket2ItemScript.mEntries, 0, sizeof(menuPocket2ItemScript.mEntries));
    menuPocket2ItemScript.mCount = 0;
}

// menuPocket2GetItemScript -- script queued for the given item, or -1 when
// the item has none queued.
s32 menuPocket2GetItemScript(u16 id)
{
    s32 script = -1;
    s32 i;

    for (i = 0; i < menuPocket2ItemScript.mCount; i++) {
        if (menuPocket2ItemScript.mEntries[i].mID == id) {
            script = menuPocket2ItemScript.mEntries[i].mScript;
            break;
        }
    }

    return script;
}
