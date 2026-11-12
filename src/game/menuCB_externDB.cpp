#include <gbaCommand.h>
#include <menu.h>
#include <menuCBCommon.h>
#include <menuCB_externDB.h>
#include <menuCB_Sub1.h>
#include <menuCBBios.h>
#include <menuCBBattleStart.h>
#include <menuCBPokemonEntry.h>
#include <menuGBA.h>
#include <menuIds.h>
#include <menuItemBios.h>
#include <menuSpriteBios.h>
#include <menuSub.h>
#include <pause.h>
#include <toolentry.h>
#include <winSprite.h>
#include <window.h>

struct FIELD_ANIME_PANEL {
    s16 mX;
    s16 mY;
    s16 mWidth;
    s16 mHeight;
    u32 mColor;
    u16 mSpriteID;
    s16 mSpriteX;
    s16 mSpriteY;
    u32 mFlags;
};

// Slide work of the tunaide disp callbacks: the panel the field moved to, and
// the one it is pushing out of the window.
struct FIELD_ANIME_WORK {
    FIELD_ANIME_PANEL mPanel;
    FIELD_ANIME_PANEL mOutgoing;
};

FIELD_ANIME_WORK field_anime;


enum FieldSlideState {
    FIELD_SLIDE_IDLE = 0,
    FIELD_SLIDE_FROM_TOP_START = 1,
    FIELD_SLIDE_FROM_TOP = 2,
    FIELD_SLIDE_FROM_BOTTOM_START = 3,
    FIELD_SLIDE_FROM_BOTTOM = 4
};

enum {
    TUNAIDE_RULE_SELECT_LAST = 1
};

enum {
    TUNAIDE_FIELD_FIRST = 0,
    TUNAIDE_FIELD_LAST = 6
};

enum {
    TUNAIDE_FIELD_ITEM = 675,
    TUNAIDE_FIELD_OUTGOING_ITEM = 728,
    TUNAIDE_FIELD_INCOMING_ITEM = 729,
    TUNAIDE_FIELD_NEXT_ARROW_ITEM = 673,
    TUNAIDE_FIELD_PREVIOUS_ARROW_ITEM = 674,
    TUNAIDE_CURSOR_ITEM_FIRST = 1005,
    TUNAIDE_CURSOR_ITEM_SECOND = 1006,
    TUNAIDE_CURSOR_ITEM_THIRD = 1007
};


enum BattleBeforeAfterItem {
    BA_AFTER_FACE_TRAINER_0_SLOT_0 = 255,
    BA_AFTER_FACE_TRAINER_0_SLOT_1 = 256,
    BA_AFTER_FACE_TRAINER_0_SLOT_2 = 257,
    BA_AFTER_FACE_TRAINER_0_SLOT_3 = 258,
    BA_AFTER_FACE_TRAINER_0_SLOT_4 = 259,
    BA_AFTER_FACE_TRAINER_0_SLOT_5 = 260,
    BA_AFTER_FACE_TRAINER_1_SLOT_0 = 692,
    BA_AFTER_FACE_TRAINER_1_SLOT_1 = 693,
    BA_AFTER_FACE_TRAINER_1_SLOT_2 = 694,
    BA_AFTER_FACE_TRAINER_1_SLOT_3 = 695,
    BA_AFTER_FACE_TRAINER_1_SLOT_4 = 696,
    BA_AFTER_FACE_TRAINER_1_SLOT_5 = 697,
    BA_AFTER_FACE_TRAINER_2_SLOT_0 = 698,
    BA_AFTER_FACE_TRAINER_2_SLOT_1 = 699,
    BA_AFTER_FACE_TRAINER_2_SLOT_2 = 700,
    BA_AFTER_FACE_TRAINER_2_SLOT_3 = 701,
    BA_AFTER_FACE_TRAINER_2_SLOT_4 = 702,
    BA_AFTER_FACE_TRAINER_2_SLOT_5 = 703,
    BA_AFTER_FACE_TRAINER_3_SLOT_0 = 704,
    BA_AFTER_FACE_TRAINER_3_SLOT_1 = 705,
    BA_AFTER_FACE_TRAINER_3_SLOT_2 = 706,
    BA_AFTER_FACE_TRAINER_3_SLOT_3 = 707,
    BA_AFTER_FACE_TRAINER_3_SLOT_4 = 708,
    BA_AFTER_FACE_TRAINER_3_SLOT_5 = 709,
    BA_BEFORE_FACE_TRAINER_0_SLOT_0 = 686,
    BA_BEFORE_FACE_TRAINER_0_SLOT_1 = 687,
    BA_BEFORE_FACE_TRAINER_0_SLOT_2 = 688,
    BA_BEFORE_FACE_TRAINER_0_SLOT_3 = 689,
    BA_BEFORE_FACE_TRAINER_0_SLOT_4 = 690,
    BA_BEFORE_FACE_TRAINER_0_SLOT_5 = 691,
    BA_BEFORE_FACE_TRAINER_1_SLOT_0 = 267,
    BA_BEFORE_FACE_TRAINER_1_SLOT_1 = 268,
    BA_BEFORE_FACE_TRAINER_1_SLOT_2 = 269,
    BA_BEFORE_FACE_TRAINER_1_SLOT_3 = 270,
    BA_BEFORE_FACE_TRAINER_1_SLOT_4 = 271,
    BA_BEFORE_FACE_TRAINER_1_SLOT_5 = 272,
    BA_BEFORE_FACE_TRAINER_2_SLOT_0 = 279,
    BA_BEFORE_FACE_TRAINER_2_SLOT_1 = 280,
    BA_BEFORE_FACE_TRAINER_2_SLOT_2 = 281,
    BA_BEFORE_FACE_TRAINER_2_SLOT_3 = 282,
    BA_BEFORE_FACE_TRAINER_2_SLOT_4 = 283,
    BA_BEFORE_FACE_TRAINER_2_SLOT_5 = 284,
    BA_BEFORE_FACE_TRAINER_3_SLOT_0 = 291,
    BA_BEFORE_FACE_TRAINER_3_SLOT_1 = 292,
    BA_BEFORE_FACE_TRAINER_3_SLOT_2 = 293,
    BA_BEFORE_FACE_TRAINER_3_SLOT_3 = 294,
    BA_BEFORE_FACE_TRAINER_3_SLOT_4 = 295,
    BA_BEFORE_FACE_TRAINER_3_SLOT_5 = 296,
    BA_PLAYER_NAME_TRAINER_3 = 334,
    BA_PLAYER_NAME_TRAINER_2 = 333,
    BA_PLAYER_NAME_TRAINER_1 = 332,
    BA_PLAYER_NAME_TRAINER_0 = 331,
    BA_FAINT_MARK_TRAINER_3_SLOT_0 = 303,
    BA_FAINT_MARK_TRAINER_3_SLOT_1 = 304,
    BA_FAINT_MARK_TRAINER_3_SLOT_2 = 305,
    BA_FAINT_MARK_TRAINER_3_SLOT_3 = 306,
    BA_FAINT_MARK_TRAINER_3_SLOT_4 = 307,
    BA_FAINT_MARK_TRAINER_3_SLOT_5 = 308,
    BA_FAINT_MARK_TRAINER_2_SLOT_0 = 309,
    BA_FAINT_MARK_TRAINER_2_SLOT_1 = 310,
    BA_FAINT_MARK_TRAINER_2_SLOT_2 = 311,
    BA_FAINT_MARK_TRAINER_2_SLOT_3 = 312,
    BA_FAINT_MARK_TRAINER_2_SLOT_4 = 313,
    BA_FAINT_MARK_TRAINER_2_SLOT_5 = 314,
    BA_FAINT_MARK_TRAINER_1_SLOT_0 = 315,
    BA_FAINT_MARK_TRAINER_1_SLOT_1 = 316,
    BA_FAINT_MARK_TRAINER_1_SLOT_2 = 317,
    BA_FAINT_MARK_TRAINER_1_SLOT_3 = 318,
    BA_FAINT_MARK_TRAINER_1_SLOT_4 = 319,
    BA_FAINT_MARK_TRAINER_1_SLOT_5 = 320,
    BA_FAINT_MARK_TRAINER_0_SLOT_0 = 321,
    BA_FAINT_MARK_TRAINER_0_SLOT_1 = 322,
    BA_FAINT_MARK_TRAINER_0_SLOT_2 = 323,
    BA_FAINT_MARK_TRAINER_0_SLOT_3 = 324,
    BA_FAINT_MARK_TRAINER_0_SLOT_4 = 325,
    BA_FAINT_MARK_TRAINER_0_SLOT_5 = 326,
    BA_TRAINER_FACE_TRAINER_3 = 328,
    BA_TRAINER_FACE_TRAINER_2 = 327,
    BA_TRAINER_FACE_TRAINER_1 = 329,
    BA_TRAINER_FACE_TRAINER_0 = 330,
    BA_FACE_PANEL_TRAINER_0_SLOT_0 = 261,
    BA_FACE_PANEL_TRAINER_0_SLOT_1 = 262,
    BA_FACE_PANEL_TRAINER_0_SLOT_2 = 263,
    BA_FACE_PANEL_TRAINER_0_SLOT_3 = 264,
    BA_FACE_PANEL_TRAINER_0_SLOT_4 = 265,
    BA_FACE_PANEL_TRAINER_0_SLOT_5 = 266,
    BA_FACE_PANEL_TRAINER_1_SLOT_0 = 273,
    BA_FACE_PANEL_TRAINER_1_SLOT_1 = 274,
    BA_FACE_PANEL_TRAINER_1_SLOT_2 = 275,
    BA_FACE_PANEL_TRAINER_1_SLOT_3 = 276,
    BA_FACE_PANEL_TRAINER_1_SLOT_4 = 277,
    BA_FACE_PANEL_TRAINER_1_SLOT_5 = 278,
    BA_FACE_PANEL_TRAINER_2_SLOT_0 = 285,
    BA_FACE_PANEL_TRAINER_2_SLOT_1 = 286,
    BA_FACE_PANEL_TRAINER_2_SLOT_2 = 287,
    BA_FACE_PANEL_TRAINER_2_SLOT_3 = 288,
    BA_FACE_PANEL_TRAINER_2_SLOT_4 = 289,
    BA_FACE_PANEL_TRAINER_2_SLOT_5 = 290,
    BA_FACE_PANEL_TRAINER_3_SLOT_0 = 297,
    BA_FACE_PANEL_TRAINER_3_SLOT_1 = 298,
    BA_FACE_PANEL_TRAINER_3_SLOT_2 = 299,
    BA_FACE_PANEL_TRAINER_3_SLOT_3 = 300,
    BA_FACE_PANEL_TRAINER_3_SLOT_4 = 301,
    BA_FACE_PANEL_TRAINER_3_SLOT_5 = 302,
    BA_HP_BAR_TRAINER_0_SLOT_0 = 468,
    BA_HP_BAR_TRAINER_0_SLOT_1 = 469,
    BA_HP_BAR_TRAINER_0_SLOT_2 = 470,
    BA_HP_BAR_TRAINER_0_SLOT_3 = 471,
    BA_HP_BAR_TRAINER_0_SLOT_4 = 472,
    BA_HP_BAR_TRAINER_0_SLOT_5 = 473,
    BA_HP_BAR_TRAINER_1_SLOT_0 = 474,
    BA_HP_BAR_TRAINER_1_SLOT_1 = 475,
    BA_HP_BAR_TRAINER_1_SLOT_2 = 476,
    BA_HP_BAR_TRAINER_1_SLOT_3 = 477,
    BA_HP_BAR_TRAINER_1_SLOT_4 = 478,
    BA_HP_BAR_TRAINER_1_SLOT_5 = 479,
    BA_HP_BAR_TRAINER_2_SLOT_0 = 480,
    BA_HP_BAR_TRAINER_2_SLOT_1 = 481,
    BA_HP_BAR_TRAINER_2_SLOT_2 = 482,
    BA_HP_BAR_TRAINER_2_SLOT_3 = 483,
    BA_HP_BAR_TRAINER_2_SLOT_4 = 484,
    BA_HP_BAR_TRAINER_2_SLOT_5 = 485,
    BA_HP_BAR_TRAINER_3_SLOT_0 = 486,
    BA_HP_BAR_TRAINER_3_SLOT_1 = 487,
    BA_HP_BAR_TRAINER_3_SLOT_2 = 488,
    BA_HP_BAR_TRAINER_3_SLOT_3 = 489,
    BA_HP_BAR_TRAINER_3_SLOT_4 = 490,
    BA_HP_BAR_TRAINER_3_SLOT_5 = 491,
    BA_XP_TRAINER_0 = 682,
    BA_XP_TRAINER_1 = 683,
    BA_XP_TRAINER_2 = 684,
    BA_XP_TRAINER_3 = 685,
};

enum GBAConnectMenuItem {
    GBACONNECT_PORT1_FRAME = 222,
    GBACONNECT_PORT3_FRAME = 224,
    GBACONNECT_PORT4_FRAME = 225,
    GBACONNECT_PORT1_LABEL = 226,
    GBACONNECT_PORT3_LABEL = 228,
    GBACONNECT_PORT4_LABEL = 229,
    GBACONNECT_PORT1_ICON = 230,
    GBACONNECT_PORT2_ICON = 231,
    GBACONNECT_PORT3_ICON = 232,
    GBACONNECT_PORT4_ICON = 233,
    GBACONNECT_PORT1_CABLE = 234,
    GBACONNECT_PORT3_CABLE = 236,
    GBACONNECT_PORT4_CABLE = 237,
    GBACONNECT_SAVE_0 = 238,
    GBACONNECT_SAVE_1 = 239,
    GBACONNECT_SAVE_2 = 240,
    GBACONNECT_PORT1_OK = 241,
    GBACONNECT_PORT2_OK = 242,
    GBACONNECT_PORT3_OK = 243,
    GBACONNECT_PORT4_OK = 244,
    GBACONNECT_PORT1_NG = 245,
    GBACONNECT_PORT2_NG = 246,
    GBACONNECT_PORT3_NG = 247,
    GBACONNECT_PORT4_NG = 248,
    GBACONNECT_MESSAGE_0 = 251,
    GBACONNECT_MESSAGE_1 = 252,
    GBACONNECT_MESSAGE_2 = 253
};

