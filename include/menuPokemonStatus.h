#ifndef MENU_POKEMON_STATUS_H
#define MENU_POKEMON_STATUS_H

#include <Pokemon.h>
#include <menuModel.h>
#include <types.h>
#include <winSprite.h>

struct FIGHT_TRAINER;

// Page the status-menu window is on. The four tab pages are the pages the
// tab cursor (slots 0 to 3) can select; the others are entered from them.
enum MENU_POKEMON_STATUS_PAGE {
    MENU_POKEMON_STATUS_PAGE_SELECT = 0,       // no tab chosen yet.
    MENU_POKEMON_STATUS_PAGE_TAB_SLOT1 = 1,    // tab cursor slot 1.
    MENU_POKEMON_STATUS_PAGE_WAZA = 2,         // tab cursor slot 2: the waza page.
    MENU_POKEMON_STATUS_PAGE_WAZA_LIST = 3,    // one waza of the list is picked.
    MENU_POKEMON_STATUS_PAGE_WAZA_REPLACE = 4, // the picked waza is moved.
    MENU_POKEMON_STATUS_PAGE_RIBBON = 5,       // tab cursor slot 3: the ribbon page.
    MENU_POKEMON_STATUS_PAGE_RIBBON_SELECT = 6, // one ribbon grid cell is picked.
    MENU_POKEMON_STATUS_PAGE_WAZA_LEARN = 7,   // a waza to learn is picked.
    MENU_POKEMON_STATUS_PAGE_TAB_SLOT0 = 8,    // tab cursor slot 0.
    MENU_POKEMON_STATUS_PAGE_CANCEL = 9,       // the page was left with B.
};

// Button bits of the window key state the status menu reacts to.
enum MENU_POKEMON_STATUS_KEY {
    // The up/down pair also selects the previous/next pokemon through the
    // callback, whose MENU_POKEMON_CB_CURSOR_* values are 1 and 2.
    MENU_POKEMON_STATUS_KEY_UP = 0x1,    // move up a row / to the previous pokemon.
    MENU_POKEMON_STATUS_KEY_DOWN = 0x2,  // move down a row / to the next pokemon.
    MENU_POKEMON_STATUS_KEY_LEFT = 0x4,  // move a column left / to the previous tab.
    MENU_POKEMON_STATUS_KEY_RIGHT = 0x8, // move a column right / to the next tab.
    MENU_POKEMON_STATUS_KEY_A = 0x10,    // confirm / step to the next page.
    MENU_POKEMON_STATUS_KEY_B = 0x20,    // cancel / step back to the previous page.
};

// Texts the waza pages draw over the move slots, and the bars the summary
// page draws. The HP bar texture shows how full the bar is: full above half,
// half below half, low below a fifth. The exp bar is drawn as wide as the
// exp of the current level is complete. The pokerus textures draw the
// infection state: active while the low nibble is set, cured for a leftover
// non-zero byte.
enum MENU_POKEMON_STATUS_TEXTURE {
    MENU_POKEMON_STATUS_TEXTURE_POKERUS_ACTIVE = 0x84,  // active pokerus infection.
    MENU_POKEMON_STATUS_TEXTURE_WAZA_CURSOR = 0xF2,      // marker on the picked move.
    MENU_POKEMON_STATUS_TEXTURE_HP_BAR_FULL = 0xF3,      // HP bar above half.
    MENU_POKEMON_STATUS_TEXTURE_HP_BAR_HALF = 0xF4,      // HP bar below half.
    MENU_POKEMON_STATUS_TEXTURE_HP_BAR_LOW = 0xF5,       // HP bar below a fifth.
    MENU_POKEMON_STATUS_TEXTURE_EXP_BAR = 0xF6,         // experience bar.
    MENU_POKEMON_STATUS_TEXTURE_WAZA_CURSOR_PREV = 0xF7, // marker on the move it comes from.
    MENU_POKEMON_STATUS_TEXTURE_WAZA_NEW = 0xF8,          // flag on a move not seen yet.
    MENU_POKEMON_STATUS_TEXTURE_POKERUS_CURED = 0x1F8,   // cured pokerus infection.
};

