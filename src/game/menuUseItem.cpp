/*
 * menuUseItem.cpp
 *
 * The item-use menus: the pocket item handlers for the field and for battle,
 * the message helpers they print through, the level-up diff the stat-raising
 * items show, and the waza-machine (TM/HM) helpers.
 */
#include <menuOffScreen.h>
#include <menuUseItem.h>
#include <menuItemMessages.h>
#include <menuIds.h>
#include <winMsg.h>
#include <PokemonData.h>
#include <GSmsg.h>
#include <itemData.h>
#include <gsFlag.h>
#include <itemUse2Pokemon.h>
#include <fightFloor.h>
#include <fightEncountData.h>
#include <gamedata.h>
#include <Pokemon.h>
#include <waza.h>
#include <evolution.h>
#include <fightTrainer.h>
#include <hero.h>
#include <menuPocket2.h>
#include <menuPokemonStatus.h>
#include <menuPokemon.h>
#include <GSsnd.h>
#include <sound.h>
#include <menuSub.h>
#include <winSprite.h>
#include <types.h>
#include <fade.h>
#include <msgctrl.h>
#include <GSthread.h>
#include <GSgfx.h>
#include <pokemonBios.h>
#include <pcbox.h>
#include <itemParam.h>
#include <GSinput.h>
#include <menuScript.h>
#include <reliveHall.h>
#include <underTakerBios.h>
#include <place.h>

static const f32 cCologneRumbleWaitSeconds = 1.0f;
static const f32 cCologneLastRumbleWaitSeconds = 1.6f;
static const f32 cEvolutionFadeSeconds = 0.5f;

/* Messages the item-use menus print. */
enum {
    MSG_RADAR_NEAR = 0x3C1C,
    MSG_RADAR_NEARBY = 0x3C1D,
    MSG_RADAR_FAR = 0x3C1E,
    MSG_RADAR_PLACE = 0x3C1F,
    MSG_RADAR_LOCKED = 0x3C31,
    MSG_RADAR_LOCKED2 = 0x3C32,
    MSG_ITEM_USE_LEVEL_UP = 0xC367,
    MSG_WAZA_MACHINE_KNOWN = 0xC368,
    MSG_WAZA_MACHINE_NO = 0xC369,
    MSG_POKEMON_IS_SHADOW = 0xC371,
    MSG_POKEMON_REVERSE_MODE = 0xC372, // refusal when the target is in reverse mode
    MSG_ITEM_NO_USE = 0xC373,
    MSG_WAZA_MACHINE_TM = 0xC375,
    MSG_WAZA_MACHINE_HM = 0xC376,
    MSG_WAZA_MACHINE_ASK = 0xC377,
    MSG_BALL_TRAINER = 0xC378,
    MSG_ITEM_NO_POKECOLO = 0xC380,
    MSG_COLOGNE_USED = 0xC3B7,       // the cologne was sprayed
    MSG_COLOGNE_REVERSE_CLEARED = 0xC3BE, // reverse mode was cleared
    MSG_JOY_SCENT_SPRAYED = 0xC3B6,  // the joy scent was sprayed
    MSG_JOY_SCENT_DP_EFFECT = 0xC3B8,  // printed after the joy scent's dark-point (DP) filter ran
    MSG_EXCITE_SCENT_SPRAYED = 0xC3BC, // the excite scent was sprayed
    MSG_EXCITE_SCENT_DP_EFFECT = 0xC3B9, // printed after the excite scent's dark-point (DP) filter ran
    MSG_VIVID_SCENT_SPRAYED = 0xC3BD,  // the vivid scent was sprayed
    MSG_VIVID_SCENT_DP_EFFECT = 0xC3BA,  // printed after the vivid scent's dark-point (DP) filter ran
    MSG_CRANE_MEMO_FIRST = 0x3C20,       // the first crane memo was read
    MSG_CRANE_MEMO_SECOND = 0x3C21,
    MSG_CRANE_MEMO_THIRD = 0x3C22,
    MSG_CRANE_MEMO_FOURTH = 0x3C23,
    MSG_CRANE_MEMO_FIFTH = 0x3C24,
    MSG_ITEM_RECOVERED = 0x4289,
    MSG_BALL_NO_ROOM = 0x4F8B,
    MSG_BALL_WRONG_BATTLE = 0x5000,
};

/* Sound effects the menus play. */
enum {
    SE_ITEM_USE = 0x1F,
    SE_ITEM_RECOVER = 0x20,
    SE_WAZA_MACHINE = 0x23,
    SE_COLOGNE = 0x444,
    ME_LEVEL_UP = 0x3C7,
};

/* Message-control slots the item-use messages fill in before printing. */
enum {
    MSG_CTRL_NICKNAME = 0x32,    // the target's nickname
    MSG_CTRL_VALUE    = 0x2F,    // one stat the item raises
    MSG_CTRL_VALUE2   = 0x30,    // the other stat it raises
    MSG_CTRL_WAZA     = 0x39,    // the waza a machine teaches
    MSG_CTRL_TEXT     = 0x4D,    // the text the message inserts: a place name or a built-up line
    MSG_CTRL_ITEM     = 0x2D,    // the item being used
};

/* Flags the radar reads: the places the player has already locked in. */
#define FLAG_RADAR_PLACE_FIRST 0x8ED
#define FLAG_RADAR_PLACE_SECOND 0x8EE

/* Shadow Pokemon reverse-mode status (joutai). Set while a Shadow Pokemon is in
 * reverse mode (DarkPokemon::isReversMode); cleared via DarkPokemon::setReversMode. */
#define JOUTAI_REVERSE_MODE 0x3E

/* Item script queued for the cologne once its field effect has run. The high
 * half is the field-script group (the same 0x596 base worldMapMoveFadeScriptID
 * uses, see WorldMap.cpp); the low half selects this script within it. */
#define ITEM_SCRIPT_COLOGNE_FIELD_EFFECT 0x5960011

/* Status ids pokemonGetStatus reports values through. */
#define POKEMON_STATUS_HP 0x83
#define POKEMON_STATUS_HP_MAX 0x87
#define POKEMON_STATUS_ATTACK 0x88
#define POKEMON_STATUS_DEFENSE 0x89
#define POKEMON_STATUS_SPEED 0x8C
#define POKEMON_STATUS_SP_ATTACK 0x8A
#define POKEMON_STATUS_SP_DEFENSE 0x8B

/* The cologne leaves no dark points (DP) on this species. */
#define SPECIES_COLOGNE_DP_EXEMPT 0xF9

/* Dark-point (DP) filter the cologne spray runs: the used item id first, then
 * the filter selector. Selector 4 is the rubbing (nadenade) filter. */
#define DP_FILTER_NADENADE 4

/* Rumble durations the cologne spray requests: two short shakes, then a longer one. */
#define COLOGNE_RUMBLE_DURATION 0x15
#define COLOGNE_RUMBLE_LONG_DURATION 0x30

/* Point-up excludes the hidden shadow moves, but not the special shadow move. */
#define WAZA_POINT_UP_LAST_ALLOWED 0x163
#define WAZA_POINT_UP_RESTRICTED_END WAZA_SHADOW_SPECIAL

/* The cologne scents: each entry below pairs one scent item with the two
 * messages it prints while it is sprayed. */
#define ITEM_JOY_SCENT 0x201
#define ITEM_EXCITE_SCENT 0x202
#define ITEM_VIVID_SCENT 0x203

/* The crane memos: the memo number in the table below is the entry's own
 * index, so the items are named by the memo they print. */
#define ITEM_CRANE_MEMO_FIRST 0x20B
#define ITEM_CRANE_MEMO_SECOND 0x20C
#define ITEM_CRANE_MEMO_THIRD 0x20D
#define ITEM_CRANE_MEMO_FOURTH 0x20E
#define ITEM_CRANE_MEMO_FIFTH 0x20F

/* The flutes: recovery items that stay in the pocket when they are used. */
#define ITEM_BLUE_FLUTE 0x27
#define ITEM_YELLOW_FLUTE 0x28
#define ITEM_RED_FLUTE 0x29
#define ITEM_BLACK_FLUTE 0x2A
#define ITEM_WHITE_FLUTE 0x2B

