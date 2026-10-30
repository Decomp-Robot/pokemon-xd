#include <CMetaMenu.h>
#include <menuPcBox.h>
#include <Pokemon.h>
#include <pcbox.h>
#include <savedata.h>
#include <menuPtSpriteWork.h>
#include <menu.h>
#include <CAnt.h>
#include <gamedata.h>
#include <memcard.h>
#include <GSfsys.h>
#include <GSres.h>
#include <floor.h>
#include <menuPocketTool.h>
#include <menuPocket2.h>
#include <menuPcBoxDougu.h>
#include <menuPcBoxPokemon.h>
#include <menuReliveHall.h>
#include <menuHologram.h>
#include <relivehall.h>
#include <CMenuGlobalState.h>

// The PC box menu: the Pokemon storage screen opened from the PC. The four
// entry callbacks reach it as the plain box, through the Relive Hall, the
// hologram or the Relive Hall tutorial, and each runs CmenuPcBox::main.
//
// The main loop drives the box and item menus and hands over to the Pokemon
// list, the bag, the item drawer, the Relive Hall and the hologram, and
// restores the sound setup it saved on entry. The tutorial entry backs up
// the save data, builds the party the tutorial expects and recovers it.

namespace {

enum PcBoxState {
    PCBOX_OPEN,
    PCBOX_MAIN_MENU,
    PCBOX_ITEM_MENU,
    PCBOX_ITEM_MENU_RETURN,
    PCBOX_SELECT,
    PCBOX_POKEMON,
    PCBOX_RELIVE_HALL,
    PCBOX_HOLOGRAM,
    PCBOX_TAKE_ITEM,
    PCBOX_BAG,
    PCBOX_THROW_AWAY_ITEM,
    PCBOX_EXIT
};

enum {
    WINDOW_PCBOX_FRAME = 0x79,
    WINDOW_PCBOX_SELECT = 0x7a,
    WINDOW_PCBOX_SELECT_EXTENDED = 0x7b,
    WINDOW_PCBOX_BAG_BACKGROUND = 0x8a,
    WINDOW_SEQ_PCBOX_OPEN = 0x1c4,
    WINDOW_SEQ_PCBOX_CLOSE = 0x1c8,
    FSYS_PCBOX_POKEMON = 0x16,
    FSYS_RELIVE_HALL = 0x824,
    FSYS_BAG = 0x66f,
    SE_PCBOX_OPEN = 0x447,
    BGM_RELIVE_HALL = 0x469,
    MSG_PCBOX_TITLE = 0x353,
    MSG_PCBOX_INFO = 0x3bb0,
    MSG_PCBOX_POKEMON = 0x3bb1,
    MSG_PCBOX_ITEMS = 0x3bb2,
    MSG_PCBOX_RELIVE_HALL = 0x3b70,
    MSG_PCBOX_CLOSE = 0x3bb4,
    MSG_PCBOX_ITEMS_TITLE = 0x37e,
    MSG_PCBOX_ITEMS_INFO = 0x3bb6,
    MSG_PCBOX_TAKE_ITEM = 0x3bb7,
    MSG_PCBOX_THROW_AWAY_ITEM = 0x3bb9,
    MSG_PCBOX_BAG = 0x3bb8,
    MSG_PCBOX_BACK = 0x3bba,
    SAVEDATA_BACKUP_SIZE = 0x27fd0,
    TUTORIAL_POKEMON_COUNT = 18,
    TUTORIAL_DARK_POKEMON_COUNT = 2,
    TUTORIAL_POKEMON_LEVEL = 25
};

struct MenuInfo {
    long mMenu;
    unsigned long mTitle;
    unsigned long mInfo;
    unsigned long mItems[6];
};

class CmenuPcBox : public CMetaMenu {
public:
    virtual void drawAll() {}
    void main(CMenuGlobalState::enumCallFrom callFrom);
    void ctrl(tagWINDOW_WORK *window);
    void drawItemText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long item);
    void drawInfo(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void drawTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void drawRedBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long item);

private:
    void tutorialPokemonRecover();
    void tutorialPokemonSetup();
    void loop();

    long mUnknown;
    MenuInfo *mMenuInfo;
    bool mReliveHallLoaded;
    bool mPokemonLoaded;
    void *mSavedataBackup;
    long mMainSelection, mItemSelection;
    MenuPtWorkSound mSound;
};