// Items of the summary pages the summary hook draws. The bar items draw the
// HP and experience bars; the rest print the field they are named for.
enum MENU_POKEMON_STATUS_SUMMARY_ITEM {
    MENU_POKEMON_STATUS_ITEM_HP_BAR = 0x83B, // HP bar, textured by fullness.
    MENU_POKEMON_STATUS_ITEM_EXP_BAR = 0x83C, // experience bar of the level.
    MENU_POKEMON_STATUS_ITEM_RELIVE_METER = 0x83E, // relive meter of the pokemon.
    MENU_POKEMON_STATUS_ITEM_ABILITY_DESCRIPTION = 0x843, // description of the ability.
    MENU_POKEMON_STATUS_ITEM_ABILITY_NAME = 0x844, // name of the ability.
    MENU_POKEMON_STATUS_ITEM_SEALED_LABEL = 0x845, // sealed waza pages label.
    MENU_POKEMON_STATUS_ITEM_EXP_REMAINING = 0x846, // exp left to the next level.
    MENU_POKEMON_STATUS_ITEM_EXP = 0x848, // total experience.
    MENU_POKEMON_STATUS_ITEM_SHADOW_STATE = 0x849, // shadow level, or the normal label.
    MENU_POKEMON_STATUS_ITEM_SPEED = 0x84A, // nimbleness/speed stat.
    MENU_POKEMON_STATUS_ITEM_SP_DEFENSE = 0x84C, // special defense stat.
    MENU_POKEMON_STATUS_ITEM_SP_ATTACK = 0x84E, // special attack stat.
    MENU_POKEMON_STATUS_ITEM_DEFENSE = 0x850, // physical defense stat.
    MENU_POKEMON_STATUS_ITEM_ATTACK = 0x852, // attack stat.
    MENU_POKEMON_STATUS_ITEM_HP = 0x854, // current and maximum HP.
    MENU_POKEMON_STATUS_ITEM_TRAINER_ID = 0x25F, // trainer id digits.
    MENU_POKEMON_STATUS_ITEM_TRAINER_NAME = 0x261, // name of the trainer.
    MENU_POKEMON_STATUS_ITEM_MET_LINE = 0x25C, // met level and origin line.
    MENU_POKEMON_STATUS_ITEM_TYPE_FIRST = 0x21D, // first type icon.
    MENU_POKEMON_STATUS_ITEM_TYPE_SECOND = 0x21C, // second type icon, hidden when the same.
    MENU_POKEMON_STATUS_ITEM_HELD_ITEM = 0x25D, // held item name.
};

// Items of the nature page the nature hook draws. The model item configures
// and renders the pokemon model; the ball item draws the ball the pokemon
// was caught with; the pokerus item draws the pokerus indicator; the level
// item prints the level; the species and nickname items print the
// species-name versus nickname text; the tab items tint the four tab
// selectors according to cursor positions 0 to 3.
enum MENU_POKEMON_STATUS_NATURE_ITEM {
    MENU_POKEMON_STATUS_ITEM_SPECIES_NAME = 0x264, // species-name text.
    MENU_POKEMON_STATUS_ITEM_POKERUS = 0x804, // pokerus indicator.
    MENU_POKEMON_STATUS_ITEM_TAB_SELECTOR3 = 0x80C, // tab selector lit on cursor 3.
    MENU_POKEMON_STATUS_ITEM_TAB_SELECTOR2 = 0x80D, // tab selector lit on cursor 2.
    MENU_POKEMON_STATUS_ITEM_TAB_SELECTOR1 = 0x80E, // tab selector lit on cursor 1.
    MENU_POKEMON_STATUS_ITEM_TAB_SELECTOR0 = 0x80F, // tab selector lit on cursor 0.
    MENU_POKEMON_STATUS_ITEM_BALL = 0x816, // ball the pokemon was caught with.
    MENU_POKEMON_STATUS_ITEM_LEVEL = 0x81D, // level of the pokemon.
    MENU_POKEMON_STATUS_ITEM_NICKNAME = 0x81E, // nickname text.
    MENU_POKEMON_STATUS_ITEM_MODEL = 0x81F, // pokemon model.
};

