#ifndef MENUDATABIOS_H
#define MENUDATABIOS_H
#include <types.h>
struct tagWINDOW_WORK;
typedef struct MenuData {
    u8 reserved00;
    u8 reserved01 : 4;
    u8 mCursorMode : 3;
    u8 reserved01Low : 1;
    u8 mButtonMode : 3;
    u8 reserved02 : 5;
    u8 reserved03;
    u8 reserved04;
    s16 mFirstItemID;
    s16 reserved08, reserved0A;
    void (*mCursorFunction)(struct tagWINDOW_WORK *);
    void (*mButtonFunction)(struct tagWINDOW_WORK *);
    void (*mCloseFunction)(struct tagWINDOW_WORK *);
    s32 reserved18;
} MenuData;

#ifdef __cplusplus
extern "C" {
#endif
MenuData *menuDataBiosGetPtr(s32 menuID);
#ifdef __cplusplus
}
#endif
#endif