/* Recover types itemParamGetRecoverType reports. That routine probes the item
 * parameter in turn and reports the first thing it finds: 0x03 to 0x09 are
 * single-bit probes (of which more than one set reports
 * RECOVER_TYPE_MULTIPLE_STATUS), 0x0B to 0x11 test the recovery bytes whole,
 * and the ids past 0x11 are further single flags. This is a different id space
 * from the effect ids the item-use log is filled with below. */
#define RECOVER_TYPE_NONE 0x00       // nothing the item can recover
#define RECOVER_TYPE_REVIVE_ALL 0x01 // revives every fainted party member
#define RECOVER_TYPE_LEVEL_UP 0x02  // raises the level
#define RECOVER_TYPE_SLEEP 0x03      // the item wakes a sleeping pokemon
#define RECOVER_TYPE_POISON 0x04     // the item clears poison
/* The remaining status bits, named after the accessors that read them. */
#define RECOVER_TYPE_BURN 0x05
#define RECOVER_TYPE_FREEZE 0x06
#define RECOVER_TYPE_PARALYZE 0x07
#define RECOVER_TYPE_CONFUSE 0x08
#define RECOVER_TYPE_MULTIPLE_STATUS 0x0A // more than one of the flags above
/* Clears infatuation in battle. */
#define RECOVER_TYPE_INFATUATION 0x09
/* HP restoration and stat-effort increases share the field recovery handler.
 * Battle permits HP restoration here, but not effort increases. */
#define RECOVER_TYPE_HP 0x0B
#define RECOVER_TYPE_EFFORT_ATTACK_UP 0x0C
#define RECOVER_TYPE_EFFORT_MAX_HP_UP 0x0D
#define RECOVER_TYPE_EFFORT_SP_ATTACK_UP 0x0E
#define RECOVER_TYPE_EFFORT_SP_DEFENSE_UP 0x0F
#define RECOVER_TYPE_EFFORT_SPEED_UP 0x10
#define RECOVER_TYPE_EFFORT_DEFENSE_UP 0x11
/* The evolution-stone flag; the nazonomi dispatchers do not route it because
 * these items are used through the evolution handler instead. */
#define RECOVER_TYPE_EVOLUTION 0x12
/* 0x13 tests the same parameter bit as itemParamGetPPMaxUpFlag and 0x14 the
 * bit itemParamGetPPMaxFullFlag reads; both reach menuUseItemWPointUp, which
 * raises a move's maximum PP. */
#define RECOVER_TYPE_PP_MAX_UP 0x13
#define RECOVER_TYPE_PP_MAX_FULL 0x14
#define RECOVER_TYPE_PP 0x15           // restores PP
#define RECOVER_TYPE_NO_FLAG 0x16     // the item carries no recover flag

/* Entry counts of the tables the item handlers scan, and of the item id list
 * the recover items are read from. */
#define COLOGNE_TABLE_COUNT 3
#define FIELD_LOG_TABLE_COUNT 21
#define BATTLE_LOG_TABLE_COUNT 7
#define RECOVER_ITEM_COUNT 5

/* Third column of the field log table: the group an effect is reported in.
 * Effects that belong to one group share a single message; an effect with
 * FIELD_LOG_GROUP_NONE is always reported with a message of its own. */
#define FIELD_LOG_GROUP_NONE (-1)
#define FIELD_LOG_GROUP_SLEEP 0    // the sleep curing effects
#define FIELD_LOG_GROUP_POISON 1   // the poison curing effects
#define FIELD_LOG_GROUP_BURN 2     // the burn curing effect
#define FIELD_LOG_GROUP_FREEZE 3   // the freeze curing effect
#define FIELD_LOG_GROUP_PARALYZE 4 // the paralysis curing effect
#define FIELD_LOG_GROUP_CONFUSE 5  // the confusion curing effect
#define FIELD_LOG_GROUP_INFATUATION 6   // infatuation cured
#define FIELD_LOG_GROUP_HP 7       // the HP restoring effect

/* First column of the two log tables: the effect ids itemUse2Pokemon and
 * hpRecover write into the log, named after what the item did. */
#define ITEMUSE_EFFECT_CURE_INFATUATION 0x00          // infatuation cured
#define ITEMUSE_EFFECT_FOCUS_ENERGY 0x01   // raises the critical-hit ratio
#define ITEMUSE_EFFECT_ATTACK_UP 0x02       // its attack is raised
#define ITEMUSE_EFFECT_DEFENCE_UP 0x03      // its defence is raised
#define ITEMUSE_EFFECT_SPEED_UP 0x04        // its speed is raised
#define ITEMUSE_EFFECT_ACCURACY_UP 0x05     // the accuracy of its moves is raised
#define ITEMUSE_EFFECT_SP_ATTACK_UP 0x06    // its sp. attack is raised
#define ITEMUSE_EFFECT_GUARD 0x07           // it guards itself
#define ITEMUSE_EFFECT_LEVEL_UP 0x08        // it gains a level
#define ITEMUSE_EFFECT_SLEEP_CURED 0x09     // it is woken up
#define ITEMUSE_EFFECT_SLEEP_CURED_BATTLE 0x0A // the pokemon fighting is woken up
#define ITEMUSE_EFFECT_POISON_CURED 0x0B    // it is cured of poison
#define ITEMUSE_EFFECT_POISON_CURED_BATTLE 0x0C // the pokemon fighting is cured
/* Reported with the other poison curing effects. */
#define ITEMUSE_EFFECT_POISON_CURED_UNKNOWN 0x0D
#define ITEMUSE_EFFECT_BURN_CURED 0x0E     // it is cured of its burn
#define ITEMUSE_EFFECT_FREEZE_CURED 0x0F   // it is thawed
#define ITEMUSE_EFFECT_PARALYZE_CURED 0x10 // it is cured of paralysis
#define ITEMUSE_EFFECT_CONFUSE_CURED 0x11  // it is snapped out of confusion
#define ITEMUSE_EFFECT_PP_MAX_UP 0x12      // the PP of one of its moves grows
#define ITEMUSE_EFFECT_EFFORT_MAX_HP_UP 0x13    // its max HP effort grows
#define ITEMUSE_EFFECT_EFFORT_ATTACK_UP 0x14   // its attack effort grows
#define ITEMUSE_EFFECT_HP_RECOVERED 0x15   // HP is restored, value the amount
#define ITEMUSE_EFFECT_RECOVERED_FROM_FAINT 0x16 // a fainted pokemon is revived
#define ITEMUSE_EFFECT_PP_RESTORED_SELECT 0x17  // the PP of the chosen move
#define ITEMUSE_EFFECT_EVOLUTION 0x18      // it evolves
#define ITEMUSE_EFFECT_EFFORT_DEFENCE_UP 0x19   // its defence effort grows
#define ITEMUSE_EFFECT_EFFORT_SPEED_UP 0x1A     // its speed effort grows
#define ITEMUSE_EFFECT_EFFORT_SP_DEFENCE_UP 0x1B // its sp. defence effort grows
#define ITEMUSE_EFFECT_EFFORT_SP_ATTACK_UP 0x1C  // its sp. attack effort grows
#define ITEMUSE_EFFECT_PP_FULLY_RESTORED 0x1D    // the PP of every move is full
#define ITEMUSE_EFFECT_FRIEND_UP 0x1E      // it grows closer to its trainer

/* Messages the field log table prints, named after the effect they report. */
#define MSG_HP_RECOVERED 0xC351
#define MSG_RECOVERED_FROM_FAINT 0xC352
#define MSG_SLEEP_CURED 0xC353
#define MSG_POISON_CURED 0xC354
#define MSG_BURN_CURED 0xC355
#define MSG_FREEZE_CURED 0xC356
#define MSG_PARALYZE_CURED 0xC357
#define MSG_CURE_INFATUATION 0xC358
#define MSG_MULTIPLE_EFFECTS 0xC359
#define MSG_EFFORT_ATTACK_UP 0xC35A
#define MSG_EFFORT_MAX_HP_UP 0xC35B
#define MSG_EFFORT_SP_ATTACK_UP 0xC35C
#define MSG_EFFORT_SP_DEFENCE_UP 0xC35D
#define MSG_EFFORT_SPEED_UP 0xC35E
#define MSG_EFFORT_DEFENCE_UP 0xC35F
#define MSG_PP_RESTORED 0xC360
#define MSG_PP_RESTORED_SELECT 0xC361
#define MSG_CONFUSE_CURED 0xC381