MenuInfo menuInfo[] = {
    { WINDOW_PCBOX_SELECT, MSG_PCBOX_TITLE, MSG_PCBOX_INFO, { MSG_PCBOX_POKEMON, MSG_PCBOX_ITEMS, MSG_PCBOX_CLOSE, 0, 0, 0 } },
    { WINDOW_PCBOX_SELECT_EXTENDED, MSG_PCBOX_TITLE, MSG_PCBOX_INFO, { MSG_PCBOX_POKEMON, MSG_PCBOX_ITEMS, MSG_PCBOX_RELIVE_HALL, MSG_PCBOX_CLOSE, 0, 0 } },
    { WINDOW_PCBOX_SELECT_EXTENDED, MSG_PCBOX_ITEMS_TITLE, MSG_PCBOX_ITEMS_INFO, { MSG_PCBOX_TAKE_ITEM, MSG_PCBOX_BAG, MSG_PCBOX_THROW_AWAY_ITEM, MSG_PCBOX_BACK, 0, 0 } }
};

CMenuGlobalState _menuGlobalState;
CmenuPcBox _menuPcBox;
long localWindowTable[] = { 0 };

}

bool menuReliveHallTutorialGetIsCancelExit()
{
    return _menuGlobalState.mCancelExit;
}

extern "C" void menuReliveHallTutorialExit() {}

extern "C" void menuReliveHallTutorialMain()
{
    _menuPcBox.main(CMenuGlobalState::CALL_FROM_RELIVE_HALL_TUTORIAL);
}

extern "C" void menuReliveHallTutorialInit() {}
extern "C" void menuHologramExit() {}

extern "C" void menuHologramMain()
{
    _menuPcBox.main(CMenuGlobalState::CALL_FROM_HOLOGRAM);
}

extern "C" void menuHologramInit() {}
extern "C" void menuReliveHallExit() {}

extern "C" void menuReliveHallMain()
{
    _menuPcBox.main(CMenuGlobalState::CALL_FROM_RELIVE_HALL);
}

extern "C" void menuReliveHallInit() {}
extern "C" void menuPcBoxExit() {}

extern "C" void menuPcBoxMain()
{
    _menuPcBox.main(CMenuGlobalState::CALL_FROM_PC_BOX);
}

extern "C" void menuPcBoxInit() {}

extern "C" long menuPcBoxSelectMenuItem(tagWINDOW_WORK *)
{
    return 0;
}

extern "C" long menuPcBoxCtrl(tagWINDOW_WORK *window)
{
    _menuPcBox.ctrl(window);
    return 0;
}

extern "C" long menuPcBoxItem3(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    _menuPcBox.drawItemText(window, sprite, 3);
    return 0;
}

extern "C" long menuPcBoxItem2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    _menuPcBox.drawItemText(window, sprite, 2);
    return 0;
}

extern "C" long menuPcBoxItem1(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    _menuPcBox.drawItemText(window, sprite, 1);
    return 0;
}

extern "C" long menuPcBoxItem0(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    _menuPcBox.drawItemText(window, sprite, 0);
    return 0;
}

extern "C" long menuPcBoxInfo(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    _menuPcBox.drawInfo(window, sprite);
    return 0;
}

extern "C" long menuPcBoxTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    _menuPcBox.drawTitle(window, sprite);
    return 0;
}

extern "C" long menuPcBoxBarRed3(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    _menuPcBox.drawRedBar(window, sprite, 3);
    return 0;
}

extern "C" long menuPcBoxBarRed2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    _menuPcBox.drawRedBar(window, sprite, 2);
    return 0;
}

extern "C" long menuPcBoxBarRed1(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    _menuPcBox.drawRedBar(window, sprite, 1);
    return 0;
}

extern "C" long menuPcBoxBarRed0(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    _menuPcBox.drawRedBar(window, sprite, 0);
    return 0;
}

