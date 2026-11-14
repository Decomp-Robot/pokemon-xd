#include <types.h>

#include <string.h>
#include <cursorBios.h>
#include <winSprite.h>
#include <menuPdaSub.h>
#include <menuPda.h>
#include <menuPdaDPM.h>
#include <GSsprite.h>
#include <menuItemBios.h>
#include <menu.h>
#include <GSmsg.h>
#include <msgctrl.h>
#include <GSsndPlayback.h>
#include <PokemonData.h>
#include <pokemonDataBios.h>
#include <heroBios.h>
#include <savedata.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <darkPokemon.h>

// One Dark Pokemon monitor entry while it is being sorted. The key is the
// value the currently selected sort orders by, and the two IDs identify the
// entry so the sorted order can be read back after sorting.
struct SnatchSortBuff {
    long key;
    u16 pokemonID;
    u16 darkPokemonID;
};

// The IDs of one Dark Pokemon monitor entry, staged while the list is sorted.
struct SnatchSortTemp {
    u16 pokemonID;
    u16 darkPokemonID;
};

// Order the Dark Pokemon monitor list is sorted by.
enum DPMSelSort {
    DPM_SEL_SORT_POKEMON = 0,  // by the Pokemon each entry holds.
    DPM_SEL_SORT_DP = 1,       // by the dark point ratio of the entry.
    DPM_SEL_SORT_PLACE = 2,    // by the place the entry was caught.
    DPM_SEL_SORT_MAX = 3       // number of orders the list cycles through.
};

// Menu data of the dark pokemon monitor list window.
enum {
    DPM_LIST_LINES = 10,             // rows the list window shows at once.
    DPM_LIST_LINE_HEIGHT = 23,       // height of one row, in pixels.
    DPM_LIST_DARK_POKEMON_MAX = 128, // dark Pokemon IDs scanned for the list.
    DPM_LIST_SEQ_OPEN = 306,         // window sequence the list opens with.
    DPM_LIST_SEQ_CLOSE = 310,        // window sequence the list closes with.
    DPM_LIST_PDA_SUBRES = 1,         // PDA subresource the list window shows.
    DPM_LIST_SCROLL_SPEED = 10,      // scroll speed of the list.
    DPM_LIST_SE_CURSOR = 1104,       // sound played when the row cursor moves.
    DPM_LIST_SE_PAGE = 1107          // sound played when the list pages.
};

enum {
    DPM_LIST_CURSOR_ID = 12,
    DPM_LIST_PARTY_SIZE = 6,
    DPM_LIST_PCBOXES = 8,
    DPM_LIST_RELIVE_HALL_SETS = 9,
};

enum {
    MSG_DPM_EMPTY = 1,
    MSG_DPM_PLACE_SODATEYA = 15226,
    MSG_DPM_PLACE_PARTY = 15372,
    MSG_DPM_PLACE_RELIVE_HALL = 15373,
    MSG_DPM_PLACE_PCBOX = 15374,
    MSG_DPM_PLACE_NOT_OWNED = 15375,
    MSG_DPM_LIST_POKEMON_NAME = 17015,
};

// Sprite item selectors dispatched by menuPdaDPMListCallBack.
enum {
    DPM_SPRITE_SCROLLBAR = 55,
    DPM_SPRITE_ROW0 = 56,
    DPM_SPRITE_ROW1 = 57,
    DPM_SPRITE_ROW2 = 58,
    DPM_SPRITE_ROW3 = 59,
    DPM_SPRITE_ROW4 = 60,
    DPM_SPRITE_ROW5 = 61,
    DPM_SPRITE_ROW6 = 62,
    DPM_SPRITE_ROW7 = 63,
    DPM_SPRITE_ROW8 = 64,
    DPM_SPRITE_ROW9 = 65,
    DPM_SPRITE_BALL_COLUMN = 69,
    DPM_SPRITE_UP_CURSOR = 70,
    DPM_SPRITE_DOWN_CURSOR = 71,
    DPM_SPRITE_LIST_AREA = 72,
    DPM_SPRITE_PLACE_COLUMN = 73,
    DPM_SPRITE_RELIVE_COLUMN = 74,
    DPM_SPRITE_LIST = 75,
    DPM_SPRITE_SORT_LABEL_PLACE = 76,
    DPM_SPRITE_SORT_LABEL_DP = 77,
    DPM_SPRITE_SORT_LABEL_POKEMON = 78,
    DPM_SPRITE_SORT_ARROW_POKEMON = 461,
    DPM_SPRITE_SORT_ARROW_DP = 462,
    DPM_SPRITE_SORT_ARROW_PLACE = 463,
};