/* Second column of the battle log table. */
#define MSG_BATTLE_GUARD 0xC362
#define MSG_BATTLE_ACCURACY_UP 0xC363
#define MSG_BATTLE_SP_ATTACK_UP 0xC364
#define MSG_BATTLE_DEFENCE_UP 0xC365
#define MSG_BATTLE_SPEED_UP 0xC366
#define MSG_BATTLE_ATTACK_UP 0xC36F
#define MSG_BATTLE_FOCUS_ENERGY 0xC370

/* First column of the battle log table: the same effects as the field table
 * numbers them, minus the HP restoring one, which has no battle message. */
#define BATTLE_EFFECT_FOCUS_ENERGY 1
#define BATTLE_EFFECT_ATTACK_UP 2
#define BATTLE_EFFECT_DEFENCE_UP 3
#define BATTLE_EFFECT_SPEED_UP 4
#define BATTLE_EFFECT_ACCURACY_UP 5
#define BATTLE_EFFECT_SP_ATTACK_UP 6
#define BATTLE_EFFECT_GUARD 7

/* One entry of the cologne table: the item, the message it starts with and
 * the message the dark-point (DP) effect prints. */
struct COLOGNE_MSG {
    u16 mItemID;    // 0x00: the cologne item.
    u16 _02;        // 0x02: padding.
    u32 mMsgID;     // 0x04: message printed when the cologne is sprayed.
    u32 mEffectMsg; // 0x08: message printed when the dark points (DP) rose.
};

/* Effect log tables the item handlers scan: the cologne items, the effects a
 * field item can leave on a pokemon and the effects a battle item can leave on
 * one. */

/* The cologne items and the two messages each one plays while it is sprayed. */
struct COLOGNE_TABLE {
    COLOGNE_MSG mEntry[COLOGNE_TABLE_COUNT];
};

/* One entry of a field effect table: the effect, its message and the group the
 * effect is reported in, or -1 when it is never reported as a group. */
struct FIELD_LOG_ENTRY {
    s32 mEffect;
    s32 mMsgID;
    s32 mGroup;
};

struct FIELD_LOG_TABLE {
    FIELD_LOG_ENTRY mEntry[FIELD_LOG_TABLE_COUNT];
};

/* One entry of a battle effect table: the effect and its message. */
struct BATTLE_LOG_ENTRY {
    s32 mEffect;
    s32 mMsgID;
};

struct BATTLE_LOG_TABLE {
    BATTLE_LOG_ENTRY mEntry[BATTLE_LOG_TABLE_COUNT];
};

static const COLOGNE_TABLE cCologneMsgs = {
    {
        {ITEM_JOY_SCENT, 0, MSG_JOY_SCENT_SPRAYED, MSG_JOY_SCENT_DP_EFFECT},
        {ITEM_EXCITE_SCENT, 0, MSG_EXCITE_SCENT_SPRAYED, MSG_EXCITE_SCENT_DP_EFFECT},
        {ITEM_VIVID_SCENT, 0, MSG_VIVID_SCENT_SPRAYED, MSG_VIVID_SCENT_DP_EFFECT},
    },
};

static const FIELD_LOG_TABLE cFieldLogMsgs = {
    {
        {ITEMUSE_EFFECT_SLEEP_CURED, MSG_SLEEP_CURED, FIELD_LOG_GROUP_SLEEP},
        {ITEMUSE_EFFECT_SLEEP_CURED_BATTLE, MSG_SLEEP_CURED, FIELD_LOG_GROUP_SLEEP},
        {ITEMUSE_EFFECT_POISON_CURED, MSG_POISON_CURED, FIELD_LOG_GROUP_POISON},
        {ITEMUSE_EFFECT_POISON_CURED_BATTLE, MSG_POISON_CURED, FIELD_LOG_GROUP_POISON},
        {ITEMUSE_EFFECT_POISON_CURED_UNKNOWN, MSG_POISON_CURED, FIELD_LOG_GROUP_POISON},
        {ITEMUSE_EFFECT_BURN_CURED, MSG_BURN_CURED, FIELD_LOG_GROUP_BURN},
        {ITEMUSE_EFFECT_FREEZE_CURED, MSG_FREEZE_CURED, FIELD_LOG_GROUP_FREEZE},
        {ITEMUSE_EFFECT_PARALYZE_CURED, MSG_PARALYZE_CURED, FIELD_LOG_GROUP_PARALYZE},
        {ITEMUSE_EFFECT_CONFUSE_CURED, MSG_CONFUSE_CURED, FIELD_LOG_GROUP_CONFUSE},
        {ITEMUSE_EFFECT_CURE_INFATUATION, MSG_CURE_INFATUATION, FIELD_LOG_GROUP_INFATUATION},
        {ITEMUSE_EFFECT_RECOVERED_FROM_FAINT, MSG_RECOVERED_FROM_FAINT, FIELD_LOG_GROUP_NONE},
        {ITEMUSE_EFFECT_HP_RECOVERED, MSG_HP_RECOVERED, FIELD_LOG_GROUP_HP},
        {ITEMUSE_EFFECT_EFFORT_ATTACK_UP, MSG_EFFORT_ATTACK_UP, FIELD_LOG_GROUP_NONE},
        {ITEMUSE_EFFECT_EFFORT_MAX_HP_UP, MSG_EFFORT_MAX_HP_UP, FIELD_LOG_GROUP_NONE},
        {ITEMUSE_EFFECT_EFFORT_SP_ATTACK_UP, MSG_EFFORT_SP_ATTACK_UP, FIELD_LOG_GROUP_NONE},
        {ITEMUSE_EFFECT_EFFORT_SP_DEFENCE_UP, MSG_EFFORT_SP_DEFENCE_UP, FIELD_LOG_GROUP_NONE},
        {ITEMUSE_EFFECT_EFFORT_SPEED_UP, MSG_EFFORT_SPEED_UP, FIELD_LOG_GROUP_NONE},
        {ITEMUSE_EFFECT_EFFORT_DEFENCE_UP, MSG_EFFORT_DEFENCE_UP, FIELD_LOG_GROUP_NONE},
        {ITEMUSE_EFFECT_PP_MAX_UP, MSG_PP_RESTORED, FIELD_LOG_GROUP_NONE},
        {ITEMUSE_EFFECT_PP_FULLY_RESTORED, MSG_PP_RESTORED, FIELD_LOG_GROUP_NONE},
        {ITEMUSE_EFFECT_PP_RESTORED_SELECT, MSG_PP_RESTORED_SELECT, FIELD_LOG_GROUP_NONE},
    },
};

static const BATTLE_LOG_TABLE cBattleLogMsgs = {
    {
        {BATTLE_EFFECT_GUARD, MSG_BATTLE_GUARD},
        {BATTLE_EFFECT_ACCURACY_UP, MSG_BATTLE_ACCURACY_UP},
        {BATTLE_EFFECT_SP_ATTACK_UP, MSG_BATTLE_SP_ATTACK_UP},
        {BATTLE_EFFECT_DEFENCE_UP, MSG_BATTLE_DEFENCE_UP},
        {BATTLE_EFFECT_SPEED_UP, MSG_BATTLE_SPEED_UP},
        {BATTLE_EFFECT_FOCUS_ENERGY, MSG_BATTLE_FOCUS_ENERGY},
        {BATTLE_EFFECT_ATTACK_UP, MSG_BATTLE_ATTACK_UP},
    },
};