enum {
    MSG_GBACONNECT_0 = 17436,
    MSG_GBACONNECT_1 = 17437,
    MSG_GBA_CONNECT_GUIDE = 17246
};

enum Tunaide03SpriteItemID {
    ITEM_TRAINER_FACE_0_A = 1531,
    ITEM_TRAINER_FACE_1_A = 1532,
    ITEM_TRAINER_FACE_2_A = 1540,
    ITEM_TRAINER_FACE_3_A = 1541,
    ITEM_TRAINER_FACE_0_B = 1549,
    ITEM_TRAINER_FACE_2_B = 1550,
    ITEM_TRAINER_FACE_1_B = 1558,
    ITEM_TRAINER_FACE_3_B = 1559,
    ITEM_TRAINER_FACE_0_C = 1567,
    ITEM_TRAINER_FACE_3_C = 1568,
    ITEM_TRAINER_FACE_1_C = 1576,
    ITEM_TRAINER_FACE_2_C = 1577,
    ITEM_TEAM_LAYOUT_0_A = 1581,
    ITEM_TEAM_LAYOUT_0_B = 1582,
    ITEM_TEAM_LAYOUT_0_C = 1583,
    ITEM_TEAM_LAYOUT_0_D = 1584,
    ITEM_TEAM_LAYOUT_1_A = 1585,
    ITEM_TEAM_LAYOUT_1_B = 1586,
    ITEM_TEAM_LAYOUT_1_C = 1587,
    ITEM_TEAM_LAYOUT_1_D = 1588,
    ITEM_TEAM_LAYOUT_2_A = 1589,
    ITEM_TEAM_LAYOUT_2_B = 1590,
    ITEM_TEAM_LAYOUT_2_C = 1591,
    ITEM_TEAM_LAYOUT_2_D = 1592,
    ITEM_TEAM_LAYOUT_3 = 1593,
    ITEM_TRAINER_NAME_0_A = 1598,
    ITEM_TRAINER_NAME_1_A = 1599,
    ITEM_TRAINER_NAME_2_A = 1600,
    ITEM_TRAINER_NAME_3_A = 1601,
    ITEM_TRAINER_NAME_0_B = 1602,
    ITEM_TRAINER_NAME_2_B = 1603,
    ITEM_TRAINER_NAME_1_B = 1604,
    ITEM_TRAINER_NAME_3_B = 1605,
    ITEM_TRAINER_NAME_0_C = 1606,
    ITEM_TRAINER_NAME_3_C = 1607,
    ITEM_TRAINER_NAME_1_C = 1608,
    ITEM_TRAINER_NAME_2_C = 1609,
    ITEM_TEAM_GUIDE = 1597,
    ITEM_MESSAGE = 1610
};

enum {
    TUNAIDE03_GUIDE_MESSAGE_ID = 17422
};

enum RuleMenuItem {
    RULE_SECOND_OPTION_CHECK_0 = 987,
    RULE_SECOND_OPTION_CHECK_1 = 988,
    RULE_SECOND_OPTION_CHECK_2 = 989,
    RULE_SECOND_OPTION_CHECK_3 = 990,
    RULE_SECOND_OPTION_CHECK_4 = 991,
    RULE_SECOND_OPTION_CHECK_5 = 992,
    RULE_FIRST_OPTION_CHECK_0 = 993,
    RULE_FIRST_OPTION_CHECK_1 = 994,
    RULE_FIRST_OPTION_CHECK_2 = 995,
    RULE_FIRST_OPTION_CHECK_3 = 996,
    RULE_FIRST_OPTION_CHECK_4 = 997,
    RULE_FIRST_OPTION_CHECK_5 = 998,
    RULE_TRAINER_NAME_0 = 999,
    RULE_TRAINER_NAME_1 = 1000,
    RULE_TRAINER_NAME_2 = 1001,
    RULE_TRAINER_NAME_3 = 1002,
    RULE_TRAINER_NAME_4 = 1003,
    RULE_TRAINER_NAME_5 = 1004
};

enum { RULE_SECOND_OPTION = 1, RULE_SUBMENU_CURSOR = 3 };

enum {
    MENU_ID_CB_ERROR01 = 167, // per-slot error detail window opened from error00
};

enum {
    MSG_CB_ERROR00_TITLE = 17345,
    MSG_CB_ERROR00_GUIDE = 17346,
    MSG_CB_ERROR01_TITLE = 17347,
};

enum { ERROR00_CURSOR_OK = 6 };

enum { ERROR01_COLUMN_ROWS = 7, ERROR01_ITEM_COUNT = 14 };

enum Error00Item {
    ERROR00_ITEM_FACE_0 = 4,
    ERROR00_ITEM_PANEL_0 = 5,
    ERROR00_ITEM_FACE_1 = 6,
    ERROR00_ITEM_PANEL_1 = 7,
    ERROR00_ITEM_FACE_2 = 8,
    ERROR00_ITEM_PANEL_2 = 9,
    ERROR00_ITEM_FACE_3 = 10,
    ERROR00_ITEM_PANEL_3 = 11,
    ERROR00_ITEM_FACE_4 = 12,
    ERROR00_ITEM_PANEL_4 = 13,
    ERROR00_ITEM_FACE_5 = 14,
    ERROR00_ITEM_PANEL_5 = 15,
    ERROR00_ITEM_OK_BUTTON = 16,
    ERROR00_ITEM_OK_CURSOR = 17,
    ERROR00_ITEM_CURSOR_0_A = 18,
    ERROR00_ITEM_CURSOR_0_B = 19,
    ERROR00_ITEM_CURSOR_0_C = 20,
    ERROR00_ITEM_CURSOR_1_A = 21,
    ERROR00_ITEM_CURSOR_1_B = 22,
    ERROR00_ITEM_CURSOR_1_C = 23,
    ERROR00_ITEM_CURSOR_2_A = 24,
    ERROR00_ITEM_CURSOR_2_B = 25,
    ERROR00_ITEM_CURSOR_2_C = 26,
    ERROR00_ITEM_CURSOR_3_A = 27,
    ERROR00_ITEM_CURSOR_3_B = 28,
    ERROR00_ITEM_CURSOR_3_C = 29,
    ERROR00_ITEM_CURSOR_4_A = 30,
    ERROR00_ITEM_CURSOR_4_B = 31,
    ERROR00_ITEM_CURSOR_4_C = 32,
    ERROR00_ITEM_CURSOR_5_A = 33,
    ERROR00_ITEM_CURSOR_5_B = 34,
    ERROR00_ITEM_CURSOR_5_C = 35,
    ERROR00_ITEM_HELD_ITEM_0 = 36,
    ERROR00_ITEM_GUIDE = 37,
    ERROR00_ITEM_HELD_ITEM_1 = 38,
    ERROR00_ITEM_HELD_ITEM_2 = 39,
    ERROR00_ITEM_HELD_ITEM_3 = 40,
    ERROR00_ITEM_HELD_ITEM_5 = 41,
    ERROR00_ITEM_LEVEL_0 = 42,
    ERROR00_ITEM_NAME_0 = 43,
    ERROR00_ITEM_LEVEL_1 = 44,
    ERROR00_ITEM_NAME_1 = 45,
    ERROR00_ITEM_LEVEL_2 = 46,
    ERROR00_ITEM_NAME_2 = 47,
    ERROR00_ITEM_LEVEL_3 = 48,
    ERROR00_ITEM_NAME_3 = 49,
    ERROR00_ITEM_HELD_ITEM_4 = 50,
    ERROR00_ITEM_LEVEL_4 = 51,
    ERROR00_ITEM_NAME_4 = 52,
    ERROR00_ITEM_LEVEL_5 = 53,
    ERROR00_ITEM_NAME_5 = 54,
    ERROR00_ITEM_TITLE = 55,
    ERROR00_ITEM_MARU_WAKU_0 = 56,
    ERROR00_ITEM_MARU_WAKU_1 = 203,
    ERROR00_ITEM_MARU_WAKU_2 = 204,
    ERROR00_ITEM_MARU_WAKU_3 = 205,
    ERROR00_ITEM_MARU_WAKU_4 = 206,
    ERROR00_ITEM_MARU_WAKU_5 = 207,
    ERROR00_ITEM_HIDDEN_0 = 732,
    ERROR00_ITEM_HIDDEN_1 = 733,
    ERROR00_ITEM_HIDDEN_2 = 734,
    ERROR00_ITEM_HIDDEN_3 = 735,
    ERROR00_ITEM_HIDDEN_4 = 736,
    ERROR00_ITEM_HIDDEN_5 = 737,
};

enum Error01Item {
    ERROR01_ITEM_CURSOR = 1244,
    ERROR01_ITEM_RIGHT_COLUMN = 1259,
    ERROR01_ITEM_LEFT_COLUMN = 1260,
    ERROR01_ITEM_TITLE = 1261,
};

enum Tunaide02Item {
    TUNAIDE02_SLOT0_FRAME_0 = 138,
    TUNAIDE02_SLOT0_FRAME_1 = 139,
    TUNAIDE02_SLOT0_FRAME_2 = 140,
    TUNAIDE02_SLOT0_FRAME_3 = 141,
    TUNAIDE02_SLOT0_FRAME_4 = 142,
    TUNAIDE02_SLOT2_FRAME_0 = 148,
    TUNAIDE02_SLOT2_FRAME_1 = 149,
    TUNAIDE02_SLOT2_FRAME_2 = 150,
    TUNAIDE02_SLOT2_FRAME_3 = 151,
    TUNAIDE02_SLOT2_FRAME_4 = 152,
    TUNAIDE02_SLOT3_FRAME_0 = 153,
    TUNAIDE02_SLOT3_FRAME_1 = 154,
    TUNAIDE02_SLOT3_FRAME_2 = 155,
    TUNAIDE02_SLOT3_FRAME_3 = 156,
    TUNAIDE02_SLOT3_FRAME_4 = 157,
    TUNAIDE02_SLOT0_FACE = 158,
    TUNAIDE02_SLOT1_FACE = 159,
    TUNAIDE02_SLOT2_FACE = 160,
    TUNAIDE02_SLOT3_FACE = 161,
    TUNAIDE02_SLOT0_PLATE = 162,
    TUNAIDE02_SLOT2_FRAME_5 = 164,
    TUNAIDE02_SLOT3_FRAME_5 = 165,
    TUNAIDE02_SLOT0_WAIT_0 = 166,
    TUNAIDE02_SLOT0_WAIT_1 = 167,
    TUNAIDE02_SLOT1_WAIT_0 = 168,
    TUNAIDE02_SLOT1_WAIT_1 = 169,
    TUNAIDE02_SLOT2_WAIT_0 = 170,
    TUNAIDE02_SLOT2_WAIT_1 = 171,
    TUNAIDE02_SLOT3_WAIT_0 = 172,
    TUNAIDE02_SLOT3_WAIT_1 = 173,
    TUNAIDE02_SLOT0_FRAME_5 = 174,
    TUNAIDE02_SLOT1_MARK = 175,
    TUNAIDE02_SLOT2_MARK = 176,
    TUNAIDE02_SLOT3_FRAME_6 = 177,
    TUNAIDE02_SLOT0_PROMPT = 178,
    TUNAIDE02_SLOT1_PROMPT = 179,
    TUNAIDE02_SLOT2_PROMPT = 180,
    TUNAIDE02_SLOT3_PROMPT = 181,
    TUNAIDE02_SLOT0_PROMPT_SHADOW = 182,
    TUNAIDE02_SLOT1_PROMPT_SHADOW = 183,
    TUNAIDE02_SLOT2_PROMPT_SHADOW = 184,
    TUNAIDE02_SLOT3_PROMPT_SHADOW = 185,
    TUNAIDE02_SLOT3_HERO_ID = 186,
    TUNAIDE02_SLOT2_HERO_ID = 187,
    TUNAIDE02_SLOT1_HERO_ID = 188,
    TUNAIDE02_SLOT0_HERO_ID = 189,
    TUNAIDE02_SLOT3_HERO_NAME = 190,
    TUNAIDE02_SLOT2_HERO_NAME = 191,
    TUNAIDE02_SLOT1_HERO_NAME = 192,
    TUNAIDE02_SLOT0_HERO_NAME = 193,
    TUNAIDE02_SLOT0_PANEL = 194,
    TUNAIDE02_SLOT1_PANEL = 195,
    TUNAIDE02_SLOT2_PANEL = 196,
    TUNAIDE02_SLOT3_PANEL = 197,
    TUNAIDE02_GUIDE = 198,
    TUNAIDE02_SLOT0_READY = 199,
    TUNAIDE02_SLOT1_READY = 200,
    TUNAIDE02_SLOT2_READY = 201,
    TUNAIDE02_SLOT3_READY = 202,
    TUNAIDE02_SLOT0_EMPTY_0 = 208,
    TUNAIDE02_SLOT0_EMPTY_1 = 209,
    TUNAIDE02_SLOT0_EMPTY_2 = 210,
    TUNAIDE02_SLOT0_START = 730,
    TUNAIDE02_SLOT1_START = 731
};

