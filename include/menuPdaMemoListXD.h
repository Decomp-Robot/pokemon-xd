#ifndef MENU_PDA_MEMO_LIST_XD_H
#define MENU_PDA_MEMO_LIST_XD_H

#include <types.h>
#include <PokemonData.h>
#include <menuModel.h>
#include <winSprite.h>
#include <GSthread.h>

// One entry of the memo list while it is being sorted. The index selects the
// entry in the source list, the key is the numeric value the list is ordered
// by, and the rest of the record holds the entry name.
struct MemoSortBuff {
    u16 index;
    u16 key;
    u16 name[12];
};

enum menuPdaMemoSort {
    MEMO_SORT_GOJYUON = 0,
    MEMO_SORT_POKEMON_ID = 1
};

// The memo screen's entry list, which can be sorted by the key of its records.
class pdaMemoList {
public:
    pdaMemoList();
    ~pdaMemoList();
    void Sort(menuPdaMemoSort order);
    void SortPokemonID();
    void SortGojyuon();
    void Sort_Small(MemoSortBuff *entries, long count);
    void Sort_Name(MemoSortBuff *entries, long count);
    u16 SortGojyuonDataSet(u16 species);
    void DispPokemonModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long itemID);
    void LoadPokemon(long index);
    u16 *GetPokemonName(long index);
    void ListCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void UpCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void DownCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void ScrollBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void ListItem(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    long Main(CURSOR *cursor);
    void CalcPosition(CURSOR *cursor);
    void _menuClose(long menuID);
    void Button(tagWINDOW_WORK *window);
    void Cursor(tagWINDOW_WORK *window);
    void Ctrl(tagWINDOW_WORK *window);

    u8 _00[60];
    u16 *mPokemonIDs;
    Pokemon *mPokemon;
    MENU_MODEL mPokemonModel;
    MENU_MODEL mHeroModel;
    f32 mAlpha[8];
    u8 _23c[4];
    CURSOR mCursor;
    f32 mScrollX;
    f32 mScrollY;
    f32 mTargetScrollX;
    f32 mTargetScrollY;
    u8 _254[12];
    GSthread *mHeroThread;
};

extern pdaMemoList *pMemoList;

#ifdef __cplusplus
extern "C" {
#endif
CURSOR *menuPdaMemoXDMain(CURSOR *cursor);
void menuPdaMemoListDispPokemonModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long layout);
MENU_MODEL *menuPdaMemoListGetMenuModelWork();
u8 menuPdaMemoListIsLoadHero(void);
void menuPdaMemoListSetHeroMotion(void);
u8 menuPdaMemoListIsTermModel(void);
void menuPdaMemoListLoadHero(bool hero);
void menuPdaMemoListLoadPokemon(long index);
// Species data of the memo list entry at the given index.
PokemonSpeciesStats *menuPdaMemoListGetPokemonDataPointer(long index);
u16 *menuPdaMemoListGetPokemonName(long index);
u16 menuPdaMemoListGetPokemonID(long index);
// Default model angle of the given memo-list entry component.
f32 menuPdaMemoListGetDefaultAngle(s16 index);
void menuPdaMemoListHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuPdaMemoList();
void menuPdaMemoListButton(tagWINDOW_WORK *window);
long menuPdaMemoListCursor(tagWINDOW_WORK *window);
long menuPdaMemoListCtrl(tagWINDOW_WORK *window);
#ifdef __cplusplus
}
#endif

#endif