/* One entry of the crane-memo item table: the item and the message it prints. */
struct ITEM_CRANE_MEMO {
    u16 mItemID; // 0x00: the item the entry belongs to.
    u16 mMemoNo; // 0x02: the memo number, counting up with the table.
    u32 mMsgID;  // 0x04: message printed when the item is used.
};

/* Number of entries of the crane-memo item table. */
u32 item_crane_memo_number = 5;
/* Function of this file with C++ linkage that is used before it is defined. */
s32 cbForgetWazaSelect(Pokemon *pokemon, u16 wazaID, long context);
/* Stats record of a pokemon, or null when there is no such pokemon. */
PokemonSpeciesStats *getPokemonData(Pokemon *pokemon)
{
    if (pokemon == 0) {
        return 0;
    }
    if (pokemonCheckValid(pokemon) == 0) {
        return 0;
    }
    return pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon));
}

/* Play the cry of a pokemon, and return the sound id that was started. */
s32 playPokemonVoice(Pokemon *pokemon)
{
    PokemonSpeciesStats *stats = getPokemonData(pokemon);

    if (stats == 0) {
        return 0;
    }
    u32 cryID = (u16)pokemonDataBiosGetVoice(stats);
    GSsndPlayNow(cryID);
    return cryID;
}

/* Wait out `time` seconds, yielding once per frame until as many frames have
 * passed as `time` seconds holds at 60 fps. */
static void waitTime(f32 time)
{
    s32 frames = (s32)(60.0f * time);
    s32 elapsed = 0;

    while (elapsed < frames) {
        GSthreadSwitch();
        elapsed += GSgfxVideoGetLastRenderTime();
    }
}

static bool pokemonHasDarkPoints(Pokemon *pokemon)
{
    return pokemonGetDp(pokemon) > 0.0f;
}

/* Play the sound effect of spending an item: a recovery item gets its own
 * sound, every other item the plain item-spent one. */
void playRecoverSE(unsigned long itemID)
{
    u16 recoverItems[RECOVER_ITEM_COUNT] = {
        ITEM_BLUE_FLUTE, ITEM_YELLOW_FLUTE, ITEM_RED_FLUTE,
        ITEM_BLACK_FLUTE, ITEM_WHITE_FLUTE,
    };
    s32 index;

    for (index = 0; index < RECOVER_ITEM_COUNT; index++) {
        if (recoverItems[index] == itemID) {
            break;
        }
    }
    GSsndPlayNow(index < RECOVER_ITEM_COUNT ? SE_ITEM_RECOVER : SE_ITEM_USE);
}


/* Message of the battle effect log: the message of the first logged effect
 * and the two values that go with it. */
void getLogMsgBattle(u16 *msg, int maxLength, ITEMUSE2POKEMON_LOG1 *log, s16 count, Pokemon *pokemon)
{
    BATTLE_LOG_TABLE table = cBattleLogMsgs;
    s32 j;
    s32 i;

    if (count <= 0) {
        GSmsgMakeGScharStr(msg, maxLength + 1, MSG_ITEM_NO_EFFECT);
        return;
    }
    for (i = 0; i < BATTLE_LOG_TABLE_COUNT; i++) {
        s32 effect = table.mEntry[i].mEffect;

        for (j = 0; j < count; j++) {
            if (effect == log[j].mEffect) {
                break;
            }
        }
        if (j < count) {
            break;
        }
    }
    if (i >= BATTLE_LOG_TABLE_COUNT) {
        GSmsgMakeGScharStr(msg, maxLength + 1, MSG_ITEM_NO_EFFECT);
        return;
    }
    s16 value;
    s16 value2;
    unsigned long nickname;
    s32 effectMsg;

    effectMsg = table.mEntry[i].mMsgID;
    value = log[j].mValue;
    value2 = log[j].mValue2;
    nickname = (unsigned long)pokemon_GetNickname(pokemon);
    msgctrlSetValue(MSG_CTRL_NICKNAME, nickname);
    msgctrlSetValue(MSG_CTRL_VALUE, value);
    msgctrlSetValue(MSG_CTRL_VALUE2, value2);
    GSmsgMakeGScharStr(msg, maxLength + 1, effectMsg);
}

/* Message of the field effect log, the same way as the battle one. An effect
 * that belongs to a group is reported with its own message, but when an effect
 * of another group is in the log as well, MSG_MULTIPLE_EFFECTS replaces it. */
void getLogMsgField(u16 *msg, int maxLength, ITEMUSE2POKEMON_LOG1 *log, s16 count, Pokemon *pokemon)
{
    s32 j;
    s32 i;
    s32 group;
    s32 effect;
    s16 value;
    s16 value2;
    unsigned long nickname;
    s32 msgID;
    FIELD_LOG_TABLE table = cFieldLogMsgs;

    if (count <= 0) {
        GSmsgMakeGScharStr(msg, maxLength + 1, MSG_ITEM_NO_EFFECT);
        return;
    }
    for (i = 0; i < FIELD_LOG_TABLE_COUNT; i++) {
        effect = table.mEntry[i].mEffect;

        for (j = 0; j < count; j++) {
            if (effect == log[j].mEffect) {
                break;
            }
        }
        if (j < count) {
            break;
        }
    }
    if (i >= FIELD_LOG_TABLE_COUNT) {
        GSmsgMakeGScharStr(msg, maxLength + 1, MSG_ITEM_NO_EFFECT);
        return;
    }
    msgID = table.mEntry[i].mMsgID;
    value = log[j].mValue;
    value2 = log[j].mValue2;
    group = table.mEntry[i].mGroup;
    if (group >= 0) {
        ++i;
        for (; i < FIELD_LOG_TABLE_COUNT; i++) {
            if (table.mEntry[i].mGroup < 0) {
                continue;
            }
            if (table.mEntry[i].mGroup == group) {
                continue;
            }
            effect = table.mEntry[i].mEffect;

            for (j = 0; j < count; j++) {
                if (effect == log[j].mEffect) {
                    break;
                }
            }
            if (j < count) {
                break;
            }
        }
        if (i < FIELD_LOG_TABLE_COUNT) {
            msgID = MSG_MULTIPLE_EFFECTS;
        }
    }
    nickname = (unsigned long)pokemon_GetNickname(pokemon);
    msgctrlSetValue(MSG_CTRL_NICKNAME, nickname);
    msgctrlSetValue(MSG_CTRL_VALUE, value);
    msgctrlSetValue(MSG_CTRL_VALUE2, value2);
    GSmsgMakeGScharStr(msg, maxLength + 1, msgID);
}

/* Ask the player a Yes/No question. Returns 0 on yes, 1 on no and 2 when the
 * question was cancelled. */
s32 dispMsgYesNo(unsigned long msgID)
{
    winMsgOpen(2, msgID, 1, 0);
    s8 answer = (s8)menuSubOpenYesNo(0, -1, -1, false);
    winMsgClose(1);
    switch (answer) {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return 2;
    }
}

/* Print a message and wait for the player to read it. */
void dispMsg(unsigned long msgID)
{
    switch (msgID) {
    case MSG_WAZA_MACHINE_KNOWN:
    case MSG_WAZA_MACHINE_NO:
    case MSG_POKEMON_IS_SHADOW:
    case MSG_POKEMON_REVERSE_MODE:
        GSsndPlayNow(MENU_SE_ERROR);
        break;
    }
    if (windowSearchID(MENU_POKEMON_LIST) == 0) {
        winMsgOpen(2, msgID, 1, 0);
    } else {
        winMsgOpen(2, msgID, 0, 0);
        do {
            menuOffScreenSetPriority(0);
            GSthreadSwitch();
        } while ((s8)winMsgCheck() != 0);
    }
    winMsgClose(1);
}

/* -------------------------------------------------------------------------
 * Item handlers
 * ---------------------------------------------------------------------- */

#pragma force_active on
extern "C" int menuUseItemEventUsohachiPhoto(unsigned long itemID, int *consumed)
{
    *consumed = 0;
    menuScriptUsohachi2();
    return ITEM_USE_EVENT;
}

