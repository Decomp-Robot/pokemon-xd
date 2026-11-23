#include "menuPdaMemoListXD.h"

#include <types.h>
#include <string.h>
#include <cursorBios.h>
#include <winSprite.h>
#include <sprite.h>
#include <winSeq.h>
#include <menuItemBios.h>
#include <menuPdaSub.h>
#include <menuPdaMemoXD.h>
#include <menu.h>
#include <memo.h>
#include <GSgfx.h>
#include <GSmsg.h>
#include <msgctrl.h>
#include <GSsndPlayback.h>
#include <gamedata.h>
#include <hero.h>
#include <peopleInfoBios.h>
#include <floorManager.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <pokemonDataBios.h>
#include <zokusei.h>
#include <pokecolo.h>

#pragma auto_inline off


// Called by the model loader's worker thread when the hero model is no
// longer needed. The result tells the thread system whether it may finish.
long _menuPdaMemoListThread(MENU_MODEL *model) {
    if (pMemoList != NULL) return pMemoList->TermThread(model);
    return 0;
}

extern "C" void menuPdaMemoListSetHeroMotion(void) {
    if (pMemoList) pMemoList->SetHeroMotion();
}

extern "C" bool menuPdaMemoListIsLoadHero(void) {
    if (pMemoList != NULL) {
        if (pMemoList->mHeroLoadFlag == 0) return false;
        return menuModelCheck(&pMemoList->mHeroModel, 0);
    }
    return false;
}

extern "C" u8 menuPdaMemoListIsTermModel(void) {
    if (pMemoList != NULL) return pMemoList->mHeroUnloadFlag;
    return 0;
}

extern "C" void menuPdaMemoListLoadHero(bool hero) {
    if (pMemoList) pMemoList->LoadHero(hero);
}

// Motion used for the hero model while a memo list is open.
int _testMotion = 3;

// Runtime Pokemon sort tables loaded by the PDA resource system.
extern u16 *pdapokemonsortdata;
extern u32 *pdapokemonsortdata_number;

#define MEMO_TEXT_RGB 0xFFFFFF00
#define MEMO_SCROLL_BAR_RGB 0xF2642200

// Default model angle of each component of a memo-list entry.
f32 _defaultAngle[3] = { 0.0f, -0.3f, 0.0f };

extern "C" f32 menuPdaMemoListGetDefaultAngle(s16 index) {
    return _defaultAngle[index];
}

pdaMemoList *pMemoList;

// Menus and menu-data tables of the list screen.
enum memoListMenu {
    MEMO_LIST_MENU = 119,
    MEMO_MENU_DATA_LIST = 120,
    MEMO_LIST_FRAME_MENU = 127,
};

// Layout slots of the entry models.
enum memoModelLayout {
    MEMO_MODEL_LAYOUT_PRIMARY = 0,
    MEMO_MODEL_LAYOUT_SECONDARY = 1,
    MEMO_MODEL_LAYOUT_TERTIARY = 2,
    MEMO_MODEL_LAYOUT_FALLBACK = 3,
};

// Window item IDs stored in the memo-list menu data.
enum memoListItem {
    MEMO_MODEL_PRIMARY_ITEM = 185,
    MEMO_MODEL_TERTIARY_ITEM = 186,
    MEMO_MODEL_SECONDARY_ITEM = 187,
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
    MEMO_LIST_AREA = 238,
    MEMO_LIST_TYPE1_COLUMN = 239,
    MEMO_LIST_TYPE2_COLUMN = 240,
    MEMO_LIST_ENTRY = 241,
    MEMO_MODEL_DEFAULT_ITEM = 455,
};

// Pages of the memo screen.
enum memoListPage {
    MEMO_PAGE_LIST = 1,
    MEMO_PAGE_MODEL = 2,
};

// List capacity, visible rows, cursor slot, scroll speed, row pitch and name ID.
enum memoListMetrics {
    MEMO_LIST_CAPACITY = 400,
    MEMO_LIST_VISIBLE_COUNT = 8,
    MEMO_LIST_CURSOR_SLOT = 13,
    MEMO_LIST_SCROLL_SPEED = 10,
    MEMO_LIST_LINE_HEIGHT = 27,
    MSG_MEMO_LIST_POKEMON_NAME = 17015,
};

