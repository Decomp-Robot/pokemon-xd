#include <types.h>
#include <GSflag.h>

/* Debug-menu flag list: flag_data_number holds the entry count and the id of
 * the first entry; flag_data holds one linked record per entry. */
typedef struct {
    s16 unk0;
    s16 unk1;
    s16 next;
} FlagEntry;

extern FlagEntry *flag_data;

/* Id of the cursor-th entry of the flag list. */
#pragma peephole off
u16 _dbgMenuFlagGetID(s32 cursor)
{
    s16 node;
    s32 i;

    if (cursor < 0) {
        return 0;
    }
    if (cursor >= flag_data_number->count) {
        return 0;
    }
    node = flag_data_number->head;
    if (cursor != 0) {
        for (i = 0; i < cursor - 1; i++) {
            node = flag_data[node].next;
        }
        node = flag_data[node].next;
    }
    return node;
}
#pragma peephole reset