enum {
    TUNAIDE02_SLOT0_BIT = 1,
    TUNAIDE02_SLOT1_BIT = 2,
    TUNAIDE02_SLOT2_BIT = 4,
    TUNAIDE02_SLOT3_BIT = 8,
    TUNAIDE02_SLOT_ALL = 15
};

enum {
    TUNAIDE02_MSG_PROMPT_SHADOW_WAIT = 17349,
    TUNAIDE02_MSG_PROMPT_WAIT = 17350,
    TUNAIDE02_MSG_PROMPT_SHADOW_START = 17351,
    TUNAIDE02_MSG_PROMPT_START = 17352,
    TUNAIDE02_MSG_GUIDE_WAIT = 17353,
    TUNAIDE02_MSG_GUIDE_START = 17354,
    TUNAIDE02_MSG_SLOT0_PANEL = 17418,
    TUNAIDE02_MSG_SLOT1_PANEL = 17419,
    TUNAIDE02_MSG_SLOT2_PANEL = 17420,
    TUNAIDE02_MSG_SLOT3_PANEL = 17421
};

enum { TUNAIDE02_BATTLE_MODE_MULTI = 2 };

enum { TUNAIDE02_KEY_START = 0x800 };

extern "C" const u32 tunaideSlotMarkSpriteIDs[] = {527, 526, 525, 524, 523};
static const u32 trainerFaceSpriteIDs[] = {520, 521, 522, 528, 529, 530, 531};
extern const u32 tunaide02_face_sprite[7] = {
    513, 514, 515, 516, 517, 518, 519
};
extern const u32 tunaide_gc_entry_sprite[8] = {
    805, 806, 807, 808, 809, 810, 811, 812
};

extern const u32 field_sprite[7] = {
    679, 681, 680, 678, 677, 676, 727
};
extern "C" const s32 error01MenuItemIds[ERROR01_ITEM_COUNT];
static const u32 tunaide02PlateSpriteIDs[] = {968, 969};
extern "C" const u32 tunaideBattleTypeSummaryMessages[] = {17265, 17266};
extern "C" const u32 tunaideBattleTypeMessages[] = {17263, 17264};
static s16 gbaIconShake[4] = {-1, 0, 1, 0};

// Player slot (1-based) allowed to start once all players have confirmed.
s32 tunaide02_last_kakunin_flag;

extern "C" void menuCB_BattleBeforeAfter3_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 itemID = sprite->mItemID;

    switch (itemID) {
    case BA_AFTER_FACE_TRAINER_0_SLOT_0:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 0, 0);
        break;

    case BA_AFTER_FACE_TRAINER_0_SLOT_1:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 0, 1);
        break;

    case BA_AFTER_FACE_TRAINER_0_SLOT_2:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 0, 2);
        break;

    case BA_AFTER_FACE_TRAINER_0_SLOT_3:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 0, 3);
        break;

    case BA_AFTER_FACE_TRAINER_0_SLOT_4:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 0, 4);
        break;

    case BA_AFTER_FACE_TRAINER_0_SLOT_5:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 0, 5);
        break;

    case BA_AFTER_FACE_TRAINER_1_SLOT_0:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 1, 0);
        break;

    case BA_AFTER_FACE_TRAINER_1_SLOT_1:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 1, 1);
        break;

    case BA_AFTER_FACE_TRAINER_1_SLOT_2:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 1, 2);
        break;

    case BA_AFTER_FACE_TRAINER_1_SLOT_3:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 1, 3);
        break;

    case BA_AFTER_FACE_TRAINER_1_SLOT_4:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 1, 4);
        break;

    case BA_AFTER_FACE_TRAINER_1_SLOT_5:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 1, 5);
        break;

    case BA_AFTER_FACE_TRAINER_2_SLOT_0:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 2, 0);
        break;

    case BA_AFTER_FACE_TRAINER_2_SLOT_1:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 2, 1);
        break;

    case BA_AFTER_FACE_TRAINER_2_SLOT_2:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 2, 2);
        break;

    case BA_AFTER_FACE_TRAINER_2_SLOT_3:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 2, 3);
        break;

    case BA_AFTER_FACE_TRAINER_2_SLOT_4:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 2, 4);
        break;

    case BA_AFTER_FACE_TRAINER_2_SLOT_5:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 2, 5);
        break;

    case BA_AFTER_FACE_TRAINER_3_SLOT_0:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 3, 0);
        break;

    case BA_AFTER_FACE_TRAINER_3_SLOT_1:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 3, 1);
        break;

    case BA_AFTER_FACE_TRAINER_3_SLOT_2:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 3, 2);
        break;

    case BA_AFTER_FACE_TRAINER_3_SLOT_3:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 3, 3);
        break;

    case BA_AFTER_FACE_TRAINER_3_SLOT_4:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 3, 4);
        break;

    case BA_AFTER_FACE_TRAINER_3_SLOT_5:
        menuCBPokemonEntryDispPokemonFace_AFTER(window, sprite, 3, 5);
        break;

    case BA_BEFORE_FACE_TRAINER_0_SLOT_0:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 0, 0);
        break;

    case BA_BEFORE_FACE_TRAINER_0_SLOT_1:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 0, 1);
        break;

    case BA_BEFORE_FACE_TRAINER_0_SLOT_2:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 0, 2);
        break;

    case BA_BEFORE_FACE_TRAINER_0_SLOT_3:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 0, 3);
        break;

    case BA_BEFORE_FACE_TRAINER_0_SLOT_4:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 0, 4);
        break;

    case BA_BEFORE_FACE_TRAINER_0_SLOT_5:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 0, 5);
        break;

    case BA_BEFORE_FACE_TRAINER_1_SLOT_0:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 1, 0);
        break;

    case BA_BEFORE_FACE_TRAINER_1_SLOT_1:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 1, 1);
        break;

    case BA_BEFORE_FACE_TRAINER_1_SLOT_2:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 1, 2);
        break;

    case BA_BEFORE_FACE_TRAINER_1_SLOT_3:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 1, 3);
        break;

    case BA_BEFORE_FACE_TRAINER_1_SLOT_4:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 1, 4);
        break;

    case BA_BEFORE_FACE_TRAINER_1_SLOT_5:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 1, 5);
        break;

    case BA_BEFORE_FACE_TRAINER_2_SLOT_0:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 2, 0);
        break;

    case BA_BEFORE_FACE_TRAINER_2_SLOT_1:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 2, 1);
        break;

    case BA_BEFORE_FACE_TRAINER_2_SLOT_2:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 2, 2);
        break;

    case BA_BEFORE_FACE_TRAINER_2_SLOT_3:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 2, 3);
        break;

    case BA_BEFORE_FACE_TRAINER_2_SLOT_4:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 2, 4);
        break;

    case BA_BEFORE_FACE_TRAINER_2_SLOT_5:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 2, 5);
        break;

    case BA_BEFORE_FACE_TRAINER_3_SLOT_0:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 3, 0);
        break;

    case BA_BEFORE_FACE_TRAINER_3_SLOT_1:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 3, 1);
        break;

    case BA_BEFORE_FACE_TRAINER_3_SLOT_2:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 3, 2);
        break;

    case BA_BEFORE_FACE_TRAINER_3_SLOT_3:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 3, 3);
        break;

    case BA_BEFORE_FACE_TRAINER_3_SLOT_4:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 3, 4);
        break;

    case BA_BEFORE_FACE_TRAINER_3_SLOT_5:
        menuCBPokemonEntryDispPokemonFace_BEFORE(window, sprite, 3, 5);
        break;

    case BA_PLAYER_NAME_TRAINER_3:
        menuCBBattleHeroNameDraw(window, sprite, 3);
        break;

    case BA_PLAYER_NAME_TRAINER_2:
        menuCBBattleHeroNameDraw(window, sprite, 2);
        break;

    case BA_PLAYER_NAME_TRAINER_1:
        menuCBBattleHeroNameDraw(window, sprite, 1);
        break;

    case BA_PLAYER_NAME_TRAINER_0:
        menuCBBattleHeroNameDraw(window, sprite, 0);
        break;

    case BA_FAINT_MARK_TRAINER_3_SLOT_0:
        menuCBBattleBABatuMark(sprite, 3, 0);
        break;

    case BA_FAINT_MARK_TRAINER_3_SLOT_1:
        menuCBBattleBABatuMark(sprite, 3, 1);
        break;

    case BA_FAINT_MARK_TRAINER_3_SLOT_2:
        menuCBBattleBABatuMark(sprite, 3, 2);
        break;

    case BA_FAINT_MARK_TRAINER_3_SLOT_3:
        menuCBBattleBABatuMark(sprite, 3, 3);
        break;

    case BA_FAINT_MARK_TRAINER_3_SLOT_4:
        menuCBBattleBABatuMark(sprite, 3, 4);
        break;

    case BA_FAINT_MARK_TRAINER_3_SLOT_5:
        menuCBBattleBABatuMark(sprite, 3, 5);
        break;

    case BA_FAINT_MARK_TRAINER_2_SLOT_0:
        menuCBBattleBABatuMark(sprite, 2, 0);
        break;

    case BA_FAINT_MARK_TRAINER_2_SLOT_1:
        menuCBBattleBABatuMark(sprite, 2, 1);
        break;

    case BA_FAINT_MARK_TRAINER_2_SLOT_2:
        menuCBBattleBABatuMark(sprite, 2, 2);
        break;

    case BA_FAINT_MARK_TRAINER_2_SLOT_3:
        menuCBBattleBABatuMark(sprite, 2, 3);
        break;

    case BA_FAINT_MARK_TRAINER_2_SLOT_4:
        menuCBBattleBABatuMark(sprite, 2, 4);
        break;

    case BA_FAINT_MARK_TRAINER_2_SLOT_5:
        menuCBBattleBABatuMark(sprite, 2, 5);
        break;

    case BA_FAINT_MARK_TRAINER_1_SLOT_0:
        menuCBBattleBABatuMark(sprite, 1, 0);
        break;

    case BA_FAINT_MARK_TRAINER_1_SLOT_1:
        menuCBBattleBABatuMark(sprite, 1, 1);
        break;

    case BA_FAINT_MARK_TRAINER_1_SLOT_2:
        menuCBBattleBABatuMark(sprite, 1, 2);
        break;

    case BA_FAINT_MARK_TRAINER_1_SLOT_3:
        menuCBBattleBABatuMark(sprite, 1, 3);
        break;

    case BA_FAINT_MARK_TRAINER_1_SLOT_4:
        menuCBBattleBABatuMark(sprite, 1, 4);
        break;

    case BA_FAINT_MARK_TRAINER_1_SLOT_5:
        menuCBBattleBABatuMark(sprite, 1, 5);
        break;

    case BA_FAINT_MARK_TRAINER_0_SLOT_0:
        menuCBBattleBABatuMark(sprite, 0, 0);
        break;

    case BA_FAINT_MARK_TRAINER_0_SLOT_1:
        menuCBBattleBABatuMark(sprite, 0, 1);
        break;

    case BA_FAINT_MARK_TRAINER_0_SLOT_2:
        menuCBBattleBABatuMark(sprite, 0, 2);
        break;

    case BA_FAINT_MARK_TRAINER_0_SLOT_3:
        menuCBBattleBABatuMark(sprite, 0, 3);
        break;

    case BA_FAINT_MARK_TRAINER_0_SLOT_4:
        menuCBBattleBABatuMark(sprite, 0, 4);
        break;

    case BA_FAINT_MARK_TRAINER_0_SLOT_5:
        menuCBBattleBABatuMark(sprite, 0, 5);
        break;

    case BA_TRAINER_FACE_TRAINER_3:
        menuCBBattleStartDispTrainerFaceSub(window, sprite, 3);
        break;

    case BA_TRAINER_FACE_TRAINER_2:
        menuCBBattleStartDispTrainerFaceSub(window, sprite, 2);
        break;

    case BA_TRAINER_FACE_TRAINER_1:
        menuCBBattleStartDispTrainerFaceSub(window, sprite, 1);
        break;

    case BA_TRAINER_FACE_TRAINER_0:
        menuCBBattleStartDispTrainerFaceSub(window, sprite, 0);
        break;

    case BA_FACE_PANEL_TRAINER_0_SLOT_0:
        menuCB_BattleBA_FacePanelSet(sprite, 0, 0);
        break;

    case BA_FACE_PANEL_TRAINER_0_SLOT_1:
        menuCB_BattleBA_FacePanelSet(sprite, 0, 1);
        break;

    case BA_FACE_PANEL_TRAINER_0_SLOT_2:
        menuCB_BattleBA_FacePanelSet(sprite, 0, 2);
        break;

    case BA_FACE_PANEL_TRAINER_0_SLOT_3:
        menuCB_BattleBA_FacePanelSet(sprite, 0, 3);
        break;

    case BA_FACE_PANEL_TRAINER_0_SLOT_4:
        menuCB_BattleBA_FacePanelSet(sprite, 0, 4);
        break;

    case BA_FACE_PANEL_TRAINER_0_SLOT_5:
        menuCB_BattleBA_FacePanelSet(sprite, 0, 5);
        break;

    case BA_FACE_PANEL_TRAINER_1_SLOT_0:
        menuCB_BattleBA_FacePanelSet(sprite, 1, 0);
        break;

    case BA_FACE_PANEL_TRAINER_1_SLOT_1:
        menuCB_BattleBA_FacePanelSet(sprite, 1, 1);
        break;

    case BA_FACE_PANEL_TRAINER_1_SLOT_2:
        menuCB_BattleBA_FacePanelSet(sprite, 1, 2);
        break;

    case BA_FACE_PANEL_TRAINER_1_SLOT_3:
        menuCB_BattleBA_FacePanelSet(sprite, 1, 3);
        break;

    case BA_FACE_PANEL_TRAINER_1_SLOT_4:
        menuCB_BattleBA_FacePanelSet(sprite, 1, 4);
        break;

    case BA_FACE_PANEL_TRAINER_1_SLOT_5:
        menuCB_BattleBA_FacePanelSet(sprite, 1, 5);
        break;

    case BA_FACE_PANEL_TRAINER_2_SLOT_0:
        menuCB_BattleBA_FacePanelSet(sprite, 2, 0);
        break;

    case BA_FACE_PANEL_TRAINER_2_SLOT_1:
        menuCB_BattleBA_FacePanelSet(sprite, 2, 1);
        break;

    case BA_FACE_PANEL_TRAINER_2_SLOT_2:
        menuCB_BattleBA_FacePanelSet(sprite, 2, 2);
        break;

    case BA_FACE_PANEL_TRAINER_2_SLOT_3:
        menuCB_BattleBA_FacePanelSet(sprite, 2, 3);
        break;

    case BA_FACE_PANEL_TRAINER_2_SLOT_4:
        menuCB_BattleBA_FacePanelSet(sprite, 2, 4);
        break;

    case BA_FACE_PANEL_TRAINER_2_SLOT_5:
        menuCB_BattleBA_FacePanelSet(sprite, 2, 5);
        break;

    case BA_FACE_PANEL_TRAINER_3_SLOT_0:
        menuCB_BattleBA_FacePanelSet(sprite, 3, 0);
        break;

    case BA_FACE_PANEL_TRAINER_3_SLOT_1:
        menuCB_BattleBA_FacePanelSet(sprite, 3, 1);
        break;

    case BA_FACE_PANEL_TRAINER_3_SLOT_2:
        menuCB_BattleBA_FacePanelSet(sprite, 3, 2);
        break;

    case BA_FACE_PANEL_TRAINER_3_SLOT_3:
        menuCB_BattleBA_FacePanelSet(sprite, 3, 3);
        break;

    case BA_FACE_PANEL_TRAINER_3_SLOT_4:
        menuCB_BattleBA_FacePanelSet(sprite, 3, 4);
        break;

    case BA_FACE_PANEL_TRAINER_3_SLOT_5:
        menuCB_BattleBA_FacePanelSet(sprite, 3, 5);
        break;

    case BA_HP_BAR_TRAINER_0_SLOT_0:
        menuCB_BattleBA_HPBarDispSet(sprite, 0, 0);
        break;

    case BA_HP_BAR_TRAINER_0_SLOT_1:
        menuCB_BattleBA_HPBarDispSet(sprite, 0, 1);
        break;

    case BA_HP_BAR_TRAINER_0_SLOT_2:
        menuCB_BattleBA_HPBarDispSet(sprite, 0, 2);
        break;

    case BA_HP_BAR_TRAINER_0_SLOT_3:
        menuCB_BattleBA_HPBarDispSet(sprite, 0, 3);
        break;

    case BA_HP_BAR_TRAINER_0_SLOT_4:
        menuCB_BattleBA_HPBarDispSet(sprite, 0, 4);
        break;

    case BA_HP_BAR_TRAINER_0_SLOT_5:
        menuCB_BattleBA_HPBarDispSet(sprite, 0, 5);
        break;

    case BA_HP_BAR_TRAINER_1_SLOT_0:
        menuCB_BattleBA_HPBarDispSet(sprite, 1, 0);
        break;

    case BA_HP_BAR_TRAINER_1_SLOT_1:
        menuCB_BattleBA_HPBarDispSet(sprite, 1, 1);
        break;

    case BA_HP_BAR_TRAINER_1_SLOT_2:
        menuCB_BattleBA_HPBarDispSet(sprite, 1, 2);
        break;

    case BA_HP_BAR_TRAINER_1_SLOT_3:
        menuCB_BattleBA_HPBarDispSet(sprite, 1, 3);
        break;

    case BA_HP_BAR_TRAINER_1_SLOT_4:
        menuCB_BattleBA_HPBarDispSet(sprite, 1, 4);
        break;

    case BA_HP_BAR_TRAINER_1_SLOT_5:
        menuCB_BattleBA_HPBarDispSet(sprite, 1, 5);
        break;

    case BA_HP_BAR_TRAINER_2_SLOT_0:
        menuCB_BattleBA_HPBarDispSet(sprite, 2, 0);
        break;

    case BA_HP_BAR_TRAINER_2_SLOT_1:
        menuCB_BattleBA_HPBarDispSet(sprite, 2, 1);
        break;

    case BA_HP_BAR_TRAINER_2_SLOT_2:
        menuCB_BattleBA_HPBarDispSet(sprite, 2, 2);
        break;

    case BA_HP_BAR_TRAINER_2_SLOT_3:
        menuCB_BattleBA_HPBarDispSet(sprite, 2, 3);
        break;

    case BA_HP_BAR_TRAINER_2_SLOT_4:
        menuCB_BattleBA_HPBarDispSet(sprite, 2, 4);
        break;

    case BA_HP_BAR_TRAINER_2_SLOT_5:
        menuCB_BattleBA_HPBarDispSet(sprite, 2, 5);
        break;

    case BA_HP_BAR_TRAINER_3_SLOT_0:
        menuCB_BattleBA_HPBarDispSet(sprite, 3, 0);
        break;

    case BA_HP_BAR_TRAINER_3_SLOT_1:
        menuCB_BattleBA_HPBarDispSet(sprite, 3, 1);
        break;

    case BA_HP_BAR_TRAINER_3_SLOT_2:
        menuCB_BattleBA_HPBarDispSet(sprite, 3, 2);
        break;

    case BA_HP_BAR_TRAINER_3_SLOT_3:
        menuCB_BattleBA_HPBarDispSet(sprite, 3, 3);
        break;

    case BA_HP_BAR_TRAINER_3_SLOT_4:
        menuCB_BattleBA_HPBarDispSet(sprite, 3, 4);
        break;

    case BA_HP_BAR_TRAINER_3_SLOT_5:
        menuCB_BattleBA_HPBarDispSet(sprite, 3, 5);
        break;

    case BA_XP_TRAINER_0:
        menuCB_PrintXP(sprite, 0);
        break;

    case BA_XP_TRAINER_1:
        menuCB_PrintXP(sprite, 1);
        break;

    case BA_XP_TRAINER_2:
        menuCB_PrintXP(sprite, 2);
        break;

    case BA_XP_TRAINER_3:
        menuCB_PrintXP(sprite, 3);
        break;

    default:
        break;
    }
}