// Marking sprites of the nature page, one per marking slot.
enum MENU_POKEMON_STATUS_MARKING_ITEM {
    MENU_POKEMON_STATUS_ITEM_MARKING_SLOT0 = 0x817, // marking slot 0.
    MENU_POKEMON_STATUS_ITEM_MARKING_SLOT1 = 0x818, // marking slot 1.
    MENU_POKEMON_STATUS_ITEM_MARKING_SLOT2 = 0x819, // marking slot 2.
    MENU_POKEMON_STATUS_ITEM_MARKING_SLOT3 = 0x81A, // marking slot 3.
};

// Mode flags the caller passes to menuPokemonStatusOpen.
enum MENU_POKEMON_STATUS_FLAG {
    MENU_POKEMON_STATUS_FLAG_FIELD = 0x1,          // the menu runs on a pushed floor.
    MENU_POKEMON_STATUS_FLAG_WAZA_REPLACE = 0x2,   // the waza of the list may be moved.
    MENU_POKEMON_STATUS_FLAG_OPEN_FIRST_TAB = 0x4, // open on the first tab, not on the waza list.
    MENU_POKEMON_STATUS_FLAG_FLOOR_FADE = 0x8,     // the floor script owns the fades.
    MENU_POKEMON_STATUS_FLAG_WAZA_LEARN_OK = 0x10, // A picks a waza to learn.
    MENU_POKEMON_STATUS_FLAG_FIGHT_TRAINER = 0x20, // the fight trainer holds the hero data.
    MENU_POKEMON_STATUS_FLAG_WAZA_LOOP = 0x40,     // keep asking until the waza can be learned.
    MENU_POKEMON_STATUS_FLAG_FAST_FADE = 0x80,     // fade in and out in a quarter second.
};

// Callback the status menu installs to be told that the cursor moved to
// another pokemon. It returns the pokemon to show next: 1 asks for the previous
// pokemon, 2 for the next one. Returning the current pokemon keeps it.
enum MENU_POKEMON_CB_TYPE {
    MENU_POKEMON_CB_CURSOR_LEFT = 1,
    MENU_POKEMON_CB_CURSOR_RIGHT = 2,
};

// Ribbon work the ribbon page fills in (0x28 bytes). Every ribbon the
// pokemon wears gets one cell of the nine-by-four grid: the cell holds the
// ribbon index, or -1 when the pokemon does not wear that ribbon.
struct RIBBON_WORK {
    s32 mCount;        // 0x00: number of ribbons the pokemon wears.
    s8 mCells[9][4];   // 0x04: ribbon index of each grid cell.
};

// Status-menu work (0x48 bytes). The page byte at 0x1 is the
// MENU_POKEMON_STATUS_PAGE the menu runs; the pokemon at 0xC is the one being shown. The
// nine-by-four table at 0x20 holds the ribbon grid cells (one byte per cell,
// negative when the cell is empty) and the four bytes at 0x44 flag the waza
// the pokemon has not seen yet.
struct MenuPokemonStatusWork {
    u8 mFlags;      // 0x00: MENU_POKEMON_STATUS_FLAG bits.
    u8 mPage;       // 0x01: MENU_POKEMON_STATUS_PAGE the menu is on.
    s8 mWazaSlot;   // 0x02: waza slot the cursor is on.
    s8 mSlotPrev;   // 0x03: waza slot before the move, -1 when unset.
    s32 mResult;    // 0x04: result of the menu loop.
    FIGHT_TRAINER *mTrainer; // 0x08: battle trainer the menu acts on.
    Pokemon *mPokemon;       // 0x0C: pokemon being shown.
    Pokemon *(*mCallback)(Pokemon *, MENU_POKEMON_CB_TYPE, long); // 0x10
    long mCallbackArg;       // 0x14: argument passed to the callback.
    u16 mWazaId;           // 0x18: waza id offered as slot 4; 0 shows only the four own slots.
    s8 mGridIndex;          // 0x1A: index into the ribbon grid cells.
    u8 _1B;                  // 0x1B: padding.
    RIBBON_WORK mRibbon;     // 0x1C: ribbons the pokemon wears.
    u8 mWazaNew[4];          // 0x44: non-zero for waza not seen yet.
};

