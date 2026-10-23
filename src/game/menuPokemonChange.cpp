#include <string.h>
#include <Pokemon.h>
#include <menuModel.h>
#include <hero.h>
#include <PokemonStatus.h>
#include <menuState.h>
#include <menuItemBios.h>
#include <window.h>
#include <fade.h>
#include <time.h>
#include <GSsnd.h>
#include <GSmodel.h>
#include <GSres.h>
#include <GSfsys.h>
#include <GSthread.h>
#include <camera.h>
#include <floor.h>
#include <floorManager.h>
#include <memcard.h>
#include <gamedata.h>
#include <menuSub.h>
#include <menuPocketTool.h>
#include <winMsg.h>
#include <menu.h>
#include <menuDataBios.h>
#include <menuFace.h>
#include <itemDataBios.h>
#include <menuPokemonChange.h>
#include <menuPokemon.h>
#include <menuPtSpriteWork.h>
#include <menuGBA.h>
#include <menuCBRule.h>
#include <menuCB_Sub1.h>
#include <GSmsg.h>
#include <savedata.h>
#include <hsdRandom.h>
#include <pokemonBios.h>
#include <pokemonEvolution.h>
#include <pokemonDataBios.h>
#include <darkPokemon.h>
#include <gbaCommand.h>
#include <memo.h>
#include <msgctrl.h>
#include <scriptModel.h>
#include <dolphin/dvd.h>
#include <libc/math.h>
#include <pokecolo.h>
#include <waza.h>

namespace {
enum PokemonChangeInterfaceID {
    CHANGE_MENU_ACTION_BACKGROUND = 0xD2,
    CHANGE_MENU_BACKGROUND = 0xD3,
    CHANGE_MENU_PARTY = 0xD4,
    CHANGE_MENU_INFO = 0xD5,
    CHANGE_MENU_CONFIRM = 0xDD,
    CHANGE_MENU_FADE = 0xDF,
    CHANGE_MENU_OVERLAY = 0xE1,
    CHANGE_MENU_TWO_ACTIONS = 0x11A,
    CHANGE_MENU_THREE_ACTIONS = 0x11B,
    CHANGE_GLYPH_MALE = 0x2642,
    CHANGE_GLYPH_FEMALE = 0x2640,
    CHANGE_BACKGROUND_MODEL_RESOURCE = 0x5681000,
    CHANGE_BACKGROUND_CAMERA_RESOURCE = 0x5691800,
    CHANGE_BACKGROUND_RESOURCE_GROUP = 0x37C,
    CHANGE_RESOURCE_GROUP = 0x88C,
    CHANGE_FLOOR = 0x394,
    CHANGE_MSG_PARTY_TOO_SMALL = 0xC482,
    CHANGE_MSG_PARTY_SHADOW_RESTRICTED = 0xC483,
    CHANGE_MSG_PARTY_ITEM_RESTRICTED = 0xC484,
    CHANGE_MSG_SAVE_BEFORE_EXCHANGE = 0xC48E,
    CHANGE_MSG_SELECT_POKEMON = 0xC491,
    CHANGE_MSG_EGG_STATUS_RESTRICTED = 0xC492,
    CHANGE_MSG_CORRUPT_STATUS_RESTRICTED = 0xC493,
    CHANGE_MSG_ACTION_STATUS = 0xC494,
    CHANGE_MSG_ITEM_EXCHANGE_RESTRICTED = 0xC496,
    CHANGE_MSG_SHADOW_EXCHANGE_RESTRICTED = 0xC497,
    CHANGE_MSG_NO_REMAINING_BATTLE_POKEMON = 0xC498,
    CHANGE_MSG_CONFIRM_EXIT = 0xC499,
    CHANGE_MSG_EXCHANGE_FAILED = 0xC49F,
    CHANGE_MSG_EXCHANGE_AGAIN = 0xC4A1,
    CHANGE_MSG_EXIT = 0xC4A2,
    CHANGE_MSG_CONNECTION_LOST = 0xC4A3,
    CHANGE_MSG_NO_EXCHANGE_POKEMON = 0xC4A5,
    CHANGE_MSG_PARTY_INVALID = 0xC4A7,
    CHANGE_MSG_PARTY_DATA_RESTRICTED = 0xC4AA,
    CHANGE_MSG_DATA_EXCHANGE_RESTRICTED = 0xC4AB,
    CHANGE_MSG_YES = 0x426A,
    CHANGE_MSG_NO = 0x426B,
    CHANGE_ITEM_PLAYER_PANEL = 0x60D,
    CHANGE_ITEM_OTHER_PLAYER_PANEL = 0x60E,
    CHANGE_ITEM_CONFIRM_PANEL = 0x63E,
    CHANGE_ITEM_EXCHANGE_PANEL = 0x650,
    CHANGE_ITEM_GBA_PARTY_FIRST = 0x627,
    CHANGE_ITEM_INFO_MESSAGE = 0x61E,
    CHANGE_ITEM_TWO_ACTION_FIRST = 0x8C3,
    CHANGE_ITEM_TWO_ACTION_SECOND = 0x8C4,
    CHANGE_ITEM_THREE_ACTION_FIRST = 0x8CD,
    CHANGE_ITEM_THREE_ACTION_SECOND = 0x8CC,
    CHANGE_ITEM_THREE_ACTION_THIRD = 0x8CB,
    CHANGE_SPRITE_LOCAL_PANEL = 0x20F,
    CHANGE_SPRITE_GBA_PANEL = 0x20E,
    CHANGE_SEQUENCE_SELECT = 0x1BC,
    CHANGE_ROM_VERSION_UNKNOWN = 0xFFFF,
    CHANGE_SOUND_ERROR = 0x13,
};

enum PokemonChangeCapacity {
    CHANGE_PARTY_CAPACITY = 6,
    CHANGE_FACE_CAPACITY = 13,
    CHANGE_MENU_CAPACITY = 8,
    CHANGE_ACTION_CAPACITY = 3
};

enum enumPlayerIndex { PLAYER_LOCAL, PLAYER_GBA, PLAYER_COUNT };
enum enumMenuState {
    MENU_CLOSED, MENU_BACKGROUND, MENU_CONNECT, MENU_SELECT_FIRST,
    MENU_SELECT_SECOND, MENU_CONFIRM, MENU_EXCHANGE
};
enum MENU_POKEMONCHANGE_USERTYPE { USER_LOCAL, USER_GBA };
enum PokemonChangeState {
    STATE_IDLE, STATE_START, STATE_SAVE_ERROR, STATE_CONNECTION_ERROR, STATE_EXIT,
    STATE_CONNECT, STATE_SELECT_START, STATE_SELECT_FIRST, STATE_FIRST_SELECTED,
    STATE_FIRST_CANCEL, STATE_FIRST_STATUS, STATE_FIRST_EXCHANGE,
    STATE_SELECT_SECOND, STATE_SECOND_SELECTED, STATE_SECOND_STATUS,
    STATE_SECOND_EXCHANGE, STATE_SECOND_DECIDE_WAIT, STATE_CONFIRM_EXCHANGE,
    STATE_EXCHANGE_START, STATE_EXCHANGE_MAIN, STATE_EXCHANGE_OK,
    STATE_EVOLUTION, STATE_SAVE_REPORT, STATE_CHECK_NEXT
};
enum PokemonExchangeState {
    POKEMON_INVALID, POKEMON_CORRUPT, POKEMON_EGG, POKEMON_SHADOW,
    POKEMON_ITEM_RESTRICTED, POKEMON_EVENT_RESTRICTED,
    POKEMON_DATA_RESTRICTED, POKEMON_EXCHANGE_OK
};

struct PokemonFaceList {
    Pokemon* mPokemon[CHANGE_FACE_CAPACITY];
    unsigned long mIndex;
};

class CMenuPokemonChange {
public:
    enumMenuState mMenuState;
    long mMenus[CHANGE_MENU_CAPACITY];
    Hero* mHero;
    Hero mGbaHero;
    enumPlayerIndex mPlayerIndex;
    CState mState;
    long mOpeningMenu;
    void* mSaveData;
    bool mPlayerReady[PLAYER_COUNT];
    unsigned long mBgModelID;
    _GSmodel* mBgModel;
    long mCursor[PLAYER_COUNT];
    long mStatusIndex;
    MENU_MODEL mPokemonModels[PLAYER_COUNT];
    long mGbaEvolutionSpecies;
    u64 mDrawTime;
    bool mSelectionActive;
    u8 mReopenSelection;

    inline Hero* getHero(enumPlayerIndex player)
    {
        if (player == PLAYER_LOCAL)
            return mHero;
        else
            return &mGbaHero;
    }

    inline Pokemon* getPokemon(long index);

    inline Pokemon* getSelectPokemon(long player)
    {
        return getPokemon(mCursor[player]);
    }

    inline void releasePokemonModels()
    {
        for (long player = 0; player < PLAYER_COUNT; ++player) {
            menuModelFree(&mPokemonModels[player]);
            menuModelTerm(&mPokemonModels[player]);
        }
    }

    inline void loadPokemonModels(tagMENU_ITEM_DATA* item)
    {
        for (long player = 0; player < PLAYER_COUNT; ++player) {
            menuModelInit(&mPokemonModels[player], item->mWidth, item->mHeight);
            menuModelLoadPokemon(&mPokemonModels[player], getPokemon(mCursor[player]));
            menuModelCheck(&mPokemonModels[player], 1);
        }
    }

    inline long getHeroPokemonIndex(long index)
    {
        if (index >= getQuantityHeroPokemon(PLAYER_LOCAL))
            index -= getQuantityHeroPokemon(PLAYER_LOCAL);
        return index;
    }
    inline void loadSelectedPokemonModels(long menu, unsigned long itemID)
    {
        tagMENU_ITEM_DATA* item = menuPtItemGetByMenuIdAndItemId(menu, itemID, 0);
        for (long player = 0; player < PLAYER_COUNT; ++player) {
            menuModelInit(&mPokemonModels[player], item->mWidth, item->mHeight);
            menuModelLoadPokemon(&mPokemonModels[player], getPokemon(mCursor[player]));
            menuModelCheck(&mPokemonModels[player], 1);
        }
    }
    inline float calcAnimationTimeRate0to1(float seconds);
    inline float calcAnimationTimeRate0to1to0(float seconds);

