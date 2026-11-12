#include "menuPdaMemoListXD.h"

#include <types.h>
#include <menuPdaSub.h>
#include <menu.h>
#include <memo.h>
#include <GSmsg.h>
#include <pokecolo.h>

// Default model angle of each component of a memo-list entry.
f32 _defaultAngle[3] = { 0.0f, -0.3f, 0.0f };

extern "C" f32 menuPdaMemoListGetDefaultAngle(s16 index) {
    return _defaultAngle[index];
}

pdaMemoList *pMemoList;

enum {
    MEMO_LIST_CAPACITY = 400,
    MEMO_LIST_VISIBLE_COUNT = 8,
    MEMO_LIST_MENU = 119,
    MEMO_LIST_FRAME_MENU = 127,
    MEMO_MODEL_LAYOUT_PRIMARY = 0,
    MEMO_MODEL_LAYOUT_SECONDARY = 1,
    MEMO_MODEL_LAYOUT_TERTIARY = 2,
    MEMO_MODEL_LAYOUT_FALLBACK = 3,
    MEMO_MODEL_PRIMARY_ITEM = 185,
    MEMO_MODEL_SECONDARY_ITEM = 187,
    MEMO_MODEL_TERTIARY_ITEM = 186,
    MEMO_MODEL_DEFAULT_ITEM = 455,
    MEMO_LIST_SCROLL_BAR = 219,
    MEMO_LIST_UP = 220,
    MEMO_LIST_DOWN = 221,
    MEMO_LIST_CURSOR_1 = 222,
    MEMO_LIST_CURSOR_2 = 224,
    MEMO_LIST_CURSOR_3 = 226,
    MEMO_LIST_CURSOR_4 = 228,
    MEMO_LIST_CURSOR_5 = 230,
    MEMO_LIST_CURSOR_6 = 232,
    MEMO_LIST_CURSOR_7 = 234,
    MEMO_LIST_CURSOR_8 = 236,
    MEMO_LIST_ENTRY = 241,
    COUNTRY_JAPAN = 0
};

void menuPdaMemoListDispPokemonModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long layout) {
    if (pMemoList) {
        unsigned long itemID;
        switch (layout) {
        case MEMO_MODEL_LAYOUT_PRIMARY: itemID = MEMO_MODEL_PRIMARY_ITEM; break;
        case MEMO_MODEL_LAYOUT_SECONDARY: itemID = MEMO_MODEL_SECONDARY_ITEM; break;
        case MEMO_MODEL_LAYOUT_TERTIARY: itemID = MEMO_MODEL_TERTIARY_ITEM; break;
        case MEMO_MODEL_LAYOUT_FALLBACK:
        default: itemID = MEMO_MODEL_DEFAULT_ITEM; break;
        }
        pMemoList->DispPokemonModel(window, sprite, itemID);
    }
}

MENU_MODEL *menuPdaMemoListGetMenuModelWork() {
    if (pMemoList == NULL) return NULL;
    return &pMemoList->mPokemonModel;
}

void menuPdaMemoListLoadPokemon(long index) {
    if (pMemoList) pMemoList->LoadPokemon(index);
}

PokemonSpeciesStats *menuPdaMemoListGetPokemonDataPointer(long index) {
    if (pMemoList == NULL) return NULL;
    return pokemonDataBiosGetPtr(menuPdaMemoListGetPokemonID(index));
}

u16 *menuPdaMemoListGetPokemonName(long index) {
    if (pMemoList == NULL) return NULL;
    return pMemoList->GetPokemonName(index);
}

u16 menuPdaMemoListGetPokemonID(long index) {
    pdaMemoList *list = pMemoList;
    if (list == NULL) return 0;
    if (index <= memoDataGetCount(NULL) && index >= 0) return list->mPokemonIDs[index];
    return 0;
}

void menuPdaMemoListHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    switch (sprite->mItemID) {
    case MEMO_LIST_CURSOR_1:
    case MEMO_LIST_CURSOR_2:
    case MEMO_LIST_CURSOR_3:
    case MEMO_LIST_CURSOR_4:
    case MEMO_LIST_CURSOR_5:
    case MEMO_LIST_CURSOR_6:
    case MEMO_LIST_CURSOR_7:
    case MEMO_LIST_CURSOR_8:
        pMemoList->ListCursor(window, sprite); break;
    case MEMO_LIST_UP: pMemoList->UpCursor(window, sprite); break;
    case MEMO_LIST_DOWN: pMemoList->DownCursor(window, sprite); break;
    case MEMO_LIST_SCROLL_BAR: pMemoList->ScrollBar(window, sprite); break;
    case MEMO_LIST_ENTRY: pMemoList->ListItem(window, sprite); break;
    }
}

void menuPdaMemoList() {
    pMemoList = new pdaMemoList;
    CURSOR cursor;
    cursor.x = cursor.y = 0;
    menuOpen(MEMO_LIST_FRAME_MENU, 0);
    menuOpen(MEMO_LIST_MENU, 0);
    while (true) {
        if (pMemoList->Main(&cursor) < 0) break;
        CURSOR *nextCursor = menuPdaMemoXDMain(&cursor);
        cursor.y = nextCursor->y;
        cursor.x = nextCursor->x;
        menuPdaSubInitAlpha(pMemoList->mAlpha, MEMO_LIST_VISIBLE_COUNT, cursor.y);
        pMemoList->CalcPosition(&cursor);
        pMemoList->mScrollY = pMemoList->mTargetScrollY;
    }
    while (true) {
        if (pMemoList->mHeroThread == NULL) break;
        if (GSthreadIsRunning(pMemoList->mHeroThread) == 0) {
            GSthreadClose(pMemoList->mHeroThread);
            break;
        }
        GSthreadSwitch();
    }
    pMemoList->_menuClose(MEMO_LIST_MENU);
    pMemoList->_menuClose(MEMO_LIST_FRAME_MENU);
    delete pMemoList;
    pMemoList = NULL;
}

void menuPdaMemoListButton(tagWINDOW_WORK *window) {
    if (pMemoList) pMemoList->Button(window);
}

long menuPdaMemoListCursor(tagWINDOW_WORK *window) {
    if (pMemoList == NULL) return 0;
    pMemoList->Cursor(window);
    return 0;
}

long menuPdaMemoListCtrl(tagWINDOW_WORK *window) {
    if (pMemoList == NULL) return 0;
    pMemoList->Ctrl(window);
    return 0;
}

void pdaMemoList::Sort(menuPdaMemoSort order) {
    switch (order) {
    case MEMO_SORT_GOJYUON:
    default: SortGojyuon(); break;
    case MEMO_SORT_POKEMON_ID: SortPokemonID(); break;
    }
}

void pdaMemoList::SortPokemonID() {
    u16 species;
    MemoSortBuff *entries;
    u16 *pokemonIDs;
    long count = memoDataGetCount(NULL);
    entries = new MemoSortBuff[MEMO_LIST_CAPACITY];
    pokemonIDs = new u16[MEMO_LIST_CAPACITY];
    for (u16 i = 0; i < count; i++) {
        species = mPokemonIDs[i];
        pokemonIDs[i] = species;
        entries[i].key = species;
        entries[i].index = i;
    }
    Sort_Small(entries, count);
    for (u16 i = 0; i < count; i++) mPokemonIDs[i] = pokemonIDs[entries[i].index];
    if (entries != NULL) delete entries;
    if (pokemonIDs != NULL) delete pokemonIDs;
}

void pdaMemoList::SortGojyuon() {
    MemoSortBuff *entries;
    u16 *pokemonIDs;
    long count = memoDataGetCount(NULL);
    entries = new MemoSortBuff[MEMO_LIST_CAPACITY];
    pokemonIDs = new u16[MEMO_LIST_CAPACITY];
    for (u16 i = 0; i < count; i++) {
        u16 species = mPokemonIDs[i];
        pokemonIDs[i] = species;
        entries[i].key = SortGojyuonDataSet(species);
        entries[i].index = i;
        GScharCpy(entries[i].name, GetPokemonName(i));
    }
    if (pokecoloGetCountryCode() == COUNTRY_JAPAN) Sort_Small(entries, count);
    else Sort_Name(entries, count);
    for (u16 i = 0; i < count; i++) mPokemonIDs[i] = pokemonIDs[entries[i].index];
    if (entries != NULL) delete entries;
    if (pokemonIDs != NULL) delete pokemonIDs;
}