namespace {

void CmenuPcBox::tutorialPokemonRecover()
{
    memcpy(savedataGetStatus(0, 0), mSavedataBackup, SAVEDATA_BACKUP_SIZE);
    mSavedataBackup = memcardMemFreeForSavedata(mSavedataBackup);
}

void CmenuPcBox::tutorialPokemonSetup()
{
    mSavedataBackup = memcardMemAllocForSavedata();
    memcpy(mSavedataBackup, savedataGetStatus(0, 0), SAVEDATA_BACKUP_SIZE);
    PCBOX::getPointer()->init();
    CReliveHall::getPointer()->init();
    static u16 _darkPokemonTbl[] = { 10, 1 };
    static u16 _pokemonTbl[] = { 0x14a, 0x173, 0x141, 0x148, 0x132, 0x151, 0x15a, 0x14f, 0x16f, 0x14c, 0x130, 0x15f, 0x122, 0x140, 0x169, 0x18b, 0x178, 0x163 };
    for (unsigned long slot = 0; slot < TUTORIAL_POKEMON_COUNT; slot++) {
        Pokemon pokemon;
        pokemonCreate(&pokemon, _pokemonTbl[slot], TUTORIAL_POKEMON_LEVEL, gamedataGetStatus(0, 1));
        if (slot < TUTORIAL_DARK_POKEMON_COUNT) menuPocketSetDarkPokemon(pokemon, 0.5f, _darkPokemonTbl[slot]);
        PCBOX::getPointer()->setPokemon(&pokemon, 0, slot);
    }
}

void CmenuPcBox::ctrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case 0:
        if (window->mFlag == 0) { winSeqSetMenu(window->mMenuID, WINDOW_SEQ_PCBOX_OPEN); window->mFlag = 1; }
        break;
    case 3:
        if (window->mFlag == 0) { winSeqSetMenu(window->mMenuID, WINDOW_SEQ_PCBOX_CLOSE); window->mFlag = 1; }
        break;
    }
}

void CmenuPcBox::drawItemText(tagWINDOW_WORK *, tagSPRITE_WORK *sprite, long item)
{
    sprite->mMsgID = mMenuInfo->mItems[item];
}
void CmenuPcBox::drawInfo(tagWINDOW_WORK *, tagSPRITE_WORK *sprite)
{
    sprite->mMsgID = mMenuInfo->mInfo;
}
void CmenuPcBox::drawTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    menuPtSpriteWorkSetMenuItem(window, sprite, mMenuInfo->mTitle);
}
void CmenuPcBox::drawRedBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long item)
{
    long value = windowGetValue(window->mMenuID);
    winSpriteSetDisp(sprite, value - item == 0);
}