// The Dark Pokemon monitor's entry list, which orders its records by the
// currently selected sort key.
class menuPdaDPMList {
public:
    menuPdaDPMList();
    ~menuPdaDPMList();
    u16 GetDarkPokemonID(long index);
    void ScrollBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void DownCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void UpCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void ListCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void SortCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void SetItemColor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    long SortPlace();
    u8 getTemochiIndex(u16 pokemonID, u16 darkID);
    long SortDP();
    long SortPokemon();
    void Button(tagWINDOW_WORK *window);
    void Cursor(tagWINDOW_WORK *window);
    long Sort(DPMSelSort sort);
    void SortSetTemp(SnatchSortBuff *dest, SnatchSortBuff *src, long count);
    void SortSub(SnatchSortBuff *entries, long count, bool ascending);
    void SortSub2(SnatchSortBuff *entries, long count, bool ascending);
    void Ctrl(tagWINDOW_WORK *window);
    void ListMain(tagWINDOW_WORK *window);
    void CalcPosition(CURSOR *cursor);
    void ListPosition();
    void DispList(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    long _menuClose(long menuID);
    long Main(long previousIndex, CURSOR *cursor);
    void InitWork();
    void _setCursor(CURSOR pos);
    CURSOR _getCursor();

    long mCount;                  // records the list holds.
    f32 mAlpha[DPM_LIST_LINES];   // fade of each row of the list.
    CURSOR mCursor;               // cursor position of the list.
    f32 mScrollX;                 // scroll position being drawn.
    f32 mScrollY;
    f32 mTargetScrollX;           // scroll position being moved to.
    f32 mTargetScrollY;
    long mReserved;
    long mVisibleNum;             // rows the window shows at once.
    DPMSelSort mSort;             // order the list is sorted by.
    SnatchSortTemp *mListTemp;    // IDs of the entries being sorted.
    SnatchSortBuff *mSortBuff;    // sorted entries the window shows.
    SnatchSortBuff *mSortDpBuff;  // work entries of the dark point sort.
    SnatchSortBuff *mSortDpTemp;  // result entries of the dark point sort.
    long mSortedNum;              // entries written to the result so far.
};

menuPdaDPMList *pDPMList;

extern "C" void menuDrawReliveUpdate(void);
extern "C" u16 mballBiosGetSpriteID(u8 ball);
extern "C" long menuPdaDPMListCallBack(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
extern "C" void menuPdaDPMListButton(tagWINDOW_WORK *window);
extern "C" long menuPdaDPMListCursor(tagWINDOW_WORK *window);
extern "C" long menuPdaDPMListCtrl(tagWINDOW_WORK *window);

u32 menuPdaDPMListGetDarkPokemonPlaceMsgID(long index, u8 *outIndex)
{
    darkPokemonWhere where;
    u32 msgID = 0;
    menuPdaDPMListGetDarkPokemonPlace(index, &where, outIndex);
    switch (where) {
    case DARK_POKEMON_WHERE_NONE:
        msgID = 0;
        break;
    case DARK_POKEMON_WHERE_PARTY:
        msgID = MSG_DPM_PLACE_PARTY;
        break;
    case DARK_POKEMON_WHERE_PCBOX:
        msgID = MSG_DPM_PLACE_PCBOX;
        break;
    case DARK_POKEMON_WHERE_RELIVE_HALL:
        msgID = MSG_DPM_PLACE_RELIVE_HALL;
        break;
    case DARK_POKEMON_WHERE_SODATEYA:
        msgID = MSG_DPM_PLACE_SODATEYA;
        break;
    case DARK_POKEMON_EXCHANGED:
    case DARK_POKEMON_WHERE_RELEASED:
        msgID = MSG_DPM_PLACE_NOT_OWNED;
        break;
    }
    return msgID;
}

long menuPdaDPMListGetDarkPokemonPlace(long index, darkPokemonWhere *outWhere, u8 *outIndex)
{
    DarkPokemon *dp;
    darkPokemonStatus status;
    darkPokemonWhere where;
    u8 placeIndex;
    if (pDPMList == 0) {
        return 0;
    }
    dp = darkPokemonGetDarkPokemon((u16)pDPMList->GetDarkPokemonID(index));
    status = dp->getStatus();
    switch (status) {
    case DARK_POKEMON_STATUS_NONE:
    case DARK_POKEMON_STATUS_FIGHT:
    case DARK_POKEMON_STATUS_UNKNOWN:
        *outWhere = DARK_POKEMON_WHERE_NONE;
        *outIndex = 0;
        break;
    case DARK_POKEMON_SNATCHED:
    case DARK_POKEMON_STATUS_RELIVED:
        dp->getPlace(where, placeIndex);
        *outWhere = where;
        *outIndex = placeIndex;
        break;
    default:
        break;
    }
    return 1;
}

u16 menuPdaDPMListGetDarkPokemonID(long index)
{
    if (pDPMList == 0) {
        return 0;
    }
    return pDPMList->GetDarkPokemonID(index);
}

u16 menuPdaDPMListGetPokemonID(long index)
{
    DarkPokemon *dp;
    if (pDPMList == 0) {
        return 0;
    }
    dp = darkPokemonGetDarkPokemon((u16)pDPMList->GetDarkPokemonID(index));
    return dp->getPokemonID();
}

u16 *menuPdaDPMListGetPokemonName(long index)
{
    DarkPokemon *dp;
    PokemonSpeciesStats *stats;
    if (pDPMList == 0) {
        return 0;
    }
    dp = darkPokemonGetDarkPokemon((u16)pDPMList->GetDarkPokemonID(index));
    stats = pokemonDataBiosGetPtr(dp->getPokemonID());
    if (stats != 0) {
        return GSmsgGetStringPtr(pokemonDataBiosGetName(stats));
    }
    return 0;
}

u16 menuPdaDPMListGetDarkpokemonID(long index)
{
    if (pDPMList != 0) {
        return pDPMList->GetDarkPokemonID(index);
    }
    return 0;
}
DarkPokemon *menuPdaDPMListGetDPointer(long index)
{
    return darkPokemonGetDarkPokemon((u16)pDPMList->GetDarkPokemonID(index));
}
long menuPdaDPMListCallBack(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    if (pDPMList == 0) {
        return 0;
    }
    switch (sprite->mItemID) {
    case DPM_SPRITE_SCROLLBAR:
        pDPMList->ScrollBar(window, sprite);
        break;
    case DPM_SPRITE_LIST:
        pDPMList->DispList(window, sprite);
        break;
    case DPM_SPRITE_ROW0:
    case DPM_SPRITE_ROW1:
    case DPM_SPRITE_ROW2:
    case DPM_SPRITE_ROW3:
    case DPM_SPRITE_ROW4:
    case DPM_SPRITE_ROW5:
    case DPM_SPRITE_ROW6:
    case DPM_SPRITE_ROW7:
    case DPM_SPRITE_ROW8:
    case DPM_SPRITE_ROW9:
        pDPMList->ListCursor(window, sprite);
        break;
    case DPM_SPRITE_UP_CURSOR:
        pDPMList->UpCursor(window, sprite);
        break;
    case DPM_SPRITE_DOWN_CURSOR:
        pDPMList->DownCursor(window, sprite);
        break;
    case DPM_SPRITE_SORT_ARROW_POKEMON:
    case DPM_SPRITE_SORT_ARROW_DP:
    case DPM_SPRITE_SORT_ARROW_PLACE:
        pDPMList->SortCursor(window, sprite);
        break;
    case DPM_SPRITE_SORT_LABEL_PLACE:
    case DPM_SPRITE_SORT_LABEL_DP:
    case DPM_SPRITE_SORT_LABEL_POKEMON:
        pDPMList->SetItemColor(window, sprite);
        break;
    default:
        break;
    }
    return 0;
}
long menuPdaDPMListGetDPNum(void)
{
    if (pDPMList == 0) {
        return 0;
    }
    return pDPMList->mCount;
}

void menuPdaDPMListButton(tagWINDOW_WORK *window)
{
    if (pDPMList != 0) {
        pDPMList->Button(window);
    }
}
long menuPdaDPMListCursor(tagWINDOW_WORK *window)
{
    if (pDPMList == 0) {
        return 0;
    }
    pDPMList->Cursor(window);
    return 0;
}
long menuPdaDPMListCtrl(tagWINDOW_WORK *window)
{
    if (pDPMList == 0) {
        return 0;
    }
    pDPMList->Ctrl(window);
    return 0;
}
void menuPdaDPMonList(void)
{
    CURSOR cursor;
    long result;
    menuPdaDPMList *list;

    result = 0;
    list = new menuPdaDPMList();
    pDPMList = list;
    list->InitWork();
    cursor.y = 0;
    cursor.x = 0;
    menuOpen(MENU_ID_PDA_DPMON_LIST_FRAME, 0);
    do {
        result = pDPMList->Main(result, &cursor);
        if (result < 0) {
            break;
        }
        u16 darkPokemonID = (u16)menuPdaDPMListGetDarkPokemonID(cursor.y + cursor.x);
        DarkPokemon *dp = darkPokemonGetDarkPokemon(darkPokemonID);
        if (dp->isUpdate()) {
            dp->setUpdate(false);
        }
        CURSOR *newCursor = menuPdaDPMon(&cursor);
        cursor.y = newCursor->y;
        cursor.x = newCursor->x;
        menuPdaSubInitAlpha(pDPMList->mAlpha, DPM_LIST_LINES, (s16)newCursor->y);
        pDPMList->CalcPosition(&cursor);
        pDPMList->mScrollY = pDPMList->mTargetScrollY;
    } while (true);
    pDPMList->_menuClose(MENU_ID_PDA_DPMON_LIST_FRAME);
    delete pDPMList;
    pDPMList = 0;
}

menuPdaDPMList::~menuPdaDPMList()
{
    if (mListTemp != 0) {
        delete[] mListTemp;
        mListTemp = 0;
    }
    if (mSortBuff != 0) {
        delete[] mSortBuff;
        mSortBuff = 0;
    }
    if (mSortDpBuff != 0) {
        delete[] mSortDpBuff;
        mSortDpBuff = 0;
    }
    if (mSortDpTemp != 0) {
        delete[] mSortDpTemp;
        mSortDpTemp = 0;
    }
}
menuPdaDPMList::menuPdaDPMList()
{
    mListTemp = 0;
    mSortBuff = 0;
    mSortDpBuff = 0;
    mSortDpTemp = 0;
    mCount = 0;
    mSortedNum = 0;
}

u16 menuPdaDPMList::GetDarkPokemonID(long index)
{
    return mSortBuff[index].darkPokemonID;
}

void menuPdaDPMList::ScrollBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    long count = mCount;
    CURSOR cursor = _getCursor();
    menuPdaSubScrollBar(window, sprite, MENU_ID_PDA_DPMON_LIST, DPM_SPRITE_SCROLLBAR, 0xF2642200, count, DPM_LIST_LINES,
                        (GSivec2 *)&cursor);
}
void menuPdaDPMList::DispList(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = _getCursor();
    long count = mCount;
    long firstRow;
    long endRow;
    tagMENU_ITEM_DATA *listItem;
    tagMENU_ITEM_DATA *reliveItem;
    tagMENU_ITEM_DATA *placeItem;
    tagMENU_ITEM_DATA *ballItem;
    tagMENU_ITEM_DATA *areaItem;
    tagMENU_ITEM_DATA *spriteItem;
    f32 y;
    long i;
    DarkPokemon *darkPokemon;
    u32 color;
    u32 baseColor;

    listItem = menuItemBiosGetPtr(MENU_ID_PDA_DPMON_LIST, DPM_SPRITE_LIST);
    reliveItem = menuItemBiosGetPtr(MENU_ID_PDA_DPMON_LIST, DPM_SPRITE_RELIVE_COLUMN);
    placeItem = menuItemBiosGetPtr(MENU_ID_PDA_DPMON_LIST, DPM_SPRITE_PLACE_COLUMN);
    ballItem = menuItemBiosGetPtr(MENU_ID_PDA_DPMON_LIST, DPM_SPRITE_BALL_COLUMN);
    areaItem = menuItemBiosGetPtr(MENU_ID_PDA_DPMON_LIST, DPM_SPRITE_LIST_AREA);
    spriteItem = menuItemBiosGetPtr(MENU_ID_PDA_DPMON_LIST, sprite->mItemID);
    spriteSetScissor(window->mLayerID, areaItem->mX - spriteItem->mX, areaItem->mY - spriteItem->mY,
                     areaItem->mWidth, areaItem->mHeight);
    y = mScrollY;
    menuDrawReliveUpdate();
    firstRow = cursor.x - 1;
    endRow = cursor.x + DPM_LIST_LINES + 1;
    for (i = 0; i < count; i++) {
        u16 *name;
        u32 msgID;
        u8 placeIndex;
        if (i >= firstRow && i < endRow) {
            darkPokemon = darkPokemonGetDarkPokemon((u16)GetDarkPokemonID(i));
            if (darkPokemon->isUpdate()) {
                baseColor = color = 0xD5AA3300;
            } else {
                baseColor = color = 0xFFFFFF00;
            }
            name = menuPdaDPMListGetPokemonName(i);
            if (name == 0) {
                name = GSmsgGetStringPtr(MSG_DPM_EMPTY);
            }
            msgID = menuPdaDPMListGetDarkPokemonPlaceMsgID(i, &placeIndex);
            msgctrlSetValue(MSGCTRL_VALUE_STRING, (unsigned long)name);
            GSmsgPrint2(window->mLayerID, 0, (s32)y, color | window->mRgba.mColorA, MSG_DPM_LIST_POKEMON_NAME);
            menuPdaDPMListDispRelive(window, sprite, reliveItem->mX - listItem->mX, (s32)y,
                                     i, window->mRgba.mColorA, true);
            if (msgID != 0) {
                msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, (unsigned long)(placeIndex + 1));
                GSmsgPrint2(window->mLayerID, placeItem->mX - listItem->mX, (s32)y,
                            baseColor | window->mRgba.mColorA, msgID);
            }
            if (darkPokemon->isSnach()) {
                u16 ballSpriteID = mballBiosGetSpriteID(darkPokemon->getCatchBallID());
                if (ballSpriteID != 0) {
                    windowDrawSprite2((s16)(ballItem->mX - listItem->mX), (s16)y,
                                      ballItem->mWidth, ballItem->mHeight,
                                      0xFFFFFF00 | window->mRgba.mColorA, window,
                                      ballSpriteID, 0, window->mLayerID);
                }
            }
        }
        y += DPM_LIST_LINE_HEIGHT;
    }
    spriteClearScissor(window->mLayerID);
}
void menuPdaDPMList::DownCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = _getCursor();
    menuPdaSubCheckUpDownCursor(window, sprite, &cursor, mCount, DPM_LIST_LINES, false);
}
void menuPdaDPMList::UpCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = _getCursor();
    menuPdaSubCheckUpDownCursor(window, sprite, &cursor, mCount, DPM_LIST_LINES, true);
}
void menuPdaDPMList::ListCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    long table[DPM_LIST_LINES] = { DPM_SPRITE_ROW0, DPM_SPRITE_ROW1, DPM_SPRITE_ROW2, DPM_SPRITE_ROW3,
                       DPM_SPRITE_ROW4, DPM_SPRITE_ROW5, DPM_SPRITE_ROW6, DPM_SPRITE_ROW7,
                       DPM_SPRITE_ROW8, DPM_SPRITE_ROW9 };
    long index;
    long cursorY = window->mCursor.y;
    if (mCount == 0) {
        cursorY = 0;
    }
    for (index = 0; index < DPM_LIST_LINES; index++) {
        if (sprite->mItemID == table[index]) {
            break;
        }
    }
    sprite->mRgba.mAlpha = mAlpha[index];
    if (cursorY == index) {
        winSpriteSetDisp(sprite, true);
    } else if (mAlpha[cursorY] == 0.0f) {
        winSpriteSetDisp(sprite, false);
    }
}