    bool pokemonExchangeCheck(enumPlayerIndex);
    bool pokmonSelectCheck(enumPlayerIndex);
    bool isPokemonLookAbleStatus(long);
    void stateFunctionError(Pokemon*, unsigned long);
    void stateFunctionCheckNext();
    bool gbaConnectionCheckIsError();
    void stateFunctionSaveReport();
    void stateFunctionEvolution();
    void _pokemonChangeEvolutionEffect(Pokemon*, MENU_POKEMONCHANGE_USERTYPE,
                                       unsigned short, POKEMON_EVOLUTION_INFO);
    void stateFunctionExChangeOK();
    void stateFunctionExChangeMain();
    void stateFunctionExChangeStart();
    void stateFunctionConfirmExChange();
    void stateFunctionSelectPokemon2DecideAfterWait();
    void stateFunctionSelectPokemon2PokeExChange();
    void stateFunctionSelectPokemon2CheckStatus();
    void stateFunctionSelectPokemon2Selected();
    void stateFunctionSelectPokemon2Main();
    void stateFunctionSelectPokemon1PokeExChange();
    void stateFunctionSelectPokemon1CheckStatus();
    void stateFunctionSelectPokemon1Chanceld();
    void stateFunctionSelectPokemon1Selected();
    void stateFunctionSelectPokemon1Main();
    void stateFunctionSelectPokemonStart();
    void stateFunctionSelectPokemon2Reset();
    void stateFunctionCheckconnection();
    void selectItemHookPrime(tagWINDOW_WORK*, tagSPRITE_WORK*, unsigned long);
    void selectItemHook(tagWINDOW_WORK*, tagSPRITE_WORK*);
    void selectDrawHook(tagWINDOW_WORK*);
    void ctrlHook(tagWINDOW_WORK*);
    long getQuantityAllPokemon();
    int getQuantityHeroPokemon(enumPlayerIndex);
    long selectPokemonMenuOpen();
    void waitMenuOpen();
    void oneMenuLoad(long);
    void menuStateChange(enumMenuState);
    void bgDrawFadeIn(float);
    void bgDrawFadeIn();
    u8 bgDrawFadeIsLive();
    void bgDrawFadeOut();
    void setMenuByTbl(long*);
    void requestPokemonFaceLoad();
    Pokemon* pokemonStatusCallback(Pokemon*, MENU_POKEMON_CB_TYPE, enumPlayerIndex);
    long pokemonStatusSearch(long, long);
    void menuPokemonStatusExecute(enumPlayerIndex);
    void loop();
    void main();
    void bgModelRelease();
    void bgModelLoad();
    void init();
};

CMenuPokemonChange MenuPokemonChange;

inline PokemonFaceList& getPokemonFaceList()
{
    static PokemonFaceList stPokemonListFace;
    return stPokemonListFace;
}

enum PokemonExchangeResource {
    EXCHANGE_MODEL_MENU = 0xDE,
    EXCHANGE_MODEL_ITEM = 0x652,
    EXCHANGE_MODEL_RESOURCE = 0xFCC1000,
    EXCHANGE_CAMERA_RESOURCE = 0xFCD1800,
    EXCHANGE_MESSAGE_FIRST_DEPARTURE = 0xC49A,
    EXCHANGE_MESSAGE_SECOND_DEPARTURE = 0xC49B,
    EXCHANGE_MESSAGE_COMPLETE = 0xC49C,
    EXCHANGE_TEXT_TRAINER = 0x4D,
    EXCHANGE_TEXT_OTHER_TRAINER = 0x57,
    EXCHANGE_TEXT_FIRST_POKEMON = 0x32,
    EXCHANGE_TEXT_SECOND_POKEMON = 0x33,
    EXCHANGE_SOUND_ACCEPT = 0x11,
    EXCHANGE_SOUND_CANCEL = 0x12,
    EXCHANGE_SOUND_ANIMATION = 0x462,
    EXCHANGE_FRIEND_LEVEL = 70,
    EXCHANGE_SAVE_BLOCK_SIZE = 0x27FD0
};

enum {
    TRADE_GBA_PORT = 2,
    TRADE_SAVE_TASK = 8,
    TRADE_SAVE_MODE = 2,
    TRADE_SAVE_PENDING = 0,
    TRADE_SAVE_SUCCEEDED = 4,
    TRADE_SAVE_COMPLETION_DELAY = 60,
    SAVE_BLOCK_SIZE = 0x27FD0,
    MSG_TRADE_SAVE_IN_PROGRESS = 0xC49D,
    MSG_TRADE_SAVE_FAILED = 0xC49E,
    MSG_TRADE_SAVE_COMPLETE = 0xC4A0,
    SOUND_TRADE_SAVE_ERROR = 0x13,
    MENU_TRADE_PREVIEW = 0xDE,
    ITEM_TRADE_PREVIEW_VIEWPORT = 0x652,
    POKEMON_EVOLUTION_TRADE = 2,
    POKEMON_SPECIES_NONE = 0,
    POKEMON_SPECIES_INVALID = 0xFFFF
};

enum PokemonChangeItemID {
    CHANGE_ITEM_PARTY_FACE_1 = 0x5F3,
    CHANGE_ITEM_PARTY_FACE_2 = 0x5F4,
    CHANGE_ITEM_PARTY_FACE_3 = 0x5F5,
    CHANGE_ITEM_PARTY_FACE_4 = 0x5F6,
    CHANGE_ITEM_PARTY_FACE_5 = 0x5F7,
    CHANGE_ITEM_PARTY_FACE_6 = 0x5F8,
    CHANGE_ITEM_PARTY_STATE_1 = 0x5F9,
    CHANGE_ITEM_PARTY_STATE_2 = 0x5FA,
    CHANGE_ITEM_PARTY_STATE_3 = 0x5FB,
    CHANGE_ITEM_PARTY_STATE_4 = 0x5FC,
    CHANGE_ITEM_PARTY_STATE_5 = 0x5FD,
    CHANGE_ITEM_PARTY_STATE_6 = 0x5FE,
    CHANGE_ITEM_PARTY_ITEM_1 = 0x5FF,
    CHANGE_ITEM_PARTY_ITEM_2 = 0x600,
    CHANGE_ITEM_PARTY_ITEM_3 = 0x601,
    CHANGE_ITEM_PARTY_ITEM_4 = 0x602,
    CHANGE_ITEM_PARTY_ITEM_5 = 0x603,
    CHANGE_ITEM_PARTY_ITEM_6 = 0x604,
    CHANGE_ITEM_PLAYER_MARK_1 = 0x605,
    CHANGE_ITEM_PLAYER_MARK_2 = 0x606,
    CHANGE_ITEM_PLAYER_MARK_3 = 0x607,
    CHANGE_ITEM_PLAYER_MARK_4 = 0x608,
    CHANGE_ITEM_PLAYER_MARK_5 = 0x609,
    CHANGE_ITEM_PLAYER_MARK_6 = 0x60A,
    CHANGE_ITEM_PLAYER_MARK_7 = 0x60B,
    CHANGE_ITEM_PLAYER_MARK_8 = 0x60C,
    CHANGE_ITEM_PARTY_SEX_1 = 0x60F,
    CHANGE_ITEM_PARTY_SEX_2 = 0x610,
    CHANGE_ITEM_PARTY_SEX_3 = 0x611,
    CHANGE_ITEM_PARTY_SEX_4 = 0x612,
    CHANGE_ITEM_PARTY_SEX_5 = 0x613,
    CHANGE_ITEM_PARTY_SEX_6 = 0x614,
    CHANGE_ITEM_PARTY_NICKNAME_1 = 0x615,
    CHANGE_ITEM_PARTY_NICKNAME_2 = 0x616,
    CHANGE_ITEM_PARTY_NICKNAME_3 = 0x617,
    CHANGE_ITEM_PARTY_NICKNAME_4 = 0x618,
    CHANGE_ITEM_PARTY_NICKNAME_5 = 0x619,
    CHANGE_ITEM_PARTY_NICKNAME_6 = 0x61A,
    CHANGE_ITEM_TRAINER_NAME = 0x61B,
    CHANGE_ITEM_SELECTION_CONTROL = 0x61F,
    CHANGE_ITEM_SELECTION_PROMPT = 0x620,
    CHANGE_ITEM_WAITING_INDICATOR = 0x637,
    CHANGE_ITEM_WAITING_DETAIL = 0x638,
    CHANGE_ITEM_POKEMON_MODEL = 0x63D,
    CHANGE_ITEM_DETAIL_TRAINER_NAME = 0x640,
    CHANGE_ITEM_DETAIL_LEVEL = 0x642,
    CHANGE_ITEM_DETAIL_SPECIES = 0x643,
    CHANGE_ITEM_DETAIL_MOVE_1 = 0x645,
    CHANGE_ITEM_DETAIL_MOVE_2 = 0x646,
    CHANGE_ITEM_DETAIL_MOVE_3 = 0x647,
    CHANGE_ITEM_DETAIL_MOVE_4 = 0x648,
    CHANGE_ITEM_DETAIL_STATIC_LABEL = 0x649,
    CHANGE_ITEM_DETAIL_ITEM = 0x64A,
    CHANGE_ITEM_NOT_READY_LABEL = 0x64B,
    CHANGE_ITEM_NOT_READY_DETAIL = 0x64C,
    CHANGE_ITEM_READY_INDICATOR = 0x64D,
    CHANGE_ITEM_CONFIRM_MODEL = 0x652,
    CHANGE_ITEM_CONFIRM_TRAINER_NAME = 0x653,
    CHANGE_ITEM_DETAIL_NICKNAME = 0x7B3,
    CHANGE_ITEM_DETAIL_SEX = 0x7B4,
    CHANGE_ITEM_SELECTED_ROW_1 = 0x8B9,
    CHANGE_ITEM_SELECTED_ROW_2 = 0x8BA,
    CHANGE_ITEM_SELECTED_ROW_3 = 0x8BB,
    CHANGE_ITEM_SELECTED_ROW_4 = 0x8BC,
    CHANGE_ITEM_SELECTED_ROW_5 = 0x8BD,
    CHANGE_ITEM_SELECTED_ROW_6 = 0x8BE,
    CHANGE_ITEM_TWO_CHOICE_MARK_1 = 0x8BF,
    CHANGE_ITEM_TWO_CHOICE_MARK_2 = 0x8C0,
    CHANGE_ITEM_THREE_CHOICE_MARK_1 = 0x8C5,
    CHANGE_ITEM_THREE_CHOICE_MARK_2 = 0x8C6,
    CHANGE_ITEM_THREE_CHOICE_MARK_3 = 0x8C7,
    CHANGE_ITEM_CURSOR_ROW_1 = 0x8CE,
    CHANGE_ITEM_CURSOR_ROW_2 = 0x8CF,
    CHANGE_ITEM_CURSOR_ROW_3 = 0x8D0,
    CHANGE_ITEM_CURSOR_ROW_4 = 0x8D1,
    CHANGE_ITEM_CURSOR_ROW_5 = 0x8D2,
    CHANGE_ITEM_CURSOR_ROW_6 = 0x8D3,
    CHANGE_ITEM_CURSOR_CANCEL = 0x8D4,
};

enum PokemonChangeDisplayConstant {
    CHANGE_TEXT_WHITE_RGB = 0xFFFFFF00,
    CHANGE_COLOR_ALPHA_MASK = 0xFF,
    CHANGE_COLOR_NORMAL = 0xFFFFFFFF,
    CHANGE_COLOR_SHADOW = 0xB473C8FF,
    CHANGE_COLOR_RESTRICTED = 0xDC6464FF,
    CHANGE_COLOR_ITEM_RESTRICTED = 0xFF0000FF,
    CHANGE_MSG_MALE = 0x9C5,
    CHANGE_MSG_FEMALE = 0x9C6,
    CHANGE_MSG_SMALL_STRING = 0x4276,
    CHANGE_MSG_STRING = 0x4277,
    CHANGE_MSG_LEVEL = 0x426F,
    CHANGE_MSG_CORRUPT = 0x56C,
    CHANGE_MSG_EGG = 0x56B,
    CHANGE_MSG_ACTION_CANCEL = 0xC4AC,
    CHANGE_MSG_ACTION_EXCHANGE = 0xC495,
    CHANGE_MSG_NOT_READY_DETAIL = 0x4295,
    CHANGE_VALUE_STRING = 0x37,
    CHANGE_VALUE_LEVEL = 0x34,
    CHANGE_LARGE_MODEL_SPECIES = 0x18E,
    CHANGE_MENU_SELECT_POKEMON = 0xD6,
};

long _getQuantityBattleAbleWoIt(Hero* hero, long excludedSlot, enumPlayerIndex player);
bool _isPokemonExchangeAble(Pokemon* pokemon, enumPlayerIndex player);
bool _isPokemonValid(Pokemon* pokemon, enumPlayerIndex player);
bool _isPokemonBattleAble(Pokemon* pokemon, enumPlayerIndex player);
bool _pokemonChangeGetHinsi(Pokemon* pokemon);
bool _isPokemonItemExchangeOk(Pokemon* pokemon, enumPlayerIndex player);
PokemonExchangeState _getPokemonState(Pokemon* pokemon, enumPlayerIndex player);
short _selectDisplay_2p_offset();
Pokemon* pokemonChangeRequestPokemonFaceLoadCallBack(void* work);
Pokemon* _cbMenuPokemonStatus(Pokemon* pokemon, MENU_POKEMON_CB_TYPE type, long player);
void _setInfo(unsigned long message);
void _menuSetMsg(long menu, unsigned short item, unsigned long message);
void _pokemonChangeFadeIn(float duration);
void _pokemonChangeFadeOut(float duration);
void _menuWait(float duration);
bool _isTemochiPokemonCheck();
long _actionSelect2or3(unsigned long first, unsigned long second, unsigned long third);

inline unsigned long randomRange(unsigned long limit);
}

