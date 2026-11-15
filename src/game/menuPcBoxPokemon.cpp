/*
 * menuPcBoxPokemon.cpp
 *
 * Pokemon selection, storage and party maintenance for the PC-box menu.
 */

#include <CMetaMenu.h>
#include <hero.h>
#include <app/pcbox/pcbox.h>
#include <darkPokemon.h>
#include "Pokemon.h"
#include "menuPokemonCursor.h"
#include <string.h>
#include <types.h>

namespace {

class CMenuPokemonLeave : public CMetaMenu {
    enum PokemonArea {
        PokemonAreaNone = 0,
        PokemonAreaCursor = 1,
        PokemonAreaParty = 2,
        PokemonAreaBox = 3
    };
    enum CursorPosition {
        PartyPositionFirst = 4,
        PartyPositionLast = 9,
        BoxPositionFirst = 10
    };

    CAnt *currentAntGetPointer();
    void currentPokemonDel(bool release);
    Pokemon *getStatusAndModelDisplayPokemonGetPointer();
    void resetStatusAndModelDisplayPokemon();
    void setStatusAndModelDisplayPokemon(Pokemon *pokemon);
    Pokemon *currentPokemonGetPointer();
    PokemonArea currentPokemonGetArea();
    void cursorUnderPokemonSet(Pokemon *pokemon, bool replace);
    void cursorUnderPokemonDel();
    Pokemon *cursorUnderPokemonGetPointer();
    CAnt *cursorUnderPokemonGetAntPointer();
    int pcBoxUsedQuantity(int box);
    int pcBoxEmptyQuantity(int box);
    void pcBoxPokemonSet(Pokemon *pokemon, int index);
    bool pcBoxPokemonAdd(Pokemon *pokemon, int box);
    void pcBoxPokemonDel(int index);
    Pokemon *pcBoxPokemonGetPointer(int box, int index);
    Pokemon *pcBoxPokemonGetPointer(int index);

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