// Sound effects and window sequences.
enum memoListSound {
    MEMO_LIST_SE_CURSOR = 1104,
    MEMO_LIST_SE_PAGE = 1107,
};
enum memoListSequence {
    MEMO_LIST_SEQ_OPEN = 306,
    MEMO_LIST_SEQ_CLOSE = 310,
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
    if (pokecoloGetCountryCode() == POKECOLO_COUNTRY_CODE_JAPAN) Sort_Small(entries, count);
    else Sort_Name(entries, count);
    for (u16 i = 0; i < count; i++) mPokemonIDs[i] = pokemonIDs[entries[i].index];
    if (entries != NULL) delete entries;
    if (pokemonIDs != NULL) delete pokemonIDs;
}


// Sorts the memo-list records by their key, in place.
void pdaMemoList::Sort_Small(MemoSortBuff *entries, long count) {
    MemoSortBuff swap;
    long gap = count / 2;

    while (gap > 0) {
        for (long i = gap; i < count; i++) {
            for (long j = i - gap; j >= 0 && entries[j].key >= entries[j + gap].key; j -= gap) {
                memcpy(&swap, &entries[j], sizeof(MemoSortBuff));
                memcpy(&entries[j], &entries[j + gap], sizeof(MemoSortBuff));
                memcpy(&entries[j + gap], &swap, sizeof(MemoSortBuff));
            }
        }
        gap /= 2;
    }
}

void pdaMemoList::Sort_Name(MemoSortBuff *entries, long count) {
    MemoSortBuff swap;
    long gap = count / 2;

    while (gap > 0) {
        for (long i = gap; i < count; i++) {
            for (long j = i - gap; j >= 0 && GScharCmp(entries[j].name, entries[j + gap].name) >= 0; j -= gap) {
                memcpy(&swap, &entries[j], sizeof(MemoSortBuff));
                memcpy(&entries[j], &entries[j + gap], sizeof(MemoSortBuff));
                memcpy(&entries[j + gap], &swap, sizeof(MemoSortBuff));
            }
        }
        gap /= 2;
    }
}

// Position of a species in the gojyuon sort table. Species IDs from the
// save data may carry extra flag bits, which are stripped before lookup.
u16 pdaMemoList::SortGojyuonDataSet(u16 species) {
    u16 i;

    u16 *entry = pdapokemonsortdata;

    if (species >= 0x8000) species &= 0x3FFF;
    for (i = 0; i < *pdapokemonsortdata_number; entry++, i++) {
        if (*entry == 0) return 0;
        if (species == *entry) return i;
    }
    return 0;
}