extern "C" int menuUseItemEventMirraboradar(unsigned long itemID, int *consumed)
{
    u32 msgID = 1;
    u32 placeLevel;
    unsigned long placeName;
    underTakerBios bios;

    *consumed = 0;
    if (GSflagGet(FLAG_RADAR_PLACE_FIRST)) {
        msgID = MSG_RADAR_LOCKED;
    } else if (GSflagGet(FLAG_RADAR_PLACE_SECOND)) {
        msgID = MSG_RADAR_LOCKED2;
    } else {
        placeLevel = GSflagGet(gPlaceWork->mPlaceFlag) & 0xFFFF;
        if (placeLevel <= 2) {
            dispMsg(MSG_RADAR_NEAR);
        } else if (placeLevel <= 5) {
            dispMsg(MSG_RADAR_NEARBY);
        } else if (placeLevel <= 8) {
            dispMsg(MSG_RADAR_FAR);
        } else {
            msgID = bios.getPlaceMsg();
        }
    }
    if (msgID != 1) {
        placeName = (unsigned long)GSmsgGetStringPtr(msgID);
        msgctrlSetValue(MSG_CTRL_TEXT, placeName);
        dispMsg(MSG_RADAR_PLACE);
    }
    return ITEM_USE_EVENT;
}

/* Table of the crane memos and the message each one plays. */
ITEM_CRANE_MEMO item_crane_memo[5] = {
    {ITEM_CRANE_MEMO_FIRST, 0, MSG_CRANE_MEMO_FIRST},
    {ITEM_CRANE_MEMO_SECOND, 1, MSG_CRANE_MEMO_SECOND},
    {ITEM_CRANE_MEMO_THIRD, 2, MSG_CRANE_MEMO_THIRD},
    {ITEM_CRANE_MEMO_FOURTH, 3, MSG_CRANE_MEMO_FOURTH},
    {ITEM_CRANE_MEMO_FIFTH, 4, MSG_CRANE_MEMO_FIFTH},
};

extern "C" int menuUseItemEventCraneMemo(unsigned long itemID, int *consumed)
{
    u32 i;

    *consumed = 0;
    for (i = 0; i < item_crane_memo_number; i++) {
        ITEM_CRANE_MEMO *memo = &item_crane_memo[i];
        if (memo->mItemID == itemID) {
            dispMsg(memo->mMsgID);
            return ITEM_USE_EVENT;
        }
    }
    dispMsg(MSG_ITEM_NO_USE);
    return ITEM_USE_REFUSED;
}

extern "C" int menuUseItemEventNotConsum(unsigned long itemID, int *consumed)
{
    s32 i;

    *consumed = 0;
    for (i = 0; i < menuPocketWork.mCount; i++) {
        if (menuPocketWork.mEntries[i].mID == itemID) {
            return ITEM_USE_EVENT;
        }
    }
    dispMsg(MSG_ITEM_NO_USE);
    return ITEM_USE_REFUSED;
}

extern "C" int menuUseItemBattleNazonomi(unsigned long itemID, int *consumed)
{
    ITEM_PARAM param;
    s32 type;

    itemParamConvertOrigFormat(&param, &itemParamOrigBattle);
    type = itemParamGetRecoverType(&param);
    switch (type) {
    case RECOVER_TYPE_NONE:
        return menuUseItemStatusUp(itemID, consumed);
    case RECOVER_TYPE_SLEEP:
    case RECOVER_TYPE_POISON:
    case RECOVER_TYPE_BURN:
    case RECOVER_TYPE_FREEZE:
    case RECOVER_TYPE_PARALYZE:
    case RECOVER_TYPE_CONFUSE:
    case RECOVER_TYPE_INFATUATION:
    case RECOVER_TYPE_MULTIPLE_STATUS:
    case RECOVER_TYPE_HP:
        return menuUseItemRecovery(itemID, consumed);
    case RECOVER_TYPE_PP:
        return menuUseItemPPRecovery(itemID, consumed);
    default:
        return menuUseItemDummy(itemID, consumed);
    }
}

extern "C" int menuUseItemBattleBall(unsigned long itemID, int *consumed)
{
    Hero *hero;
    FIGHT_TRAINER *trainer;
    Pokemon *pokemon;
    FightPokemon *fightPokemon;
    s32 i;
    u32 room = 0;
    u8 stock = fightFloor_GetStockPokemonNum(0);
    s8 boxes;
    u8 battleType;

    for (i = 0; i < 6; i++) {
        menuPocket2GetTemochiPokemon(i, &pokemon, &fightPokemon);
        if (!pokemonCheckValid(pokemon)) {
            room++;
        }
    }
    boxes = pcboxGetNbPokemonBox();
    for (i = 0; i < (s8)boxes; i++) {
        room += pcboxGetPokemonBoxNbEmptySlot(0, (s8)i);
    }
    if ((s32)room < (s32)stock + 1) {
        dispMsg(MSG_BALL_NO_ROOM);
        return ITEM_USE_REFUSED;
    }
    menuPocket2GetHero(&hero, &trainer);
    battleType = fightFloor_GetFightKind(fightFloorGetNowPtr());
    if (hero->getHeroStyle() != HERO_STYLE_SNATCH && battleType != FIGHT_BATTLE_KIND_WILD_POKEMON && battleType != FIGHT_BATTLE_KIND_BATTLE_BINGO) {
        dispMsg(MSG_BALL_WRONG_BATTLE);
        return ITEM_USE_REFUSED;
    }
    if (fightTrainerIsSelectedItemBall(trainer)) {
        dispMsg(MSG_BALL_TRAINER);
        return ITEM_USE_REFUSED;
    }
    *consumed = 0;
    return ITEM_USE_APPLIED;
}

extern "C" int menuUseItemStatusUp(unsigned long itemID, int *consumed)
{
    Pokemon *pokemon;
    FightPokemon *fightPokemon;
    ITEMUSE2POKEMON_LOG1 log[32];
    u16 msg[0x42];
    s16 count;
    unsigned long nickname;

    menuPocket2GetTemochiPokemon(menuPocket2GetCurrentTemochiPokemonSlotID(), &pokemon, &fightPokemon);
    if (pokemonIsJoutaiDataId(pokemon, JOUTAI_REVERSE_MODE)) {
        nickname = (unsigned long)pokemon_GetNickname(pokemon);
        msgctrlSetValue(MSG_CTRL_NICKNAME, nickname);
        dispMsg(MSG_POKEMON_REVERSE_MODE);
        return ITEM_USE_REFUSED;
    }
    count = itemUse2Pokemon(log, pokemon, fightPokemon, itemID, 0);
    if (count > 0) {
        playRecoverSE(itemID);
        menuPokemonUpdate();
    }
    getLogMsgBattle(msg, 0x40, log, count, pokemon);
    msgctrlSetValue(MSG_CTRL_TEXT, (unsigned long)msg);
    dispMsg(MSG_ITEM_RECOVERED);
    if (count > 0) {
        *consumed = 1;
        return ITEM_USE_APPLIED;
    }
    return ITEM_USE_REFUSED;
}

extern "C" int menuUseItemFieldNazonomi(unsigned long itemID, int *consumed)
{
    ITEM_PARAM param;
    s32 type;

    itemParamConvertOrigFormat(&param, &itemParamOrigField);
    type = itemParamGetRecoverType(&param);
    switch (type) {
    case RECOVER_TYPE_SLEEP:
    case RECOVER_TYPE_POISON:
    case RECOVER_TYPE_BURN:
    case RECOVER_TYPE_FREEZE:
    case RECOVER_TYPE_PARALYZE:
    case RECOVER_TYPE_MULTIPLE_STATUS:
    case RECOVER_TYPE_HP:
    case RECOVER_TYPE_EFFORT_ATTACK_UP:
    case RECOVER_TYPE_EFFORT_MAX_HP_UP:
    case RECOVER_TYPE_EFFORT_SP_ATTACK_UP:
    case RECOVER_TYPE_EFFORT_SP_DEFENSE_UP:
    case RECOVER_TYPE_EFFORT_SPEED_UP:
    case RECOVER_TYPE_EFFORT_DEFENSE_UP:
        return menuUseItemRecovery(itemID, consumed);
    case RECOVER_TYPE_REVIVE_ALL:
        return menuUseItemAlldeathRcv(itemID, consumed);
    case RECOVER_TYPE_LEVEL_UP:
        return menuUseItemListLVUp(itemID, consumed);
    case RECOVER_TYPE_PP_MAX_UP:
    case RECOVER_TYPE_PP_MAX_FULL:
        return menuUseItemWPointUp(itemID, consumed);
    case RECOVER_TYPE_PP:
        return menuUseItemPPRecovery(itemID, consumed);
    default:
        return menuUseItemDummy(itemID, consumed);
    }
}