void menuPdaDPMList::SortCursor(tagWINDOW_WORK *, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case DPM_SPRITE_SORT_ARROW_POKEMON:
        if (mSort == DPM_SEL_SORT_POKEMON) {
            winSpriteSetDisp(sprite, true);
            sprite->mColor = 0xFFB400FF;
        } else {
            winSpriteSetDisp(sprite, false);
            sprite->mColor = -1;
        }
        break;
    case DPM_SPRITE_SORT_ARROW_DP:
        if (mSort == DPM_SEL_SORT_DP) {
            winSpriteSetDisp(sprite, true);
            sprite->mColor = 0xFFB400FF;
        } else {
            winSpriteSetDisp(sprite, false);
            sprite->mColor = -1;
        }
        break;
    case DPM_SPRITE_SORT_ARROW_PLACE:
        if (mSort == DPM_SEL_SORT_PLACE) {
            sprite->mColor = 0xFFB400FF;
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
            sprite->mColor = -1;
        }
        break;
    default:
        break;
    }
}
void menuPdaDPMList::SetItemColor(tagWINDOW_WORK *, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case DPM_SPRITE_SORT_LABEL_PLACE:
        if (mSort == DPM_SEL_SORT_PLACE) {
            sprite->mColor = 0xFFB400FF;
        } else {
            sprite->mColor = -1;
        }
        break;
    case DPM_SPRITE_SORT_LABEL_DP:
        if (mSort == DPM_SEL_SORT_DP) {
            sprite->mColor = 0xFFB400FF;
        } else {
            sprite->mColor = -1;
        }
        break;
    case DPM_SPRITE_SORT_LABEL_POKEMON:
        if (mSort == DPM_SEL_SORT_POKEMON) {
            sprite->mColor = 0xFFB400FF;
        } else {
            sprite->mColor = -1;
        }
        break;
    default:
        break;
    }
}
long menuPdaDPMList::SortPlace()
{
    DarkPokemon *dp;
    darkPokemonWhere where;
    u8 index;
    u16 pokemonID;
    long i;
    long outer;
    long count;
    u16 darkID;
    long total;
    total = Sort(DPM_SEL_SORT_POKEMON);
    mSortedNum = 0;
    count = 0;
    for (i = 0; i < total; i++) {
        darkID = GetDarkPokemonID(i);
        dp = darkPokemonGetDarkPokemon(darkID);
        dp->getPlace(where, index);
        if (!dp->isSnach() || where != DARK_POKEMON_WHERE_PARTY) {
            continue;
        }
        pokemonID = dp->getPokemonID();
        index = getTemochiIndex(pokemonID, darkID);
        mSortDpBuff[count].key = index;
        mSortDpBuff[count].pokemonID = pokemonID;
        mSortDpBuff[count].darkPokemonID = darkID;
        count++;
    }
    if (count != 0) {
        SortSub(mSortDpBuff, count, true);
        SortSetTemp(mSortDpTemp, mSortDpBuff, count);
    }
    for (outer = 0; outer < DPM_LIST_RELIVE_HALL_SETS; outer++) {
        count = 0;
        for (i = 0; i < total; i++) {
            darkID = GetDarkPokemonID(i);
            dp = darkPokemonGetDarkPokemon(darkID);
            dp->getPlace(where, index);
            if (!dp->isSnach() || where != DARK_POKEMON_WHERE_RELIVE_HALL || index != outer) {
                continue;
            }
            pokemonID = dp->getPokemonID();
            mSortDpBuff[count].key = index;
            mSortDpBuff[count].pokemonID = pokemonID;
            mSortDpBuff[count].darkPokemonID = darkID;
            count++;
        }
        if (count != 0) {
            SortSetTemp(mSortDpTemp, mSortDpBuff, count);
        }
    }
    for (outer = 0; outer < DPM_LIST_PCBOXES; outer++) {
        count = 0;
        for (i = 0; i < total; i++) {
            darkID = GetDarkPokemonID(i);
            dp = darkPokemonGetDarkPokemon(darkID);
            dp->getPlace(where, index);
            if (!dp->isSnach() || where != DARK_POKEMON_WHERE_PCBOX || index != outer) {
                continue;
            }
            pokemonID = dp->getPokemonID();
            mSortDpBuff[count].key = index;
            mSortDpBuff[count].pokemonID = pokemonID;
            mSortDpBuff[count].darkPokemonID = darkID;
            count++;
        }
        if (count != 0) {
            SortSetTemp(mSortDpTemp, mSortDpBuff, count);
        }
    }
    count = 0;
    for (i = 0; i < total; i++) {
        darkID = GetDarkPokemonID(i);
        dp = darkPokemonGetDarkPokemon(darkID);
        dp->getPlace(where, index);
        if (!dp->isSnach() || where != DARK_POKEMON_WHERE_SODATEYA) {
            continue;
        }
        pokemonID = dp->getPokemonID();
        mSortDpBuff[count].key = index;
        mSortDpBuff[count].pokemonID = pokemonID;
        mSortDpBuff[count].darkPokemonID = darkID;
        count++;
    }
    if (count != 0) {
        SortSub(mSortDpBuff, count, true);
        SortSetTemp(mSortDpTemp, mSortDpBuff, count);
    }
    count = 0;
    for (i = 0; i < total; i++) {
        darkID = GetDarkPokemonID(i);
        dp = darkPokemonGetDarkPokemon(darkID);
        dp->getPlace(where, index);
        if (!dp->isSnach() || (where != DARK_POKEMON_EXCHANGED && where != DARK_POKEMON_WHERE_RELEASED)) {
            continue;
        }
        pokemonID = dp->getPokemonID();
        mSortDpBuff[count].key = index;
        mSortDpBuff[count].pokemonID = pokemonID;
        mSortDpBuff[count].darkPokemonID = darkID;
        count++;
    }
    if (count != 0) {
        SortSetTemp(mSortDpTemp, mSortDpBuff, count);
    }
    count = 0;
    for (i = 0; i < total; i++) {
        darkID = GetDarkPokemonID(i);
        dp = darkPokemonGetDarkPokemon(darkID);
        if (dp->isSnach()) {
            continue;
        }
        pokemonID = dp->getPokemonID();
        mSortDpBuff[count].key = (u8)dp->getSnatchIndex();
        mSortDpBuff[count].pokemonID = pokemonID;
        mSortDpBuff[count].darkPokemonID = darkID;
        count++;
    }
    if (count != 0) {
        SortSub(mSortDpBuff, count, true);
        SortSetTemp(mSortDpTemp, mSortDpBuff, count);
    }
    mSortedNum = 0;
    SortSetTemp(mSortBuff, mSortDpTemp, total);
    return total;
}
u8 menuPdaDPMList::getTemochiIndex(u16 pokemonID, u16 darkID)
{
    Pokemon *pokemon;
    void *status = savedataGetStatus(0, SAVE_DATA_HERO);
    u8 i;
    u16 dataID;
    for (i = 0; i < DPM_LIST_PARTY_SIZE; i++) {
        pokemon = heroBiosGetPokemonPtr((Hero *)status, i);
        if (!pokemonCheckValid(pokemon)) {
            continue;
        }
        dataID = pokemonBiosGetDarkpokemonDataId(pokemon);
        pokemonBiosGetPokemonDataId(pokemon);
        if (dataID == darkID) {
            return i;
        }
    }
    return 0;
}