// Model rendered by the status menu.
extern "C" {
extern MenuPokemonStatusWork menuPokemonStatusWork;
extern MENU_MODEL_STATUS menuPokemonStatusModel;
}

// Status-menu window/menu ids the status menu opens. The head window owns the
// tab cursor; each tab slot opens its own sub-menu: slot 0 and slot 1 show the
// summary sides, slot 2 the waza (move) list and slot 3 the ribbon grid.
enum MENU_POKEMON_STATUS_MENU {
    MENU_POKEMON_STATUS_MENU_HEAD = 0x5E,      // status-menu head window.
    MENU_POKEMON_STATUS_MENU_TAB_SLOT0 = 0x5F, // tab cursor slot 0 sub-menu.
    MENU_POKEMON_STATUS_MENU_TAB_SLOT1 = 0x60, // tab cursor slot 1 sub-menu.
    MENU_POKEMON_STATUS_MENU_WAZA = 0x62,       // tab cursor slot 2: waza (move) sub-menu.
    MENU_POKEMON_STATUS_MENU_RIBBON = 0x63,     // tab cursor slot 3: ribbon sub-menu.
};

// Floor the status menu pushes when it runs on its own floor.
enum MENU_POKEMON_STATUS_FLOOR {
    MENU_POKEMON_STATUS_FLOOR_ID = 0x39D, // status-menu floor.
};

// Waza page sprites the waza hook draws. The five icon rows are the rows of
// the move list from top to bottom; which move slot a row shows depends on
// whether a waza to learn is offered as slot 4. The cursor sprite draws the
// move-selection cursor (and the previous-selection cursor while replacing).
enum MENU_POKEMON_STATUS_WAZA_ITEM {
    MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW4 = 0x200, // bottom type-icon row.
    MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW3 = 0x201, // fourth type-icon row.
    MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW2 = 0x202, // middle type-icon row.
    MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW1 = 0x203, // second type-icon row.
    MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW0 = 0x204, // top type-icon row.
    MENU_POKEMON_STATUS_ITEM_WAZA_CURSOR = 0x205,    // move-selection cursors.
};

// Ribbon page sprites the ribbon hook draws. The count sprite prints how many
// ribbons the pokemon wears; the description sprite prints the name of the
// ribbon the grid cursor stands on.
enum MENU_POKEMON_STATUS_RIBBON_ITEM {
    MENU_POKEMON_STATUS_ITEM_RIBBON_DESCRIPTION = 0x25A, // selected-ribbon description.
    MENU_POKEMON_STATUS_ITEM_RIBBON_COUNT = 0x25B,       // ribbon count.
};

// Trainer identities the origin checks recognize. Each pair is the trainer id
// recorded on the pokemon and the trainer-name message it is compared with.
enum MENU_POKEMON_STATUS_TRAINER_IDENTITY {
    MENU_POKEMON_STATUS_DANNY_TRAINER_ID = 0xA1B8,       // trainer id of Danny.
    MENU_POKEMON_STATUS_DANNY_TRAINER_NAME_MSG = 0x1828, // trainer-name message of Danny.
    MENU_POKEMON_STATUS_GINZARU_TRAINER_ID = 0x911D,     // trainer id of Ginzaru.
    MENU_POKEMON_STATUS_GINZARU_TRAINER_NAME_MSG = 0x17B9, // trainer-name message of Ginzaru.
};