void wazaLean(Pokemon *pokemon, u16 wazaID, u8 index)
{
    u16 heldEffectID;

    pokemonWazaCreate(pokemon, index, wazaID);
    heldEffectID = pokemonGetSoubiItemSoubiDataId(pokemon);
    pokemonGetFriendFormPokemonFriendFilterId(pokemon, heldEffectID, POKEMON_FRIEND_FILTER_LEARN_MOVE);
}

s32 wazaMachineProc(Pokemon *pokemon, int itemID)
{
    s32 i;
    u16 wazaID;
    s32 result;
    u8 learnIndex;
    unsigned long nickname;

    menuOffScreenSetPriority(0);
    if (pokemonIsDarkPokemon(pokemon)) {
        dispMsg(MSG_POKEMON_IS_SHADOW);
        return 0;
    }
    wazaID = itemDataBiosGetWazaIDByWazaMachineNo((u8)itemID);
    for (i = 0; i < 4; i++) {
        if (wazaID == pokemonBiosGetPokemonWazaDataId(pokemon, i)) {
            break;
        }
    }
    if (i < 4) {
        nickname = (unsigned long)pokemon_GetNickname(pokemon);
        msgctrlSetValue(MSG_CTRL_NICKNAME, nickname);
        msgctrlSetValue(MSG_CTRL_WAZA, (unsigned long)wazaID);
        dispMsg(MSG_WAZA_MACHINE_KNOWN);
        return 0;
    }
    if (pokemonDataBiosGetWazaMcn(pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon)), (u16)itemID) == 0) {
        nickname = (unsigned long)pokemon_GetNickname(pokemon);
        msgctrlSetValue(MSG_CTRL_NICKNAME, nickname);
        msgctrlSetValue(MSG_CTRL_WAZA, (unsigned long)wazaID);
        dispMsg(MSG_WAZA_MACHINE_NO);
        return 0;
    }
    result = evolutionWazaLearn(pokemon, wazaID, &learnIndex, EVOLUTION_WINDOW_GENERAL, cbForgetWazaSelect, 0);
    if (result != 0) {
        wazaLean(pokemon, wazaID, learnIndex);
    }
    return result;
}

extern "C" int menuUseItemSkill(unsigned long itemID, int *consumed)
{
    u16 wazaID;
    u8 hidenNo;
    int machineNo;
    s32 answer;
    ItemData *itemData;
    u32 msgID;
    s32 slot;
    Pokemon *pokemon;
    FightPokemon *fightPokemon;
    s32 result;

    itemData = itemDataBiosGetPtr((u16)itemID);
    machineNo = itemDataBiosGetWazaMachineNo(itemData);
    wazaID = (u16)itemDataBiosGetWazaIDByWazaMachineNo(machineNo);
    GSsndPlayNow(SE_WAZA_MACHINE);
    hidenNo = itemDataBiosGetHidenMachineNo(itemData);
    if (hidenNo != 0xFF) {
        msgID = MSG_WAZA_MACHINE_HM;
    } else {
        msgID = MSG_WAZA_MACHINE_TM;
    }
    msgctrlSetValue(MSG_CTRL_WAZA, (unsigned long)wazaID);
    dispMsg(msgID);
    answer = dispMsgYesNo(MSG_WAZA_MACHINE_ASK);
    if (answer == 1 || answer == 2) {
        return ITEM_USE_REFUSED;
    }
    slot = menuPocket2OpenPokemonMenu(itemID, 1);
    if (slot >= 0) {
        menuPocket2GetTemochiPokemon(slot, &pokemon, &fightPokemon);
        result = wazaMachineProc(pokemon, machineNo);
    }
    menuOffScreenSetPriority(0);
    menuPocket2ClosePokemonMenu(slot);
    if (slot >= 0 && result != 0) {
        if (hidenNo != 0xFF) {
            *consumed = 0;
        } else {
            *consumed = 1;
        }
        return ITEM_USE_APPLIED;
    }
    return ITEM_USE_REFUSED;
}

extern "C" u32 menuUseItemScriptCologn(u32 itemID, s32 slot)
{
    COLOGNE_TABLE table = cCologneMsgs;
    Pokemon *pokemon;
    FightPokemon *fightPokemon;
    s32 i;
    s32 j;
    s32 cryID;
    s32 expCount;
    unsigned long nickname;
    f32 dp;

    menuPocket2GetTemochiPokemon(slot, &pokemon, &fightPokemon);
    if (!pokemonCheckValid(pokemon)) {
        return 1;
    }
    for (i = 0; i < COLOGNE_TABLE_COUNT; i++) {
        if ((u16)itemID == table.mEntry[i].mItemID) {
            break;
        }
    }
    if (i >= COLOGNE_TABLE_COUNT) {
        return 1;
    }
    nickname = (unsigned long)pokemon_GetNickname(pokemon);
    msgctrlSetValue(MSG_CTRL_NICKNAME, nickname);
    msgctrlSetValue(MSG_CTRL_ITEM, (unsigned long)itemID);
    dispMsg(table.mEntry[i].mMsgID);
    GSsndPlayVolume(SE_COLOGNE, 0, 0xFF, 0);

    if ((s32)gamedatasaveGetStatus(0, GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION) == 0) {
        for (j = 0; j < 2; j++) {
            GSinputMotorStart(1, 0, 0xFF, COLOGNE_RUMBLE_DURATION, 0);
            waitTime(cCologneRumbleWaitSeconds);
        }
        GSinputMotorStart(1, 0, 0xFF, COLOGNE_RUMBLE_LONG_DURATION, 0);
        waitTime(cCologneLastRumbleWaitSeconds);
    }
    cryID = playPokemonVoice(pokemon);
    while (GSsndGetStatus(cryID) == GS_SOUND_PLAYING) {
        GSthreadSwitch();
    }
    msgctrlSetValue(MSG_CTRL_NICKNAME, (unsigned long)pokemon_GetNickname(pokemon));
    dispMsg(MSG_COLOGNE_USED);
    if (pokemonIsJoutaiDataId(pokemon, JOUTAI_REVERSE_MODE)) {
        pokemonInitJoutaiDataId(pokemon, JOUTAI_REVERSE_MODE);
        dispMsg(MSG_COLOGNE_REVERSE_CLEARED);
    }
    if (pokemonHasDarkPoints(pokemon)) {
        if ((u16)pokemon->getPokemonDataId() != SPECIES_COLOGNE_DP_EXEMPT) {
            pokemonAddDpFormPokemonDpFilterId(pokemon, (u16)itemID, DP_FILTER_NADENADE);
            msgctrlSetValue(MSG_CTRL_NICKNAME, (unsigned long)pokemon_GetNickname(pokemon));
            dispMsg(table.mEntry[i].mEffectMsg);
            dp = pokemonGetDp(pokemon);
            if (!dp) {
                reliveHallInfomationPokemonDP0(pokemon);
            }
        }
    }
    return 1;
}

extern "C" int menuUseItemCologn(unsigned long itemID, int *consumed)
{
    Pokemon *pokemon;
    FightPokemon *fightPokemon;
    s32 slot = menuPocket2OpenPokemonMenu(itemID, 1);

    if (slot >= 0) {
        menuPocket2GetTemochiPokemon(slot, &pokemon, &fightPokemon);
    }
    menuOffScreenSetPriority(0);
    menuPocket2ClosePokemonMenu(slot);
    if (slot >= 0) {
        menuPocket2AddItemScript(itemID, ITEM_SCRIPT_COLOGNE_FIELD_EFFECT);
        *consumed = 1;
        return ITEM_USE_EVENT;
    }
    return ITEM_USE_REFUSED;
}