extern "C" void menuTool_BattleBA03_Button(tagWINDOW_WORK *window)
{
    if (pauseIsPause() == 0 && menuCBBattleBANextMenu(window) != 0 &&
        (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL) != 0) {
        window->mAccept = 1;
    }
}

extern "C" void menuTool_BattleBA03_Ctrl(tagWINDOW_WORK *window)
{
    menuCB_DispEffect(window);
    menuSetStatus(window->mMenuID, MENU_STATUS_NO_DEFAULT_INPUT);
    if (_CB.connectMode == 0 && menuCBBattleStartGetStatus() == 0 &&
        menuCB_GetDisconnectedGBA() != 0) {
        _CB.battleDisconnected = 1;
        window->mAccept = 1;
        window->mCancel = 1;
    }
}

extern "C" void menuTool_BattleBA01_Button(tagWINDOW_WORK *window)
{
    if (pauseIsPause() == 0 && (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_ACCEPT) != 0) {
        window->mAccept = 1;
    }
}

extern "C" void menuTool_BattleBA01_Ctrl(tagWINDOW_WORK *window)
{
    menuCB_DispEffect(window);
    menuSetStatus(window->mMenuID, MENU_STATUS_NO_DEFAULT_INPUT);
}

extern "C" void menuCB_GBAConnect_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case GBACONNECT_PORT1_OK:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mPhase[0] == GBA_PHASE_SAVE_COMPLETE
                                                  || gbaConectGBA_Connect_WORK.mPhase[0] == GBA_PHASE_COMPLETE);
        break;
    case GBACONNECT_PORT2_OK:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mPhase[1] == GBA_PHASE_COMPLETE);
        break;
    case GBACONNECT_PORT3_OK:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mPhase[2] == GBA_PHASE_COMPLETE);
        break;
    case GBACONNECT_PORT4_OK:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mPhase[3] == GBA_PHASE_COMPLETE);
        break;
    case GBACONNECT_PORT1_NG:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mPhase[0] == GBA_PHASE_SAVE_ERROR
                                                  || gbaConectGBA_Connect_WORK.mPhase[0] == GBA_PHASE_ERROR
                                                  || gbaConectGBA_Connect_WORK.mPhase[0] == GBA_PHASE_DISCONNECTED);
        break;
    case GBACONNECT_PORT2_NG:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mPhase[1] == GBA_PHASE_ERROR
                                                  || gbaConectGBA_Connect_WORK.mPhase[1] == GBA_PHASE_DISCONNECTED);
        break;
    case GBACONNECT_PORT3_NG:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mPhase[2] == GBA_PHASE_ERROR
                                                  || gbaConectGBA_Connect_WORK.mPhase[2] == GBA_PHASE_DISCONNECTED);
        break;
    case GBACONNECT_PORT4_NG:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mPhase[3] == GBA_PHASE_ERROR
                                                  || gbaConectGBA_Connect_WORK.mPhase[3] == GBA_PHASE_DISCONNECTED);
        break;
    case GBACONNECT_PORT1_FRAME:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[0] != GBA_SLOT_SAVE
                                                  && gbaConectGBA_Connect_WORK.mSlot[0] != GBA_SLOT_SAVE_GBA);
        break;
    case GBACONNECT_PORT1_LABEL:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[0] != GBA_SLOT_SAVE
                                                  && gbaConectGBA_Connect_WORK.mSlot[0] != GBA_SLOT_SAVE_GBA);
        break;
    case GBACONNECT_PORT1_ICON:
        if (gbaConectGBA_Connect_WORK.mSlot[0] == GBA_SLOT_SAVE || gbaConectGBA_Connect_WORK.mSlot[0] == GBA_SLOT_SAVE_GBA) {
            winSpriteSetDisp(sprite, false);
            break;
        }
        winSpriteSetDisp(sprite, true);
        if (gbaConectGBA_Connect_WORK.mIconPosReset[0]) {
            gbaConectGBA_Connect_WORK.mIconX[0] = sprite->mX;
            gbaConectGBA_Connect_WORK.mIconY[0] = sprite->mY;
            gbaConectGBA_Connect_WORK.mIconPosReset[0] = 0;
        }
        if (gbaConectGBA_Connect_WORK.mPhase[0] == GBA_PHASE_CONNECT) {
            if (!pauseIsPause()) {
                gbaConectGBA_Connect_WORK.mShakeCount[0]++;
            }
            sprite->mX = gbaConectGBA_Connect_WORK.mIconX[0] + gbaIconShake[gbaConectGBA_Connect_WORK.mShakeCount[0] & 3];
            sprite->mY = gbaConectGBA_Connect_WORK.mIconY[0];
        } else {
            sprite->mX = gbaConectGBA_Connect_WORK.mIconX[0];
            sprite->mY = gbaConectGBA_Connect_WORK.mIconY[0];
        }
        break;
    case GBACONNECT_PORT1_CABLE:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[0] != GBA_SLOT_SAVE);
        break;
    case GBACONNECT_SAVE_0:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[0] == GBA_SLOT_SAVE
                                                  || gbaConectGBA_Connect_WORK.mSlot[0] == GBA_SLOT_SAVE_GBA);
        break;
    case GBACONNECT_SAVE_1:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[0] == GBA_SLOT_SAVE
                                                  || gbaConectGBA_Connect_WORK.mSlot[0] == GBA_SLOT_SAVE_GBA);
        break;
    case GBACONNECT_SAVE_2:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[0] == GBA_SLOT_SAVE
                                                  || gbaConectGBA_Connect_WORK.mSlot[0] == GBA_SLOT_SAVE_GBA);
        break;
    case GBACONNECT_PORT2_ICON:
        if (gbaConectGBA_Connect_WORK.mIconPosReset[1]) {
            gbaConectGBA_Connect_WORK.mIconX[1] = sprite->mX;
            gbaConectGBA_Connect_WORK.mIconY[1] = sprite->mY;
            gbaConectGBA_Connect_WORK.mIconPosReset[1] = 0;
        }
        if (gbaConectGBA_Connect_WORK.mPhase[1] == GBA_PHASE_CONNECT) {
            if (!pauseIsPause()) {
                gbaConectGBA_Connect_WORK.mShakeCount[1]++;
            }
            sprite->mX = gbaConectGBA_Connect_WORK.mIconX[1] + gbaIconShake[gbaConectGBA_Connect_WORK.mShakeCount[1] & 3];
            sprite->mY = gbaConectGBA_Connect_WORK.mIconY[1];
        } else {
            sprite->mX = gbaConectGBA_Connect_WORK.mIconX[1];
            sprite->mY = gbaConectGBA_Connect_WORK.mIconY[1];
        }
        break;
    case GBACONNECT_PORT3_FRAME:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[2] != GBA_SLOT_NONE);
        break;
    case GBACONNECT_PORT3_LABEL:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[2] != GBA_SLOT_NONE);
        break;
    case GBACONNECT_PORT3_ICON:
        if (gbaConectGBA_Connect_WORK.mSlot[2] == GBA_SLOT_NONE) {
            winSpriteSetDisp(sprite, false);
            break;
        }
        if (gbaConectGBA_Connect_WORK.mIconPosReset[2]) {
            gbaConectGBA_Connect_WORK.mIconX[2] = sprite->mX;
            gbaConectGBA_Connect_WORK.mIconY[2] = sprite->mY;
            gbaConectGBA_Connect_WORK.mIconPosReset[2] = 0;
        }
        if (gbaConectGBA_Connect_WORK.mPhase[2] == GBA_PHASE_CONNECT) {
            if (!pauseIsPause()) {
                gbaConectGBA_Connect_WORK.mShakeCount[2]++;
            }
            sprite->mX = gbaConectGBA_Connect_WORK.mIconX[2] + gbaIconShake[gbaConectGBA_Connect_WORK.mShakeCount[2] & 3];
            sprite->mY = gbaConectGBA_Connect_WORK.mIconY[2];
        } else {
            sprite->mX = gbaConectGBA_Connect_WORK.mIconX[2];
            sprite->mY = gbaConectGBA_Connect_WORK.mIconY[2];
        }
        break;
    case GBACONNECT_PORT3_CABLE:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[2] != GBA_SLOT_NONE);
        break;
    case GBACONNECT_PORT4_FRAME:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[3] != GBA_SLOT_NONE);
        break;
    case GBACONNECT_PORT4_LABEL:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[3] != GBA_SLOT_NONE);
        break;
    case GBACONNECT_PORT4_ICON:
        if (gbaConectGBA_Connect_WORK.mSlot[3] == GBA_SLOT_NONE) {
            winSpriteSetDisp(sprite, false);
            break;
        }
        if (gbaConectGBA_Connect_WORK.mIconPosReset[3]) {
            gbaConectGBA_Connect_WORK.mIconX[3] = sprite->mX;
            gbaConectGBA_Connect_WORK.mIconY[3] = sprite->mY;
            gbaConectGBA_Connect_WORK.mIconPosReset[3] = 0;
        }
        if (gbaConectGBA_Connect_WORK.mPhase[3] == GBA_PHASE_CONNECT) {
            if (!pauseIsPause()) {
                gbaConectGBA_Connect_WORK.mShakeCount[3]++;
            }
            sprite->mX = gbaConectGBA_Connect_WORK.mIconX[3] + gbaIconShake[gbaConectGBA_Connect_WORK.mShakeCount[3] & 3];
            sprite->mY = gbaConectGBA_Connect_WORK.mIconY[3];
        } else {
            sprite->mX = gbaConectGBA_Connect_WORK.mIconX[3];
            sprite->mY = gbaConectGBA_Connect_WORK.mIconY[3];
        }
        break;
    case GBACONNECT_PORT4_CABLE:
        menuCB_SpriteDispCheckONorOFF(sprite, gbaConectGBA_Connect_WORK.mSlot[3] != GBA_SLOT_NONE);
        break;
    case GBACONNECT_MESSAGE_0:
        sprite->mMsgID = MSG_GBACONNECT_0;
        break;
    case GBACONNECT_MESSAGE_1:
        sprite->mMsgID = MSG_GBACONNECT_1;
        break;
    case GBACONNECT_MESSAGE_2:
        sprite->mMsgID = MSG_GBA_CONNECT_GUIDE;
        break;
    }
}