namespace {

PokemonExchangeState _getPokemonState(Pokemon* pokemon, enumPlayerIndex player)
{
    if (!pokemon)
        return POKEMON_INVALID;
    if (pokemonBiosGetFuseiFlag(pokemon))
        return POKEMON_CORRUPT;
    if (!pokemonCheckValid(pokemon))
        return POKEMON_INVALID;
    if (pokemonBiosGetTamagoFlag(pokemon))
        return POKEMON_EGG;
    if (pokemon->isDarkPokemon())
        return POKEMON_SHADOW;
    if (!menuCBRule_CheckPokemonEventFlag(pokemon))
        return POKEMON_EVENT_RESTRICTED;
    if (!menuCBRule_CheckPokemonKind(pokemon, 0))
        return POKEMON_DATA_RESTRICTED;
    if (_isPokemonItemExchangeOk(pokemon, player))
        return POKEMON_EXCHANGE_OK;
    return POKEMON_ITEM_RESTRICTED;
}

bool _isPokemonItemExchangeOk(Pokemon* pokemon, enumPlayerIndex player)
{
    unsigned short item = pokemon->getItemDataId();
    if (item == 0)
        return true;
    if (player == PLAYER_LOCAL) {
        if (!itemDataBiosCheckExportable(itemDataBiosGetPtr(item)))
            return false;
    } else {
        if (!itemDataBiosCheckImportable(itemDataBiosGetPtr(item)))
            return false;
    }
    return true;
}

bool _pokemonChangeGetHinsi(Pokemon* pokemon)
{
    return pokemon->getHp() == 0;
}

bool _isPokemonBattleAble(Pokemon* pokemon, enumPlayerIndex player)
{
    switch (_getPokemonState(pokemon, player)) {
    case POKEMON_INVALID:
    case POKEMON_CORRUPT:
    case POKEMON_EGG:
        return false;
    }
    return !_pokemonChangeGetHinsi(pokemon);
}

bool _isPokemonValid(Pokemon* pokemon, enumPlayerIndex player)
{
    return _getPokemonState(pokemon, player) != POKEMON_INVALID;
}

bool _isPokemonExchangeAble(Pokemon* pokemon, enumPlayerIndex player)
{
    return _getPokemonState(pokemon, player) == POKEMON_EXCHANGE_OK;
}

long _getQuantityBattleAbleWoIt(Hero* hero, long excludedSlot, enumPlayerIndex player)
{
    long count = 0;
    for (long slot = 0; slot < CHANGE_PARTY_CAPACITY; ++slot) {
        if (slot != excludedSlot && _isPokemonBattleAble(hero->getPokemon(slot), player))
            ++count;
    }
    return count;
}

void _menuWait(float duration)
{
    float time = 0.0f;
    while (time < duration) {
        GSthreadSwitch();
        time += timeGetLastFrameSec();
    }
}

void _pokemonChangeFadeOut(float duration)
{
    fadeSetToBlackOutIfNotBlack(duration);
    fadeCheck(1);
}

void _pokemonChangeFadeIn(float duration)
{
    fadeSetToBlackInIfBlack(duration);
}

void _menuSetMsg(long menu, unsigned short item, unsigned long message)
{
    tagWINDOW_WORK* window = windowSearchID(menu);
    tagSPRITE_WORK* sprite = windowSearchItemID(window, item);
    if (message) {
        if (window && sprite) {
            winSpriteSetDisp(sprite, 1);
            sprite->mMsgID = message;
        }
    } else {
        if (window && sprite) {
            sprite->mMsgID = 0;
            winSpriteSetDisp(sprite, 0);
        }
    }
}

void _setInfo(unsigned long message)
{
    _menuSetMsg(CHANGE_MENU_INFO, CHANGE_ITEM_INFO_MESSAGE, message);
}

long _actionSelect2or3(unsigned long first, unsigned long second, unsigned long third)
{
    unsigned long messages[CHANGE_ACTION_CAPACITY] = {first, second, third};
    menuOpenCustom(CHANGE_MENU_ACTION_BACKGROUND, windowGetActiveID(), 0, 0, 0, 0);
    long menu;
    if (third == 0)
        menu = CHANGE_MENU_TWO_ACTIONS;
    else
        menu = CHANGE_MENU_THREE_ACTIONS;
    menuOpenCustom(menu, windowGetActiveID(), 0, 0, 0, 0);
    if (third == 0) {
        _menuSetMsg(menu, CHANGE_ITEM_TWO_ACTION_FIRST, first);
        _menuSetMsg(menu, CHANGE_ITEM_TWO_ACTION_SECOND, second);
    } else {
        _menuSetMsg(menu, CHANGE_ITEM_THREE_ACTION_FIRST, first);
        _menuSetMsg(menu, CHANGE_ITEM_THREE_ACTION_SECOND, second);
        _menuSetMsg(menu, CHANGE_ITEM_THREE_ACTION_THIRD, third);
    }
    windowCheckCursor(menu, 1);
    long selection = windowGetValue(menu);
    menuClose(menu);
    menuClose(CHANGE_MENU_ACTION_BACKGROUND);
    if (selection == -1) {
        if (third == 0)
            selection = 1;
        else
            selection = 2;
    }
    return messages[selection];
}

void CMenuPokemonChange::init()
{
    memset(this, 0, sizeof(*this));
    mHero = Hero::getHeroPtr();
    mState.init();
    fadeSet(3, 0.0f);
    GSfsysReadWait(CHANGE_RESOURCE_GROUP);
    menuStateChange(MENU_CONNECT);
    bgModelLoad();
    _pokemonChangeFadeIn(0.3f);
}

void CMenuPokemonChange::bgModelLoad()
{
    unsigned long group = floorGetCurrentGroupID();
    mBgModelID = scriptModelOpen(CHANGE_BACKGROUND_MODEL_RESOURCE);
    mBgModel = static_cast<_GSmodel*>(GSresGetResource(group, mBgModelID));
    cameraPlayAnime(CHANGE_BACKGROUND_RESOURCE_GROUP, CHANGE_BACKGROUND_CAMERA_RESOURCE, 0.0f, 1);
    cameraSetType(4);
    GSmodelSetVisibility(mBgModel, 0);
}

void CMenuPokemonChange::bgModelRelease()
{
    scriptModelClose(mBgModelID);
}

void CMenuPokemonChange::main()
{
    init();
    mSaveData = memcardMemAllocForSavedata();
    mState.mNextState = STATE_START;
    memset(mState.mWork, 0, sizeof(mState.mWork));
    loop();
    menuGBACSetRomVer(0, CHANGE_ROM_VERSION_UNKNOWN);
    _pokemonChangeFadeOut(0.3f);
    menuStateChange(MENU_CLOSED);
    while (menuFaceCheckLoading())
        GSthreadSwitch();
    bgModelRelease();
    GSfsysFree(CHANGE_RESOURCE_GROUP);
    GSresFreeGroup(CHANGE_RESOURCE_GROUP);
    if (GSfloorIsFloorPushed())
        GSfloorPop();
    else
        floorLink(floorGetPrevFloorID(), 0);
}

void CMenuPokemonChange::loop()
{
    u8* stateWork = mState.mWork;
    for (;;) {
        mState.update();
        switch (mState.mState) {
        case STATE_START:
            mState.mNextState = STATE_CONNECT;
            memset(stateWork, 0, sizeof(mState.mWork));
            break;
        case STATE_SAVE_ERROR:
            menuStateChange(MENU_CONNECT);
            bgDrawFadeIn();
            winMsgOpen(2, CHANGE_MSG_EXIT, 1, 0);
            winMsgClose(1);
            mState.mNextState = STATE_EXIT;
            memset(stateWork, 0, sizeof(mState.mWork));
            break;
        case STATE_CONNECTION_ERROR:
            menuStateChange(MENU_CONNECT);
            bgDrawFadeIn();
            winMsgOpen(2, CHANGE_MSG_EXCHANGE_FAILED, 1, 0);
            winMsgClose(1);
            mState.mNextState = STATE_EXIT;
            memset(stateWork, 0, sizeof(mState.mWork));
            break;
        case STATE_EXIT:
            mSaveData = memcardMemFreeForSavedata(mSaveData);
            return;
        case STATE_CONNECT:
            stateFunctionCheckconnection(); break;
        case STATE_SELECT_START:
            stateFunctionSelectPokemonStart(); break;
        case STATE_SELECT_FIRST:
            stateFunctionSelectPokemon1Main(); break;
        case STATE_FIRST_SELECTED:
            stateFunctionSelectPokemon1Selected(); break;
        case STATE_FIRST_CANCEL:
            stateFunctionSelectPokemon1Chanceld(); break;
        case STATE_FIRST_STATUS:
            stateFunctionSelectPokemon1CheckStatus(); break;
        case STATE_FIRST_EXCHANGE:
            stateFunctionSelectPokemon1PokeExChange(); break;
        case STATE_SELECT_SECOND:
            stateFunctionSelectPokemon2Main(); break;
        case STATE_SECOND_SELECTED:
            stateFunctionSelectPokemon2Selected(); break;
        case STATE_SECOND_STATUS:
            stateFunctionSelectPokemon2CheckStatus(); break;
        case STATE_SECOND_EXCHANGE:
            stateFunctionSelectPokemon2PokeExChange(); break;
        case STATE_SECOND_DECIDE_WAIT:
            stateFunctionSelectPokemon2DecideAfterWait(); break;
        case STATE_CONFIRM_EXCHANGE:
            stateFunctionConfirmExChange(); break;
        case STATE_EXCHANGE_START:
            stateFunctionExChangeStart(); break;
        case STATE_EXCHANGE_MAIN:
            stateFunctionExChangeMain(); break;
        case STATE_EXCHANGE_OK:
            stateFunctionExChangeOK(); break;
        case STATE_EVOLUTION:
            stateFunctionEvolution(); break;
        case STATE_SAVE_REPORT:
            stateFunctionSaveReport(); break;
        case STATE_CHECK_NEXT:
            stateFunctionCheckNext(); break;
        }
    }
}

Pokemon* _cbMenuPokemonStatus(Pokemon* pokemon, MENU_POKEMON_CB_TYPE type, long player)
{
    return MenuPokemonChange.pokemonStatusCallback(pokemon, type,
                                                   static_cast<enumPlayerIndex>(player));
}

void CMenuPokemonChange::menuPokemonStatusExecute(enumPlayerIndex player)
{
    mStatusIndex = mCursor[player];
    menuPokemonStatusOpenFromPokemonChange(getPokemon(mCursor[player]), _cbMenuPokemonStatus, player);
    mCursor[player] = mStatusIndex;
}

long CMenuPokemonChange::pokemonStatusSearch(long, long step)
{
    long count = getQuantityAllPokemon();
    long slot = mStatusIndex;
    for (;;) {
        slot += step;
        if (slot < 0 || slot >= count)
            return mStatusIndex;
        if (isPokemonLookAbleStatus(slot))
            return slot;
    }
}

Pokemon* CMenuPokemonChange::pokemonStatusCallback(Pokemon*, MENU_POKEMON_CB_TYPE type,
                                                   enumPlayerIndex)
{
    getQuantityAllPokemon();
    long current = mStatusIndex;
    long index = current;
    if (type == MENU_POKEMON_CB_CURSOR_LEFT)
        index = pokemonStatusSearch(current, -1);
    else if (type == MENU_POKEMON_CB_CURSOR_RIGHT)
        index = pokemonStatusSearch(current, 1);
    mStatusIndex = index;
    enumPlayerIndex player = PLAYER_LOCAL;
    long slot = mStatusIndex;
    if (slot >= getQuantityHeroPokemon(PLAYER_LOCAL)) {
        slot -= getQuantityHeroPokemon(PLAYER_LOCAL);
        player = PLAYER_GBA;
    }
    Hero* hero;
    if (player == PLAYER_LOCAL)
        hero = mHero;
    else
        hero = &mGbaHero;
    return hero->getPokemon(slot);
}

Pokemon* pokemonChangeRequestPokemonFaceLoadCallBack(void* work)
{
    PokemonFaceList* list = static_cast<PokemonFaceList*>(work);
    if (!list)
        return 0;
    if (list->mIndex >= CHANGE_FACE_CAPACITY)
        return 0;
    return list->mPokemon[list->mIndex++];
}

void CMenuPokemonChange::requestPokemonFaceLoad()
{
    long count = 0;
    for (enumPlayerIndex player = PLAYER_LOCAL; player < PLAYER_COUNT; player = static_cast<enumPlayerIndex>(player + 1)) {
        for (long slot = 0; slot < CHANGE_PARTY_CAPACITY; ++slot) {
            Pokemon* pokemon = getHero(player)->getPokemon(slot);
            if (_isPokemonValid(pokemon, player))
                getPokemonFaceList().mPokemon[count++] = pokemon;
        }
    }
    getPokemonFaceList().mPokemon[count] = 0;
    getPokemonFaceList().mIndex = 0;
    menuFaceLoad(0, pokemonChangeRequestPokemonFaceLoadCallBack, &getPokemonFaceList());
}

short _selectDisplay_2p_offset()
{
    MENU_ITEM_dd_* first = menuPtItemGetByMenuIdAndItemId(CHANGE_MENU_PARTY, CHANGE_ITEM_PLAYER_PANEL, 0);
    MENU_ITEM_dd_* second = menuPtItemGetByMenuIdAndItemId(CHANGE_MENU_PARTY, CHANGE_ITEM_OTHER_PLAYER_PANEL, 0);
    return second->mMessageX - first->mMessageX;
}

void CMenuPokemonChange::setMenuByTbl(long* table)
{
    unsigned long count = 0;
    for (long* next = table; *next; ++next, ++count) {}
    unsigned long common = 0;
    for (; common < CHANGE_MENU_CAPACITY; ++common) {
        if (!mMenus[common] || mMenus[common] != table[common])
            break;
    }
    unsigned long firstChanged = common;
    for (; mMenus[common] && common < CHANGE_MENU_CAPACITY; ++common) {
        long menu = mMenus[common];
        long* next = table;
        bool keep = false;
        for (; *next; ++next) {
            if (menu == *next) {
                keep = true;
                break;
            }
        }
        if (!keep)
            menuCloseCustom(menu, 2, 0);
    }
    bool opened = false;
    for (common = firstChanged; table[common] && common < CHANGE_MENU_CAPACITY; ++common) {
        oneMenuLoad(table[common]);
        opened = true;
    }
    if (!opened) {
        switch (table[count - 1]) {
        case CHANGE_MENU_SELECT_POKEMON:
            oneMenuLoad(table[count - 1]);
            break;
        }
    }
    memset(mMenus, 0, sizeof(mMenus));
    memcpy(mMenus, table, (count + 1) * sizeof(long));
}

void CMenuPokemonChange::bgDrawFadeOut()
{
    menuOpenCustom(CHANGE_MENU_FADE, windowGetActiveID(), 0, 2, 0, 0);
    menuSetDisp(CHANGE_MENU_FADE, 1);
    tagWINDOW_WORK* window = windowSearchID(CHANGE_MENU_FADE);
    float time = 0.0f;
    while (time < 0.3f) {
        window->mRgba.mColorA = 255.0f * (time / 0.3f);
        GSthreadSwitch();
        time += timeGetLastFrameSec();
    }
    window->mRgba.mColorA = 255;
}

u8 CMenuPokemonChange::bgDrawFadeIsLive()
{
    tagWINDOW_WORK* window = windowSearchID(CHANGE_MENU_FADE);
    u8 live;
    if (window)
        live = 1;
    else
        live = 0;
    return live;
}

void CMenuPokemonChange::bgDrawFadeIn()
{
    bgDrawFadeIn(0.3f);
}

void CMenuPokemonChange::bgDrawFadeIn(float duration)
{
    tagWINDOW_WORK* window = windowSearchID(CHANGE_MENU_FADE);
    if (window) {
        float time = 0.0f;
        while (time < duration) {
            window->mRgba.mColorA = 255.0f * (1.0f - time / duration);
            GSthreadSwitch();
            time += timeGetLastFrameSec();
        }
        window->mRgba.mColorA = 0;
        menuCloseCustom(CHANGE_MENU_FADE, 0, 1);
    }
}

void CMenuPokemonChange::menuStateChange(enumMenuState state)
{
    mMenuState = state;
    switch (state) {
    case MENU_CLOSED: {
        static long tbl[] = {0};
        setMenuByTbl(tbl);
        break;
    }
    case MENU_BACKGROUND: {
        static long tbl[] = {CHANGE_MENU_BACKGROUND, 0};
        setMenuByTbl(tbl);
        break;
    }
    case MENU_CONNECT: {
        static long tbl[] = {CHANGE_MENU_BACKGROUND, CHANGE_MENU_OVERLAY, 0};
        setMenuByTbl(tbl);
        break;
    }
    case MENU_SELECT_FIRST:
    case MENU_SELECT_SECOND: {
        static long tbl[] = {CHANGE_MENU_BACKGROUND, CHANGE_MENU_OVERLAY, CHANGE_MENU_PARTY, CHANGE_MENU_INFO, CHANGE_MENU_SELECT_POKEMON, 0};
        setMenuByTbl(tbl);
        break;
    }
    case MENU_CONFIRM: {
        static long tbl[] = {CHANGE_MENU_BACKGROUND, CHANGE_MENU_OVERLAY, CHANGE_MENU_CONFIRM, 0};
        setMenuByTbl(tbl);
        break;
    }
    case MENU_EXCHANGE: {
        static long tbl[] = {CHANGE_MENU_BACKGROUND, CHANGE_MENU_OVERLAY, EXCHANGE_MODEL_MENU, 0};
        setMenuByTbl(tbl);
        break;
    }
    }
    mReopenSelection = false;
}

void CMenuPokemonChange::oneMenuLoad(long menu)
{
    mOpeningMenu = menu;
    if (menu == CHANGE_MENU_SELECT_POKEMON) {
        selectPokemonMenuOpen();
    } else {
        menuOpenCustom(menu, windowGetActiveID(), 0, 4, 0, 0);
        menuSetDisp(menu, menu == CHANGE_MENU_BACKGROUND);
    }
}

void CMenuPokemonChange::waitMenuOpen()
{
    while (winSeqCheckMove(mOpeningMenu))
        GSthreadSwitch();
}

long CMenuPokemonChange::selectPokemonMenuOpen()
{
    enumPlayerIndex player = mPlayerIndex;
    Hero* hero;
    short offset;
    long flags;
    tagMENU_ITEM_DATA* item;
    long owner;
    windowSearchID(CHANGE_MENU_SELECT_POKEMON);
    MenuData* data = reinterpret_cast<MenuData*>(menuDataBiosGetPtr(CHANGE_MENU_SELECT_POKEMON));
    item = menuItemBiosGetPtr(CHANGE_MENU_SELECT_POKEMON, data->mFirstItemID);
    for (owner = 0; owner < PLAYER_COUNT; ++owner) {
        if (owner == PLAYER_LOCAL)
            hero = mHero;
        else
            hero = &mGbaHero;
        for (long slot = 0; slot < CHANGE_PARTY_CAPACITY; ++slot) {
            item->mShownFlag = _isPokemonValid(hero->getPokemon(slot), player);
            item = menuItemBiosGetPtr(CHANGE_MENU_SELECT_POKEMON, item->mNextID);
        }
    }
    static u8 adjusted = 0;
    if (!adjusted) {
        adjusted = 1;
        offset = _selectDisplay_2p_offset();
        item = menuItemBiosGetPtr(CHANGE_MENU_SELECT_POKEMON, CHANGE_ITEM_GBA_PARTY_FIRST);
        for (long slot = 0; slot < CHANGE_PARTY_CAPACITY; ++slot) {
            item->mMessageX += offset;
            item = menuItemBiosGetPtr(CHANGE_MENU_SELECT_POKEMON, item->mNextID);
        }
    }
    if (mReopenSelection)
        flags = 4;
    else
        flags = 0;
    menuOpenCustom(CHANGE_MENU_SELECT_POKEMON, windowGetActiveID(), &mCursor[player], flags, 0, 0);
    _setInfo(CHANGE_MSG_SELECT_POKEMON);
    return CHANGE_MENU_SELECT_POKEMON;
}

int CMenuPokemonChange::getQuantityHeroPokemon(enumPlayerIndex player)
{
    Hero* hero;
    if (player == PLAYER_LOCAL)
        hero = mHero;
    else
        hero = &mGbaHero;
    long count;
    for (count = 0; count < CHANGE_PARTY_CAPACITY; ++count) {
        if (!_isPokemonValid(hero->getPokemon(count), player))
            break;
    }
    return count;
}

long CMenuPokemonChange::getQuantityAllPokemon()
{
    return getQuantityHeroPokemon(PLAYER_LOCAL) + getQuantityHeroPokemon(PLAYER_GBA);
}

void CMenuPokemonChange::ctrlHook(tagWINDOW_WORK* window)
{
    switch (window->mState) {
    case 0:
        if (!window->mFlag) {
            switch (window->mMenuID) {
            case CHANGE_MENU_PARTY:
            case CHANGE_MENU_SELECT_POKEMON:
                winSeqSetMenu(window->mMenuID, CHANGE_SEQUENCE_SELECT);
                break;
            }
            window->mFlag = 1;
        }
        break;
    case 3:
        if (!window->mFlag)
            window->mFlag = 1;
        break;
    }
}

void CMenuPokemonChange::selectDrawHook(tagWINDOW_WORK*)
{
    if (mMenuState == MENU_CLOSED)
        return;
    tagWINDOW_WORK* window = windowSearchID(CHANGE_MENU_OVERLAY);
    if (window)
        menuPtWindowWorkDraw(window);
    window = windowSearchID(CHANGE_MENU_PARTY);
    if (window) {
        tagSPRITE_WORK* sprite = menuPtSpriteWorkGetByItemID(window->mMenuID, CHANGE_ITEM_PLAYER_PANEL, 0);
        short x = window->mScrollX;
        window->mScrollX = x;
        menuPtSpriteWorkSetSpriteId(sprite, CHANGE_SPRITE_LOCAL_PANEL);
        windowSetParam(window, 0, 0);
        menuPtWindowWorkDraw(window);
        window->mScrollX = _selectDisplay_2p_offset() - x;
        menuPtSpriteWorkSetSpriteId(sprite, CHANGE_SPRITE_GBA_PANEL);
        windowSetParam(window, 0, 1);
        menuPtWindowWorkDraw(window);
        window->mScrollX = x;
        menuPtWindowWorkDraw(CHANGE_MENU_INFO);
    }
    window = windowSearchID(CHANGE_MENU_CONFIRM);
    if (window) {
        tagSPRITE_WORK* sprite = menuPtSpriteWorkGetByItemID(window->mMenuID, CHANGE_ITEM_CONFIRM_PANEL, 0);
        window->mScrollX = 0;
        menuPtSpriteWorkSetSpriteId(sprite, CHANGE_SPRITE_LOCAL_PANEL);
        windowSetParam(window, 0, 0);
        menuPtWindowWorkDraw(window);
        window->mScrollX = _selectDisplay_2p_offset();
        menuPtSpriteWorkSetSpriteId(sprite, CHANGE_SPRITE_GBA_PANEL);
        windowSetParam(window, 0, 1);
        menuPtWindowWorkDraw(window);
    }
    window = windowSearchID(EXCHANGE_MODEL_MENU);
    if (window) {
        tagSPRITE_WORK* sprite = menuPtSpriteWorkGetByItemID(window->mMenuID, CHANGE_ITEM_EXCHANGE_PANEL, 0);
        window->mScrollX = 0;
        menuPtSpriteWorkSetSpriteId(sprite, CHANGE_SPRITE_LOCAL_PANEL);
        windowSetParam(window, 0, 0);
        menuPtWindowWorkDraw(window);
        window->mScrollX = _selectDisplay_2p_offset();
        menuPtSpriteWorkSetSpriteId(sprite, CHANGE_SPRITE_GBA_PANEL);
        windowSetParam(window, 0, 1);
        menuPtWindowWorkDraw(window);
    }
    mDrawTime += timeGetLastFrameTime();
}

void CMenuPokemonChange::selectItemHook(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    selectItemHookPrime(window, sprite, sprite->mItemID);
}

void CMenuPokemonChange::selectItemHookPrime(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, unsigned long itemID)
{
    long cursorSlot;
    long playerValue = windowGetParam(window, 0);
    enumPlayerIndex player = (enumPlayerIndex)playerValue;
    Hero* hero;
    if (player == PLAYER_LOCAL)
        hero = mHero;
    else
        hero = &mGbaHero;
    enumPlayerIndex selectedPlayer = PLAYER_LOCAL;
    long selectedSlot = mCursor[player];
    if (selectedSlot >= getQuantityHeroPokemon(PLAYER_LOCAL)) {
        selectedSlot -= getQuantityHeroPokemon(PLAYER_LOCAL);
        selectedPlayer = PLAYER_GBA;
    }
    Hero* selectedHero;
    if (selectedPlayer == PLAYER_LOCAL)
        selectedHero = mHero;
    else
        selectedHero = &mGbaHero;
    Pokemon* selectedPokemon = selectedHero->getPokemon(selectedSlot);
    u8 alpha = window->mRgba.mColorA;
    sprite->mRgba.mAlpha = alpha;
    unsigned long textColor = CHANGE_TEXT_WHITE_RGB | (alpha & CHANGE_COLOR_ALPHA_MASK);
    u8 cursorAlpha;
    if (mState.mState == STATE_SECOND_SELECTED || mState.mState == STATE_FIRST_SELECTED) {
        cursorAlpha = 255;
    } else {
        unsigned long pulseAlpha = (unsigned long)(50.0f + 205.0f * calcAnimationTimeRate0to1to0(1.0f));
        cursorAlpha = (sprite->mRgba.mAlpha * pulseAlpha) / 255;
    }
    bool drawCursor;
    switch (itemID) {
    case CHANGE_ITEM_PARTY_FACE_1: case CHANGE_ITEM_PARTY_FACE_2: case CHANGE_ITEM_PARTY_FACE_3: case CHANGE_ITEM_PARTY_FACE_4: case CHANGE_ITEM_PARTY_FACE_5: case CHANGE_ITEM_PARTY_FACE_6: {
        long slot = menuPtSpriteWorkGetIndexFromId(window, sprite, CHANGE_ITEM_PARTY_FACE_1);
        Pokemon* pokemon = hero->getPokemon(slot);
        if (_isPokemonValid(pokemon, player)) {
            menuFaceDisp(window, sprite, pokemon);
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    }
    case CHANGE_ITEM_PARTY_STATE_1: case CHANGE_ITEM_PARTY_STATE_2: case CHANGE_ITEM_PARTY_STATE_3: case CHANGE_ITEM_PARTY_STATE_4: case CHANGE_ITEM_PARTY_STATE_5: case CHANGE_ITEM_PARTY_STATE_6: {
        long slot = menuPtSpriteWorkGetIndexFromId(window, sprite, CHANGE_ITEM_PARTY_STATE_1);
        Pokemon* pokemon = hero->getPokemon(slot);
        switch (_getPokemonState(pokemon, player)) {
        case POKEMON_INVALID: case POKEMON_ITEM_RESTRICTED: case POKEMON_EXCHANGE_OK:
            sprite->mColor = CHANGE_COLOR_NORMAL;
            break;
        case POKEMON_SHADOW:
            sprite->mColor = CHANGE_COLOR_SHADOW;
            break;
        default:
            sprite->mColor = CHANGE_COLOR_RESTRICTED;
            break;
        }
        break;
    }
    case CHANGE_ITEM_PARTY_ITEM_1: case CHANGE_ITEM_PARTY_ITEM_2: case CHANGE_ITEM_PARTY_ITEM_3: case CHANGE_ITEM_PARTY_ITEM_4: case CHANGE_ITEM_PARTY_ITEM_5: case CHANGE_ITEM_PARTY_ITEM_6: {
        long slot = menuPtSpriteWorkGetIndexFromId(window, sprite, CHANGE_ITEM_PARTY_ITEM_1);
        Pokemon* pokemon = hero->getPokemon(slot);
        u16 heldItem = 0;
        if (_isPokemonValid(pokemon, player))
            heldItem = pokemonBiosGetItemDataId(pokemon);
        sprite->mColor = CHANGE_COLOR_NORMAL;
        if (heldItem && !_isPokemonItemExchangeOk(pokemon, player))
            sprite->mColor = CHANGE_COLOR_ITEM_RESTRICTED;
        winSpriteSetDisp(sprite, heldItem != 0);
        break;
    }
    case CHANGE_ITEM_PARTY_SEX_1: case CHANGE_ITEM_PARTY_SEX_2: case CHANGE_ITEM_PARTY_SEX_3: case CHANGE_ITEM_PARTY_SEX_4: case CHANGE_ITEM_PARTY_SEX_5: case CHANGE_ITEM_PARTY_SEX_6: {
        long slot = menuPtSpriteWorkGetIndexFromId(window, sprite, CHANGE_ITEM_PARTY_SEX_1);
        Pokemon* pokemon = hero->getPokemon(slot);
        unsigned long message = 0;
        switch (_getPokemonState(pokemon, player)) {
        case POKEMON_INVALID: case POKEMON_CORRUPT: case POKEMON_EGG:
            break;
        default:
            switch (menuPokemonChangeCursor(menuSubGetPokemonSexForFightDisp(pokemon), pokemon)) {
            case 0: message = CHANGE_MSG_MALE; break;
            case 1: message = CHANGE_MSG_FEMALE; break;
            }
            break;
        }
        if (message) {
            msgctrlSetValue(CHANGE_VALUE_STRING, (unsigned long)GSmsgGetStringPtr(message));
            GSmsgPrint2(window->mLayerID, 0, 0, textColor, CHANGE_MSG_SMALL_STRING);
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    }
    case CHANGE_ITEM_PARTY_NICKNAME_1: case CHANGE_ITEM_PARTY_NICKNAME_2: case CHANGE_ITEM_PARTY_NICKNAME_3: case CHANGE_ITEM_PARTY_NICKNAME_4: case CHANGE_ITEM_PARTY_NICKNAME_5: case CHANGE_ITEM_PARTY_NICKNAME_6: {
        long slot = menuPtSpriteWorkGetIndexFromId(window, sprite, CHANGE_ITEM_PARTY_NICKNAME_1);
        Pokemon* pokemon = hero->getPokemon(slot);
        unsigned long message = CHANGE_MSG_STRING;
        switch (_getPokemonState(pokemon, player)) {
        case POKEMON_CORRUPT:
            msgctrlSetValue(CHANGE_VALUE_STRING, (unsigned long)GSmsgGetStringPtr(CHANGE_MSG_CORRUPT));
            break;
        case POKEMON_EGG:
            msgctrlSetValue(CHANGE_VALUE_STRING, (unsigned long)GSmsgGetStringPtr(CHANGE_MSG_EGG));
            break;
        case POKEMON_INVALID:
            message = 0;
            break;
        default:
            {
                u16* nickname = pokemon_GetNickname(pokemon);
                msgctrlSetValue(CHANGE_VALUE_STRING, (unsigned long)nickname);
            }
            break;
        }
        if (message) {
            short width = GSmsgGetMessageSize(message) >> 16;
            short x = sprite->mWidth / 2 - width / 2;
            GSmsgPrint2(window->mLayerID, x - 1, 0, textColor, CHANGE_MSG_STRING);
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    }
    case CHANGE_ITEM_TRAINER_NAME: {
        u16* name = heroBiosGetNamePtr(hero);
        msgctrlSetValue(CHANGE_VALUE_STRING, (unsigned long)name);
        GSmsgPrint2(window->mLayerID, 0, 0, textColor, CHANGE_MSG_STRING);
        break;
    }
    case CHANGE_ITEM_PLAYER_MARK_1: case CHANGE_ITEM_PLAYER_MARK_2: case CHANGE_ITEM_PLAYER_MARK_3: case CHANGE_ITEM_PLAYER_MARK_4: case CHANGE_ITEM_PLAYER_MARK_5: case CHANGE_ITEM_PLAYER_MARK_6: case CHANGE_ITEM_PLAYER_MARK_7: case CHANGE_ITEM_PLAYER_MARK_8:
        winSpriteSetDisp(sprite, mPlayerIndex == player);
        break;
    case CHANGE_ITEM_SELECTED_ROW_1: case CHANGE_ITEM_SELECTED_ROW_2: case CHANGE_ITEM_SELECTED_ROW_3: case CHANGE_ITEM_SELECTED_ROW_4: case CHANGE_ITEM_SELECTED_ROW_5: case CHANGE_ITEM_SELECTED_ROW_6: {
        long slot = mCursor[playerValue];
        if (slot >= getQuantityHeroPokemon(PLAYER_LOCAL))
            slot -= getQuantityHeroPokemon(PLAYER_LOCAL);
        long index = menuPtSpriteWorkGetIndexFromId(window, sprite, CHANGE_ITEM_SELECTED_ROW_1);
        bool visible = false;
        if (slot == index) {
            long state = mState.mState;
            if (player == PLAYER_LOCAL)
                visible = state >= STATE_SELECT_SECOND && state <= STATE_SECOND_DECIDE_WAIT;
            else if (player == PLAYER_GBA)
                visible = state == STATE_SECOND_DECIDE_WAIT;
        }
        winSpriteSetDisp(sprite, visible);
        break;
    }
    case CHANGE_ITEM_CURSOR_ROW_1: case CHANGE_ITEM_CURSOR_ROW_2: case CHANGE_ITEM_CURSOR_ROW_3: case CHANGE_ITEM_CURSOR_ROW_4: case CHANGE_ITEM_CURSOR_ROW_5: case CHANGE_ITEM_CURSOR_ROW_6: {
        long cursor = windowGetValue(CHANGE_MENU_SELECT_POKEMON);
        cursorSlot = cursor;
        if (cursor >= getQuantityHeroPokemon(PLAYER_LOCAL))
            cursorSlot -= getQuantityHeroPokemon(PLAYER_LOCAL);
        enumPlayerIndex cursorPlayer;
        if (cursor >= getQuantityHeroPokemon(PLAYER_LOCAL))
            cursorPlayer = PLAYER_GBA;
        else
            cursorPlayer = PLAYER_LOCAL;
        long index = menuPtSpriteWorkGetIndexFromId(window, sprite, CHANGE_ITEM_CURSOR_ROW_1);
        drawCursor = index == cursorSlot && cursorPlayer == player && mState.mState != STATE_SECOND_DECIDE_WAIT && index < getQuantityHeroPokemon(cursorPlayer);
        winSpriteSetDisp(sprite, drawCursor);
        sprite->mRgba.mAlpha = cursorAlpha;
        break;
    }
    case CHANGE_ITEM_SELECTION_PROMPT:
        winSpriteSetDisp(sprite, mSelectionActive);
        if (mSelectionActive)
            menuCBMsgDrawOnSprite(window, sprite, CHANGE_MSG_ACTION_CANCEL);
        break;
    case CHANGE_ITEM_SELECTION_CONTROL:
        winSpriteSetDisp(sprite, mSelectionActive);
        break;
    case CHANGE_ITEM_CURSOR_CANCEL: {
        long cursor = windowGetValue(CHANGE_MENU_SELECT_POKEMON);
        winSpriteSetDisp(sprite, cursor == getQuantityHeroPokemon(PLAYER_LOCAL) + getQuantityHeroPokemon(PLAYER_GBA));
        sprite->mRgba.mAlpha = cursorAlpha;
        break;
    }
    case CHANGE_ITEM_TWO_CHOICE_MARK_1:
        winSpriteSetDisp(sprite, windowGetValue(window->mMenuID) == 0);
        break;
    case CHANGE_ITEM_TWO_CHOICE_MARK_2:
        winSpriteSetDisp(sprite, windowGetValue(window->mMenuID) == 1);
        break;
    case CHANGE_ITEM_THREE_CHOICE_MARK_1:
        winSpriteSetDisp(sprite, windowGetValue(window->mMenuID) == 0);
        break;
    case CHANGE_ITEM_THREE_CHOICE_MARK_2:
        winSpriteSetDisp(sprite, windowGetValue(window->mMenuID) == 1);
        break;
    case CHANGE_ITEM_THREE_CHOICE_MARK_3:
        winSpriteSetDisp(sprite, windowGetValue(window->mMenuID) == 2);
        break;
    case CHANGE_ITEM_POKEMON_MODEL:
        if (window->mRgba.mColorA == 255) {
            MENU_MODEL* model = &mPokemonModels[player];
            menuModelAdjustCamera(model);
            menuModelSetSize(model, window->mScrollX + sprite->mX, sprite->mY, sprite->mWidth, sprite->mHeight);
            float zoom;
            if (pokemonBiosGetPokemonDataId(selectedPokemon) == CHANGE_LARGE_MODEL_SPECIES)
                zoom = 0.4f;
            else
                zoom = 0.65f;
            menuModelSetZoom(model, zoom);
            menuModelRender(model);
        }
        break;
    case CHANGE_ITEM_DETAIL_TRAINER_NAME:
        selectItemHookPrime(window, sprite, CHANGE_ITEM_TRAINER_NAME);
        break;
    case CHANGE_ITEM_DETAIL_LEVEL:
        msgctrlSetValue(CHANGE_VALUE_LEVEL, selectedPokemon->getLevel());
        GSmsgPrint2(window->mLayerID, 0, 0, textColor, CHANGE_MSG_LEVEL);
        break;
    case CHANGE_ITEM_DETAIL_NICKNAME:
        msgctrlSetValue(CHANGE_VALUE_STRING, (unsigned long)selectedPokemon->getNicknamePtr());
        switch (pokecoloGetCountryCode()) {
        case 0:
            GSmsgPrint2(window->mLayerID, 0, 0, textColor, CHANGE_MSG_SMALL_STRING);
            break;
        case 1: case 2:
            GSmsgPrint2(window->mLayerID, 0, 0, textColor, CHANGE_MSG_STRING);
            break;
        }
        break;
    case CHANGE_ITEM_DETAIL_SEX: {
        unsigned long message;
        switch (menuPokemonChangeCursor(menuSubGetPokemonSexForFightDisp(selectedPokemon), selectedPokemon)) {
        case 0: message = CHANGE_MSG_MALE; break;
        case 1: message = CHANGE_MSG_FEMALE; break;
        case 2:
        default: message = 0; break;
        }
        if (message) {
            msgctrlSetValue(CHANGE_VALUE_STRING, (unsigned long)GSmsgGetStringPtr(message));
            GSmsgPrint2(window->mLayerID, 0, 0, textColor, CHANGE_MSG_SMALL_STRING);
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    }
    case CHANGE_ITEM_DETAIL_SPECIES:
        msgctrlSetValue(CHANGE_VALUE_STRING, (unsigned long)GSmsgGetStringPtr(pokemonDataBiosGetName(selectedPokemon->getPokemonDataPtr())));
        switch (pokecoloGetCountryCode()) {
        case 0:
            GSmsgPrint2(window->mLayerID, 0, 0, textColor, CHANGE_MSG_SMALL_STRING);
            break;
        case 1: case 2:
            GSmsgPrint2(window->mLayerID, 0, 0, textColor, CHANGE_MSG_STRING);
            break;
        }
        break;
    case CHANGE_ITEM_DETAIL_MOVE_1: case CHANGE_ITEM_DETAIL_MOVE_2: case CHANGE_ITEM_DETAIL_MOVE_3: case CHANGE_ITEM_DETAIL_MOVE_4: {
        long slot = menuPtSpriteWorkGetIndexFromId(window, sprite, CHANGE_ITEM_DETAIL_MOVE_1);
        u16 move = pokemonBiosGetPokemonWazaDataId(selectedPokemon, slot);
        if (pokemonWazaCheckValid(selectedPokemon, slot)) {
            unsigned long message = wazaGetStatus(0, move, 1, 0);
            if (message) {
                msgctrlSetValue(CHANGE_VALUE_STRING, (unsigned long)GSmsgGetStringPtr(message));
                GSmsgPrintRect(window->mLayerID, 0, 0, sprite->mWidth, sprite->mHeight, textColor, CHANGE_MSG_STRING);
                winSpriteSetDisp(sprite, true);
            } else {
                winSpriteSetDisp(sprite, false);
            }
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    }
    case CHANGE_ITEM_DETAIL_ITEM: {
        u16 heldItem = pokemonGetSoubiItemDataId(selectedPokemon);
        if (heldItem) {
            msgctrlSetValue(CHANGE_VALUE_STRING, (unsigned long)GSmsgGetStringPtr(itemDataBiosGetName(itemDataBiosGetPtr(heldItem))));
            GSmsgPrint2(window->mLayerID, 0, 0, textColor, CHANGE_MSG_STRING);
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    }
    case CHANGE_ITEM_NOT_READY_LABEL:
        sprite->mMsgID = CHANGE_MSG_ACTION_EXCHANGE;
        winSpriteSetDisp(sprite, !mPlayerReady[player]);
        break;
    case CHANGE_ITEM_NOT_READY_DETAIL:
        winSpriteSetDisp(sprite, !mPlayerReady[player]);
        sprite->mMsgID = CHANGE_MSG_NOT_READY_DETAIL;
        break;
    case CHANGE_ITEM_WAITING_INDICATOR:
        winSpriteSetDisp(sprite, !mPlayerReady[player]);
        break;
    case CHANGE_ITEM_WAITING_DETAIL:
        winSpriteSetDisp(sprite, !mPlayerReady[player]);
        break;
    case CHANGE_ITEM_READY_INDICATOR:
        winSpriteSetDisp(sprite, mPlayerReady[player]);
        break;
    case CHANGE_ITEM_CONFIRM_MODEL:
        selectItemHookPrime(window, sprite, CHANGE_ITEM_POKEMON_MODEL);
        break;
    case CHANGE_ITEM_CONFIRM_TRAINER_NAME:
        selectItemHookPrime(window, sprite, CHANGE_ITEM_TRAINER_NAME);
        break;
    case CHANGE_ITEM_DETAIL_STATIC_LABEL:
        break;
    }
}

inline float CMenuPokemonChange::calcAnimationTimeRate0to1to0(float seconds)
{
    return (float)sin(3.1415927f * calcAnimationTimeRate0to1(seconds));
}

void CMenuPokemonChange::stateFunctionCheckconnection()
{
    Hero* heroes[4] = {0};
    heroes[1] = &mGbaHero;
    if (openMenuGBA(0, 9, heroes) < 0) {
        mState.mNextState = STATE_EXIT;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    } else {
        requestPokemonFaceLoad();
        mState.mNextState = mState.mState + 1;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    }
}

void CMenuPokemonChange::stateFunctionSelectPokemon2Reset()
{
    mCursor[PLAYER_GBA] = getQuantityHeroPokemon(PLAYER_LOCAL);
}

void CMenuPokemonChange::stateFunctionSelectPokemonStart()
{
    mCursor[PLAYER_LOCAL] = 0;
    mCursor[PLAYER_GBA] = getQuantityHeroPokemon(PLAYER_LOCAL);
    mState.mNextState = mState.mState + 1;
    memset(mState.mWork, 0, sizeof(mState.mWork));
}

void CMenuPokemonChange::stateFunctionSelectPokemon1Main()
{
    stateFunctionSelectPokemon2Reset();
    mPlayerIndex = PLAYER_LOCAL;
    if (bgDrawFadeIsLive()) {
        menuStateChange(MENU_CONNECT);
        bgDrawFadeIn();
    }
    menuStateChange(MENU_SELECT_FIRST);
    mSelectionActive = true;
    windowCheckCursor(CHANGE_MENU_SELECT_POKEMON, 1);
    mSelectionActive = false;
    long selection = windowGetValue(CHANGE_MENU_SELECT_POKEMON);
    if (selection == -1 || selection >= getQuantityAllPokemon()) {
        mState.mNextState = STATE_FIRST_CANCEL;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    } else {
        mCursor[mPlayerIndex] = selection;
        mState.mNextState = STATE_FIRST_SELECTED;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    }
}

void CMenuPokemonChange::stateFunctionSelectPokemon1Selected()
{
    if (!pokmonSelectCheck(PLAYER_LOCAL))
        return;
    long slot = mCursor[mPlayerIndex];
    enumPlayerIndex owner = slot >= getQuantityHeroPokemon(PLAYER_LOCAL) ? PLAYER_GBA : PLAYER_LOCAL;
    long choice;
    if (owner == mPlayerIndex)
        choice = _actionSelect2or3(CHANGE_MSG_ACTION_STATUS, CHANGE_MSG_ACTION_EXCHANGE, CHANGE_MSG_ACTION_CANCEL);
    else
        choice = _actionSelect2or3(CHANGE_MSG_ACTION_STATUS, CHANGE_MSG_ACTION_CANCEL, 0);
    switch (choice) {
    case CHANGE_MSG_ACTION_STATUS:
        mState.mNextState = STATE_FIRST_STATUS;
        memset(mState.mWork, 0, sizeof(mState.mWork));
        break;
    case CHANGE_MSG_ACTION_EXCHANGE:
        mState.mNextState = STATE_FIRST_EXCHANGE;
        memset(mState.mWork, 0, sizeof(mState.mWork));
        break;
    case CHANGE_MSG_ACTION_CANCEL:
        mState.mNextState = STATE_SELECT_FIRST;
        memset(mState.mWork, 0, sizeof(mState.mWork));
        break;
    }
}

void CMenuPokemonChange::stateFunctionSelectPokemon1Chanceld()
{
    _setInfo(CHANGE_MSG_CONFIRM_EXIT);
    long choice = _actionSelect2or3(CHANGE_MSG_YES, CHANGE_MSG_NO, 0);
    _setInfo(0);
    switch (choice) {
    case CHANGE_MSG_YES:
        mState.mNextState = STATE_SAVE_ERROR;
        memset(mState.mWork, 0, sizeof(mState.mWork));
        break;
    case CHANGE_MSG_NO:
        mState.mNextState = STATE_SELECT_FIRST;
        memset(mState.mWork, 0, sizeof(mState.mWork));
        break;
    }
}

void CMenuPokemonChange::stateFunctionSelectPokemon1CheckStatus()
{
    _pokemonChangeFadeOut(0.3f);
    menuStateChange(MENU_CLOSED);
    menuPokemonStatusExecute(PLAYER_LOCAL);
    mReopenSelection = true;
    menuStateChange(MENU_SELECT_FIRST);
    waitMenuOpen();
    _pokemonChangeFadeIn(0.3f);
    fadeCheck(1);
    mState.mNextState = STATE_FIRST_SELECTED;
    memset(mState.mWork, 0, sizeof(mState.mWork));
}

void CMenuPokemonChange::stateFunctionSelectPokemon1PokeExChange()
{
    if (pokemonExchangeCheck(PLAYER_LOCAL)) {
        mState.mNextState = STATE_SELECT_SECOND;
        memset(mState.mWork, 0, sizeof(mState.mWork));
        stateFunctionSelectPokemon2Reset();
    }
}

void CMenuPokemonChange::stateFunctionSelectPokemon2Main()
{
    mPlayerIndex = PLAYER_GBA;
    menuSetEnablePort(2);
    menuStateChange(MENU_SELECT_SECOND);
    mSelectionActive = true;
    windowCheckCursor(CHANGE_MENU_SELECT_POKEMON, 1);
    mSelectionActive = false;
    menuSetEnablePort(1);
    if (!gbaConnectionCheckIsError()) {
        long selection = windowGetValue(CHANGE_MENU_SELECT_POKEMON);
        if (selection == -1 || selection >= getQuantityAllPokemon()) {
            mState.mNextState = STATE_SELECT_FIRST;
            memset(mState.mWork, 0, sizeof(mState.mWork));
        } else {
            mCursor[mPlayerIndex] = selection;
            mState.mNextState = STATE_SECOND_SELECTED;
            memset(mState.mWork, 0, sizeof(mState.mWork));
        }
    }
}

void CMenuPokemonChange::stateFunctionSelectPokemon2Selected()
{
    if (!pokmonSelectCheck(PLAYER_GBA))
        return;
    menuSetEnablePort(2);
    long slot = mCursor[mPlayerIndex];
    enumPlayerIndex owner = slot >= getQuantityHeroPokemon(PLAYER_LOCAL) ? PLAYER_GBA : PLAYER_LOCAL;
    long choice;
    if (owner == mPlayerIndex)
        choice = _actionSelect2or3(CHANGE_MSG_ACTION_STATUS, CHANGE_MSG_ACTION_EXCHANGE, CHANGE_MSG_ACTION_CANCEL);
    else
        choice = _actionSelect2or3(CHANGE_MSG_ACTION_STATUS, CHANGE_MSG_ACTION_CANCEL, 0);
    menuSetEnablePort(1);
    if (gbaConnectionCheckIsError())
        return;
    switch (choice) {
    case CHANGE_MSG_ACTION_STATUS:
        mState.mNextState = STATE_SECOND_STATUS;
        memset(mState.mWork, 0, sizeof(mState.mWork));
        break;
    case CHANGE_MSG_ACTION_EXCHANGE:
        mState.mNextState = STATE_SECOND_EXCHANGE;
        memset(mState.mWork, 0, sizeof(mState.mWork));
        break;
    case CHANGE_MSG_ACTION_CANCEL:
        mState.mNextState = STATE_SELECT_SECOND;
        memset(mState.mWork, 0, sizeof(mState.mWork));
        break;
    }
}

void CMenuPokemonChange::stateFunctionSelectPokemon2CheckStatus()
{
    _pokemonChangeFadeOut(0.3f);
    menuStateChange(MENU_CLOSED);
    menuSetEnablePort(2);
    menuPokemonStatusExecute(PLAYER_GBA);
    menuSetEnablePort(1);
    mReopenSelection = true;
    menuStateChange(MENU_SELECT_SECOND);
    waitMenuOpen();
    _pokemonChangeFadeIn(0.3f);
    fadeCheck(1);
    if (menuGetLastError() == 1) {
        GSsndPlayNow(CHANGE_SOUND_ERROR);
        winMsgOpen(2, CHANGE_MSG_CONNECTION_LOST, 1, 0);
        mState.mNextState = STATE_SAVE_ERROR;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    } else {
        mState.mNextState = STATE_SECOND_SELECTED;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    }
}

void CMenuPokemonChange::stateFunctionSelectPokemon2PokeExChange()
{
    if (pokemonExchangeCheck(PLAYER_GBA)) {
        mState.mNextState = STATE_SECOND_DECIDE_WAIT;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    }
}

void CMenuPokemonChange::stateFunctionSelectPokemon2DecideAfterWait()
{
    _menuWait(0.6f);
    tagMENU_ITEM_DATA* item = menuPtItemGetByMenuIdAndItemId(CHANGE_MENU_CONFIRM, CHANGE_ITEM_POKEMON_MODEL, 0);
    loadPokemonModels(item);
    bgDrawFadeOut();
    mState.mNextState = STATE_CONFIRM_EXCHANGE;
    memset(mState.mWork, 0, sizeof(mState.mWork));
}

void CMenuPokemonChange::stateFunctionConfirmExChange()
{
    mPlayerReady[0] = 0;
    mPlayerReady[1] = 0;
    menuStateChange(MENU_CONFIRM);
    bgDrawFadeIn();
    menuSetEnablePort(3);
    bool localReady = false;
    bool gbaReady = false;
    while (!localReady || !gbaReady) {
        unsigned long localKeys = windowGetPortKeyInfo(1)->mKeyTrg;
        unsigned long gbaKeys = windowGetPortKeyInfo(2)->mKeyTrg;
        if (gbaConnectionCheckIsError())
            goto cleanup;
        if (GSfsysGetDriveStatus() != DVD_STATE_COVER_OPEN) {
            if (localKeys & WINDOW_KEY_ACCEPT) {
                if (!localReady)
                    GSsndPlayNow(EXCHANGE_SOUND_ACCEPT);
                localReady = 1;
            } else if (localKeys & WINDOW_KEY_CANCEL) {
                GSsndPlayNow(EXCHANGE_SOUND_CANCEL);
                if (!localReady)
                    break;
                localReady = 0;
            }
            if (gbaKeys & WINDOW_KEY_ACCEPT) {
                if (!gbaReady)
                    GSsndPlayNow(EXCHANGE_SOUND_ACCEPT);
                gbaReady = 1;
            } else if (gbaKeys & WINDOW_KEY_CANCEL) {
                GSsndPlayNow(EXCHANGE_SOUND_CANCEL);
                if (!gbaReady)
                    break;
                gbaReady = 0;
            }
        }
        GSthreadSwitch();
        mPlayerReady[0] = localReady;
        mPlayerReady[1] = gbaReady;
    }
    mPlayerReady[0] = localReady;
    mPlayerReady[1] = gbaReady;
    if (!localReady || !gbaReady) {
        mState.mNextState = STATE_SELECT_FIRST;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    } else {
        void* saveData = (void*)savedataGetStatus(0, SAVE_DATA_IMAGE);
        memcpy(mSaveData, saveData, EXCHANGE_SAVE_BLOCK_SIZE);
        mState.mNextState = STATE_EXCHANGE_START;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    }
cleanup:
    bgDrawFadeOut();
    releasePokemonModels();
    menuSetEnablePort(1);
}

void CMenuPokemonChange::stateFunctionExChangeStart()
{
    loadSelectedPokemonModels(EXCHANGE_MODEL_MENU, EXCHANGE_MODEL_ITEM);
    menuStateChange(MENU_EXCHANGE);
    bgDrawFadeIn();
    _menuWait(1.0f);
    const u16* localName = (const u16*)heroGetStatus(mHero, HERO_STATUS_NAME, 0);
    const u16* gbaName = (const u16*)heroGetStatus(&mGbaHero, HERO_STATUS_NAME, 0);
    u16* firstNickname = pokemon_GetNickname(getPokemon(mCursor[0]));
    u16* secondNickname = pokemon_GetNickname(getPokemon(mCursor[1]));
    msgctrlSetValue(EXCHANGE_TEXT_TRAINER, (unsigned long)gbaName);
    msgctrlSetValue(EXCHANGE_TEXT_OTHER_TRAINER, (unsigned long)localName);
    msgctrlSetValue(EXCHANGE_TEXT_FIRST_POKEMON, (unsigned long)firstNickname);
    msgctrlSetValue(EXCHANGE_TEXT_SECOND_POKEMON, (unsigned long)secondNickname);
    winMsgOpen(2, EXCHANGE_MESSAGE_FIRST_DEPARTURE, 1, 0);
    winMsgOpen(2, EXCHANGE_MESSAGE_SECOND_DEPARTURE, 1, 0);
    winMsgClose(1);
    bgDrawFadeOut();
    releasePokemonModels();
    menuStateChange(MENU_BACKGROUND);
    mState.mNextState = STATE_EXCHANGE_MAIN;
    memset(mState.mWork, 0, sizeof(mState.mWork));
}

void CMenuPokemonChange::stateFunctionExChangeMain()
{
    long player;
    static Pokemon pokemon;
    pokemonInit(&pokemon);
    Pokemon* selected[2];
    for (player = 0; player < 2; ++player)
        selected[player] = getPokemon(mCursor[player]);
    DarkPokemon* darkPokemon = selected[0]->getDarkPokemon();
    if (darkPokemon)
        darkPokemon->setPlace(DARK_POKEMON_EXCHANGED, 0);
    pokemonBiosCopy(&pokemon, selected[1]);
    pokemonBiosCopy(selected[1], selected[0]);
    pokemonBiosCopy(selected[0], &pokemon);
    bgDrawFadeIn(2.0f);
    _pokemonChangeFadeOut(0.0f);
    _pokemonChangeFadeIn(1.0f);
    cameraPlayAnime(floorGetCurrentGroupID(), EXCHANGE_CAMERA_RESOURCE, 0, 0.0f);
    GSthreadSwitch();
    GSsndPlayNow(EXCHANGE_SOUND_ANIMATION);
    floorMapSetDisp(1);
    scriptModelSetMotion(EXCHANGE_MODEL_RESOURCE, 0, 0, 0);
    _pokemonChangeFadeIn(3.0f);
    cameraWaitSyncAnime(1);
    _pokemonChangeFadeOut(0.7f);
    floorMapSetDisp(0);
    loadSelectedPokemonModels(EXCHANGE_MODEL_MENU, EXCHANGE_MODEL_ITEM);
    menuStateChange(MENU_EXCHANGE);
    _pokemonChangeFadeIn(0.15f);
    mState.mNextState = STATE_EXCHANGE_OK;
    memset(mState.mWork, 0, sizeof(mState.mWork));
}

void CMenuPokemonChange::stateFunctionExChangeOK()
{
    pokemonBiosSetFriend(getPokemon(mCursor[0]), EXCHANGE_FRIEND_LEVEL);
    pokemonBiosSetFriend(getPokemon(mCursor[1]), EXCHANGE_FRIEND_LEVEL);
    memoDataSet(0, getPokemon(mCursor[0]));
    const u16* localName = (const u16*)heroGetStatus(mHero, HERO_STATUS_NAME, 0);
    const u16* gbaName = (const u16*)heroGetStatus(&mGbaHero, HERO_STATUS_NAME, 0);
    u16* firstNickname = pokemon_GetNickname(getPokemon(mCursor[0]));
    u16* secondNickname = pokemon_GetNickname(getPokemon(mCursor[1]));
    msgctrlSetValue(EXCHANGE_TEXT_TRAINER, (unsigned long)localName);
    msgctrlSetValue(EXCHANGE_TEXT_OTHER_TRAINER, (unsigned long)gbaName);
    msgctrlSetValue(EXCHANGE_TEXT_FIRST_POKEMON, (unsigned long)firstNickname);
    msgctrlSetValue(EXCHANGE_TEXT_SECOND_POKEMON, (unsigned long)secondNickname);
    winMsgOpen(2, EXCHANGE_MESSAGE_COMPLETE, 1, 0);
    winMsgClose(1);
    mState.mNextState = STATE_EVOLUTION;
    memset(mState.mWork, 0, sizeof(mState.mWork));
}

void CMenuPokemonChange::_pokemonChangeEvolutionEffect(Pokemon* pokemon,
    MENU_POKEMONCHANGE_USERTYPE owner, unsigned short species, POKEMON_EVOLUTION_INFO info)
{
    if (species != POKEMON_SPECIES_NONE && species != POKEMON_SPECIES_INVALID) {
        switch (owner) {
        case USER_LOCAL:
            pokemonEvolutionAll(pokemon, species, 0, &info, 0, 0, 0, 1);
            break;
        case USER_GBA:
            pokemonEvolutionAll(pokemon, species, 0, &info, 0, 1, 0, 1);
            break;
        }
    }
}

void CMenuPokemonChange::stateFunctionEvolution()
{
    mGbaEvolutionSpecies = 0;
    u8 evolves = 0;
    u16 additionalSpecies;
    POKEMON_EVOLUTION_INFO firstInfo;
    POKEMON_EVOLUTION_INFO secondInfo;
    u16 firstSpecies = pokemonEvolutionCheck(getPokemon(mCursor[PLAYER_LOCAL]), POKEMON_EVOLUTION_TRADE, 0, &additionalSpecies, &firstInfo);
    if (firstSpecies != POKEMON_SPECIES_NONE && firstSpecies != POKEMON_SPECIES_INVALID)
        evolves = 1;
    u16 secondSpecies = pokemonEvolutionCheck(getPokemon(mCursor[PLAYER_GBA]), POKEMON_EVOLUTION_TRADE, 0, &additionalSpecies, &secondInfo);
    if (secondSpecies != POKEMON_SPECIES_NONE && secondSpecies != POKEMON_SPECIES_INVALID) {
        evolves = 1;
        mGbaEvolutionSpecies = pokemonBiosGetPokemonDataId(getPokemon(mCursor[PLAYER_GBA]));
    }
    if (evolves) {
        _pokemonChangeFadeOut(0.3f);
        menuStateChange(MENU_CLOSED);
        releasePokemonModels();
        bgModelRelease();
        GSthreadSwitch();
        _pokemonChangeFadeIn(0.0f);
        _pokemonChangeEvolutionEffect(getPokemon(mCursor[PLAYER_LOCAL]), USER_GBA, firstSpecies, firstInfo);
        _pokemonChangeEvolutionEffect(getPokemon(mCursor[PLAYER_GBA]), USER_LOCAL, secondSpecies, secondInfo);
        _pokemonChangeFadeOut(0.0f);
        bgModelLoad();
        loadSelectedPokemonModels(MENU_TRADE_PREVIEW, ITEM_TRADE_PREVIEW_VIEWPORT);
        menuStateChange(MENU_EXCHANGE);
        _pokemonChangeFadeIn(0.3f);
        for (long i = 0; i < 2; ++i) {
            Pokemon* pokemon = getPokemon(mCursor[i]);
            menuFaceLoad(pokemon, 0, 0);
            while (!menuFaceSearchEnable(menuFaceGetID(pokemon)))
                GSthreadSwitch();
        }
    }
    mState.mNextState = STATE_SAVE_REPORT;
    memset(mState.mWork, 0, sizeof(mState.mWork));
}

void CMenuPokemonChange::stateFunctionSaveReport()
{
    u8 gbaPrepared;
    u8 saved;
    saved = 0;
    gbaPrepared = 0;
    winMsgOpen(2, MSG_TRADE_SAVE_IN_PROGRESS, 1, 1);
    Pokemon* pokemon = getPokemon(mCursor[PLAYER_GBA]);
    long slot = getHeroPokemonIndex(mCursor[PLAYER_GBA]);
    if (!gbaCommandChangePokemon(TRADE_GBA_PORT, pokemon, slot, mGbaEvolutionSpecies)) {
        memcardSetupTask(TRADE_SAVE_TASK, TRADE_SAVE_MODE, 0);
        long result;
        do {
            if (!gbaPrepared && memcardCheckTaskWaitGBA()) {
                long prepared = gbaCommandChangePokemonIsPrepared(TRADE_GBA_PORT);
                if (prepared >= 0) {
                    if (prepared)
                        memcardUnlockWaitGBA(0);
                    else
                        memcardUnlockWaitGBA(1);
                    gbaPrepared = 1;
                }
            }
            GSthreadSwitch();
            result = memcardGetTaskResult();
        } while (result == TRADE_SAVE_PENDING);
        gbaCommandSetKeyState(TRADE_GBA_PORT, 1);
        memcardFinalizeTask();
        if (result == TRADE_SAVE_SUCCEEDED)
            saved = 1;
    }
    if (!gbaPrepared || !saved) {
        memcpy(reinterpret_cast<void*>(savedataGetStatus(0, SAVE_DATA_IMAGE)), mSaveData, SAVE_BLOCK_SIZE);
        GSsndPlayNow(SOUND_TRADE_SAVE_ERROR);
        winMsgOpen(2, MSG_TRADE_SAVE_FAILED, 1, 0);
        winMsgClose(1);
        releasePokemonModels();
        mState.mNextState = STATE_CONNECTION_ERROR;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    } else {
        unsigned long wait = randomRange(TRADE_SAVE_COMPLETION_DELAY);
        while (wait--)
            GSthreadSwitch();
        winMsgOpen(2, MSG_TRADE_SAVE_COMPLETE, 1, 0);
        winMsgClose(1);
        mState.mNextState = STATE_CHECK_NEXT;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    }
}

bool CMenuPokemonChange::gbaConnectionCheckIsError()
{
    if (!gbaCommandIsConnected(2)) {
        menuSetEnablePort(1);
        GSsndPlayNow(CHANGE_SOUND_ERROR);
        winMsgOpen(2, CHANGE_MSG_CONNECTION_LOST, 1, 0);
        mState.mNextState = STATE_SAVE_ERROR;
        memset(mState.mWork, 0, sizeof(mState.mWork));
        return true;
    }
    return false;
}

void CMenuPokemonChange::stateFunctionCheckNext()
{
    winMsgOpen(2, CHANGE_MSG_EXCHANGE_AGAIN, 0, 0);
    s8 again = menuSubOpenYesNo(0, -1, -1, 0);
    winMsgClose(1);
    _menuWait(0.5f);
    bgDrawFadeOut();
    releasePokemonModels();
    if (again == 0) {
        mState.mNextState = STATE_SELECT_START;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    } else {
        mState.mNextState = STATE_SAVE_ERROR;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    }
}

void CMenuPokemonChange::stateFunctionError(Pokemon* pokemon, unsigned long message)
{
    u16* nickname = pokemon_GetNickname(pokemon);
    msgctrlSetValue(EXCHANGE_TEXT_FIRST_POKEMON, reinterpret_cast<unsigned long>(nickname));
    _setInfo(message);
    GSsndPlayNow(CHANGE_SOUND_ERROR);
    _menuWait(1.5f);
    _setInfo(0);
    if (mState.mState >= STATE_SELECT_SECOND) {
        mState.mNextState = STATE_SELECT_SECOND;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    } else {
        mState.mNextState = STATE_SELECT_FIRST;
        memset(mState.mWork, 0, sizeof(mState.mWork));
    }
}

bool CMenuPokemonChange::isPokemonLookAbleStatus(long index)
{
    long cursor = index;
    Pokemon* pokemon = getPokemon(cursor);
    switch (_getPokemonState(pokemon,
        index >= getQuantityHeroPokemon(PLAYER_LOCAL) ? PLAYER_GBA : PLAYER_LOCAL)) {
    case POKEMON_CORRUPT:
    case POKEMON_EGG:
        return false;
    }
    return true;
}

bool CMenuPokemonChange::pokmonSelectCheck(enumPlayerIndex player)
{
    Pokemon* pokemon = getSelectPokemon(player);
    if (isPokemonLookAbleStatus(mCursor[player]))
        return true;
    switch (_getPokemonState(pokemon, player)) {
    case POKEMON_EGG:
        stateFunctionError(pokemon, CHANGE_MSG_EGG_STATUS_RESTRICTED);
        break;
    case POKEMON_CORRUPT:
        stateFunctionError(pokemon, CHANGE_MSG_CORRUPT_STATUS_RESTRICTED);
        break;
    }
    return false;
}

bool CMenuPokemonChange::pokemonExchangeCheck(enumPlayerIndex player)
{
    long cursor = mCursor[player];
    Pokemon* pokemon = getPokemon(cursor);
    switch (_getPokemonState(pokemon, player)) {
    case POKEMON_INVALID:
    case POKEMON_CORRUPT:
    case POKEMON_EGG:
        break;
    case POKEMON_SHADOW:
        stateFunctionError(pokemon, CHANGE_MSG_SHADOW_EXCHANGE_RESTRICTED);
        return false;
    case POKEMON_ITEM_RESTRICTED:
        stateFunctionError(pokemon, CHANGE_MSG_ITEM_EXCHANGE_RESTRICTED);
        return false;
    case POKEMON_EVENT_RESTRICTED:
    case POKEMON_DATA_RESTRICTED:
        stateFunctionError(pokemon, CHANGE_MSG_DATA_EXCHANGE_RESTRICTED);
        return false;
    case POKEMON_EXCHANGE_OK:
        break;
    }
    Hero* hero = getHero(player);
    long excludedSlot = mCursor[player];
    if (excludedSlot >= getQuantityHeroPokemon(PLAYER_LOCAL))
        excludedSlot -= getQuantityHeroPokemon(PLAYER_LOCAL);
    if (_getQuantityBattleAbleWoIt(hero, excludedSlot, player) == 0) {
        stateFunctionError(pokemon, CHANGE_MSG_NO_REMAINING_BATTLE_POKEMON);
        return false;
    }
    return true;
}

bool _isTemochiPokemonCheck()
{
    Hero* hero = Hero::getHeroPtr();
    long slot = 0;
    long exchangeAble = 0;
    long counts[POKEMON_EXCHANGE_OK + 1];
    memset(counts, 0, sizeof(counts));
    do {
        Pokemon* pokemon = hero->getPokemon(slot);
        if (_isPokemonExchangeAble(pokemon, PLAYER_LOCAL))
            ++exchangeAble;
        ++counts[_getPokemonState(pokemon, PLAYER_LOCAL)];
        if (!_isPokemonValid(pokemon, PLAYER_LOCAL))
            break;
        ++slot;
    } while (slot < 6);
    if (slot <= 1) {
        GSsndPlayNow(CHANGE_SOUND_ERROR);
        winMsgOpenWithSE(3, CHANGE_MSG_PARTY_TOO_SMALL, 1, 0, 2);
        winMsgClose(1);
        return false;
    }
    if (exchangeAble == 0) {
        unsigned long message = CHANGE_MSG_NO_EXCHANGE_POKEMON;
        if (counts[POKEMON_SHADOW])
            message = CHANGE_MSG_PARTY_SHADOW_RESTRICTED;
        else if (counts[POKEMON_ITEM_RESTRICTED])
            message = CHANGE_MSG_PARTY_ITEM_RESTRICTED;
        else if (counts[POKEMON_EGG] || counts[POKEMON_CORRUPT])
            message = CHANGE_MSG_PARTY_INVALID;
        else if (counts[POKEMON_EVENT_RESTRICTED] || counts[POKEMON_DATA_RESTRICTED])
            message = CHANGE_MSG_PARTY_DATA_RESTRICTED;
        GSsndPlayNow(CHANGE_SOUND_ERROR);
        winMsgOpenWithSE(3, message, 1, 0, 2);
        winMsgClose(1);
        return false;
    }
    return true;
}

}

extern "C" void menuPokemonChange()
{
    unsigned long group;
    if (_isTemochiPokemonCheck()) {
        group = GSfloorGetCurrentFloorGroupID();
        winMsgOpenWithSE(3, CHANGE_MSG_SAVE_BEFORE_EXCHANGE, 1, 0, 2);
        if (menuSubOpenYesNoField() == 0) {
            long nextFloor = floorGetNextFloorID();
            gamedatasaveSetStatus(0, 5, nextFloor);
            long result = memcardTaskCall(4, 2, 0);
            winMsgClose(1);
            if (result == 4) {
                GSfloorPush(CHANGE_FLOOR);
                floorWaitForChange();
            }
            return;
        }
    }
    winMsgClose(1);
    GSthreadUnblockGroup(group);
}

extern "C" __declspec(export) void menuPokemonChangeInit()
{
    floorMapSetDisp(0);
    scriptModelStopMotion(EXCHANGE_MODEL_RESOURCE);
}

extern "C" __declspec(export) void menuPokemonChangeMain()
{
    MenuPokemonChange.main();
}

extern "C" __declspec(export) void menuPokemonChangeExit() {}

extern "C" void menuPokemonChangeDraw(tagWINDOW_WORK* window)
{
    MenuPokemonChange.selectDrawHook(window);
}

extern "C" void menuPokemonChangeItemHook(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    MenuPokemonChange.selectItemHook(window, sprite);
}

extern "C" long menuPokemonChangeWindowHook(tagWINDOW_WORK* window)
{
    MenuPokemonChange.ctrlHook(window);
    return 0;
}

extern "C" void menuPokemonChangeWindowCursor(tagWINDOW_WORK* window)
{
    if (!winSeqCheckMove(window->mMenuID)) {
        u16 primary = winSysWork.mDirectionRepeat;
        u16 secondary = winSysWork2.mDirectionRepeat;
        u16 combined = primary | secondary;
        if ((combined & 12) != 0 && (combined & 3) != 0) {
            u16 primaryStick = winSysWork.mStickDirectionRepeat;
            u16 secondaryStick = winSysWork2.mStickDirectionRepeat;
            winSysWork.mDirectionRepeat = primary & 12;
            winSysWork.mStickDirectionRepeat = primaryStick & 12;
            winSysWork2.mDirectionRepeat = secondary & 12;
            winSysWork2.mStickDirectionRepeat = secondaryStick & 12;
            menuCursorNormal(window, 1);
            winSysWork.mDirectionRepeat = primary & 3;
            winSysWork.mStickDirectionRepeat = primaryStick & 3;
            winSysWork2.mDirectionRepeat = secondary & 3;
            winSysWork2.mStickDirectionRepeat = secondaryStick & 3;
            menuCursorNormal(window, 1);
            winSysWork.mDirectionRepeat = primary;
            winSysWork.mStickDirectionRepeat = primaryStick;
            winSysWork2.mDirectionRepeat = secondary;
            winSysWork2.mStickDirectionRepeat = secondaryStick;
        } else {
            menuCursorNormal(window, 1);
        }
    }
}

extern "C" u8 menuPokemonChangeCursor(u8 sex, Pokemon* pokemon)
{
    u16* nextCharacter;
    u16 character;
    u16 ownCount;
    u16 otherCount;
    u16* nickname = reinterpret_cast<u16*>(pokemonGetStatus(pokemon, 0, POKEMON_STATUS_NICKNAME, 0));
    unsigned long length = GSmsgGetCharLength(nickname);
    otherCount = 0;
    ownCount = 0;
    u16 ownSymbol;
    u16 otherSymbol;
    switch (sex) {
    case 0:
        ownSymbol = CHANGE_GLYPH_MALE;
        otherSymbol = CHANGE_GLYPH_FEMALE;
        break;
    case 1:
        ownSymbol = CHANGE_GLYPH_FEMALE;
        otherSymbol = CHANGE_GLYPH_MALE;
        break;
    default:
        return sex;
    }
    nextCharacter = nickname;
    for (unsigned long i = 0; i < length; ++i) {
        character = *nextCharacter;
        if (ownSymbol == character)
            ++ownCount;
        if (otherSymbol == character)
            ++otherCount;
        ++nextCharacter;
    }
    if (otherCount == 0 && ownCount != 0)
        return 2;
    return sex;
}

namespace {

inline unsigned long randomRange(unsigned long limit)
{
    return ((HSD_Rand() << 16) | HSD_Rand()) % limit;
}

inline float CMenuPokemonChange::calcAnimationTimeRate0to1(float seconds)
{
    unsigned long frames = seconds * GSgfxVideoGetVsyncRate();
    return (float)(unsigned long)(mDrawTime % frames) / frames;
}

inline Pokemon* CMenuPokemonChange::getPokemon(long index)
{
    enumPlayerIndex player = PLAYER_LOCAL;
    if (index >= getQuantityHeroPokemon(PLAYER_LOCAL)) {
        index -= getQuantityHeroPokemon(PLAYER_LOCAL);
        player = PLAYER_GBA;
    }
    Hero* hero;
    if (player == PLAYER_LOCAL)
        hero = mHero;
    else
        hero = &mGbaHero;
    return hero->getPokemon(index);
}

}