// Status menu modes passed to menuPokemonStatusOpen.
enum {
    MENU_POKEMON_STATUS_FIGHT_MENU = 0x58,
    MENU_POKEMON_STATUS_WAZA_LEARN = 0x59,
    MENU_POKEMON_STATUS_WAZA_LEARN_LIST = 0x19,
    MENU_POKEMON_STATUS_FROM_POKEMON_CHANGE = 0x0C,
    MENU_POKEMON_STATUS_FROM_PC_BOX = 0x0E,
    MENU_POKEMON_STATUS_WAZA_LIST = 0x11,
    MENU_POKEMON_STATUS_FIELD_MENU = 0x8E,
    MENU_POKEMON_STATUS_FIGHT_LIST = 0xAC,
};

// Callback the status menu installs to follow the cursor to another pokemon.
typedef Pokemon *(*MENU_POKEMON_CB)(Pokemon *, MENU_POKEMON_CB_TYPE, long);

extern "C" {

void menuPokemonStatus(void);
void menuPokemonStatusClose(void);
// Refresh the party status table after an item changes a Pokemon.
void menuPokemonUpdate(void);
s32 menuPokemonStatusCtrl(tagWINDOW_WORK *window);
void menuPokemonStatusCursor(tagWINDOW_WORK *window);
void menuPokemonStatusCtrlRibbon(void);
long menuPokemonStatusOpenWazaList(u8 slot);
long menuPokemonStatusOpenWazaMachine(Pokemon *pokemon, u16 arg);
long menuPokemonStatusOpenWazaLearnList(Pokemon *pokemon, u16 arg);
long menuPokemonStatusOpenWazaLearn(Pokemon *pokemon, u16 arg);
long menuPokemonStatusOpenFromPokemonChange(Pokemon *pokemon, MENU_POKEMON_CB callback, long arg);
long menuPokemonStatusOpenFromPcBox(Pokemon *pokemon, MENU_POKEMON_CB callback, long arg);
long menuPokemonStatusOpenFightMenu(FIGHT_TRAINER *trainer, Pokemon *pokemon, MENU_POKEMON_CB callback, long arg);
long menuPokemonStatusOpenFieldMenu(Pokemon *pokemon, MENU_POKEMON_CB callback, long arg);

long menuPokemonStatusExecute(u8 flags, FIGHT_TRAINER *trainer, Pokemon *pokemon, u16 wazaId,
                 MENU_POKEMON_CB callback, long callbackArg);

void menuPokemonStatusDrawRibbon(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
bool menuPokemonStatusCheckWazaPage(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuPokemonStatusDispNoOp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
u16 menuPokemonStatusGetWazaTypeIcon(u16 type);
void menuPokemonStatusDispNature(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuPokemonStatusCtrlPage(tagWINDOW_WORK *window);
void menuPokemonStatusDispWaza(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuPokemonStatusDispSummary(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);

} // extern "C"

// Helpers of the status menu.
bool _menuPokemonStatusCheckWazaItem(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long NotDarkWaza(long waza);
u32 _menuPokemonStatusGetWazaDataID(Pokemon *pokemon, u16 slot);
u8 _menuPokemonStatusCheckDanny(Pokemon *pokemon);
u8 _menuPokemonStatusCheckGinzaru(Pokemon *pokemon);
u8 _menuPokemonStatusCheckOya(Pokemon *pokemon);
void _menuPokemonStatusMakeRibbon(Pokemon *pokemon, RIBBON_WORK *work);
long _menuPokemonStatusChangeSubMenu(CURSOR cursor);
long _menuPokemonStatusOpen(u8 type, FIGHT_TRAINER *trainer, Pokemon *pokemon, u16 wazaId,
                            MENU_POKEMON_CB callback, long arg);

#endif