extern "C" void menuTool_GBAConnect_Button(tagWINDOW_WORK *)
{
}

extern "C" void menuTool_GBAConnect_Ctrl(tagWINDOW_WORK *window)
{
    menuCB_DispEffect(window);
    menuSetStatus(window->mMenuID, MENU_STATUS_NO_DEFAULT_INPUT);
    if (window->mState != WINDOW_STATE_ACTIVE) {
        return;
    }

    if (gbaConectGBA_Connect_WORK.mPhase[0] == GBA_PHASE_COMPLETE && gbaCommandIsConnected(1) == 0) {
        gbaConectGBA_Connect_WORK.mPhase[0] = GBA_PHASE_DISCONNECTED;
        gbaConectGBA_Connect_WORK.mResult = GBA_CONNECT_RESULT_DISCONNECTED;
    }
    if (gbaConectGBA_Connect_WORK.mPhase[1] == GBA_PHASE_COMPLETE && gbaCommandIsConnected(2) == 0) {
        gbaConectGBA_Connect_WORK.mPhase[1] = GBA_PHASE_DISCONNECTED;
        gbaConectGBA_Connect_WORK.mResult = GBA_CONNECT_RESULT_DISCONNECTED;
    }
    if (gbaConectGBA_Connect_WORK.mPhase[2] == GBA_PHASE_COMPLETE && gbaCommandIsConnected(3) == 0) {
        gbaConectGBA_Connect_WORK.mPhase[2] = GBA_PHASE_DISCONNECTED;
        gbaConectGBA_Connect_WORK.mResult = GBA_CONNECT_RESULT_DISCONNECTED;
    }
    if (gbaConectGBA_Connect_WORK.mPhase[3] == GBA_PHASE_COMPLETE && gbaCommandIsConnected(4) == 0) {
        gbaConectGBA_Connect_WORK.mPhase[3] = GBA_PHASE_DISCONNECTED;
        gbaConectGBA_Connect_WORK.mResult = GBA_CONNECT_RESULT_DISCONNECTED;
    }
}

extern "C" void menuCB_error01_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case ERROR01_ITEM_CURSOR: {
        s16 cursor;
        s32 x;

        winSpriteSetDisp(sprite, false);
        cursor = window->mCursor.y;
        if (cursor >= 0) {
            x = 0;
            if (cursor >= ERROR01_COLUMN_ROWS)
                x = 160; // right column
            menuSubDrawCursorBackAnimeOnOffAuto(window, winSpriteGetLayerID(window, sprite), x,
                                                (cursor % ERROR01_COLUMN_ROWS) << 5,
                                                sprite->mWidth, sprite->mHeight,
                                                window->mRgba.mColorA,
                                                MENU_SUB_CURSOR_BACK_STYLE_VARIANT);
        }
        break;
    }
    case ERROR01_ITEM_LEFT_COLUMN:
        menuCBPokemon_ErrorListDisp(window, sprite, _CB.error00Cursor, 0);
        break;
    case ERROR01_ITEM_RIGHT_COLUMN:
        menuCBPokemon_ErrorListDisp(window, sprite, _CB.error00Cursor, 1);
        break;
    case ERROR01_ITEM_TITLE:
        sprite->mMsgID = MSG_CB_ERROR01_TITLE;
        break;
    }
}

extern "C" void menuTool_error01_Button(tagWINDOW_WORK *window)
{
    _CB.error01SelectedItem = window->mCursor.y;
    if (pauseIsPause() == 0) {
        if ((windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_ACCEPT) != 0) {
            window->mAccept = 1;
        } else if ((windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL) != 0) {
            _CB.error01Confirmed = 1;
            window->mAccept = 1;
            window->mCancel = 1;
        }
    }
}

extern "C" void menuTool_error01_Ctrl(tagWINDOW_WORK *window)
{
    if (window->mState == WINDOW_STATE_ACTIVE) {
        s32 i;
        for (i = 0; i <= _CB.error01LastItem; ++i) {
            menuItemBiosSetSelectFlag(MENU_ID_CB_ERROR01, error01MenuItemIds[i], true);
        }
        for (; i < ERROR01_ITEM_COUNT; ++i) {
            menuItemBiosSetSelectFlag(MENU_ID_CB_ERROR01, error01MenuItemIds[i], false);
        }
    }
    menuCB_DispEffect(window);
    menuSetStatus(window->mMenuID, MENU_STATUS_NO_DEFAULT_INPUT);
}

extern "C" void menuCB_error00_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case ERROR00_ITEM_PANEL_0:
        menuCBPokemon_ErrorDispSet(sprite, 0);
        break;
    case ERROR00_ITEM_HIDDEN_0:
        winSpriteSetDisp(sprite, false);
        break;
    case ERROR00_ITEM_FACE_0:
        menuCBPokemonEntryDispPokemonFace(window, sprite, _CB.entryNo, 0);
        break;
    case ERROR00_ITEM_CURSOR_0_A:
    case ERROR00_ITEM_CURSOR_0_B:
    case ERROR00_ITEM_CURSOR_0_C:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.error00Cursor == 0);
        menuCB_Cursol_Disp_AlphaSet(window, sprite, _CB.error00Cursor == 0 && !window->mAccept);
        break;
    case ERROR00_ITEM_LEVEL_0:
        menuCBPokemonLevel(window, _CB.entryNo, 0, 1);
        break;
    case ERROR00_ITEM_NAME_0:
        menuCBPokemonName(window, _CB.entryNo, 0, 1);
        break;
    case ERROR00_ITEM_MARU_WAKU_0:
        menuCBPokemonMaruWaku_ErrorSet(sprite, 0);
        break;
    case ERROR00_ITEM_HELD_ITEM_0:
        menuCBPokemonItem_ErrorSet(sprite, 0);
        break;

    case ERROR00_ITEM_PANEL_1:
        menuCBPokemon_ErrorDispSet(sprite, 1);
        break;
    case ERROR00_ITEM_HIDDEN_1:
        winSpriteSetDisp(sprite, false);
        break;
    case ERROR00_ITEM_FACE_1:
        menuCBPokemonEntryDispPokemonFace(window, sprite, _CB.entryNo, 1);
        break;
    case ERROR00_ITEM_CURSOR_1_A:
    case ERROR00_ITEM_CURSOR_1_B:
    case ERROR00_ITEM_CURSOR_1_C:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.error00Cursor == 1);
        menuCB_Cursol_Disp_AlphaSet(window, sprite, _CB.error00Cursor == 1 && !window->mAccept);
        break;
    case ERROR00_ITEM_LEVEL_1:
        menuCBPokemonLevel(window, _CB.entryNo, 1, 1);
        break;
    case ERROR00_ITEM_NAME_1:
        menuCBPokemonName(window, _CB.entryNo, 1, 1);
        break;
    case ERROR00_ITEM_MARU_WAKU_1:
        menuCBPokemonMaruWaku_ErrorSet(sprite, 1);
        break;
    case ERROR00_ITEM_HELD_ITEM_1:
        menuCBPokemonItem_ErrorSet(sprite, 1);
        break;

    case ERROR00_ITEM_PANEL_2:
        menuCBPokemon_ErrorDispSet(sprite, 2);
        break;
    case ERROR00_ITEM_HIDDEN_2:
        winSpriteSetDisp(sprite, false);
        break;
    case ERROR00_ITEM_FACE_2:
        menuCBPokemonEntryDispPokemonFace(window, sprite, _CB.entryNo, 2);
        break;
    case ERROR00_ITEM_CURSOR_2_A:
    case ERROR00_ITEM_CURSOR_2_B:
    case ERROR00_ITEM_CURSOR_2_C:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.error00Cursor == 2);
        menuCB_Cursol_Disp_AlphaSet(window, sprite, _CB.error00Cursor == 2 && !window->mAccept);
        break;
    case ERROR00_ITEM_LEVEL_2:
        menuCBPokemonLevel(window, _CB.entryNo, 2, 1);
        break;
    case ERROR00_ITEM_NAME_2:
        menuCBPokemonName(window, _CB.entryNo, 2, 1);
        break;
    case ERROR00_ITEM_MARU_WAKU_2:
        menuCBPokemonMaruWaku_ErrorSet(sprite, 2);
        break;
    case ERROR00_ITEM_HELD_ITEM_2:
        menuCBPokemonItem_ErrorSet(sprite, 2);
        break;

    case ERROR00_ITEM_PANEL_3:
        menuCBPokemon_ErrorDispSet(sprite, 3);
        break;
    case ERROR00_ITEM_HIDDEN_3:
        winSpriteSetDisp(sprite, false);
        break;
    case ERROR00_ITEM_FACE_3:
        menuCBPokemonEntryDispPokemonFace(window, sprite, _CB.entryNo, 3);
        break;
    case ERROR00_ITEM_CURSOR_3_A:
    case ERROR00_ITEM_CURSOR_3_B:
    case ERROR00_ITEM_CURSOR_3_C:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.error00Cursor == 3);
        menuCB_Cursol_Disp_AlphaSet(window, sprite, _CB.error00Cursor == 3 && !window->mAccept);
        break;
    case ERROR00_ITEM_LEVEL_3:
        menuCBPokemonLevel(window, _CB.entryNo, 3, 1);
        break;
    case ERROR00_ITEM_NAME_3:
        menuCBPokemonName(window, _CB.entryNo, 3, 1);
        break;
    case ERROR00_ITEM_MARU_WAKU_3:
        menuCBPokemonMaruWaku_ErrorSet(sprite, 3);
        break;
    case ERROR00_ITEM_HELD_ITEM_3:
        menuCBPokemonItem_ErrorSet(sprite, 3);
        break;

    case ERROR00_ITEM_PANEL_4:
        menuCBPokemon_ErrorDispSet(sprite, 4);
        break;
    case ERROR00_ITEM_HIDDEN_4:
        winSpriteSetDisp(sprite, false);
        break;
    case ERROR00_ITEM_FACE_4:
        menuCBPokemonEntryDispPokemonFace(window, sprite, _CB.entryNo, 4);
        break;
    case ERROR00_ITEM_CURSOR_4_A:
    case ERROR00_ITEM_CURSOR_4_B:
    case ERROR00_ITEM_CURSOR_4_C:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.error00Cursor == 4);
        menuCB_Cursol_Disp_AlphaSet(window, sprite, _CB.error00Cursor == 4 && !window->mAccept);
        break;
    case ERROR00_ITEM_LEVEL_4:
        menuCBPokemonLevel(window, _CB.entryNo, 4, 1);
        break;
    case ERROR00_ITEM_NAME_4:
        menuCBPokemonName(window, _CB.entryNo, 4, 1);
        break;
    case ERROR00_ITEM_MARU_WAKU_4:
        menuCBPokemonMaruWaku_ErrorSet(sprite, 4);
        break;
    case ERROR00_ITEM_HELD_ITEM_4:
        menuCBPokemonItem_ErrorSet(sprite, 4);
        break;

    case ERROR00_ITEM_PANEL_5:
        menuCBPokemon_ErrorDispSet(sprite, 5);
        break;
    case ERROR00_ITEM_HIDDEN_5:
        winSpriteSetDisp(sprite, false);
        break;
    case ERROR00_ITEM_FACE_5:
        menuCBPokemonEntryDispPokemonFace(window, sprite, _CB.entryNo, 5);
        break;
    case ERROR00_ITEM_CURSOR_5_A:
    case ERROR00_ITEM_CURSOR_5_B:
    case ERROR00_ITEM_CURSOR_5_C:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.error00Cursor == 5);
        menuCB_Cursol_Disp_AlphaSet(window, sprite, _CB.error00Cursor == 5 && !window->mAccept);
        break;
    case ERROR00_ITEM_LEVEL_5:
        menuCBPokemonLevel(window, _CB.entryNo, 5, 1);
        break;
    case ERROR00_ITEM_NAME_5:
        menuCBPokemonName(window, _CB.entryNo, 5, 1);
        break;
    case ERROR00_ITEM_MARU_WAKU_5:
        menuCBPokemonMaruWaku_ErrorSet(sprite, 5);
        break;
    case ERROR00_ITEM_HELD_ITEM_5:
        menuCBPokemonItem_ErrorSet(sprite, 5);
        break;

    case ERROR00_ITEM_OK_CURSOR:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.error00Cursor == ERROR00_CURSOR_OK);
        menuCB_Cursol_Disp_AlphaSet(window, sprite,
                                    _CB.error00Cursor == ERROR00_CURSOR_OK && !window->mAccept);
        break;
    case ERROR00_ITEM_GUIDE:
        if (windowSearchID(MENU_ID_CB_ERROR01) != NULL) {
            sprite->mMsgID = 0;
        } else {
            menuCBMessageCenterDisp(window, sprite, MSG_CB_ERROR00_GUIDE);
        }
        break;
    case ERROR00_ITEM_OK_BUTTON:
        menuCB_SpriteDispCheckONorOFF(sprite, windowSearchID(MENU_ID_CB_ERROR01) == NULL);
        break;
    case ERROR00_ITEM_TITLE:
        sprite->mMsgID = MSG_CB_ERROR00_TITLE;
        break;
    }
}