extern "C" int menuUseItemEvolution(unsigned long itemID, int *consumed)
{
    Pokemon *pokemon;
    FightPokemon *fightPokemon;
    ITEMUSE2POKEMON_LOG1 log[32];
    s16 result = -1;
    s32 slot = menuPocket2OpenPokemonMenu(itemID, 1);
    s32 evoSpecies;
    u16 addedSpecies;
    POKEMON_EVOLUTION_INFO evoInfo;

    if (slot >= 0) {
        menuPocket2GetTemochiPokemon(slot, &pokemon, &fightPokemon);
        if (pokemonIsDarkPokemon(pokemon) == 0) {
            result = itemUse2Pokemon(log, pokemon, fightPokemon, (u16)itemID, 0);
            if (result <= 0) {
                dispMsg(MSG_ITEM_NO_EFFECT);
            }
        } else {
            dispMsg(MSG_POKEMON_IS_SHADOW);
        }
    }
    menuOffScreenSetPriority(0);
    menuPocket2ClosePokemonMenu(slot);
    if (slot >= 0 && result > 0) {
        evoSpecies = pokemonEvolutionCheck(pokemon, EVOLUTION_CHECK_ITEM, (u16)itemID, &addedSpecies, &evoInfo);
        fadeSet(FADE_OUT_BLACK, cEvolutionFadeSeconds);
        fadeCheck(1);
        menuOffScreenSetDisp(0);
        pokemonEvolutionAll(pokemon, evoSpecies, addedSpecies, &evoInfo, 0, 1, 0, 0);
        menuOffScreenSetDisp(1);
        fadeSet(FADE_IN_BLACK, cEvolutionFadeSeconds);
        fadeCheck(1);
        *consumed = 1;
        return ITEM_USE_APPLIED;
    }
    return ITEM_USE_REFUSED;
}

extern "C" int menuUseItemWPointUp(unsigned long itemID, int *consumed)
{
    unsigned long nickname;
    Pokemon *pokemon;
    FightPokemon *fightPokemon;
    ITEMUSE2POKEMON_LOG1 log[32];
    u16 msg[0x42];
    s32 reverseMode = 0;
    s32 slot;
    s16 count;
    s32 wazaIndex;
    u16 wazaDataID;

    do {
        slot = menuPocket2OpenPokemonMenu(itemID, 1);
        if (slot < 0) {
            break;
        }
        menuPocket2GetTemochiPokemon(slot, &pokemon, &fightPokemon);
        if (pokemonIsJoutaiDataId(pokemon, JOUTAI_REVERSE_MODE)) {
            reverseMode = 1;
            break;
        }
        wazaIndex = menuPokemonOpenWazaLight((s8)slot);
        menuPokemonCloseWazaLight();
    } while (wazaIndex < 0);

    if (slot >= 0) {
        if (reverseMode) {
            nickname = (unsigned long)pokemon_GetNickname(pokemon);
            msgctrlSetValue(MSG_CTRL_NICKNAME, nickname);
            dispMsg(MSG_POKEMON_REVERSE_MODE);
            count = 0;
        } else {
            wazaDataID = pokemonBiosGetPokemonWazaDataId(pokemon, (u16)wazaIndex);
            if ((u16)wazaDataID > WAZA_POINT_UP_LAST_ALLOWED && (u16)wazaDataID < WAZA_POINT_UP_RESTRICTED_END) {
                dispMsg(MSG_ITEM_NO_EFFECT);
                count = 0;
            } else {
                count = itemUse2Pokemon(log, pokemon, fightPokemon, (u16)itemID, (u8)wazaIndex);
                if (count > 0) {
                    playRecoverSE(itemID);
                    menuPokemonUpdate();
                }
                msgctrlSetValue(MSG_CTRL_TEXT, (unsigned long)msg);
                msgctrlSetValue(MSG_CTRL_WAZA, (unsigned long)wazaDataID);
                getLogMsgField(msg, 0x40, log, count, pokemon);
                dispMsg(MSG_ITEM_RECOVERED);
            }
        }
    }
    menuOffScreenSetPriority(0);
    menuPocket2ClosePokemonMenu(slot);
    if (slot >= 0 && count > 0) {
        *consumed = 1;
        return ITEM_USE_APPLIED;
    }
    return ITEM_USE_REFUSED;
}

void getPokemonLvupStatus(MENU_LVUP_STATUS *status, Pokemon *pokemon)
{
    status->mHp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_HP_MAX, 0);
    status->mAttack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_ATTACK, 0);
    status->mDefense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_DEFENSE, 0);
    status->mSpAttack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SP_ATTACK, 0);
    status->mSpDefense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SP_DEFENSE, 0);
    status->mSpeed = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SPEED, 0);
}

void getDifLvupStatus(MENU_LVUP_STATUS *dst, MENU_LVUP_STATUS *newer, MENU_LVUP_STATUS *older)
{
    dst->mHp = newer->mHp - older->mHp;
    dst->mAttack = newer->mAttack - older->mAttack;
    dst->mDefense = newer->mDefense - older->mDefense;
    dst->mSpAttack = newer->mSpAttack - older->mSpAttack;
    dst->mSpDefense = newer->mSpDefense - older->mSpDefense;
    dst->mSpeed = newer->mSpeed - older->mSpeed;
}

s32 cbForgetWazaSelect(Pokemon *pokemon, u16 wazaID, long context)
{
    s32 result;

    fadeSet(FADE_OUT_BLACK, cEvolutionFadeSeconds);
    fadeCheck(1);
    result = menuPokemonStatusOpenWazaMachine(pokemon, wazaID);
    if (result >= 4) {
        result = -1;
    }
    fadeSet(FADE_IN_BLACK, cEvolutionFadeSeconds);
    fadeCheck(1);
    return (s8)result;
}

extern "C" int menuUseItemListLVUp(unsigned long itemID, int *consumed)
{
    u8 learnIndex;
    Pokemon *pokemon;
    u8 oboeIndex = 0;
    FightPokemon *fightPokemon;
    s32 slot = menuPocket2OpenPokemonMenu(itemID, 1);
    unsigned long nickname;
    s32 evoSpecies;
    u8 level;
    s16 count;
    POKEMON_EVOLUTION_INFO evoInfo;
    u16 addedSpecies;
    u16 wazaID;
    MENU_LVUP_STATUS diff;
    MENU_LVUP_STATUS before;
    MENU_LVUP_STATUS after;
    ITEMUSE2POKEMON_LOG1 log[32];

    if (slot >= 0) {
        menuPocket2GetTemochiPokemon(slot, &pokemon, &fightPokemon);
        if (pokemonIsDarkPokemon(pokemon) == 0) {
            getPokemonLvupStatus(&before, pokemon);
            count = itemUse2Pokemon(log, pokemon, fightPokemon, (u16)itemID, 0);
            if (count > 0) {
                menuPokemonUpdate();
            } else {
                count = 0;
            }
        } else {
            count = -1;
        }
        switch (count) {
        case 0:
            dispMsg(MSG_ITEM_NO_EFFECT);
            break;
        case -1:
            dispMsg(MSG_POKEMON_IS_SHADOW);
            break;
        default:
            menuOffScreenSetPriority(0);
            soundOpenPlayME(ME_LEVEL_UP, 0, 0xFF);
            nickname = (unsigned long)pokemon_GetNickname(pokemon);
            msgctrlSetValue(MSG_CTRL_NICKNAME, nickname);
            level = pokemonBiosGetLevel(pokemon);
            msgctrlSetValue(MSG_CTRL_VALUE, (unsigned char)level);
            dispMsg(MSG_ITEM_USE_LEVEL_UP);
            getPokemonLvupStatus(&after, pokemon);
            getDifLvupStatus(&diff, &after, &before);
            diff.mFlag = 1;
            winMsgOpenLevelUpFiledStatus(&diff, 1);
            after.mFlag = 0;
            winMsgOpenLevelUpFiledStatus(&after, 1);
            winMsgCloseLevelUpStatus(1);
            level = pokemonBiosGetLevel(pokemon);
            oboeIndex = 0;
            while ((wazaID = pokemonGetOboeWazaDataId(pokemon, level, &oboeIndex)) != 0) {
                if (pokemonSearchWazaDataId(pokemon, wazaID) == -1) {
                    if (evolutionWazaLearn(pokemon, wazaID, &learnIndex, EVOLUTION_WINDOW_GENERAL, cbForgetWazaSelect, 0) != 0) {
                        pokemonWazaCreate(pokemon, learnIndex, wazaID);
                    }
                }
                oboeIndex++;
            }
            evoSpecies = pokemonEvolutionCheck(pokemon, EVOLUTION_CHECK_LEVEL_UP, 0, &addedSpecies, &evoInfo);
            break;
        }
    }
    menuOffScreenSetPriority(0);
    menuPocket2ClosePokemonMenu(slot);
    if (slot >= 0 && count > 0) {
        if ((u16)evoSpecies != 0) {
            fadeSet(FADE_OUT_BLACK, cEvolutionFadeSeconds);
            fadeCheck(1);
            menuOffScreenSetDisp(0);
            pokemonEvolutionAll(pokemon, evoSpecies, addedSpecies, &evoInfo, 0, 1, 1, 0);
            menuOffScreenSetDisp(1);
            fadeSet(FADE_IN_BLACK, cEvolutionFadeSeconds);
            fadeCheck(1);
        }
        *consumed = 1;
        return ITEM_USE_APPLIED;
    }
    return ITEM_USE_REFUSED;
}

