#include <menuPokemonCursor.h>

extern PocketVec<short> sDummyPosition;

// MSL memory routine used to copy the pokemon record.
extern "C" void *memcpy(void *dest, const void *src, unsigned long count);

// setBeforAnt -- keep the model state that is about to be replaced so the
// cursor can restore it afterwards.
void CMenuPokemonCursor::setBeforAnt(CAnt &ant)
{
    mBeforAnt = ant;
}

// setPokemon -- point the cursor at the given party pokemon, remembering the
// current model first. A null pokemon clears both records instead.
void CMenuPokemonCursor::setPokemon(Pokemon *pokemon)
{
    mBeforAnt = mAnt;

    if (pokemon == 0) {
        pokemonInit(&mPokemon);
        mAnt.init();
    } else {
        memcpy(&mPokemon, pokemon, sizeof(Pokemon));
        battlePokemonGetPokeBody(&mAnt, &mPokemon);
        mAnt.setAnimation(0);
    }
}

void CMenuPokemonCursor::setPositionTableDummy()
{
    PocketVec<short> *dummy = &sDummyPosition;
    long zero = 0;
    PocketVec<short> pos = mPositionTable[mPositionIndex];
    *dummy = pos;
    mPositionTable = dummy;
    mPositionIndex = zero;
    mPrevPositionIndex = zero;
}

void CMenuPokemonCursor::setPositionIndex(long index)
{
    PocketVec<short> pos = mPositionTable[mPositionIndex];
    mPositionX = pos.x;
    mPositionY = pos.y;
    mPrevPositionIndex = mPositionIndex;
    mPositionIndex = index;
}

void CMenuPokemonCursor::setPositionTable(PocketVec<short> *table, long index)
{
    setPositionIndex(index);
    mPrevPositionIndex = 0;
    mPositionTable = table;
}