extern "C" void menuTool_error00_Button(tagWINDOW_WORK *window)
{
    if (pauseIsPause() != 0) {
        return;
    }
    if ((windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_ACCEPT) != 0) {
        window->mAccept = 1;
        menuCB_SE_Play(MENU_SE_CONFIRM);
        return;
    }
    if ((windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL) != 0) {
        window->mAccept = 1;
        _CB.error00Confirmed = 1;
        menuCB_SE_Play(MENU_SE_CANCEL);
        return;
    }
    if (_CB.error00LastCursor != -1) {
        if ((windowGetKeyRepeat() & WINDOW_KEY_UP) != 0) {
            if (_CB.error00Cursor == ERROR00_CURSOR_OK) {
                _CB.error00Cursor = _CB.error00LastCursor;
                menuCB_SE_Play(MENU_SE_CURSOR);
                return;
            } else if (_CB.error00Cursor != 0) {
                --_CB.error00Cursor;
                menuCB_SE_Play(MENU_SE_CURSOR);
                return;
            }
        }
        if ((windowGetKeyRepeat() & WINDOW_KEY_DOWN) != 0) {
            if (_CB.error00Cursor < _CB.error00LastCursor) {
                ++_CB.error00Cursor;
                menuCB_SE_Play(MENU_SE_CURSOR);
                return;
            }
        }
        if (((windowGetKeyInfo()->mKeyTrg | windowGetKeyInfo2()->mKeyTrg) &
             (WINDOW_KEY_LEFT | WINDOW_KEY_RIGHT)) != 0) {
            if (_CB.error00Cursor == ERROR00_CURSOR_OK) {
                _CB.error00Cursor = 0;
                menuCB_SE_Play(MENU_SE_CURSOR);
            } else {
                _CB.error00Cursor = ERROR00_CURSOR_OK;
                menuCB_SE_Play(MENU_SE_CURSOR);
            }
        }
    } else {
        _CB.error00Cursor = ERROR00_CURSOR_OK;
    }
}

extern "C" void menuTool_error00_Ctrl(tagWINDOW_WORK *window)
{
    menuCB_DispEffect(window);
    menuSetStatus(window->mMenuID, MENU_STATUS_NO_DEFAULT_INPUT);
    menuCB_AlphaGet(window);
}

extern "C" void menuCB_Tunaide03_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case ITEM_TEAM_GUIDE:
        sprite->mMsgID = TUNAIDE03_GUIDE_MESSAGE_ID;
        break;
    case ITEM_MESSAGE:
        menuCBMessageCenterDisp(window, sprite, MSG_GBA_CONNECT_GUIDE);
        break;
    case ITEM_TRAINER_FACE_0_A:
    case ITEM_TRAINER_FACE_0_B:
    case ITEM_TRAINER_FACE_0_C: {
        u32 face = toolbattle_trainerFace(0);
        menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(trainerFaceSpriteIDs[face]));
        break;
    }
    case ITEM_TRAINER_FACE_1_A:
    case ITEM_TRAINER_FACE_1_B:
    case ITEM_TRAINER_FACE_1_C: {
        u32 face = toolbattle_trainerFace(1);
        menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(trainerFaceSpriteIDs[face]));
        break;
    }
    case ITEM_TRAINER_FACE_2_A:
    case ITEM_TRAINER_FACE_2_B:
    case ITEM_TRAINER_FACE_2_C: {
        u32 face = toolbattle_trainerFace(2);
        menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(trainerFaceSpriteIDs[face]));
        break;
    }
    case ITEM_TRAINER_FACE_3_A:
    case ITEM_TRAINER_FACE_3_B:
    case ITEM_TRAINER_FACE_3_C: {
        u32 face = toolbattle_trainerFace(3);
        menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(trainerFaceSpriteIDs[face]));
        break;
    }
    case ITEM_TEAM_LAYOUT_0_A:
    case ITEM_TEAM_LAYOUT_0_B:
    case ITEM_TEAM_LAYOUT_0_C:
    case ITEM_TEAM_LAYOUT_0_D:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.mTeamSelect == 0);
        break;
    case ITEM_TEAM_LAYOUT_1_A:
    case ITEM_TEAM_LAYOUT_1_B:
    case ITEM_TEAM_LAYOUT_1_C:
    case ITEM_TEAM_LAYOUT_1_D:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.mTeamSelect == 1);
        break;
    case ITEM_TEAM_LAYOUT_2_A:
    case ITEM_TEAM_LAYOUT_2_B:
    case ITEM_TEAM_LAYOUT_2_C:
    case ITEM_TEAM_LAYOUT_2_D:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.mTeamSelect == 2);
        break;
    case ITEM_TEAM_LAYOUT_3:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.mTeamSelect == 3);
        menuCB_Cursol_Disp_AlphaSet(window, sprite, true);
        break;
    case ITEM_TRAINER_NAME_0_A:
    case ITEM_TRAINER_NAME_0_B:
    case ITEM_TRAINER_NAME_0_C:
        toobattle_drawHeroName(window, 0);
        break;
    case ITEM_TRAINER_NAME_1_A:
    case ITEM_TRAINER_NAME_1_B:
    case ITEM_TRAINER_NAME_1_C:
        toobattle_drawHeroName(window, 1);
        break;
    case ITEM_TRAINER_NAME_2_A:
    case ITEM_TRAINER_NAME_2_B:
    case ITEM_TRAINER_NAME_2_C:
        toobattle_drawHeroName(window, 2);
        break;
    case ITEM_TRAINER_NAME_3_A:
    case ITEM_TRAINER_NAME_3_B:
    case ITEM_TRAINER_NAME_3_C:
        toobattle_drawHeroName(window, 3);
        break;
    }
}

extern "C" void menuTool_tunaide03_Button(tagWINDOW_WORK *window)
{
    if (pauseIsPause()) {
        return;
    }

    if (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_ACCEPT) {
        window->mAccept = 1;
        menuCB_SE_Play(MENU_SE_CONFIRM);
    } else if (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL) {
        window->mAccept = 1;
        window->mCancel = 1;
        _CB.mTeamSelectCancel = 1;
        menuCB_SE_Play(MENU_SE_CANCEL);
    } else {
        if ((windowGetKeyInfo()->mKeyTrg | windowGetKeyInfo2()->mKeyTrg) & (WINDOW_KEY_UP | WINDOW_KEY_DOWN)) {
            switch (_CB.mTeamSelect) {
            case 0:
                _CB.mTeamSelect = 2;
                break;
            case 1:
                _CB.mTeamSelect = 3;
                break;
            case 2:
                _CB.mTeamSelect = 0;
                break;
            case 3:
                _CB.mTeamSelect = 1;
                break;
            }
            menuCB_SE_Play(MENU_SE_CURSOR);
        }
        if ((windowGetKeyInfo()->mKeyTrg | windowGetKeyInfo2()->mKeyTrg) & (WINDOW_KEY_LEFT | WINDOW_KEY_RIGHT)) {
            switch (_CB.mTeamSelect) {
            case 0:
                _CB.mTeamSelect = 1;
                break;
            case 1:
                _CB.mTeamSelect = 0;
                break;
            case 2:
                _CB.mTeamSelect = 3;
                break;
            case 3:
                _CB.mTeamSelect = 2;
                break;
            }
            menuCB_SE_Play(MENU_SE_CURSOR);
        }
    }
}

extern "C" void menuTool_tunaide03_Ctrl(tagWINDOW_WORK *window)
{
    menuCB_DispEffect(window);
    menuSetStatus(window->mMenuID, MENU_STATUS_NO_DEFAULT_INPUT);
    if (menuCB_GetDisconnectedGBA() != 0) {
        _CB.mTeamSelectGbaDisconnected = 1;
        window->mAccept = 1;
        window->mCancel = 1;
    }
    menuCB_AlphaGet(window);
}