void CmenuPcBox::loop()
{
    long *work = mState.mWork;
    MenuInfo *reliveHallMenu = &menuInfo[1];
    MenuInfo *itemMenu = &menuInfo[2];
    for (;;) {
        mState.update();
        switch (mState.mCurrent) {
        case PCBOX_OPEN:
            setState(PCBOX_MAIN_MENU, work);
            menuOpen(WINDOW_PCBOX_FRAME, 0);
            break;
        case PCBOX_MAIN_MENU:
            if (CReliveHall::isOpen()) mMenuInfo = reliveHallMenu;
            else mMenuInfo = &menuInfo[0];
            setState(PCBOX_SELECT, work);
            break;
        case PCBOX_ITEM_MENU:
            mItemSelection = 0;
            mMenuInfo = itemMenu;
            setState(PCBOX_SELECT, work);
            break;
        case PCBOX_ITEM_MENU_RETURN:
            mMenuInfo = itemMenu;
            setState(PCBOX_SELECT, work);
            break;
        case PCBOX_SELECT: {
            long *selection;
            if (mState.mPrevious == PCBOX_MAIN_MENU) selection = &mMainSelection;
            else selection = &mItemSelection;
            menuOpenCustom(mMenuInfo->mMenu, windowGetActiveID(), selection, 0, 1, 0);
            *selection = windowGetValue(mMenuInfo->mMenu);
            menuClose(mMenuInfo->mMenu);
            menuCloseSync(mMenuInfo->mMenu, 1);
            if (mState.mPrevious == PCBOX_MAIN_MENU) {
                if (*selection != -1) {
                    switch (mMenuInfo->mItems[*selection]) {
                    case MSG_PCBOX_POKEMON: systemFadeOutSetAndWait(-1.0f); setState(PCBOX_POKEMON, work); break;
                    case MSG_PCBOX_ITEMS: setState(PCBOX_ITEM_MENU, work); break;
                    case MSG_PCBOX_RELIVE_HALL: systemFadeOutSetAndWait(-1.0f); setState(PCBOX_RELIVE_HALL, work); break;
                    case MSG_PCBOX_CLOSE: setState(PCBOX_EXIT, work); break;
                    }
                } else setState(PCBOX_EXIT, work);
            } else {
                if (*selection != -1) {
                    switch (mMenuInfo->mItems[*selection]) {
                    case MSG_PCBOX_TAKE_ITEM: setState(PCBOX_TAKE_ITEM, work); break;
                    case MSG_PCBOX_BAG: setState(PCBOX_BAG, work); break;
                    case MSG_PCBOX_THROW_AWAY_ITEM: setState(PCBOX_THROW_AWAY_ITEM, work); break;
                    case MSG_PCBOX_BACK: setState(PCBOX_MAIN_MENU, work); break;
                    }
                } else setState(PCBOX_MAIN_MENU, work);
            }
            break;
        }
        case PCBOX_POKEMON:
            if (mState.mPrevious == PCBOX_RELIVE_HALL) _menuGlobalState.mMode = 2;
            else _menuGlobalState.mMode = 0;
            if (!mPokemonLoaded) { GSfsysReadWait(FSYS_PCBOX_POKEMON); mPokemonLoaded = true; }
            localWindowDeleteAll();
            if (windowSearchID(WINDOW_PCBOX_FRAME)) menuClose(WINDOW_PCBOX_FRAME);
            menuPcBoxPokemon(_menuGlobalState);
            localWindowCreateAll();
            if (_menuGlobalState.mExit) { setState(PCBOX_EXIT, work); break; }
            if (_menuGlobalState.mResult == 2) { setState(PCBOX_RELIVE_HALL, work); break; }
            systemFadeInKick();
            setState(PCBOX_OPEN, work);
            break;
        case PCBOX_RELIVE_HALL:
            if (mState.mPrevious == PCBOX_POKEMON) _menuGlobalState.mMode = 1;
            else if (mState.mPrevious == PCBOX_HOLOGRAM) _menuGlobalState.mMode = 3;
            else _menuGlobalState.mMode = 0;
            if (!mReliveHallLoaded) { GSfsysReadWait(FSYS_RELIVE_HALL); mReliveHallLoaded = true; }
            localWindowDeleteAll();
            if (windowSearchID(WINDOW_PCBOX_FRAME)) menuClose(WINDOW_PCBOX_FRAME);
            menuPtPlayBgm(BGM_RELIVE_HALL);
            menuReliveHallExecFromPcBox(_menuGlobalState);
            localWindowCreateAll();
            if (_menuGlobalState.mExit) { setState(PCBOX_EXIT, work); break; }
            if (_menuGlobalState.mResult == 1) { setState(PCBOX_POKEMON, work); break; }
            if (_menuGlobalState.mResult == 4) { setState(PCBOX_EXIT, work); break; }
            if (_menuGlobalState.mCallFrom == CMenuGlobalState::CALL_FROM_RELIVE_HALL) { setState(PCBOX_EXIT, work); break; }
            if (_menuGlobalState.mCallFrom == CMenuGlobalState::CALL_FROM_HOLOGRAM) { setState(PCBOX_HOLOGRAM, work); break; }
            menuPtRestoreSound(&mSound);
            systemFadeInKick();
            setState(PCBOX_OPEN, work);
            break;
        case PCBOX_HOLOGRAM:
            _menuGlobalState.mMode = 0;
            localWindowDeleteAll();
            menuHologramExecFromPcBox(_menuGlobalState);
            localWindowCreateAll();
            if (_menuGlobalState.mResult == 2) { setState(PCBOX_RELIVE_HALL, work); break; }
            setState(PCBOX_EXIT, work);
            break;
        case PCBOX_TAKE_ITEM:
            systemFadeOutSetAndWait(-1.0f);
            localWindowDeleteAll();
            menuClose(WINDOW_PCBOX_FRAME);
            menuPcBoxDouguHikidashi(MENU_PCBOX_DOUGU_HIKIDASHI_TAKE_OUT);
            menuOpen(WINDOW_PCBOX_FRAME, 0);
            localWindowCreateAll();
            systemFadeInKick();
            setState(PCBOX_ITEM_MENU_RETURN, work);
            break;
        case PCBOX_THROW_AWAY_ITEM:
            systemFadeOutSetAndWait(-1.0f);
            localWindowDeleteAll();
            menuClose(WINDOW_PCBOX_FRAME);
            menuPcBoxDouguHikidashi(MENU_PCBOX_DOUGU_HIKIDASHI_THROW_AWAY);
            menuOpen(WINDOW_PCBOX_FRAME, 0);
            localWindowCreateAll();
            systemFadeInKick();
            setState(PCBOX_ITEM_MENU_RETURN, work);
            break;
        case PCBOX_BAG:
            systemFadeOutSetAndWait(-1.0f);
            GSfsysReadWait(FSYS_BAG);
            localWindowDeleteAll();
            menuClose(WINDOW_PCBOX_FRAME);
            menuOpen(WINDOW_PCBOX_BAG_BACKGROUND, 0);
            systemFadeInKick();
            systemFadeWaitToEnd();
            menuPocket2Call(4, 0, 0);
            menuClose(WINDOW_PCBOX_BAG_BACKGROUND);
            menuOpen(WINDOW_PCBOX_FRAME, 0);
            localWindowCreateAll();
            GSfsysFree(FSYS_BAG);
            GSresFreeGroup(FSYS_BAG);
            systemFadeInKick();
            setState(PCBOX_ITEM_MENU_RETURN, work);
            break;
        case PCBOX_EXIT:
            systemFadeOutSetAndWait(-1.0f);
            if (windowSearchID(WINDOW_PCBOX_FRAME)) menuClose(WINDOW_PCBOX_FRAME);
            return;
        }
    }
}