// Orders the records by the dark point ratio of the Pokemon they hold: the
// snatched ones first, then the relived ones, then the rest of the list.
long menuPdaDPMList::SortDP() {
    long i;
    long count = 0;
    long num = Sort(DPM_SEL_SORT_POKEMON);
    DarkPokemon *pokemon;

    mSortedNum = 0;
    for (i = 0; i < num; i++) {
        u16 darkPokemonID = GetDarkPokemonID(i);
        pokemon = darkPokemonGetDarkPokemon(darkPokemonID);

        if (pokemon->isSnach() && !pokemon->isRelive()) {
            u16 pokemonID = pokemon->getPokemonID();
            long initPoint = pokemon->getInitDarkPointDirect();

            mSortDpBuff[count].key = (long)((f32)(pokemon->getDarkPointDirect() * 100) /
                                            (f32)initPoint);
            mSortDpBuff[count].pokemonID = pokemonID;
            mSortDpBuff[count].darkPokemonID = darkPokemonID;
            count++;
        }
    }
    if (count != 0) {
        SortSub2(mSortDpBuff, count, true);
        SortSetTemp(mSortDpTemp, mSortDpBuff, count);
    }
    count = 0;
    for (i = 0; i < num; i++) {
        u16 darkPokemonID = GetDarkPokemonID(i);
        pokemon = darkPokemonGetDarkPokemon(darkPokemonID);

        if (pokemon->isRelive()) {
            u16 pokemonID = pokemon->getPokemonID();

            mSortDpBuff[count].key = pokemon->getSnatchIndex();
            mSortDpBuff[count].pokemonID = pokemonID;
            mSortDpBuff[count].darkPokemonID = darkPokemonID;
            count++;
        }
    }
    if (count != 0) {
        SortSub(mSortDpBuff, count, true);
        SortSetTemp(mSortDpTemp, mSortDpBuff, count);
    }
    count = 0;
    for (i = 0; i < num; i++) {
        u16 darkPokemonID = GetDarkPokemonID(i);
        pokemon = darkPokemonGetDarkPokemon(darkPokemonID);

        if (!pokemon->isSnach()) {
            u16 pokemonID = pokemon->getPokemonID();

            mSortDpBuff[count].key = pokemon->getSnatchIndex();
            mSortDpBuff[count].pokemonID = pokemonID;
            mSortDpBuff[count].darkPokemonID = darkPokemonID;
            count++;
        }
    }
    if (count != 0) {
        SortSub(mSortDpBuff, count, true);
        SortSetTemp(mSortDpTemp, mSortDpBuff, count);
    }
    mSortedNum = 0;
    SortSetTemp(mSortBuff, mSortDpTemp, num);
    return num;
}