extern "C" void menuCB_Tunaide02_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case TUNAIDE02_GUIDE:
        if (tunaide02_last_kakunin_flag == 0) {
            sprite->mMsgID = TUNAIDE02_MSG_GUIDE_WAIT;
        } else {
            sprite->mMsgID = TUNAIDE02_MSG_GUIDE_START;
        }
        break;
    case TUNAIDE02_SLOT0_START:
        menuCB_SpriteDispCheckONorOFF(sprite, tunaide02_last_kakunin_flag == 1);
        break;
    case TUNAIDE02_SLOT1_START:
        menuCB_SpriteDispCheckONorOFF(sprite, tunaide02_last_kakunin_flag == 2);
        break;

    // slot 0: used in multi battles or when trainer 0 has no face.
    case TUNAIDE02_SLOT0_READY:
        menuCB_SpriteDispCheckONorOFF(sprite, (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT0_BIT) && tunaide02_last_kakunin_flag != 1);
        break;
    case TUNAIDE02_SLOT0_EMPTY_0:
    case TUNAIDE02_SLOT0_EMPTY_1:
    case TUNAIDE02_SLOT0_EMPTY_2:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.battleMode != TUNAIDE02_BATTLE_MODE_MULTI && toolbattle_trainerFace(0) != 0);
        break;
    case TUNAIDE02_SLOT0_FRAME_0:
    case TUNAIDE02_SLOT0_FRAME_1:
    case TUNAIDE02_SLOT0_FRAME_2:
    case TUNAIDE02_SLOT0_FRAME_3:
    case TUNAIDE02_SLOT0_FRAME_4:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI || toolbattle_trainerFace(0) == 0);
        break;
    case TUNAIDE02_SLOT0_WAIT_0:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI || toolbattle_trainerFace(0) == 0) {
            if (tunaide02_last_kakunin_flag == 1) {
                winSpriteSetDisp(sprite, true);
            } else {
                menuCB_SpriteDispCheckONorOFF(sprite, (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT0_BIT) == 0);
            }
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case TUNAIDE02_SLOT0_WAIT_1:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI || toolbattle_trainerFace(0) == 0) {
            menuCB_SpriteDispCheckONorOFF(sprite, (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT0_BIT) == 0);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case TUNAIDE02_SLOT0_PROMPT:
        if (tunaide02_last_kakunin_flag == 1) {
            sprite->mMsgID = TUNAIDE02_MSG_PROMPT_START;
        } else if (toolbattle_trainerFace(0) == 0 || _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            if (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT0_BIT) {
                sprite->mMsgID = 0;
            } else {
                sprite->mMsgID = TUNAIDE02_MSG_PROMPT_WAIT;
            }
        } else {
            sprite->mMsgID = 0;
        }
        break;
    case TUNAIDE02_SLOT0_PROMPT_SHADOW:
        if (tunaide02_last_kakunin_flag == 1) {
            sprite->mMsgID = TUNAIDE02_MSG_PROMPT_SHADOW_START;
        } else if (toolbattle_trainerFace(0) == 0 || _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            if (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT0_BIT) {
                sprite->mMsgID = 0;
            } else {
                sprite->mMsgID = TUNAIDE02_MSG_PROMPT_SHADOW_WAIT;
            }
        } else {
            sprite->mMsgID = 0;
        }
        break;
    case TUNAIDE02_SLOT0_PANEL:
        sprite->mMsgID = TUNAIDE02_MSG_SLOT0_PANEL;
        break;
    case TUNAIDE02_SLOT0_FRAME_5:
        menuCB_SpriteDispCheckONorOFF(sprite, toolbattle_trainerFace(0) == 0 || _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI);
        break;
    case TUNAIDE02_SLOT0_PLATE:
        if (toolbattle_trainerFace(0) == 0) {
            menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(tunaide02PlateSpriteIDs[1]));
        } else if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(tunaide02PlateSpriteIDs[0]));
        } else {
            menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(tunaide02PlateSpriteIDs[1]));
        }
        break;
    case TUNAIDE02_SLOT0_FACE:
        if (toolbattle_trainerFace(0) == 0 || _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(tunaide02_face_sprite[toolbattle_trainerFace(0)]));
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case TUNAIDE02_SLOT0_HERO_ID:
        if (toolbattle_trainerFace(0) == 0 || _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            toolbattle_drawHeroID(window, 0);
        }
        break;
    case TUNAIDE02_SLOT0_HERO_NAME:
        if (toolbattle_trainerFace(0) == 0 || _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            toobattle_drawHeroName(window, 0);
        }
        break;

    // slot 1: always used.
    case TUNAIDE02_SLOT1_READY:
        menuCB_SpriteDispCheckONorOFF(sprite, (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT1_BIT) && tunaide02_last_kakunin_flag != 2);
        break;
    case TUNAIDE02_SLOT1_WAIT_0:
        if (tunaide02_last_kakunin_flag == 2) {
            winSpriteSetDisp(sprite, true);
        } else {
            menuCB_SpriteDispCheckONorOFF(sprite, (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT1_BIT) == 0);
        }
        break;
    case TUNAIDE02_SLOT1_WAIT_1:
        menuCB_SpriteDispCheckONorOFF(sprite, (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT1_BIT) == 0);
        break;
    case TUNAIDE02_SLOT1_PROMPT:
        if (tunaide02_last_kakunin_flag == 2) {
            sprite->mMsgID = TUNAIDE02_MSG_PROMPT_START;
        } else if (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT1_BIT) {
            sprite->mMsgID = 0;
        } else {
            sprite->mMsgID = TUNAIDE02_MSG_PROMPT_WAIT;
        }
        break;
    case TUNAIDE02_SLOT1_PROMPT_SHADOW:
        if (tunaide02_last_kakunin_flag == 2) {
            sprite->mMsgID = TUNAIDE02_MSG_PROMPT_SHADOW_START;
        } else if (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT1_BIT) {
            sprite->mMsgID = 0;
        } else {
            sprite->mMsgID = TUNAIDE02_MSG_PROMPT_SHADOW_WAIT;
        }
        break;
    case TUNAIDE02_SLOT1_PANEL:
        sprite->mMsgID = TUNAIDE02_MSG_SLOT1_PANEL;
        break;
    case TUNAIDE02_SLOT1_MARK:
        if (toolbattle_trainerFace(0) == 0 || _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            winSpriteSetDisp(sprite, true);
        } else {
            menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(tunaideSlotMarkSpriteIDs[0]));
        }
        break;
    case TUNAIDE02_SLOT1_FACE:
        if (toolbattle_trainerFace(0) == 0 || _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(tunaide02_face_sprite[toolbattle_trainerFace(1)]));
        } else {
            menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(tunaide02_face_sprite[toolbattle_trainerFace(0)]));
        }
        break;
    case TUNAIDE02_SLOT1_HERO_ID:
        if (toolbattle_trainerFace(0) == 0 || _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            toolbattle_drawHeroID(window, 1);
        } else {
            toolbattle_drawHeroID(window, 0);
        }
        break;
    case TUNAIDE02_SLOT1_HERO_NAME:
        if (toolbattle_trainerFace(0) == 0 || _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            toobattle_drawHeroName(window, 1);
        } else {
            toobattle_drawHeroName(window, 0);
        }
        break;

    // slot 2: used in multi battles or when trainer 0 has a face.
    case TUNAIDE02_SLOT2_READY:
        menuCB_SpriteDispCheckONorOFF(sprite, (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT2_BIT) != 0);
        break;
    case TUNAIDE02_SLOT2_FRAME_0:
    case TUNAIDE02_SLOT2_FRAME_1:
    case TUNAIDE02_SLOT2_FRAME_2:
    case TUNAIDE02_SLOT2_FRAME_3:
    case TUNAIDE02_SLOT2_FRAME_4:
    case TUNAIDE02_SLOT2_FRAME_5:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI || toolbattle_trainerFace(0) != 0);
        break;
    case TUNAIDE02_SLOT2_WAIT_0:
    case TUNAIDE02_SLOT2_WAIT_1:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI || toolbattle_trainerFace(0) != 0) {
            menuCB_SpriteDispCheckONorOFF(sprite, (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT2_BIT) == 0);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case TUNAIDE02_SLOT2_PROMPT:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI || toolbattle_trainerFace(0) != 0) {
            if (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT2_BIT) {
                sprite->mMsgID = 0;
            } else {
                sprite->mMsgID = TUNAIDE02_MSG_PROMPT_WAIT;
            }
        } else {
            sprite->mMsgID = 0;
        }
        break;
    case TUNAIDE02_SLOT2_PROMPT_SHADOW:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI || toolbattle_trainerFace(0) != 0) {
            if (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT2_BIT) {
                sprite->mMsgID = 0;
            } else {
                sprite->mMsgID = TUNAIDE02_MSG_PROMPT_SHADOW_WAIT;
            }
        } else {
            sprite->mMsgID = 0;
        }
        break;
    case TUNAIDE02_SLOT2_PANEL:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI || toolbattle_trainerFace(0) != 0) {
            sprite->mMsgID = TUNAIDE02_MSG_SLOT2_PANEL;
        } else {
            sprite->mMsgID = 0;
        }
        break;
    case TUNAIDE02_SLOT2_MARK:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            winSpriteSetDisp(sprite, true);
        } else if (toolbattle_trainerFace(0) != 0) {
            menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(tunaideSlotMarkSpriteIDs[1]));
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case TUNAIDE02_SLOT2_FACE:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(tunaide02_face_sprite[toolbattle_trainerFace(2)]));
        } else if (toolbattle_trainerFace(0) != 0) {
            menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(tunaide02_face_sprite[toolbattle_trainerFace(1)]));
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case TUNAIDE02_SLOT2_HERO_ID:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            toolbattle_drawHeroID(window, 2);
        } else if (toolbattle_trainerFace(0) != 0) {
            toolbattle_drawHeroID(window, 1);
        }
        break;
    case TUNAIDE02_SLOT2_HERO_NAME:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            toobattle_drawHeroName(window, 2);
        } else if (toolbattle_trainerFace(0) != 0) {
            toobattle_drawHeroName(window, 1);
        }
        break;

    // slot 3: multi battles only.
    case TUNAIDE02_SLOT3_READY:
        menuCB_SpriteDispCheckONorOFF(sprite, (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT3_BIT) != 0);
        break;
    case TUNAIDE02_SLOT3_FRAME_0:
    case TUNAIDE02_SLOT3_FRAME_1:
    case TUNAIDE02_SLOT3_FRAME_2:
    case TUNAIDE02_SLOT3_FRAME_3:
    case TUNAIDE02_SLOT3_FRAME_4:
    case TUNAIDE02_SLOT3_FRAME_5:
    case TUNAIDE02_SLOT3_FRAME_6:
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI);
        break;
    case TUNAIDE02_SLOT3_WAIT_0:
    case TUNAIDE02_SLOT3_WAIT_1:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            menuCB_SpriteDispCheckONorOFF(sprite, (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT3_BIT) == 0);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case TUNAIDE02_SLOT3_PROMPT:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            if (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT3_BIT) {
                sprite->mMsgID = 0;
            } else {
                sprite->mMsgID = TUNAIDE02_MSG_PROMPT_WAIT;
            }
        } else {
            sprite->mMsgID = 0;
        }
        break;
    case TUNAIDE02_SLOT3_PROMPT_SHADOW:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            if (_CB.mTunaide02TempBuff & TUNAIDE02_SLOT3_BIT) {
                sprite->mMsgID = 0;
            } else {
                sprite->mMsgID = TUNAIDE02_MSG_PROMPT_SHADOW_WAIT;
            }
        } else {
            sprite->mMsgID = 0;
        }
        break;
    case TUNAIDE02_SLOT3_PANEL:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            sprite->mMsgID = TUNAIDE02_MSG_SLOT3_PANEL;
        } else {
            sprite->mMsgID = 0;
        }
        break;
    case TUNAIDE02_SLOT3_FACE:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            menuCBSpriteSetData(sprite, menuSpriteBiosGetPtr(tunaide02_face_sprite[toolbattle_trainerFace(3)]));
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case TUNAIDE02_SLOT3_HERO_ID:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            toolbattle_drawHeroID(window, 3);
        }
        break;
    case TUNAIDE02_SLOT3_HERO_NAME:
        if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
            toobattle_drawHeroName(window, 3);
        }
        break;
    }
}

extern "C" void menuTool_tunaide02_Button(tagWINDOW_WORK *window)
{
    u8 ports[4] = { 1, 2, 4, 8 };
    u16 trg[4];
    s32 i;

    if (_CB.battleMode == TUNAIDE02_BATTLE_MODE_MULTI) {
        for (i = 0; i < 4; i++) {
            trg[i] = windowGetPortKeyInfo(ports[toolentryTaisenGetControlerType(i) - 1])->mKeyTrg;
        }
        if (_CB.mTunaide02TempBuff == TUNAIDE02_SLOT_ALL) {
            tunaide02_last_kakunin_flag = 1;
            if (trg[0] & WINDOW_KEY_CANCEL) {
                window->mAccept = 1;
                window->mCancel = 1;
                _CB.mTunaide02Cancel = 1;
                menuCB_SE_Play(MENU_SE_CANCEL);
            } else if (trg[0] & TUNAIDE02_KEY_START) {
                window->mAccept = 1;
                menuCB_SE_Play(MENU_SE_CONFIRM);
            }
        } else {
            tunaide02_last_kakunin_flag = 0;
            if ((trg[0] & WINDOW_KEY_ACCEPT) && !toolBattle_tunaideMenu02menuTempBuffGet(TUNAIDE02_SLOT0_BIT)) {
                toolBattle_tunaideMenu02menuTempBuffSet(TUNAIDE02_SLOT0_BIT);
                menuCB_SE_Play(MENU_SE_CONFIRM);
            }
            if ((trg[1] & WINDOW_KEY_ACCEPT) && !toolBattle_tunaideMenu02menuTempBuffGet(TUNAIDE02_SLOT1_BIT)) {
                toolBattle_tunaideMenu02menuTempBuffSet(TUNAIDE02_SLOT1_BIT);
                menuCB_SE_Play(MENU_SE_CONFIRM);
            }
            if ((trg[2] & WINDOW_KEY_ACCEPT) && !toolBattle_tunaideMenu02menuTempBuffGet(TUNAIDE02_SLOT2_BIT)) {
                toolBattle_tunaideMenu02menuTempBuffSet(TUNAIDE02_SLOT2_BIT);
                menuCB_SE_Play(MENU_SE_CONFIRM);
            }
            if ((trg[3] & WINDOW_KEY_ACCEPT) && !toolBattle_tunaideMenu02menuTempBuffGet(TUNAIDE02_SLOT3_BIT)) {
                toolBattle_tunaideMenu02menuTempBuffSet(TUNAIDE02_SLOT3_BIT);
                menuCB_SE_Play(MENU_SE_CONFIRM);
            }
            if ((trg[0] | trg[1] | trg[2] | trg[3]) & WINDOW_KEY_CANCEL) {
                window->mAccept = 1;
                window->mCancel = 1;
                _CB.mTunaide02Cancel = 1;
                menuCB_SE_Play(MENU_SE_CANCEL);
            }
        }
    } else if (toolbattle_trainerFace(0) != 0) {
        trg[1] = windowGetPortKeyInfo(ports[toolentryTaisenGetControlerType(1) - 1])->mKeyTrg;
        trg[2] = windowGetPortKeyInfo(ports[toolentryTaisenGetControlerType(2) - 1])->mKeyTrg;
        if (_CB.mTunaide02TempBuff == (TUNAIDE02_SLOT1_BIT | TUNAIDE02_SLOT2_BIT)) {
            tunaide02_last_kakunin_flag = 2;
            if (trg[1] & WINDOW_KEY_CANCEL) {
                window->mAccept = 1;
                window->mCancel = 1;
                _CB.mTunaide02Cancel = 1;
                menuCB_SE_Play(MENU_SE_CANCEL);
            } else if (trg[1] & TUNAIDE02_KEY_START) {
                window->mAccept = 1;
                menuCB_SE_Play(MENU_SE_CONFIRM);
            }
        } else {
            tunaide02_last_kakunin_flag = 0;
            if ((trg[1] & WINDOW_KEY_ACCEPT) && !toolBattle_tunaideMenu02menuTempBuffGet(TUNAIDE02_SLOT1_BIT)) {
                toolBattle_tunaideMenu02menuTempBuffSet(TUNAIDE02_SLOT1_BIT);
                menuCB_SE_Play(MENU_SE_CONFIRM);
            }
            if ((trg[2] & WINDOW_KEY_ACCEPT) && !toolBattle_tunaideMenu02menuTempBuffGet(TUNAIDE02_SLOT2_BIT)) {
                toolBattle_tunaideMenu02menuTempBuffSet(TUNAIDE02_SLOT2_BIT);
                menuCB_SE_Play(MENU_SE_CONFIRM);
            }
            if ((trg[1] | trg[2]) & WINDOW_KEY_CANCEL) {
                window->mAccept = 1;
                window->mCancel = 1;
                _CB.mTunaide02Cancel = 1;
                menuCB_SE_Play(MENU_SE_CANCEL);
            }
        }
    } else {
        trg[0] = windowGetPortKeyInfo(ports[toolentryTaisenGetControlerType(0) - 1])->mKeyTrg;
        trg[1] = windowGetPortKeyInfo(ports[toolentryTaisenGetControlerType(1) - 1])->mKeyTrg;
        if (_CB.mTunaide02TempBuff == (TUNAIDE02_SLOT0_BIT | TUNAIDE02_SLOT1_BIT)) {
            tunaide02_last_kakunin_flag = 1;
            if (trg[0] & WINDOW_KEY_CANCEL) {
                window->mAccept = 1;
                window->mCancel = 1;
                _CB.mTunaide02Cancel = 1;
                menuCB_SE_Play(MENU_SE_CANCEL);
            } else if (trg[0] & TUNAIDE02_KEY_START) {
                window->mAccept = 1;
                menuCB_SE_Play(MENU_SE_CONFIRM);
            }
        } else {
            tunaide02_last_kakunin_flag = 0;
            if ((trg[0] & WINDOW_KEY_ACCEPT) && !toolBattle_tunaideMenu02menuTempBuffGet(TUNAIDE02_SLOT0_BIT)) {
                toolBattle_tunaideMenu02menuTempBuffSet(TUNAIDE02_SLOT0_BIT);
                menuCB_SE_Play(MENU_SE_CONFIRM);
            }
            if ((trg[1] & WINDOW_KEY_ACCEPT) && !toolBattle_tunaideMenu02menuTempBuffGet(TUNAIDE02_SLOT1_BIT)) {
                toolBattle_tunaideMenu02menuTempBuffSet(TUNAIDE02_SLOT1_BIT);
                menuCB_SE_Play(MENU_SE_CONFIRM);
            }
            if ((trg[0] | trg[1]) & WINDOW_KEY_CANCEL) {
                window->mAccept = 1;
                window->mCancel = 1;
                _CB.mTunaide02Cancel = 1;
                menuCB_SE_Play(MENU_SE_CANCEL);
            }
        }
    }
}