    u8 _pad[992 - sizeof(CMetaMenu)];
    int mSelectedBox;
    u8 _3e4[8];
    CAnt mBoxAnt[PCBOX::PokemonBoxCount][PCBOX::PokemonPerBox];
    u8 _34ac[8];
    CAnt mPartyAnt[6];
    u8 _35ec[300];
    Pokemon mDisplayPokemon;
    bool mUseDisplayPokemon;
    bool mHideStatusAndModel;
    u8 _37de[18];
    CMenuPokemonCursor *mCursor;
};

CAnt *CMenuPokemonLeave::currentAntGetPointer() {
    if (mCursor->isCatchPokemon()) {
        return mCursor->getAntPointer();
    }
    return cursorUnderPokemonGetAntPointer();
}

void CMenuPokemonLeave::currentPokemonDel(bool release) {
    Pokemon *pokemon = currentPokemonGetPointer();
    if (pokemon != NULL) {
        if (release) {
            DarkPokemon *darkPokemon = pokemon->getDarkPokemon();
            if (darkPokemon != NULL) {
                darkPokemon->setPlace(DARK_POKEMON_WHERE_RELEASED, 0);
            }
        }
        if (mCursor->isCatchPokemon()) {
            mCursor->deletePokemon();
        } else {
            cursorUnderPokemonDel();
        }
    }
}

Pokemon *CMenuPokemonLeave::getStatusAndModelDisplayPokemonGetPointer() {
    if (mHideStatusAndModel) {
        return NULL;
    }
    if (mUseDisplayPokemon) {
        if (pokemonCheckValid(&mDisplayPokemon)) {
            return &mDisplayPokemon;
        }
        return NULL;
    }
    return currentPokemonGetPointer();
}

void CMenuPokemonLeave::resetStatusAndModelDisplayPokemon() {
    pokemonInit(&mDisplayPokemon);
    mUseDisplayPokemon = false;
}

void CMenuPokemonLeave::setStatusAndModelDisplayPokemon(Pokemon *pokemon) {
    mUseDisplayPokemon = true;
    if (pokemon == NULL) {
        pokemonInit(&mDisplayPokemon);
    } else {
        memcpy(&mDisplayPokemon, pokemon, sizeof(Pokemon));
    }
}

Pokemon *CMenuPokemonLeave::currentPokemonGetPointer() {
    if (mCursor->isCatchPokemon()) {
        return mCursor->getPokemonPointer();
    }
    return cursorUnderPokemonGetPointer();
}

CMenuPokemonLeave::PokemonArea CMenuPokemonLeave::currentPokemonGetArea() {
    if (mCursor->isCatchPokemon()) {
        return PokemonAreaCursor;
    }
    long position = mCursor->getPositionIndex();
    if (position >= PartyPositionFirst && position <= PartyPositionLast) {
        return PokemonAreaParty;
    }
    if (position >= BoxPositionFirst) {
        return PokemonAreaBox;
    }
    return PokemonAreaNone;
}

void CMenuPokemonLeave::cursorUnderPokemonSet(Pokemon *pokemon, bool replace) {
    long position = mCursor->getPositionIndex();
    if (position >= PartyPositionFirst && position <= PartyPositionLast) {
        if (replace) {
            int index = position - PartyPositionFirst;
            temochiPokemonDel(index);
            temochiPokemonSet(pokemon, index);
        } else {
            temochiPokemonAdd(pokemon);
        }
    } else if (position >= BoxPositionFirst) {
        int index = position - BoxPositionFirst;
        pcBoxPokemonDel(index);
        pcBoxPokemonSet(pokemon, index);
    }
}

void CMenuPokemonLeave::cursorUnderPokemonDel() {
    long position = mCursor->getPositionIndex();
    if (position >= PartyPositionFirst && position <= PartyPositionLast) {
        temochiPokemonDelShift(position - PartyPositionFirst);
    } else if (position >= BoxPositionFirst) {
        pcBoxPokemonDel(position - BoxPositionFirst);
    }
}

Pokemon *CMenuPokemonLeave::cursorUnderPokemonGetPointer() {
    long position = mCursor->getPositionIndex();
    if (position >= PartyPositionFirst && position <= PartyPositionLast) {
        return temochiPokemonGetPointer(position - PartyPositionFirst);
    }
    if (position >= BoxPositionFirst) {
        return pcBoxPokemonGetPointer(position - BoxPositionFirst);
    }
    return NULL;
}

CAnt *CMenuPokemonLeave::cursorUnderPokemonGetAntPointer() {
    long position = mCursor->getPositionIndex();
    if (position >= PartyPositionFirst && position <= PartyPositionLast) {
        return &mPartyAnt[position - PartyPositionFirst];
    }
    if (position >= BoxPositionFirst) {
        return &mBoxAnt[mSelectedBox][position - BoxPositionFirst];
    }
    return NULL;
}

int CMenuPokemonLeave::pcBoxUsedQuantity(int box) {
    return PCBOX::getPointer()->getPokemonBoxNbUsedSlot(box);
}

int CMenuPokemonLeave::pcBoxEmptyQuantity(int box) {
    return PCBOX::getPointer()->getPokemonBoxNbEmptySlot(box);
}

void CMenuPokemonLeave::pcBoxPokemonSet(Pokemon *pokemon, int index) {
    PCBOX::getPointer()->setPokemon(pokemon, mSelectedBox, index);
}

bool CMenuPokemonLeave::pcBoxPokemonAdd(Pokemon *pokemon, int box) {
    if (box == -1) {
        box = mSelectedBox;
    }
    return PCBOX::getPointer()->addPokemon(pokemon, box);
}

void CMenuPokemonLeave::pcBoxPokemonDel(int index) {
    PCBOX::getPointer()->delPokemon(mSelectedBox, index);
}

Pokemon *CMenuPokemonLeave::pcBoxPokemonGetPointer(int box, int index) {
    Pokemon *pokemon = PCBOX::getPointer()->getPokemon(box, index);
    if (pokemon->checkValid()) {
        return pokemon;
    }
    return NULL;
}

Pokemon *CMenuPokemonLeave::pcBoxPokemonGetPointer(int index) {
    return pcBoxPokemonGetPointer(mSelectedBox, index);
}

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
