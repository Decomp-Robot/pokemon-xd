#ifndef MENU_SE_BIOS_H
#define MENU_SE_BIOS_H

#include <types.h>

// Sound event a menu reports; it selects the field of the menu's sound-effect
// record that holds the sound ID to play.
enum MenuSound {
    MENU_SOUND_CURSOR = 1,
    MENU_SOUND_ACCEPT = 2,
    MENU_SOUND_CANCEL = 3,
    MENU_SOUND_OPEN = 4,
    MENU_SOUND_CLOSE = 5,
    MENU_SOUND_PAGE = 6,
};

// One record of a menu's sound-effect table, selected by the menu data entry's
// mSoundEffectTable ID; the sound event to play picks the field inside it.
typedef struct MenuSeData {
    u16 mOpen;
    u16 mClose;
    u16 mCursor;
    u16 mAccept;
    u16 mCancel;
    u16 mPage;
} MenuSeData;

#ifdef __cplusplus
extern "C" {
#endif

MenuSeData *menuSeBiosGetPtr(u32 tableID);
u16 menuSeBiosGetSndId(MenuSeData *data, u8 event);

#ifdef __cplusplus
}
#endif

#endif
