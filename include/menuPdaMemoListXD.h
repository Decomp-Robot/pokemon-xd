#ifndef MENU_PDA_MEMO_LIST_XD_H
#define MENU_PDA_MEMO_LIST_XD_H

#include <types.h>
#include <PokemonData.h>
#include <menuModel.h>
#include <winSprite.h>

// One entry of the memo list while it is being sorted. The index selects the
// entry in the source list, the key is the numeric value the list is ordered
// by, and the rest of the record holds the entry name.
struct MemoSortBuff {
    u16 index;
    u16 key;
    u16 name[12];
};

// The memo screen's entry list, which can be sorted by the key of its records.
class pdaMemoList {
public:
    void Sort_Small(MemoSortBuff *entries, long count);
};

#ifdef __cplusplus
extern "C" {
#endif
MENU_MODEL *menuPdaMemoListGetMenuModelWork(void);
u8 menuPdaMemoListIsLoadHero(void);
void menuPdaMemoListSetHeroMotion(void);
u8 menuPdaMemoListIsTermModel(void);
void menuPdaMemoListLoadHero(bool hero);
void menuPdaMemoListLoadPokemon(long index);
void menuPdaMemoListDispPokemonModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long model);
u16 menuPdaMemoListGetPokemonID(long index);
u16 *menuPdaMemoListGetPokemonName(long index);
// Species data of the memo list entry at the given index.
PokemonSpeciesStats *menuPdaMemoListGetPokemonDataPointer(long index);
// Default model angle of the given memo-list entry component.
f32 menuPdaMemoListGetDefaultAngle(s16 index);
#ifdef __cplusplus
}
#endif

#endif