// Draw every memo row inside the scroll window: the species name and the
// two type symbols of the entry's pokemon.
void pdaMemoList::ListItem(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    long i;
    CURSOR cursor = _getCursor();
    long count = memoDataGetCount(NULL);
    GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);
    tagMENU_ITEM_DATA *entry = menuItemBiosGetPtr(MEMO_MENU_DATA_LIST, MEMO_LIST_ENTRY);
    tagMENU_ITEM_DATA *type1 = menuItemBiosGetPtr(MEMO_MENU_DATA_LIST, MEMO_LIST_TYPE1_COLUMN);
    tagMENU_ITEM_DATA *type2 = menuItemBiosGetPtr(MEMO_MENU_DATA_LIST, MEMO_LIST_TYPE2_COLUMN);
    tagMENU_ITEM_DATA *area = menuItemBiosGetPtr(MEMO_MENU_DATA_LIST, MEMO_LIST_AREA);
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(MEMO_MENU_DATA_LIST, sprite->mItemID);

    spriteSetScissor(layer, area->mX - item->mX, area->mY - item->mY, area->mWidth, area->mHeight);

    f32 y = mScrollY;
    f32 nameOffset;
    if (pokecoloGetCountryCode() != POKECOLO_COUNTRY_CODE_JAPAN) nameOffset = 0.0f;
    else nameOffset = 0.0f;

    for (i = 0; i < count; i++) {
        if (i >= cursor.x - 1 && i < cursor.x + MEMO_LIST_VISIBLE_COUNT + 1) {
            u16 *name = GetPokemonName(i);
            Pokemon *pokemon = GetPokemonPointer(i);
            msgctrlSetValue(MSGCTRL_VALUE_STRING, (unsigned long)name);
            GSmsgPrint2(layer, 0, (s32)(y + nameOffset), window->mRgba.mColorA | MEMO_TEXT_RGB,
                        MSG_MEMO_LIST_POKEMON_NAME);
            u16 typeA = pokemonGetStatus(NULL, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0),
                                         POKEMON_QUERY_SPECIES_TYPE, 0);
            windowDrawSprite2(type1->mX - entry->mX, y, type1->mWidth, type1->mHeight,
                              window->mRgba.mColorA | MEMO_TEXT_RGB, window, zokuseiBiosGetSmallSpriteID(typeA), 0,
                              layer);
            u16 typeB = pokemonGetStatus(NULL, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0),
                                         POKEMON_QUERY_SPECIES_TYPE, 1);
            if (typeB != 0 && typeB != typeA) {
                windowDrawSprite2(type2->mX - entry->mX, y, type2->mWidth, type2->mHeight,
                                  window->mRgba.mColorA | MEMO_TEXT_RGB, window, zokuseiBiosGetSmallSpriteID(typeB), 0,
                                  layer);
            }
        }
        y += MEMO_LIST_LINE_HEIGHT;
    }
    spriteClearScissor(layer);
}

// Render the selected pokemon, and the hero when the model page is open.
void pdaMemoList::DispPokemonModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long itemID) {
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(MEMO_MENU_DATA_LIST, itemID);

    if (GSgfxLayerFindByID(window->mLayerID) == NULL) return;
    menuModelSetSize(&mPokemonModel, item->mX, item->mY, item->mWidth, item->mHeight);
    if (menuModelCheck(&mPokemonModel, 0)) return;
    switch (menuPdaMemoXDGetItem()) {
    case MEMO_PAGE_MODEL:
        menuModelAdjustCamera2(&mPokemonModel, &mHeroModel);
        menuModelRender2(&mPokemonModel, &mHeroModel);
        break;
    case MEMO_PAGE_LIST:
        menuModelAdjustCamera(&mPokemonModel);
        menuModelRender(&mPokemonModel);
        break;
    default:
        menuModelAdjustCamera(&mPokemonModel);
        menuModelRender(&mPokemonModel);
        break;
    }
}

// Message string of the memo entry at the given index.
u16 *pdaMemoList::GetPokemonName(long index) {
    u16 species;
    PokemonSpeciesStats *data;

    if (index <= memoDataGetCount(NULL) && index >= 0) species = mPokemonIDs[index];
    else species = 0;
    data = pokemonDataBiosGetPtr(species);
    return data != NULL ? GSmsgGetStringPtr(pokemonDataBiosGetName(data)) : NULL;
}

void pdaMemoList::ScrollBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    long count = memoDataGetCount(NULL);
    CURSOR cursor = _getCursor();

    menuPdaSubScrollBar(window, sprite, MEMO_MENU_DATA_LIST, MEMO_LIST_SCROLL_BAR, MEMO_SCROLL_BAR_RGB, count,
                        MEMO_LIST_VISIBLE_COUNT, (GSivec2 *)&cursor);
}