// Collects encountered Dark Pokemon records and orders them by the
// Pokemon they hold.
long menuPdaDPMList::SortPokemon() {
    long i;
    long count = 0;
    DarkPokemon *pokemon;

    mSortedNum = 0;
    for (i = 1; i < DPM_LIST_DARK_POKEMON_MAX; i++) {
        pokemon = darkPokemonGetDarkPokemon((u16)i);
        u16 pokemonID = pokemon->getPokemonID();

        if (pokemonID != 0 && pokemon->getStatus() >= DARK_POKEMON_STATUS_FIGHT) {
            mListTemp[count].pokemonID = pokemonID;
            mListTemp[count].darkPokemonID = i;
            mSortBuff[count].key = pokemon->getSnatchIndex();
            mSortBuff[count].pokemonID = mListTemp[count].pokemonID;
            mSortBuff[count].darkPokemonID = mListTemp[count].darkPokemonID;
            count++;
        }
    }
    SortSub(mSortBuff, count, true);
    return count;
}

// Handles the accept and cancel buttons of the list window.
void menuPdaDPMList::Button(tagWINDOW_WORK *window) {
    long num;
    u16 keys;

    if (window == 0) {
        return;
    }
    num = menuPdaDPMListGetDPNum();
    if (winSeqCheckMove(window->mMenuID)) {
        return;
    }
    keys = windowGetKeyInfo()->mKeyTrg;
    if (num != 0 && (keys & WINDOW_KEY_ACCEPT)) {
        window->mAccept = 1;
    }
    if (keys & WINDOW_KEY_CANCEL) {
        window->mAccept = 1;
        window->mCancel = 1;
    }
}

