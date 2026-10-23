#include <types.h>

/* Debug-menu flag list: gDbgMenuFlagWork holds the entry count and the id of
 * the first entry; gDbgMenuFlagIdTable holds one linked record per entry. */
typedef struct {
    u32 count;
    s16 head;
} FlagWork;

typedef struct {
    s16 unk0;
    s16 unk1;
    s16 next;
} FlagEntry;

extern FlagWork *gDbgMenuFlagWork;
extern FlagEntry *gDbgMenuFlagIdTable;

/* Id of the cursor-th entry of the flag list. */
#pragma peephole off
u16 _dbgMenuFlagGetID(s32 cursor)
{
    s16 node;
    s32 i;

    if (cursor < 0) {
        return 0;
    }
    if (cursor >= gDbgMenuFlagWork->count) {
        return 0;
    }
    node = gDbgMenuFlagWork->head;
    if (cursor != 0) {
        for (i = 0; i < cursor - 1; i++) {
            node = gDbgMenuFlagIdTable[node].next;
        }
        node = gDbgMenuFlagIdTable[node].next;
    }
    return node;
}
#pragma peephole reset