// Load the hero model when the model page is first shown. Unloading on the
// main thread would block, so it is handed to a short-lived worker.
void pdaMemoList::LoadHero(bool hero) {
    if (hero) {
        if (mHeroLoadFlag) return;
        menuModelInit(&mHeroModel, 640, 480);
        menuModelLoadObject(&mHeroModel, heroGetObjID(1));
        mHeroLoadFlag = 1;
        mHeroMotionFlag = 0;
        return;
    }
    if (mHeroLoadFlag == 0) return;
    if (GSthreadGetCurrentThread() != NULL) {
        menuModelTerm(&mHeroModel);
    } else {
        menuModelRenderOffAll(&mHeroModel);
        mHeroUnloadFlag = 1;
        mHeroThread = GSthreadCreate(1, GSfloorGetCurrentFloorGroupID(), 16384, 1, 1,
                                     (GSthreadEntry)_menuPdaMemoListThread);
        if (mHeroThread != NULL) GSthreadSetArgs(mHeroThread, 1, &mHeroModel);
    }
    mHeroLoadFlag = 0;
}

long pdaMemoList::TermThread(MENU_MODEL *model) {
    menuModelTerm(model);
    mHeroThread = NULL;
    mHeroUnloadFlag = 0;
    return 1;
}

// Give the hero its standing motion once the model has finished loading.
void pdaMemoList::SetHeroMotion() {
    int motion;
    u8 hasMotion;

    if (mHeroMotionFlag) return;
    PeopleInfo *info = peopleInfoBiosGetPtr(heroGetObjID(1));
    peopleInfoBiosGetMotion(info, 1, &motion, &hasMotion);
    motion = _testMotion;
    menuModelSetMotion(&mHeroModel, motion);
    mHeroMotionFlag = 1;
}

// Show the selected memo entry's pokemon on the model page.
void pdaMemoList::LoadPokemon(long index) {
    Pokemon *pokemon;

    if ((int)(u16)memoDataGetCount(NULL) == 0) return;
    pokemon = GetPokemonPointer(index);
    if (menuModelComparePokemon(&mPokemonModel, pokemon)) return;
    menuModelFree(&mPokemonModel);
    menuModelLoadPokemon(&mPokemonModel, pokemon);
    menuModelRenderOffAll(&mHeroModel);
}

// Build the pokemon shown for a memo entry, using the random values stored
// with the memo so it matches the pokemon the player wrote about.
Pokemon *pdaMemoList::GetPokemonPointer(long index) {
    long count = memoDataGetCount(NULL);
    u16 species;
    Pokemon *pokemon;
    unsigned long rnd;
    unsigned long trainerRnd;

    if (index <= count && index >= 0) species = mPokemonIDs[index];
    else species = 0;
    if (species != 0) {
        pokemon = mPokemon;
        pokemonCreate(pokemon, species, 10, gamedataGetStatus(NULL, GAMEDATA_STATUS_ATTEST));
        rnd = memoDataGetPokemonRndFromID(NULL, species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(NULL, species);
        pokemonBiosSetRnd(pokemon, rnd);
        pokemonBiosSetCatchTrainerRnd(pokemon, trainerRnd);
        return pokemon;
    }
    return NULL;
}

// Fade the row sprites as the cursor moves; the selected row stays visible.
void pdaMemoList::ListCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    long cursorY = window->mCursor.y;
    long number = GetMenuItemNumber(sprite);

    sprite->mRgba.mAlpha = mAlpha[number];
    if (cursorY == number) {
        winSpriteSetDisp(sprite, true);
    } else if (mAlpha[number] == 0.0f) {
        winSpriteSetDisp(sprite, false);
    }
}

// Translate a row sprite's item ID into its row number.
long pdaMemoList::GetMenuItemNumber(tagSPRITE_WORK *sprite) {
    long table[MEMO_LIST_VISIBLE_COUNT] = {
        MEMO_LIST_CURSOR_1, MEMO_LIST_CURSOR_2, MEMO_LIST_CURSOR_3, MEMO_LIST_CURSOR_4,
        MEMO_LIST_CURSOR_5, MEMO_LIST_CURSOR_6, MEMO_LIST_CURSOR_7, MEMO_LIST_CURSOR_8
    };
    long number;

    memoDataGetCount(NULL);
    for (number = 0; number < MEMO_LIST_VISIBLE_COUNT; number++) {
        if (sprite->mItemID == table[number]) break;
    }
    return number;
}