extern "C" int menuUseItemAlldeathRcv(unsigned long itemID, int *consumed)
{
    Pokemon *pokemon;
    FightPokemon *fightPokemon;
    ITEMUSE2POKEMON_LOG1 log[32];
    u16 msg[0x42];
    s32 i;
    u16 revived = 0;
    s16 count;
    s32 result;

    menuPocket2OpenPokemonMenu(itemID, 0);
    for (i = 0; i < 6; i++) {
        menuPocket2GetTemochiPokemon(i, &pokemon, &fightPokemon);
        if (!pokemonCheckValid(pokemon)) {
            continue;
        }
        if (pokemonGetStatus(pokemon, 0, POKEMON_STATUS_HP, 0) > 0) {
            continue;
        }
        if (pokemonIsJoutaiDataId(pokemon, JOUTAI_REVERSE_MODE)) {
            continue;
        }
        count = itemUse2Pokemon(log, pokemon, fightPokemon, (u16)itemID, 0);
        if (count <= 0) {
            continue;
        }
        playRecoverSE(itemID);
        menuPokemonUpdate();
        getLogMsgField(msg, 0x40, log, count, pokemon);
        msgctrlSetValue(MSG_CTRL_TEXT, (unsigned long)msg);
        dispMsg(MSG_ITEM_RECOVERED);
        revived += count;
    }
    if (revived == 0) {
        dispMsg(MSG_ITEM_NO_EFFECT);
        result = -1;
    } else {
        result = 1;
    }
    menuOffScreenSetPriority(0);
    menuPocket2ClosePokemonMenu(-1);
    if (result < 0) {
        return ITEM_USE_REFUSED;
    }
    *consumed = 1;
    return ITEM_USE_APPLIED;
}

extern "C" int menuUseItemPPRecovery(unsigned long itemID, int *consumed)
{
    unsigned long nickname;
    Pokemon *pokemon;
    FightPokemon *fightPokemon;
    ITEMUSE2POKEMON_LOG1 log[32];
    u16 msg[0x42];
    s32 slot;
    s16 count;
    s32 moveIndex;
    s32 reverseMode = 0;

    do {
        slot = menuPocket2OpenPokemonMenu(itemID, 1);
        if (slot < 0) {
            break;
        }
        menuPocket2GetTemochiPokemon(slot, &pokemon, &fightPokemon);
        if (pokemonIsJoutaiDataId(pokemon, JOUTAI_REVERSE_MODE)) {
            reverseMode = 1;
            break;
        }
        if (itemParamGetPPSelectFlag(itemParamGetPtr(itemDataBiosGetItemEffectParam(itemDataBiosGetPtr((u16)itemID))))) {
            moveIndex = menuPokemonOpenWazaLight((s8)slot);
            menuPokemonCloseWazaLight();
        } else {
            moveIndex = 0;
        }
    } while (moveIndex < 0);
    if (slot >= 0) {
        if (reverseMode) {
            nickname = (unsigned long)pokemon_GetNickname(pokemon);
            msgctrlSetValue(MSG_CTRL_NICKNAME, nickname);
            dispMsg(MSG_POKEMON_REVERSE_MODE);
            count = 0;
        } else {
            count = itemUse2Pokemon(log, pokemon, fightPokemon, (u16)itemID, (u8)moveIndex);
            if (count > 0) {
                playRecoverSE(itemID);
                menuPokemonUpdate();
            }
            getLogMsgField(msg, 0x40, log, count, pokemon);
            msgctrlSetValue(MSG_CTRL_TEXT, (unsigned long)msg);
            dispMsg(MSG_ITEM_RECOVERED);
        }
    }
    menuOffScreenSetPriority(0);
    menuPocket2ClosePokemonMenu(slot);
    if (slot >= 0 && count > 0) {
        *consumed = 1;
        return ITEM_USE_APPLIED;
    }
    return ITEM_USE_REFUSED;
}

extern "C" int menuUseItemRecovery(unsigned long itemID, int *consumed)
{
    unsigned long nickname;
    Pokemon *pokemon;
    FightPokemon *fightPokemon;
    ITEMUSE2POKEMON_LOG1 log[32];
    u16 msg[0x42];
    s32 slot = menuPocket2OpenPokemonMenu(itemID, 1);
    s16 count;

    if (slot >= 0) {
        menuPocket2GetTemochiPokemon(slot, &pokemon, &fightPokemon);
        if (pokemonIsJoutaiDataId(pokemon, JOUTAI_REVERSE_MODE) == 0) {
            count = itemUse2Pokemon(log, pokemon, fightPokemon, (u16)itemID, 0);
            if (count > 0) {
                playRecoverSE(itemID);
                menuPokemonUpdate();
            }
            getLogMsgField(msg, 0x40, log, count, pokemon);
            msgctrlSetValue(MSG_CTRL_TEXT, (unsigned long)msg);
            dispMsg(MSG_ITEM_RECOVERED);
        } else {
            nickname = (unsigned long)pokemon_GetNickname(pokemon);
            msgctrlSetValue(MSG_CTRL_NICKNAME, nickname);
            dispMsg(MSG_POKEMON_REVERSE_MODE);
            count = 0;
        }
    }
    menuOffScreenSetPriority(0);
    menuPocket2ClosePokemonMenu(slot);
    if (slot >= 0 && count > 0) {
        switch (itemID) {
        case ITEM_BLUE_FLUTE:
        case ITEM_YELLOW_FLUTE:
        case ITEM_RED_FLUTE:
        case ITEM_BLACK_FLUTE:
        case ITEM_WHITE_FLUTE:
            *consumed = 0;
            break;
        default:
            *consumed = 1;
            break;
        }
        return ITEM_USE_APPLIED;
    }
    return ITEM_USE_REFUSED;
}

extern "C" int menuUseItemNoPokecolo(unsigned long itemID, int *consumed)
{
    dispMsg(MSG_ITEM_NO_POKECOLO);
    return ITEM_USE_REFUSED;
}

extern "C" int menuUseItemNoEffects(unsigned long itemID, int *consumed)
{
    dispMsg(MSG_ITEM_NO_EFFECT);
    return ITEM_USE_REFUSED;
}

extern "C" int menuUseItemDummy(unsigned long itemID, int *consumed)
{
    dispMsg(MSG_ITEM_NO_USE);
    return ITEM_USE_REFUSED;
}

#pragma force_active reset