// Moves the cursor of the list window and switches the sort order.
void menuPdaDPMList::Cursor(tagWINDOW_WORK *window) {
    long num;
    long visible;
    u16 keys;
    long sort;

    windowGetKeyInfo();
    CURSOR cursor = _getCursor();
    cursor.x = window->mCursor.x;
    cursor.y = window->mCursor.y;
    num = menuPdaDPMListGetDPNum();
    visible = DPM_LIST_LINES;
    if (num < DPM_LIST_LINES) {
        visible = num;
    }
    keys = windowGetKeyRepeat();
    sort = mSort;
    if (keys & WINDOW_KEY_TRIGGER_R) {
        sort++;
        if (sort >= DPM_SEL_SORT_MAX) {
            sort = DPM_SEL_SORT_POKEMON;
        }
        Sort((DPMSelSort)sort);
    } else if (keys & WINDOW_KEY_TRIGGER_L) {
        sort--;
        if (sort < DPM_SEL_SORT_POKEMON) {
            sort = DPM_SEL_SORT_PLACE;
        }
        Sort((DPMSelSort)sort);
    }
    mSort = (DPMSelSort)sort;
    if (visible != 0) {
        if (keys & WINDOW_KEY_UP) {
            if (cursor.y + cursor.x != 0) {
                GSsndPlayNow(DPM_LIST_SE_CURSOR);
            }
            cursor.y--;
        } else if (keys & WINDOW_KEY_DOWN) {
            if (num - 1 != cursor.y + cursor.x) {
                GSsndPlayNow(DPM_LIST_SE_CURSOR);
            }
            cursor.y++;
        } else if (keys & WINDOW_KEY_RIGHT) {
            if (num - 1 != cursor.y + cursor.x) {
                GSsndPlayNow(DPM_LIST_SE_PAGE);
            }
            cursor.y += visible;
        } else if (keys & WINDOW_KEY_LEFT) {
            if (cursor.y + cursor.x != 0) {
                GSsndPlayNow(DPM_LIST_SE_PAGE);
            }
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
            cursor.y = (s8)(visible - 1);
            if (cursor.x + cursor.y >= num) {
                cursor.y = (s8)(visible - 1);
                cursor.x = (s8)(num - visible);
            }
        }
    }
    _setCursor(cursor);
    window->mCursor.x = cursor.x;
    window->mCursor.y = cursor.y;
}