void CmenuPcBox::main(CMenuGlobalState::enumCallFrom callFrom)
{
    CAnt::systemInit();
    init(menuMetaDefaults);
    mMenuInfo = 0;
    mReliveHallLoaded = false;
    mPokemonLoaded = false;
    mMainSelection = 0;
    mItemSelection = 0;
    memset(&mSound, 0, sizeof(mSound));
    menuPtSaveSound(&mSound);
    localWindowSetTable(localWindowTable);
    localWindowCreateAll();
    _menuGlobalState.init();
    _menuGlobalState.mCallFrom = callFrom;
    CReliveHall *reliveHall = CReliveHall::getPointer();
    reliveHall->allocMakeBackup();
    switch (_menuGlobalState.mCallFrom) {
    case CMenuGlobalState::CALL_FROM_PC_BOX:
        menuPtPlaySe(SE_PCBOX_OPEN);
        mPokemonLoaded = true;
        setState(PCBOX_OPEN);
        loop();
        localWindowDeleteAll();
        if (mReliveHallLoaded) { GSfsysFree(FSYS_RELIVE_HALL); GSresFreeGroup(FSYS_RELIVE_HALL); }
        break;
    case CMenuGlobalState::CALL_FROM_RELIVE_HALL:
        mReliveHallLoaded = true;
        setState(PCBOX_RELIVE_HALL);
        loop();
        localWindowDeleteAll();
        if (mPokemonLoaded) { GSfsysFree(FSYS_PCBOX_POKEMON); GSresFreeGroup(FSYS_PCBOX_POKEMON); }
        menuPtRestoreSound(&mSound);
        break;
    case CMenuGlobalState::CALL_FROM_RELIVE_HALL_TUTORIAL: {
        long lastStage = relivehallGetLastViewStage();
        long currentBox = menuPcBoxPokemonGetCurrentBox();
        menuPcBoxPokemonSetCurrentBox(0);
        relivehallSetLastViewStage(0);
        tutorialPokemonSetup();
        _menuGlobalState.mTutorial = true;
        mReliveHallLoaded = true;
        setState(PCBOX_RELIVE_HALL);
        loop();
        localWindowDeleteAll();
        if (mPokemonLoaded) { GSfsysFree(FSYS_PCBOX_POKEMON); GSresFreeGroup(FSYS_PCBOX_POKEMON); }
        tutorialPokemonRecover();
        menuPcBoxPokemonSetCurrentBox(currentBox);
        relivehallSetLastViewStage(lastStage);
        menuPtRestoreSound(&mSound);
        break;
    }
    case CMenuGlobalState::CALL_FROM_HOLOGRAM:
        setState(PCBOX_HOLOGRAM);
        loop();
        localWindowDeleteAll();
        if (mReliveHallLoaded) { GSfsysFree(FSYS_RELIVE_HALL); GSresFreeGroup(FSYS_RELIVE_HALL); }
        if (mPokemonLoaded) { GSfsysFree(FSYS_PCBOX_POKEMON); GSresFreeGroup(FSYS_PCBOX_POKEMON); }
        break;
    }
    reliveHall->checkWithBackupSet();
    CAnt::systemFree();
    GSfloorPop();
}

}
