#ifndef GS_FLAG_H
#define GS_FLAG_H
#include <types.h>

/* Debug-menu flag list: flag_data_number holds the entry count and the id of
 * the first entry; flag_data holds one linked record per entry. */
typedef struct FlagWork {
    u32 count;
    s16 head;
} FlagWork;

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
extern FlagLevel *flag_level;
#ifdef __cplusplus
}
#endif
#endif