// Orders the records by the sort the player selected.
long menuPdaDPMList::Sort(DPMSelSort sort) {
    switch (sort) {
    case DPM_SEL_SORT_POKEMON:
        mCount = SortPokemon();
        break;
    case DPM_SEL_SORT_DP:
        mCount = SortDP();
        break;
    case DPM_SEL_SORT_PLACE:
        mCount = SortPlace();
        break;
    }
    return mCount;
}

// Appends the sorted entries to the entries already collected in the result.
void menuPdaDPMList::SortSetTemp(SnatchSortBuff *dest, SnatchSortBuff *src, long count) {
    long i;

    for (i = 0; i < count; i++) {
        memcpy(&dest[mSortedNum], &src[i], sizeof(SnatchSortBuff));
        mSortedNum++;
    }
}

// Inserts every record into the already sorted prefix of the list.
void menuPdaDPMList::SortSub2(SnatchSortBuff *entries, long count, bool ascending) {
    SnatchSortBuff temp;
    long i;
    long j;
    if (ascending) {
        for (i = 1; i < count; i++) {
            memcpy(&temp, &entries[i], sizeof(SnatchSortBuff));
            for (j = i - 1; j >= 0 && entries[j].key > temp.key; j--) {
                memcpy(&entries[j + 1], &entries[j], sizeof(SnatchSortBuff));
            }
            memcpy(&entries[j + 1], &temp, sizeof(SnatchSortBuff));
        }
    } else {
        for (i = 1; i < count; i++) {
            memcpy(&temp, &entries[i], sizeof(SnatchSortBuff));
            for (j = i - 1; j >= 0 && entries[j].key < temp.key; j--) {
                memcpy(&entries[j + 1], &entries[j], sizeof(SnatchSortBuff));
            }
            memcpy(&entries[j + 1], &temp, sizeof(SnatchSortBuff));
        }
    }
}

