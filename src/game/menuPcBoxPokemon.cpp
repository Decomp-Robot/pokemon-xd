/*
 * menuPcBoxPokemon.cpp
 *
 * The party maintenance of the pokemon-leave state machine: the occupied-slot
 * walk over the hero's party, the capacity and free-slot searches, the
 * live-member count, and the shift, clear, store and add operations its
 * action and animation states consult.
 */

#include <CMetaMenu.h>
#include <hero.h>
#include "Pokemon.h"
#include "menuPokemonCursor.h"
#include <string.h>
#include <types.h>

namespace {

class CMenuPokemonLeave : public CMetaMenu {
    int searchTemochiPokemonBlankSlot();
    bool temochiPokemonIsFull();
    bool temochiPokemonAddAble();
    int temochiPokemonQuantity();
    int temochiPokemonLiveQuantity(bool cursorLimit);
    void temochiPokemonDelShift(int index);
    void temochiPokemonDel(int index);
    void temochiPokemonSet(Pokemon *pokemon, int index);
    bool temochiPokemonAdd(Pokemon *pokemon);
    int temochiPokemonGetFreePosition();
    Pokemon *temochiPokemonGetPointer(int index);

    u8 _pad[14320 - sizeof(CMetaMenu)];
    CMenuPokemonCursor *mCursor;         // the party cursor.
};

/*
 * Returns the first blank slot of the hero's party, or -1 when the party
 * already holds six members.
 */
int CMenuPokemonLeave::searchTemochiPokemonBlankSlot() {
    int quantity = temochiPokemonQuantity();
    if (quantity == 6) {
        return -1;
    }
    return quantity;
}

/*
 * Returns true when the hero's party is full.
 */
bool CMenuPokemonLeave::temochiPokemonIsFull() {
    return temochiPokemonQuantity() >= 6;
}

/*
 * Returns true when the hero's party still has room for another member.
 */
bool CMenuPokemonLeave::temochiPokemonAddAble() {
    return temochiPokemonQuantity() < 6;
}

/*
 * Counts the valid leading slots of the hero's party, stopping at the
 * first blank slot or after six.
 */
int CMenuPokemonLeave::temochiPokemonQuantity() {
    int count;
    for (count = 0; count < 6; count++) {
        if (temochiPokemonGetPointer(count) == NULL) {
            break;
        }
    }
    return count;
}

/*
 * Counts the leading party slots that still hold a live pokemon. With
 * cursorLimit the slot the party cursor selects is left out of the count:
 * when the cursor sits on a party table entry (index 4-9), the walk skips
 * that entry's member position and keeps going.
 */
int CMenuPokemonLeave::temochiPokemonLiveQuantity(bool cursorLimit) {
    int limit = -1;
    if (cursorLimit) {
        long position = mCursor->getPositionIndex();
        if (position >= 4 && position <= 9) {
            limit = position - 4;
        }
    }
    int count = 0;
    for (int i = 0; i < 6; i++) {
        if (i == limit) {
            continue;
        }
        Pokemon *pokemon = temochiPokemonGetPointer(i);
        if (pokemon == NULL) {
            break;
        }
        if (pokemon->getHp() != 0) {
            count++;
        }
    }
    return count;
}

/*
 * Removes the occupied member at index: every later member shifts one slot
 * forward over it, then the vacated tail slot is cleared.
 */
void CMenuPokemonLeave::temochiPokemonDelShift(int index) {
    int i;
    for (i = index; i < 5; i++) {
        Pokemon *current = temochiPokemonGetPointer(i);
        Pokemon *next = temochiPokemonGetPointer(i + 1);
        if (current != NULL) {
            if (next == NULL) {
                temochiPokemonDel(i);
            } else {
                memcpy(current, next, sizeof(Pokemon));
            }
        }
    }
    temochiPokemonDel(5);
}

/*
 * Re-initializes the party member at index, blanking its slot.
 */
void CMenuPokemonLeave::temochiPokemonDel(int index) {
    pokemonInit(temochiPokemonGetPointer(index));
}

/*
 * Stores the pokemon in the hero's party slot at index.
 */
void CMenuPokemonLeave::temochiPokemonSet(Pokemon *pokemon, int index) {
    Hero::getHeroPtr()->setPokemon(pokemon, index);
}

/*
 * Adds the pokemon to the hero's party; true when a slot was free.
 */
bool CMenuPokemonLeave::temochiPokemonAdd(Pokemon *pokemon) {
    return Hero::getHeroPtr()->addPokemon(pokemon) != -1;
}

/*
 * Returns the first blank party slot, or -1 when the party is full.
 */
int CMenuPokemonLeave::temochiPokemonGetFreePosition() {
    int position;
    for (position = 0; position < 6; position++) {
        if (temochiPokemonGetPointer(position) == NULL) {
            break;
        }
    }
    if (position == 6) {
        return -1;
    }
    return position;
}

/*
 * Returns the party slot the hero keeps the given index in, or NULL when the
 * slot is blank or does not hold a valid pokemon.
 */
Pokemon *CMenuPokemonLeave::temochiPokemonGetPointer(int index) {
    Pokemon *pokemon = Hero::getHeroPtr()->getPokemon(index);
    if (pokemon->checkValid()) {
        return pokemon;
    }
    return NULL;
}

} // namespace
