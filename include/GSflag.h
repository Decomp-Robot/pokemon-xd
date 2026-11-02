#ifndef GS_FLAG_H
#define GS_FLAG_H
#include <types.h>

/* Debug-menu flag list: flag_data_number holds the entry count and the id of
 * the first entry; flag_data holds one linked record per entry. */
typedef struct FlagWork {
    u32 count;
    s16 head;
} FlagWork;

/* One shared flag catalog record per entry: level selects the flag block the
 * entry's bits live in, width is that bit-array width, bitPosition is the
 * entry's slot inside the block and nextId links to the next entry. */
typedef struct FlagEntry {
    u8 level : 2;
    u8 width : 6;
    u8 unknown01;
    u16 bitPosition;
    s16 nextId;
} FlagEntry;

/* One registered flag level: its capacity in words and the array that
 * holds the packed bits. Entry 0 stays unused; GSflagInit registers 1-3. */
typedef struct FlagLevel {
    u32 size;
    u32 *buffer;
} FlagLevel;

#ifdef __cplusplus
extern "C" {
#endif
void GSflagInit(u32 *buffer1, u32 size1, u32 *buffer2, u32 size2, u32 *buffer3, u32 size3);
void GSflagClear(s32 level);
u8 GSflagTest(s32 flag);
u32 GSflagGet(s32 flag);
void GSflagSet(s32 flag, u32 value);
void GSflagOn(s32 flag);
void GSflagOff(s32 flag);
extern FlagWork *flag_data_number;
extern FlagEntry *flag_data;
extern FlagLevel *flag_level;
#ifdef __cplusplus
}
#endif
#endif
