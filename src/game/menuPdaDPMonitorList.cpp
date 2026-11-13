#include <types.h>
#include <cursorBios.h>
#include <darkPokemon.h>
#include <GSsndPlayback.h>
#include <menu.h>
#include <menuPda.h>
#include <menuPdaSub.h>
#include <winSprite.h>

#include <string.h>

extern "C" {
long menuPdaDPMListGetDPNum(void);
}

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

// The Dark Pokemon monitor's entry list, which orders its records by the
// currently selected sort key.
class menuPdaDPMList {
public:
    void SortSub2(SnatchSortBuff *entries, long count, bool ascending);
    void SortSub(SnatchSortBuff *entries, long count, bool ascending);
    void SortSetTemp(SnatchSortBuff *dest, SnatchSortBuff *src, long count);
    long Sort(DPMSelSort sort);
    long SortDP();
    long SortPokemon();
    long SortPlace();
    u16 GetDarkPokemonID(long index);
    void Button(tagWINDOW_WORK *window);
    void Cursor(tagWINDOW_WORK *window);
    void Ctrl(tagWINDOW_WORK *window);
    void ListMain(tagWINDOW_WORK *window);
    void CalcPosition(CURSOR *cursor);
    void ListPosition();
    CURSOR _getCursor();
    void _setCursor(CURSOR cursor);

    long mCount;                  // records the list holds.
    f32 mAlpha[DPM_LIST_LINES];   // fade of each row of the list.
    CURSOR mCursor;               // cursor position of the list.
    f32 mScrollX;                 // scroll position being drawn.
    f32 mScrollY;
    f32 mTargetScrollX;           // scroll position being moved to.
    f32 mTargetScrollY;
    u8 _40[4];
    long mVisibleNum;             // rows the window shows at once.
    DPMSelSort mSort;             // order the list is sorted by.
    SnatchSortTemp *mListTemp;    // IDs of the entries being sorted.
    SnatchSortBuff *mSortBuff;    // sorted entries the window shows.
    SnatchSortBuff *mSortDpBuff;  // work entries of the dark point sort.
    SnatchSortBuff *mSortDpTemp;  // result entries of the dark point sort.
    long mSortedNum;              // entries written to the result so far.
};

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
