#ifndef GS_FLAG_H
#define GS_FLAG_H
#include <types.h>

/* Debug-menu flag list: flag_data_number holds the entry count and the id of
 * the first entry; flag_data holds one linked record per entry. */
typedef struct FlagWork {
    u32 count;
    s16 head;
} FlagWork;

#ifdef __cplusplus
extern "C" {
#endif
u8 GSflagTest(s32 flag);
u32 GSflagGet(s32 flag);
void GSflagSet(s32 flag, u32 value);
void GSflagOn(s32 flag);
void GSflagOff(s32 flag);
extern FlagWork *flag_data_number;
#ifdef __cplusplus
}
#endif
#endif