void pdaMemoList::DownCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    CURSOR cursor = _getCursor();
    long count = memoDataGetCount(NULL);

    menuPdaSubCheckUpDownCursor(window, sprite, &cursor, count, MEMO_LIST_VISIBLE_COUNT, false);
}

void pdaMemoList::UpCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    CURSOR cursor = _getCursor();
    long count = memoDataGetCount(NULL);

    menuPdaSubCheckUpDownCursor(window, sprite, &cursor, count, MEMO_LIST_VISIBLE_COUNT, true);
}

void pdaMemoList::Release() {
    menuModelTerm(&mPokemonModel);
    menuModelTerm(&mHeroModel);
    if (mPokemonIDs != NULL) {
        delete[] mPokemonIDs;
        mPokemonIDs = NULL;
    }
    if (mPokemon != NULL) {
        delete mPokemon;
        mPokemon = NULL;
    }
}

inline void pdaMemoList::SetPokemonIDs() {
    for (long i = 0; i < memoDataGetCount(NULL); i++) {
        mPokemonIDs[i] = memoDataGetPokemonID(NULL, i) & 0x3FFF;
    }
}

void pdaMemoList::InitWork() {
    long count = memoDataGetCount(NULL);
    tagMENU_ITEM_DATA *item;

    mCursor.y = 0;
    mCursor.x = 0;
    mScrollX = 0.0f;
    mScrollY = 0.0f;
    mTargetScrollX = 0.0f;
    mTargetScrollY = 0.0f;
    mReserved = 0;
    mHeroThread = NULL;
    mScrollState = 0;
    if (count >= MEMO_LIST_VISIBLE_COUNT) mVisibleNum = MEMO_LIST_VISIBLE_COUNT;
    else mVisibleNum = count;
    if (count != 0) {
        mPokemonIDs = new u16[count];
        mPokemon = new Pokemon;
    }
    item = menuItemBiosGetPtr(MEMO_MENU_DATA_LIST, MEMO_MODEL_SECONDARY_ITEM);
    menuModelInit(&mPokemonModel, item->mWidth, item->mHeight);
    menuModelInit(&mHeroModel, 640, 480);
    menuModelTerm(&mHeroModel);
    SetPokemonIDs();
    Sort(MEMO_SORT_GOJYUON);
    menuPdaSubInitAlpha(mAlpha, MEMO_LIST_VISIBLE_COUNT, 0);
    LoadPokemon(0);
    mHeroLoadFlag = 0;
    mHeroMotionFlag = 0;
    mHeroUnloadFlag = 0;
}

// Run the list window until a memo is chosen or the screen is closed.
long pdaMemoList::Main(CURSOR *cursor) {
    long count = memoDataGetCount(NULL);
    CURSOR start;
    long windowID;

    start.x = cursor->x;
    start.y = cursor->y;
    _setCursor(start);
    menuModelSetZoom(&mPokemonModel, 1.0f);
    do {
        windowID = menuOpen(MEMO_MENU_DATA_LIST, 1);
        start = _getCursor();
    } while (windowID != -1 && start.y >= MEMO_LIST_VISIBLE_COUNT);
    _menuClose(MEMO_MENU_DATA_LIST);
    if (windowID == -1 || count == 0) return -1;
    cursor->y = start.y;
    cursor->x = start.x;
    return start.x + start.y;
}

void pdaMemoList::Button(tagWINDOW_WORK *window) {
    long count;
    u16 key;

    if (window == NULL) return;
    count = memoDataGetCount(NULL);
    if (winSeqCheckMove(window->mMenuID)) return;
    key = windowGetKeyInfo()->mKeyTrg;
    if (count != 0 && (key & WINDOW_KEY_ACCEPT)) window->mAccept = 1;
    if (key & WINDOW_KEY_CANCEL) {
        window->mAccept = 1;
        window->mCancel = 1;
    }
}