extern "C" void menuTool_tunaide02_Ctrl(tagWINDOW_WORK *window)
{
    menuCB_DispEffect(window);
    menuSetStatus(window->mMenuID, MENU_STATUS_NO_DEFAULT_INPUT);
    if (menuCB_GetDisconnectedGBA() != 0) {
        _CB.mGbaDisconnected = 1;
        window->mAccept = 1;
        window->mCancel = 1;
    }
}

extern "C" void menuCB_Tunaide01_4_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case RULE_SECOND_OPTION_CHECK_0: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect == RULE_SECOND_OPTION && _CB.mRuleNo == 0); break;
    case RULE_SECOND_OPTION_CHECK_1: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect == RULE_SECOND_OPTION && _CB.mRuleNo == 1); break;
    case RULE_SECOND_OPTION_CHECK_2: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect == RULE_SECOND_OPTION && _CB.mRuleNo == 2); break;
    case RULE_SECOND_OPTION_CHECK_3: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect == RULE_SECOND_OPTION && _CB.mRuleNo == 3); break;
    case RULE_SECOND_OPTION_CHECK_4: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect == RULE_SECOND_OPTION && _CB.mRuleNo == 4); break;
    case RULE_SECOND_OPTION_CHECK_5: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect == RULE_SECOND_OPTION && _CB.mRuleNo == 5); break;
    case RULE_FIRST_OPTION_CHECK_0: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect != RULE_SECOND_OPTION && _CB.mRuleNo == 0); break;
    case RULE_FIRST_OPTION_CHECK_1: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect != RULE_SECOND_OPTION && _CB.mRuleNo == 1); break;
    case RULE_FIRST_OPTION_CHECK_2: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect != RULE_SECOND_OPTION && _CB.mRuleNo == 2); break;
    case RULE_FIRST_OPTION_CHECK_3: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect != RULE_SECOND_OPTION && _CB.mRuleNo == 3); break;
    case RULE_FIRST_OPTION_CHECK_4: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect != RULE_SECOND_OPTION && _CB.mRuleNo == 4); break;
    case RULE_FIRST_OPTION_CHECK_5: menuCB_SpriteDispCheckONorOFF(sprite, _CB.mRuleSelect != RULE_SECOND_OPTION && _CB.mRuleNo == 5); break;
    case RULE_TRAINER_NAME_0:
        menuCBRuleSetSpriteColor(sprite, _CB.mRuleNo == 0);
        menuCBTrainerNameDraw(window, sprite, 0);
        break;
    case RULE_TRAINER_NAME_1:
        menuCBRuleSetSpriteColor(sprite, _CB.mRuleNo == 1);
        menuCBTrainerNameDraw(window, sprite, 1);
        break;
    case RULE_TRAINER_NAME_2:
        menuCBRuleSetSpriteColor(sprite, _CB.mRuleNo == 2);
        menuCBTrainerNameDraw(window, sprite, 2);
        break;
    case RULE_TRAINER_NAME_3:
        menuCBRuleSetSpriteColor(sprite, _CB.mRuleNo == 3);
        menuCBTrainerNameDraw(window, sprite, 3);
        break;
    case RULE_TRAINER_NAME_4:
        menuCBRuleSetSpriteColor(sprite, _CB.mRuleNo == 4);
        menuCBTrainerNameDraw(window, sprite, 4);
        break;
    case RULE_TRAINER_NAME_5:
        menuCBRuleSetSpriteColor(sprite, _CB.mRuleNo == 5);
        menuCBTrainerNameDraw(window, sprite, 5);
        break;
    }
}

extern "C" void menuTool_tunaide01_4_Button(tagWINDOW_WORK *)
{
}

extern "C" void menuTool_tunaide01_4_Ctrl(tagWINDOW_WORK *window)
{
    menuCB_DispEffect(window);
    menuSetStatus(window->mMenuID, MENU_STATUS_NO_DEFAULT_INPUT);
    menuCB_MenuDispCheckONorOFF_TUNAIDESUB(window, RULE_SUBMENU_CURSOR);
}

extern "C" {
void menuCB_Tunaide01_3_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
}

// Disp callback of the tunaide carousel. The window shows one entry of
// the field sprite table; when the selection moves, the two panels are slid past each
// other by _CB.tunaideSlide before the new entry is shown.
void menuCB_Tunaide01_3_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case TUNAIDE_FIELD_ITEM:
        if (_CB.tunaideSlide == FIELD_SLIDE_IDLE) {
            menuCBSpriteSetData(
                sprite, menuSpriteBiosGetPtr(field_sprite[_CB.tunaideSelect]));
            break;
        }
        winSpriteSetDisp(sprite, false);
        if (_CB.tunaideSlide != FIELD_SLIDE_IDLE) {
            switch (_CB.tunaideSlide) {
            case FIELD_SLIDE_FROM_TOP_START:
                field_anime.mPanel.mX = 0;
                field_anime.mPanel.mY = 0;
                field_anime.mPanel.mWidth = 278;
                field_anime.mPanel.mHeight = 0;
                field_anime.mPanel.mColor = 0xFFFFFFFF;
                field_anime.mPanel.mSpriteID =
                    (u16)field_sprite[_CB.tunaideSelect];
                field_anime.mPanel.mSpriteX = 0;
                field_anime.mPanel.mSpriteY = 167;
                field_anime.mPanel.mFlags = 0;
                field_anime.mOutgoing.mX = 0;
                field_anime.mOutgoing.mY = 0;
                field_anime.mOutgoing.mWidth = 278;
                field_anime.mOutgoing.mHeight = 167;
                field_anime.mOutgoing.mColor = 0xFFFFFFFF;
                field_anime.mOutgoing.mSpriteID =
                    (u16)field_sprite[_CB.tunaideSelectPrev];
                field_anime.mOutgoing.mSpriteX = 0;
                field_anime.mOutgoing.mSpriteY = 0;
                field_anime.mOutgoing.mFlags = 0;
                _CB.tunaideSlide = FIELD_SLIDE_FROM_TOP;
                break;
            case FIELD_SLIDE_FROM_BOTTOM_START:
                field_anime.mPanel.mX = 0;
                field_anime.mPanel.mY = 167;
                field_anime.mPanel.mWidth = 278;
                field_anime.mPanel.mHeight = 0;
                field_anime.mPanel.mColor = 0xFFFFFFFF;
                field_anime.mPanel.mSpriteID =
                    (u16)field_sprite[_CB.tunaideSelect];
                field_anime.mPanel.mSpriteX = 0;
                field_anime.mPanel.mSpriteY = 0;
                field_anime.mPanel.mFlags = 0;
                field_anime.mOutgoing.mX = 0;
                field_anime.mOutgoing.mY = 0;
                field_anime.mOutgoing.mWidth = 278;
                field_anime.mOutgoing.mHeight = 167;
                field_anime.mOutgoing.mColor = 0xFFFFFFFF;
                field_anime.mOutgoing.mSpriteID =
                    (u16)field_sprite[_CB.tunaideSelectPrev];
                field_anime.mOutgoing.mSpriteX = 0;
                field_anime.mOutgoing.mSpriteY = 0;
                field_anime.mOutgoing.mFlags = 0;
                _CB.tunaideSlide = FIELD_SLIDE_FROM_BOTTOM;
                break;
            case FIELD_SLIDE_FROM_TOP:
                field_anime.mPanel.mHeight += 15;
                if (field_anime.mPanel.mHeight >= 167) {
                    field_anime.mPanel.mHeight = 0;
                    _CB.tunaideSlide = FIELD_SLIDE_IDLE;
                    winSpriteSetDisp(sprite, true);
                    menuCBSpriteSetData(
                        sprite,
                        menuSpriteBiosGetPtr(field_sprite[_CB.tunaideSelect]));
                }
                {
                    FIELD_ANIME_PANEL *panel = &field_anime.mPanel;
                    panel->mSpriteY = 167 - panel->mHeight;
                    field_anime.mOutgoing.mY = panel->mHeight;
                    field_anime.mOutgoing.mHeight = 167 - panel->mHeight;
                }
                break;
            case FIELD_SLIDE_FROM_BOTTOM:
                field_anime.mPanel.mHeight += 15;
                if (field_anime.mPanel.mHeight >= 167) {
                    field_anime.mPanel.mHeight = 0;
                    _CB.tunaideSlide = FIELD_SLIDE_IDLE;
                    winSpriteSetDisp(sprite, true);
                    menuCBSpriteSetData(
                        sprite,
                        menuSpriteBiosGetPtr(field_sprite[_CB.tunaideSelect]));
                }
                {
                    FIELD_ANIME_PANEL *panel = &field_anime.mPanel;
                    panel->mY = 167 - panel->mHeight;
                    field_anime.mOutgoing.mSpriteY = panel->mHeight;
                    field_anime.mOutgoing.mHeight = 167 - panel->mHeight;
                }
                break;
            }
        }
        break;
    case TUNAIDE_FIELD_INCOMING_ITEM:
        if (_CB.tunaideSlide != FIELD_SLIDE_IDLE) {
            windowDrawSprite3(field_anime.mPanel.mX, field_anime.mPanel.mY,
                        field_anime.mPanel.mWidth, field_anime.mPanel.mHeight,
                        field_anime.mPanel.mColor, window,
                        field_anime.mPanel.mSpriteID,
                        field_anime.mPanel.mSpriteX,
                        field_anime.mPanel.mSpriteY,
                        field_anime.mPanel.mWidth,
                        field_anime.mPanel.mHeight,
                        field_anime.mPanel.mFlags, window->mLayerID);
        }
        break;
    case TUNAIDE_FIELD_OUTGOING_ITEM:
        if (_CB.tunaideSlide != FIELD_SLIDE_IDLE) {
            windowDrawSprite3(field_anime.mOutgoing.mX,
                        field_anime.mOutgoing.mY,
                        field_anime.mOutgoing.mWidth,
                        field_anime.mOutgoing.mHeight,
                        field_anime.mOutgoing.mColor, window,
                        field_anime.mOutgoing.mSpriteID,
                        field_anime.mOutgoing.mSpriteX,
                        field_anime.mOutgoing.mSpriteY,
                        field_anime.mOutgoing.mWidth,
                        field_anime.mOutgoing.mHeight,
                        field_anime.mOutgoing.mFlags, window->mLayerID);
        }
        break;
    case TUNAIDE_FIELD_NEXT_ARROW_ITEM:
        if (_CB.mRuleSelect != TUNAIDE_RULE_SELECT_LAST) {
            sprite->mRgba.mAlpha = 127;
        }
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.tunaideSelect != TUNAIDE_FIELD_LAST);
        break;
    case TUNAIDE_FIELD_PREVIOUS_ARROW_ITEM:
        if (_CB.mRuleSelect != TUNAIDE_RULE_SELECT_LAST) {
            sprite->mRgba.mAlpha = 127;
        }
        menuCB_SpriteDispCheckONorOFF(sprite, _CB.tunaideSelect != TUNAIDE_FIELD_FIRST);
        break;
    case TUNAIDE_CURSOR_ITEM_FIRST:
    case TUNAIDE_CURSOR_ITEM_SECOND:
    case TUNAIDE_CURSOR_ITEM_THIRD:
        if (_CB.tunaideSlide != FIELD_SLIDE_IDLE) {
            winSpriteSetDisp(sprite, false);
        } else {
            winSpriteSetDisp(sprite, true);
            menuCB_Cursol_Disp_AlphaSet(window, sprite, _CB.mRuleSelect == TUNAIDE_RULE_SELECT_LAST);
        }
        break;
    }
}