// Orders the records with Shell sort, repeatedly halving the gap down to one.
void menuPdaDPMList::SortSub(SnatchSortBuff *entries, long count, bool ascending) {
    SnatchSortBuff temp;
    long gap;
    long i;
    long j;
    if (ascending) {
        gap = count / 2;
        while (gap > 0) {
            for (i = gap; i < count; i++) {
                for (j = i - gap; j >= 0 && entries[j].key > entries[j + gap].key; j -= gap) {
                    memcpy(&temp, &entries[j], sizeof(SnatchSortBuff));
                    memcpy(&entries[j], &entries[j + gap], sizeof(SnatchSortBuff));
                    memcpy(&entries[j + gap], &temp, sizeof(SnatchSortBuff));
                }
            }
            gap /= 2;
        }
    } else {
        gap = count / 2;
        while (gap > 0) {
            for (i = gap; i < count; i++) {
                for (j = i - gap; j >= 0 && entries[j].key < entries[j + gap].key; j -= gap) {
                    memcpy(&temp, &entries[j], sizeof(SnatchSortBuff));
                    memcpy(&entries[j], &entries[j + gap], sizeof(SnatchSortBuff));
                    memcpy(&entries[j + gap], &temp, sizeof(SnatchSortBuff));
                }
            }
            gap /= 2;
        }
    }
}

// Runs the list window through its open, active and closing steps.
void menuPdaDPMList::Ctrl(tagWINDOW_WORK *window) {
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, DPM_LIST_SEQ_OPEN);
            menuPdaSetSubres(PDATOP_MENU_RES_DP_MON, DPM_LIST_PDA_SUBRES);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_ACTIVE:
        ListMain(window);
        break;
    case WINDOW_STATE_CLOSING:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, DPM_LIST_SEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
}

// Updates the list scroll and the fade of its rows each frame.
void menuPdaDPMList::ListMain(tagWINDOW_WORK *window) {
    CURSOR cursor = _getCursor();

    ListPosition();
    CalcPosition(&cursor);
    menuPdaSubCalcAlpha(mAlpha, DPM_LIST_LINES, cursor.y);
}

// Points the vertical scroll of the list at the row the cursor is on.
void menuPdaDPMList::CalcPosition(CURSOR *cursor) {
    mTargetScrollY = (f32)(-cursor->x * DPM_LIST_LINE_HEIGHT);
}

// Steps the scroll position of the list towards the target one frame at a time.
void menuPdaDPMList::ListPosition() {
    if (mScrollX != mTargetScrollX) {
        menuPdaSubCalcPositon(&mScrollX, mTargetScrollX, DPM_LIST_SCROLL_SPEED);
    }
    if (mScrollY != mTargetScrollY) {
        menuPdaSubCalcPositon(&mScrollY, mTargetScrollY, DPM_LIST_SCROLL_SPEED);
    }
}
long menuPdaDPMList::Main(long previousIndex, CURSOR *cursor)
{
    CURSOR start;
    long windowID;
    start = *cursor;
    _setCursor(start);
    do {
        windowID = menuOpen(MENU_ID_PDA_DPMON_LIST, 1);
        start = _getCursor();
    } while (windowID != -1 && start.y >= DPM_LIST_LINES);
    _menuClose(MENU_ID_PDA_DPMON_LIST);
    if (windowID == -1 || mCount == 0) {
        return -1;
    }
    cursor->y = start.y;
    cursor->x = start.x;
    return start.x + start.y;
}

void menuPdaDPMList::InitWork()
{
    long count = mCount;
    darkPokemonGetMeetNum();
    mCursor.y = 0;
    mCursor.x = 0;
    mSort = DPM_SEL_SORT_POKEMON;
    mScrollX = 0.0f;
    mScrollY = 0.0f;
    mTargetScrollX = 0.0f;
    mTargetScrollY = 0.0f;
    mReserved = 0;
    if (count >= DPM_LIST_LINES) {
        mVisibleNum = DPM_LIST_LINES;
    } else {
        mVisibleNum = count;
    }
    mListTemp = new SnatchSortTemp[DPM_LIST_DARK_POKEMON_MAX];
    mSortBuff = new SnatchSortBuff[DPM_LIST_DARK_POKEMON_MAX];
    mSortDpBuff = new SnatchSortBuff[DPM_LIST_DARK_POKEMON_MAX];
    mSortDpTemp = new SnatchSortBuff[DPM_LIST_DARK_POKEMON_MAX];
    Sort(mSort);
    menuPdaSubInitAlpha(mAlpha, DPM_LIST_LINES, 0);
}

void menuPdaDPMList::_setCursor(CURSOR pos)
{
    cursorBiosSetPos(DPM_LIST_CURSOR_ID, pos);
}
CURSOR menuPdaDPMList::_getCursor()
{
    return cursorBiosGetPos(DPM_LIST_CURSOR_ID);
}
long menuPdaDPMList::_menuClose(long menuID)
{
    menuClose(menuID);
    return menuCloseSync(menuID, true);
}
