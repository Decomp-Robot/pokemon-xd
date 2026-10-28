#include <types.h>
#include <winSprite.h>
#include <dbgMenuPokemon.h>
#include <menu.h>
#include <menuDataBios.h>
#include <Pokemon.h>

/* Keys the editor reacts to. */
enum {
    POKEMON_EDIT_KEY_ACCEPT = 0x10,   // leaves the editor, keeping the changes.
    POKEMON_EDIT_KEY_CANCEL = 0x20,   // leaves the editor, dropping the changes.
    POKEMON_EDIT_KEY_DECREASE = 0x40, // steps the value down.
    POKEMON_EDIT_KEY_INCREASE = 0x80, // steps the value up.
    POKEMON_EDIT_KEY_LIMIT = 0x100,   // pushes the value onto one of its bounds.
    POKEMON_EDIT_KEY_FAST = 0x200,    // widens a step and selects the bound.
    POKEMON_EDIT_KEY_FASTER = 0x400   // widens a step further.
};

/* Amount one step adds to the value of the selected entry. */
enum {
    POKEMON_EDIT_STEP = 1,
    POKEMON_EDIT_STEP_FAST = 10,
    POKEMON_EDIT_STEP_FASTER = 100,
    POKEMON_EDIT_STEP_LIMIT = 65535
};

/* Results the editor window hands back to the menu that opened it. */
enum {
    POKEMON_EDIT_RESULT_CANCEL = -1, // the edited record is dropped.
    POKEMON_EDIT_RESULT_CONFIRM = -2 // the edited record is kept.
};

/* Species entry, the first entry of the editor. The hook selects it whenever
 * the record under edit is unusable. */
enum { POKEMON_EDIT_ITEM_SPECIES = 22 };

/* Hook of the debug pokemon editor. mArgs[0] is the record under edit, mArgs[1]
 * receives the step the editor applies to the value of the selected entry. */
void dbgMenuPokemonEditButton(tagWINDOW_WORK *window)
{
    Pokemon *pokemon;
    s32 *deltaOut;
    u16 keys[2]; // keys[0] repeats over a held key, keys[1] is held.
    s32 delta;
    s32 itemID;

    pokemon = (Pokemon *)window->mArgs[0];
    deltaOut = (s32 *)window->mArgs[1];
    if (window == NULL) {
        return;
    }
    keys[0] = windowGetKeyInfo()->mKeyRepeat;
    keys[1] = windowGetKeyInfo()->mKeyOn;
    if (keys[0] & POKEMON_EDIT_KEY_CANCEL) {
        window->mCursorIndex = POKEMON_EDIT_RESULT_CANCEL;
        window->mAccept = 1;
        return;
    }
    if (keys[0] & POKEMON_EDIT_KEY_ACCEPT) {
        window->mCursorIndex = POKEMON_EDIT_RESULT_CONFIRM;
        window->mAccept = 1;
        return;
    }
    if (pokemon == NULL) {
        window->mCursorIndex = POKEMON_EDIT_RESULT_CANCEL;
        window->mAccept = 1;
        return;
    }
    delta = 0;
    if ((keys[0] & POKEMON_EDIT_KEY_INCREASE) && (keys[1] & POKEMON_EDIT_KEY_FAST)
        && (keys[1] & POKEMON_EDIT_KEY_FASTER)) {
        delta = POKEMON_EDIT_STEP_FASTER;
    } else if ((keys[0] & POKEMON_EDIT_KEY_DECREASE) && (keys[1] & POKEMON_EDIT_KEY_FAST)
               && (keys[1] & POKEMON_EDIT_KEY_FASTER)) {
        delta = -POKEMON_EDIT_STEP_FASTER;
    } else if ((keys[0] & POKEMON_EDIT_KEY_INCREASE) && (keys[1] & POKEMON_EDIT_KEY_FAST)) {
        delta = POKEMON_EDIT_STEP_FAST;
    } else if ((keys[0] & POKEMON_EDIT_KEY_DECREASE) && (keys[1] & POKEMON_EDIT_KEY_FAST)) {
        delta = -POKEMON_EDIT_STEP_FAST;
    } else if (keys[0] & POKEMON_EDIT_KEY_INCREASE) {
        delta = POKEMON_EDIT_STEP;
    } else if (keys[0] & POKEMON_EDIT_KEY_DECREASE) {
        delta = -POKEMON_EDIT_STEP;
    } else if ((keys[0] & POKEMON_EDIT_KEY_LIMIT) && (keys[1] & POKEMON_EDIT_KEY_FAST)) {
        delta = POKEMON_EDIT_STEP_LIMIT;
    } else if (keys[0] & POKEMON_EDIT_KEY_LIMIT) {
        delta = -POKEMON_EDIT_STEP_LIMIT;
    }
    if (!pokemonCheckValid(pokemon)) {
        window->mCursor.y = 0;
        itemID = POKEMON_EDIT_ITEM_SPECIES;
    } else {
        menuDataBiosGetPtr(window->mMenuID);
        itemID = menuGetCursorItemID(window->mMenuID);
    }
    if (delta != 0) {
        if (deltaOut != NULL) {
            *deltaOut = delta;
        }
        window->mCursorIndex = itemID;
        window->mAccept = 1;
    }
}