void pdaMemoList::Cursor(tagWINDOW_WORK *window) {
    long index;
    long num;
    long visible;
    u16 key;
    int y;
    int x;
    windowGetKeyInfo();
    CURSOR cursor = _getCursor();
    cursor = window->mCursor;
    num = memoDataGetCount(NULL);
    visible = MEMO_LIST_VISIBLE_COUNT;
    if (num < MEMO_LIST_VISIBLE_COUNT) visible = num;
    y = cursor.y;
    x = cursor.x;
    index = cursor.y + cursor.x;
    key = windowGetKeyRepeat();
    if (key & WINDOW_KEY_UP) {
        if (index != 0) GSsndPlayNow(MEMO_LIST_SE_CURSOR);
        cursor.y--;
    } else if (key & WINDOW_KEY_DOWN) {
        if (num - 1 != y + x) GSsndPlayNow(MEMO_LIST_SE_CURSOR);
        cursor.y++;
    } else if (key & WINDOW_KEY_RIGHT) {
        if (num - 1 != y + x) GSsndPlayNow(MEMO_LIST_SE_PAGE);
        cursor.y += visible;
    } else if (key & WINDOW_KEY_LEFT) {
        if (index != 0) GSsndPlayNow(MEMO_LIST_SE_PAGE);
        cursor.y -= visible;
    }
    if (cursor.y < 0) {
        cursor.x += cursor.y;
        cursor.y = 0;
        if (cursor.x < 0) {
            cursor.x = 0;
            cursor.y = 0;
        }
    } else if (cursor.y >= visible) {
        cursor.x += cursor.y - (visible - 1);
        cursor.y = visible - 1;
        if (cursor.x + cursor.y >= num) {
            cursor.x = num - visible;
            cursor.y = visible - 1;
        }
    }
    _setCursor(cursor);
    window->mCursor = cursor;
    if (index != cursor.y + cursor.x) LoadPokemon(cursor.y + cursor.x);
}

void pdaMemoList::Ctrl(tagWINDOW_WORK *window) {
    f32 angle[3];
    f32 y;
    f32 z;

    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, MEMO_LIST_SEQ_OPEN);
            window->mFlag = 1;
            z = menuPdaMemoListGetDefaultAngle(2);
            y = menuPdaMemoListGetDefaultAngle(1);
            angle[0] = menuPdaMemoListGetDefaultAngle(0);
            angle[1] = y;
            angle[2] = z;
            menuModelSetAngle(menuPdaMemoListGetMenuModelWork(), angle);
        }
        break;
    case WINDOW_STATE_ACTIVE:
        ListMain(window);
        break;
    case WINDOW_STATE_CLOSING:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, MEMO_LIST_SEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
}

void pdaMemoList::ListMain(tagWINDOW_WORK *window) {
    CURSOR cursor = _getCursor();

    ListPosition();
    CalcPosition(&cursor);
    menuPdaSubCalcAlpha(mAlpha, MEMO_LIST_VISIBLE_COUNT, cursor.y);
}

void pdaMemoList::ListPosition() {
    if (mScrollX != mTargetScrollX) menuPdaSubCalcPositon(&mScrollX, mTargetScrollX, MEMO_LIST_SCROLL_SPEED);
    if (mScrollY != mTargetScrollY) menuPdaSubCalcPositon(&mScrollY, mTargetScrollY, MEMO_LIST_SCROLL_SPEED);
}

void pdaMemoList::CalcPosition(CURSOR *cursor) {
    mTargetScrollY = -cursor->x * MEMO_LIST_LINE_HEIGHT;
}

void pdaMemoList::_setCursor(CURSOR pos) {
    cursorBiosSetPos(MEMO_LIST_CURSOR_SLOT, pos);
}

CURSOR pdaMemoList::_getCursor() {
    return cursorBiosGetPos(MEMO_LIST_CURSOR_SLOT);
}

void pdaMemoList::_menuClose(long menuID) {
    menuClose(menuID);
    menuCloseSync(menuID, true);
}

pdaMemoList::pdaMemoList() {
    mPokemonIDs = NULL;
    mPokemon = NULL;
    InitWork();
}

pdaMemoList::~pdaMemoList() {
    Release();
}
