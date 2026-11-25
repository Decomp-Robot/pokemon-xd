#ifndef WAZA_H
#define WAZA_H

#include <types.h>
#include <bool.h>

typedef struct FIGHT_WAZA FIGHT_WAZA;
typedef struct FightOutPokemon FightOutPokemon;

enum WAZA_FIELD {
    WAZA_FIELD_NONE = 0,
    WAZA_FIELD_NAME = 1,
    WAZA_FIELD_PP = 2,
    WAZA_FIELD_TYPE = 3,
    WAZA_FIELD_PRIORITY = 4,
    WAZA_FIELD_RANGE_ID = 5,
    WAZA_FIELD_ACCURACY = 6,
    WAZA_FIELD_POWER = 7,
    WAZA_FIELD_FIGHT_EFFECT_ID = 8,
    WAZA_FIELD_SEQUENCE_ID = 9,
    WAZA_FIELD_ATTACK_CONNECT_MESSAGE = 10,
    WAZA_FIELD_ATTACK_MESSAGE = 11,
    WAZA_FIELD_ADD_EFFECT_ACCURACY = 12,
    WAZA_FIELD_CONTACT = 13,
    WAZA_FIELD_PROTECT = 14,
    WAZA_FIELD_MAGIC_COAT = 15,
    WAZA_FIELD_SNATCH = 16,
    WAZA_FIELD_MIRROR_MOVE = 17,
    WAZA_FIELD_KINGS_ROCK = 18,
    WAZA_FIELD_METRONOME = 19,
    WAZA_FIELD_MIMIC = 20,
    WAZA_FIELD_ASSIST = 21,
    WAZA_FIELD_SLEEP_TALK = 22,
    WAZA_FIELD_SOUNDPROOF = 23,
    WAZA_FIELD_PRESSURE = 24,
    WAZA_FIELD_HIDEN_FLAG = 25,
    WAZA_FIELD_CATEGORY = 26,
    WAZA_FIELD_RISK_FLAG = 27,
    WAZA_FIELD_TRAINER_AI_VALUE_FUNC = 28,
    WAZA_FIELD_TRAINER_AI_HIT_FUNC = 29,
    WAZA_FIELD_TRAINER_AI_DAMAGE_FUNC = 30,
    WAZA_FIELD_WAZA_WZX_ID = 31,
    WAZA_FIELD_WAZA_WZX_TYPE_FUNC = 32,
    WAZA_FIELD_WAZA_WZX_VARIATION_FUNC = 33,
    WAZA_FIELD_DESCRIPTION = 34,
    WAZA_FIELD_STATIC_END = 37,
    WAZA_FIELD_MOVE_SLOT = 38,
    WAZA_FIELD_ORIGINAL_MOVE_ID = 39,
    WAZA_FIELD_USED_MOVE_ID = 40,
    WAZA_FIELD_TARGET_ID = 41,
    WAZA_FIELD_JOUTAI = 42,
    WAZA_FIELD_CRITICAL = 43,
    WAZA_FIELD_DAMAGE_VALUE = 44,
    WAZA_FIELD_DAMAGE = 45,
    WAZA_FIELD_HIT_DAMAGE = 46,
    WAZA_FIELD_FIGHT_POWER = 47,
    WAZA_FIELD_FIGHT_TYPE = 48,
    WAZA_FIELD_USE_COUNT = 49,
    WAZA_FIELD_AUTO_MAKE_FLAG = 50,
    WAZA_FIELD_ORIGINAL_MOVE_TYPE = 51,
    WAZA_FIELD_USED_MOVE_TYPE = 52,
    WAZA_FIELD_COUNT = 53,
};

enum WAZA_DATA_ID {
    WAZA_DATA_ID_UNUSED = 0,
    WAZA_DATA_ID_POUND,
    WAZA_DATA_ID_KARATE_CHOP,
    WAZA_DATA_ID_DOUBLE_SLAP,
    WAZA_DATA_ID_COMET_PUNCH,
    WAZA_DATA_ID_MEGA_PUNCH,
    WAZA_DATA_ID_PAY_DAY,
    WAZA_DATA_ID_FIRE_PUNCH,
    WAZA_DATA_ID_ICE_PUNCH,
    WAZA_DATA_ID_THUNDER_PUNCH,
    WAZA_DATA_ID_SCRATCH,
    WAZA_DATA_ID_VICE_GRIP,
    WAZA_DATA_ID_GUILLOTINE,
    WAZA_DATA_ID_RAZOR_WIND,
    WAZA_DATA_ID_SWORDS_DANCE,
    WAZA_DATA_ID_CUT,
    WAZA_DATA_ID_GUST,
    WAZA_DATA_ID_WING_ATTACK,
    WAZA_DATA_ID_WHIRLWIND,
    WAZA_DATA_ID_FLY,
    WAZA_DATA_ID_BIND,
    WAZA_DATA_ID_SLAM,
    WAZA_DATA_ID_VINE_WHIP,
    WAZA_DATA_ID_STOMP,
    WAZA_DATA_ID_DOUBLE_KICK,
    WAZA_DATA_ID_MEGA_KICK,
    WAZA_DATA_ID_JUMP_KICK,
    WAZA_DATA_ID_ROLLING_KICK,
    WAZA_DATA_ID_SAND_ATTACK,
    WAZA_DATA_ID_HEADBUTT,
    WAZA_DATA_ID_HORN_ATTACK,
    WAZA_DATA_ID_FURY_ATTACK,
    WAZA_DATA_ID_HORN_DRILL,
    WAZA_DATA_ID_TACKLE,
    WAZA_DATA_ID_BODY_SLAM,
    WAZA_DATA_ID_WRAP,
    WAZA_DATA_ID_TAKE_DOWN,
    WAZA_DATA_ID_THRASH,
    WAZA_DATA_ID_DOUBLE_EDGE,
    WAZA_DATA_ID_TAIL_WHIP,
    WAZA_DATA_ID_POISON_STING,
    WAZA_DATA_ID_TWINEEDLE,
    WAZA_DATA_ID_PIN_MISSILE,
    WAZA_DATA_ID_LEER,
    WAZA_DATA_ID_BITE,
    WAZA_DATA_ID_GROWL,
    WAZA_DATA_ID_ROAR,
    WAZA_DATA_ID_SING,
    WAZA_DATA_ID_SUPERSONIC,
    WAZA_DATA_ID_SONIC_BOOM,
    WAZA_DATA_ID_DISABLE,
    WAZA_DATA_ID_ACID,
    WAZA_DATA_ID_EMBER,
    WAZA_DATA_ID_FLAMETHROWER,
    WAZA_DATA_ID_MIST,
    WAZA_DATA_ID_WATER_GUN,
    WAZA_DATA_ID_HYDRO_PUMP,
    WAZA_DATA_ID_SURF,
    WAZA_DATA_ID_ICE_BEAM,
    WAZA_DATA_ID_BLIZZARD,
    WAZA_DATA_ID_PSYBEAM,
    WAZA_DATA_ID_BUBBLE_BEAM,
    WAZA_DATA_ID_AURORA_BEAM,
    WAZA_DATA_ID_HYPER_BEAM,
    WAZA_DATA_ID_PECK,
    WAZA_DATA_ID_DRILL_PECK,
    WAZA_DATA_ID_SUBMISSION,
    WAZA_DATA_ID_LOW_KICK,
    WAZA_DATA_ID_COUNTER,
    WAZA_DATA_ID_SEISMIC_TOSS,
    WAZA_DATA_ID_STRENGTH,
    WAZA_DATA_ID_ABSORB,
    WAZA_DATA_ID_MEGA_DRAIN,
    WAZA_DATA_ID_LEECH_SEED,
    WAZA_DATA_ID_GROWTH,
    WAZA_DATA_ID_RAZOR_LEAF,
    WAZA_DATA_ID_SOLAR_BEAM,
    WAZA_DATA_ID_POISON_POWDER,
    WAZA_DATA_ID_STUN_SPORE,
    WAZA_DATA_ID_SLEEP_POWDER,
    WAZA_DATA_ID_PETAL_DANCE,
    WAZA_DATA_ID_STRING_SHOT,
    WAZA_DATA_ID_DRAGON_RAGE,
    WAZA_DATA_ID_FIRE_SPIN,
    WAZA_DATA_ID_THUNDER_SHOCK,
    WAZA_DATA_ID_THUNDERBOLT,
    WAZA_DATA_ID_THUNDER_WAVE,
    WAZA_DATA_ID_THUNDER,
    WAZA_DATA_ID_ROCK_THROW,
    WAZA_DATA_ID_EARTHQUAKE,
    WAZA_DATA_ID_FISSURE,
    WAZA_DATA_ID_DIG,
    WAZA_DATA_ID_TOXIC,
    WAZA_DATA_ID_CONFUSION,
    WAZA_DATA_ID_PSYCHIC,
    WAZA_DATA_ID_HYPNOSIS,
    WAZA_DATA_ID_MEDITATE,
    WAZA_DATA_ID_AGILITY,
    WAZA_DATA_ID_QUICK_ATTACK,
    WAZA_DATA_ID_RAGE,
    WAZA_DATA_ID_TELEPORT,
    WAZA_DATA_ID_NIGHT_SHADE,
    WAZA_DATA_ID_MIMIC,
    WAZA_DATA_ID_SCREECH,
    WAZA_DATA_ID_DOUBLE_TEAM,
    WAZA_DATA_ID_RECOVER,
    WAZA_DATA_ID_HARDEN,
    WAZA_DATA_ID_MINIMIZE,
    WAZA_DATA_ID_SMOKE_SCREEN,
    WAZA_DATA_ID_CONFUSE_RAY,
    WAZA_DATA_ID_WITHDRAW,
    WAZA_DATA_ID_DEFENSE_CURL,
    WAZA_DATA_ID_BARRIER,
    WAZA_DATA_ID_LIGHT_SCREEN,
    WAZA_DATA_ID_HAZE,
    WAZA_DATA_ID_REFLECT,
    WAZA_DATA_ID_FOCUS_ENERGY,
    WAZA_DATA_ID_BIDE,
    WAZA_DATA_ID_METRONOME,
    WAZA_DATA_ID_MIRROR_MOVE,
    WAZA_DATA_ID_SELF_DESTRUCT,
    WAZA_DATA_ID_EGG_BOMB,
    WAZA_DATA_ID_LICK,
    WAZA_DATA_ID_SMOG,
    WAZA_DATA_ID_SLUDGE,
    WAZA_DATA_ID_BONE_CLUB,
    WAZA_DATA_ID_FIRE_BLAST,
    WAZA_DATA_ID_WATERFALL,
    WAZA_DATA_ID_CLAMP,
    WAZA_DATA_ID_SWIFT,
    WAZA_DATA_ID_SKULL_BASH,
    WAZA_DATA_ID_SPIKE_CANNON,
    WAZA_DATA_ID_CONSTRICT,
    WAZA_DATA_ID_AMNESIA,
    WAZA_DATA_ID_KINESIS,
    WAZA_DATA_ID_SOFT_BOILED,
    WAZA_DATA_ID_HIGH_JUMP_KICK,
    WAZA_DATA_ID_GLARE,
    WAZA_DATA_ID_DREAM_EATER,
    WAZA_DATA_ID_POISON_GAS,
    WAZA_DATA_ID_BARRAGE,
    WAZA_DATA_ID_LEECH_LIFE,
    WAZA_DATA_ID_LOVELY_KISS,
    WAZA_DATA_ID_SKY_ATTACK,
    WAZA_DATA_ID_TRANSFORM,
    WAZA_DATA_ID_BUBBLE,
    WAZA_DATA_ID_DIZZY_PUNCH,
    WAZA_DATA_ID_SPORE,
    WAZA_DATA_ID_FLASH,
    WAZA_DATA_ID_PSYWAVE,
    WAZA_DATA_ID_SPLASH,
    WAZA_DATA_ID_ACID_ARMOR,
    WAZA_DATA_ID_CRABHAMMER,
    WAZA_DATA_ID_EXPLOSION,
    WAZA_DATA_ID_FURY_SWIPES,
    WAZA_DATA_ID_BONEMERANG,
    WAZA_DATA_ID_REST,
    WAZA_DATA_ID_ROCK_SLIDE,
    WAZA_DATA_ID_HYPER_FANG,
    WAZA_DATA_ID_SHARPEN,
    WAZA_DATA_ID_CONVERSION,
    WAZA_DATA_ID_TRI_ATTACK,
    WAZA_DATA_ID_SUPER_FANG,
    WAZA_DATA_ID_SLASH,
    WAZA_DATA_ID_SUBSTITUTE,
    WAZA_DATA_ID_STRUGGLE,
    WAZA_DATA_ID_SKETCH,
    WAZA_DATA_ID_TRIPLE_KICK,
    WAZA_DATA_ID_THIEF,
    WAZA_DATA_ID_SPIDER_WEB,
    WAZA_DATA_ID_MIND_READER,
    WAZA_DATA_ID_NIGHTMARE,
    WAZA_DATA_ID_FLAME_WHEEL,
    WAZA_DATA_ID_SNORE,
    WAZA_DATA_ID_CURSE,
    WAZA_DATA_ID_FLAIL,
    WAZA_DATA_ID_CONVERSION_2,
    WAZA_DATA_ID_AEROBLAST,
    WAZA_DATA_ID_COTTON_SPORE,
    WAZA_DATA_ID_REVERSAL,
    WAZA_DATA_ID_SPITE,
    WAZA_DATA_ID_POWDER_SNOW,
    WAZA_DATA_ID_PROTECT,
    WAZA_DATA_ID_MACH_PUNCH,
    WAZA_DATA_ID_SCARY_FACE,
    WAZA_DATA_ID_FAINT_ATTACK,
    WAZA_DATA_ID_SWEET_KISS,
    WAZA_DATA_ID_BELLY_DRUM,
    WAZA_DATA_ID_SLUDGE_BOMB,
    WAZA_DATA_ID_MUD_SLAP,
    WAZA_DATA_ID_OCTAZOOKA,
    WAZA_DATA_ID_SPIKES,
    WAZA_DATA_ID_ZAP_CANNON,
    WAZA_DATA_ID_FORESIGHT,
    WAZA_DATA_ID_DESTINY_BOND,
    WAZA_DATA_ID_PERISH_SONG,
    WAZA_DATA_ID_ICY_WIND,
    WAZA_DATA_ID_DETECT,
    WAZA_DATA_ID_BONE_RUSH,
    WAZA_DATA_ID_LOCK_ON,
    WAZA_DATA_ID_OUTRAGE,
    WAZA_DATA_ID_SANDSTORM,
    WAZA_DATA_ID_GIGA_DRAIN,
    WAZA_DATA_ID_ENDURE,
    WAZA_DATA_ID_CHARM,
    WAZA_DATA_ID_ROLLOUT,
    WAZA_DATA_ID_FALSE_SWIPE,
    WAZA_DATA_ID_SWAGGER,
    WAZA_DATA_ID_MILK_DRINK,
    WAZA_DATA_ID_SPARK,
    WAZA_DATA_ID_FURY_CUTTER,
    WAZA_DATA_ID_STEEL_WING,
    WAZA_DATA_ID_MEAN_LOOK,
    WAZA_DATA_ID_ATTRACT,
    WAZA_DATA_ID_SLEEP_TALK,
    WAZA_DATA_ID_HEAL_BELL,
    WAZA_DATA_ID_RETURN,
    WAZA_DATA_ID_PRESENT,
    WAZA_DATA_ID_FRUSTRATION,
    WAZA_DATA_ID_SAFEGUARD,
    WAZA_DATA_ID_PAIN_SPLIT,
    WAZA_DATA_ID_SACRED_FIRE,
    WAZA_DATA_ID_MAGNITUDE,
    WAZA_DATA_ID_DYNAMIC_PUNCH,
    WAZA_DATA_ID_MEGAHORN,
    WAZA_DATA_ID_DRAGON_BREATH,
    WAZA_DATA_ID_BATON_PASS,
    WAZA_DATA_ID_ENCORE,
    WAZA_DATA_ID_PURSUIT,
    WAZA_DATA_ID_RAPID_SPIN,
    WAZA_DATA_ID_SWEET_SCENT,
    WAZA_DATA_ID_IRON_TAIL,
    WAZA_DATA_ID_METAL_CLAW,
    WAZA_DATA_ID_VITAL_THROW,
    WAZA_DATA_ID_MORNING_SUN,
    WAZA_DATA_ID_SYNTHESIS,
    WAZA_DATA_ID_MOONLIGHT,
    WAZA_DATA_ID_HIDDEN_POWER,
    WAZA_DATA_ID_CROSS_CHOP,
    WAZA_DATA_ID_TWISTER,
    WAZA_DATA_ID_RAIN_DANCE,
    WAZA_DATA_ID_SUNNY_DAY,
    WAZA_DATA_ID_CRUNCH,
    WAZA_DATA_ID_MIRROR_COAT,
    WAZA_DATA_ID_PSYCH_UP,
    WAZA_DATA_ID_EXTREME_SPEED,
    WAZA_DATA_ID_ANCIENT_POWER,
    WAZA_DATA_ID_SHADOW_BALL,
    WAZA_DATA_ID_FUTURE_SIGHT,
    WAZA_DATA_ID_ROCK_SMASH,
    WAZA_DATA_ID_WHIRLPOOL,
    WAZA_DATA_ID_BEAT_UP,
    WAZA_DATA_ID_FAKE_OUT,
    WAZA_DATA_ID_UPROAR,
    WAZA_DATA_ID_STOCKPILE,
    WAZA_DATA_ID_SPIT_UP,
    WAZA_DATA_ID_SWALLOW,
    WAZA_DATA_ID_HEAT_WAVE,
    WAZA_DATA_ID_HAIL,
    WAZA_DATA_ID_TORMENT,
    WAZA_DATA_ID_FLATTER,
    WAZA_DATA_ID_WILL_O_WISP,
    WAZA_DATA_ID_MEMENTO,
    WAZA_DATA_ID_FACADE,
    WAZA_DATA_ID_FOCUS_PUNCH,
    WAZA_DATA_ID_SMELLING_SALT,
    WAZA_DATA_ID_FOLLOW_ME,
    WAZA_DATA_ID_NATURE_POWER,
    WAZA_DATA_ID_CHARGE,
    WAZA_DATA_ID_TAUNT,
    WAZA_DATA_ID_HELPING_HAND,
    WAZA_DATA_ID_TRICK,
    WAZA_DATA_ID_ROLE_PLAY,
    WAZA_DATA_ID_WISH,
    WAZA_DATA_ID_ASSIST,
    WAZA_DATA_ID_INGRAIN,
    WAZA_DATA_ID_SUPERPOWER,
    WAZA_DATA_ID_MAGIC_COAT,
    WAZA_DATA_ID_RECYCLE,
    WAZA_DATA_ID_REVENGE,
    WAZA_DATA_ID_BRICK_BREAK,
    WAZA_DATA_ID_YAWN,
    WAZA_DATA_ID_KNOCK_OFF,
    WAZA_DATA_ID_ENDEAVOR,
    WAZA_DATA_ID_ERUPTION,
    WAZA_DATA_ID_SKILL_SWAP,
    WAZA_DATA_ID_IMPRISON,
    WAZA_DATA_ID_REFRESH,
    WAZA_DATA_ID_GRUDGE,
    WAZA_DATA_ID_SNATCH,
    WAZA_DATA_ID_SECRET_POWER,
    WAZA_DATA_ID_DIVE,
    WAZA_DATA_ID_ARM_THRUST,
    WAZA_DATA_ID_CAMOUFLAGE,
    WAZA_DATA_ID_TAIL_GLOW,
    WAZA_DATA_ID_LUSTER_PURGE,
    WAZA_DATA_ID_MIST_BALL,
    WAZA_DATA_ID_FEATHER_DANCE,
    WAZA_DATA_ID_TEETER_DANCE,
    WAZA_DATA_ID_BLAZE_KICK,
    WAZA_DATA_ID_MUD_SPORT,
    WAZA_DATA_ID_ICE_BALL,
    WAZA_DATA_ID_NEEDLE_ARM,
    WAZA_DATA_ID_SLACK_OFF,
    WAZA_DATA_ID_HYPER_VOICE,
    WAZA_DATA_ID_POISON_FANG,
    WAZA_DATA_ID_CRUSH_CLAW,
    WAZA_DATA_ID_BLAST_BURN,
    WAZA_DATA_ID_HYDRO_CANNON,
    WAZA_DATA_ID_METEOR_MASH,
    WAZA_DATA_ID_ASTONISH,
    WAZA_DATA_ID_WEATHER_BALL,
    WAZA_DATA_ID_AROMATHERAPY,
    WAZA_DATA_ID_FAKE_TEARS,
    WAZA_DATA_ID_AIR_CUTTER,
    WAZA_DATA_ID_OVERHEAT,
    WAZA_DATA_ID_ODOR_SLEUTH,
    WAZA_DATA_ID_ROCK_TOMB,
    WAZA_DATA_ID_SILVER_WIND,
    WAZA_DATA_ID_METAL_SOUND,
    WAZA_DATA_ID_GRASS_WHISTLE,
    WAZA_DATA_ID_TICKLE,
    WAZA_DATA_ID_COSMIC_POWER,
    WAZA_DATA_ID_WATER_SPOUT,
    WAZA_DATA_ID_SIGNAL_BEAM,
    WAZA_DATA_ID_SHADOW_PUNCH,
    WAZA_DATA_ID_EXTRASENSORY,
    WAZA_DATA_ID_SKY_UPPERCUT,
    WAZA_DATA_ID_SAND_TOMB,
    WAZA_DATA_ID_SHEER_COLD,
    WAZA_DATA_ID_MUDDY_WATER,
    WAZA_DATA_ID_BULLET_SEED,
    WAZA_DATA_ID_AERIAL_ACE,
    WAZA_DATA_ID_ICICLE_SPEAR,
    WAZA_DATA_ID_IRON_DEFENSE,
    WAZA_DATA_ID_BLOCK,
    WAZA_DATA_ID_HOWL,
    WAZA_DATA_ID_DRAGON_CLAW,
    WAZA_DATA_ID_FRENZY_PLANT,
    WAZA_DATA_ID_BULK_UP,
    WAZA_DATA_ID_BOUNCE,
    WAZA_DATA_ID_MUD_SHOT,
    WAZA_DATA_ID_POISON_TAIL,
    WAZA_DATA_ID_COVET,
    WAZA_DATA_ID_VOLT_TACKLE,
    WAZA_DATA_ID_MAGICAL_LEAF,
    WAZA_DATA_ID_WATER_SPORT,
    WAZA_DATA_ID_CALM_MIND,
    WAZA_DATA_ID_LEAF_BLADE,
    WAZA_DATA_ID_DRAGON_DANCE,
    WAZA_DATA_ID_ROCK_BLAST,
    WAZA_DATA_ID_SHOCK_WAVE,
    WAZA_DATA_ID_WATER_PULSE,
    WAZA_DATA_ID_DOOM_DESIRE,
    WAZA_DATA_ID_PSYCHO_BOOST,
    WAZA_DATA_ID_NORMAL_END,
    WAZA_DATA_ID_DARK_BLITZ,
    WAZA_DATA_ID_DARK_RUSH,
    WAZA_DATA_ID_DARK_BREAK,
    WAZA_DATA_ID_DARK_END,
    WAZA_DATA_ID_DARK_WAVE,
    WAZA_DATA_ID_DARK_RAVE,
    WAZA_DATA_ID_DARK_STORM,
    WAZA_DATA_ID_DARK_FIRE,
    WAZA_DATA_ID_DARK_BOLT,
    WAZA_DATA_ID_DARK_CHILL,
    WAZA_DATA_ID_DARK_BLAST,
    WAZA_DATA_ID_DARK_SKY,
    WAZA_DATA_ID_DARK_HOLD,
    WAZA_DATA_ID_DARK_MIST,
    WAZA_DATA_ID_DARK_PANIC,
    WAZA_DATA_ID_DARK_DOWN,
    WAZA_DATA_ID_DARK_SHED,
    WAZA_DATA_ID_DARK_HALF,
    WAZA_DATA_ID_SHADOW_SPECIAL,
};
enum WAZA_SHADOW {
    WAZA_SHADOW_FIRST = WAZA_DATA_ID_DARK_BLITZ,
    WAZA_SHADOW_LAST = WAZA_DATA_ID_DARK_HALF,
    WAZA_SHADOW_SPECIAL = WAZA_DATA_ID_SHADOW_SPECIAL,
    WAZA_SHADOW_SPECIAL_ICON = 123,
};

static const u16 WAZA_DATA_ID_NONE = WAZA_DATA_ID_UNUSED;
static const u16 WAZA_DATA_ID_INVALID = 0xFFFF;

enum {
    WAZA_STATUS_NAME_MESSAGE_ID = WAZA_FIELD_NAME,
    WAZA_STATUS_TYPE_ID = WAZA_FIELD_TYPE
};

enum WAZA_TYPE_ID {
    WAZA_TYPE_ID_NONE = 0,
    WAZA_TYPE_ID_IRYOKU = 1,
    WAZA_TYPE_ID_KAIFUKU = 2,
    WAZA_TYPE_ID_NOURYOKU_HENKOU = 3,
    WAZA_TYPE_ID_KYOUKA = 4,
    WAZA_TYPE_ID_POKEMON_JOUTAI_HENKOU = 5,
    WAZA_TYPE_ID_SENJOU_JOUTAI_HENKOU = 6,
    WAZA_TYPE_ID_MAMORI = 7,
    WAZA_TYPE_ID_ICHIGEKI_HISSATSU = 8,
    WAZA_TYPE_ID_TAME = 9,
    WAZA_TYPE_ID_TOKUSHU = 10,
    WAZA_TYPE_ID_HISENTOU = 13,
};

enum WAZA_COMPATIBILITY {
    WAZA_COMPATIBILITY_UNDISPLAYED = 0,
    WAZA_COMPATIBILITY_TYPE_CHART = 1,
    WAZA_COMPATIBILITY_UNDISPLAYED_ALTERNATE = 2,
    WAZA_COMPATIBILITY_UNDISPLAYED_OTHER = 3,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GHOST = 4,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GRASS = 5,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_POISON_OR_STEEL = 6,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GROUND = 7,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_FLYING = 8,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_NORMAL = 9,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY = 10,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY_ALTERNATE = 11,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_DARK = 12,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_FIRE = 13,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY_OTHER = 14,
    WAZA_COMPATIBILITY_ALL_TYPES_NORMAL = 15,
};

enum WAZA_FIGHT_EFFECT_ID {
    WAZA_FIGHT_EFFECT_SLEEP = 1,
    WAZA_FIGHT_EFFECT_POISON = 2,
    WAZA_FIGHT_EFFECT_PARALYSIS = 5,
    WAZA_FIGHT_EFFECT_CONFUSION = 7,
    WAZA_FIGHT_EFFECT_FLINCH = 8,
    WAZA_FIGHT_EFFECT_ATTACK_DOWN = 22,
    WAZA_FIGHT_EFFECT_DEFENSE_DOWN = 23,
    WAZA_FIGHT_EFFECT_SPEED_DOWN = 24,
    WAZA_FIGHT_EFFECT_ACCURACY_DOWN = 27
};

enum { WAZA_ID_MASK = 0xFFFF };
enum { WAZA_DATA_TYPE_ID_COUNT = 3 };
enum {
    ZOKUSEI_DATA_ID_NORMAL = 0,
    ZOKUSEI_DATA_ID_GROUND = 4,
    ZOKUSEI_DATA_ID_ROCK = 5,
    ZOKUSEI_DATA_ID_GRASS = 12,
    ZOKUSEI_DATA_ID_MYSTERY = 9,
    ZOKUSEI_DATA_ID_FLYING = 2,
    ZOKUSEI_DATA_ID_STEEL = 8,
    ZOKUSEI_DATA_ID_FIRE = 10,
    ZOKUSEI_DATA_ID_WATER = 11,
    ZOKUSEI_DATA_ID_ICE = 15,
    ZOKUSEI_DATA_ID_ELECTRIC = 13
};

enum {
    WAZA_STATUS_TYPE = 26,
    WAZA_STATUS_RISK_FLAG = 27
};

// An entry of the move database: everything the fight reads about one move.
struct WazaData {
    u8 mPri; // 0x00
    u8 mPp; // 0x01
    u8 mZokuseiDataId; // 0x02
    u8 mRangeId; // 0x03
    u8 mAvg; // 0x04
    u8 mAddFightKoukaAvg; // 0x05
    u8 mDageki; // 0x06
    u8 mMamoru; // 0x07
    u8 mMajikku; // 0x08
    u8 mYokodori; // 0x09
    u8 mOumugaesi; // 0x0A
    u8 mOujanosirusi; // 0x0B
    u8 mYubiwohuru;
    u8 mMonomane;
    u8 mNekonote;
    u8 mNegoto;
    u8 mBouon; // 0x10
    u8 mPressure;
    u8 mHidenFlag; // 0x12
    u8 mZokuseiKindData; // 0x13
    u8 mRiskFlag; // 0x14
    u8 mCompatible; // 0x15
    u8 _16[2];
    s16 mIryoku; // 0x18
    u16 mFightKoukaDataId;
    u16 mSeqId; // 0x1C
    u16 mWazaFuncDataId; // 0x1E
    u32 mNameID; // 0x20
    u32 mFightAttackTunagiMsgId;
    u32 mFightAttackMsgId;
    u32 mDoc;
    u32 mWazawzxdataId; // 0x30
    u8 mTypeId[WAZA_DATA_TYPE_ID_COUNT]; // 0x34
    u8 _37[1];
};

struct FIGHT_TRAINER;
struct FightOutPokemon;
struct FightPokemon;

// Handlers a move is fought through: the trainer AI weighs the move up, asks
// whether it lands and how hard it hits, and the fight plays its animation.
typedef s32 (*FightTrainerAiWazaValueFunc)(struct FIGHT_TRAINER *trainer, struct FightOutPokemon *attacker,
                                           u16 wazaDataId, struct FightOutPokemon *defender);
typedef u8 (*FightTrainerAiWazaHitFunc)(struct FIGHT_TRAINER *trainer, struct FightOutPokemon *attacker,
                                        u16 wazaDataId, struct FightOutPokemon *defender);
typedef s32 (*FightTrainerAiWazaDamageFunc)(struct FIGHT_TRAINER *trainer, struct FightOutPokemon *attacker,
                                            u16 wazaDataId, struct FightOutPokemon *defender);
typedef s32 (*FightTrainerAiWazaDamageFightPokemonFunc)(struct FIGHT_TRAINER *trainer,
                                                        struct FightOutPokemon *attacker, u16 wazaDataId,
                                                        struct FightPokemon *defender);
typedef void (*FightWazaWzxTypeFunc)(u16 wazaId, struct FightOutPokemon *attacker,
                                     struct FightOutPokemon *defender, u16 invocation, u8 variation);
typedef u8 (*FightWazaWzxVariationFunc)(u16 wazaId, struct FightOutPokemon *attacker,
                                        struct FightOutPokemon *defender, u16 invocation);

// Function table the fight dispatches a move through, looked up by the
// function data id of the move.
struct WazaFuncData {
    FightTrainerAiWazaValueFunc mFightTrainerAiWazaValueFunc; // 0x00
    FightTrainerAiWazaHitFunc mFightTrainerAiWazaHitFunc; // 0x04
    FightTrainerAiWazaDamageFunc mFightTrainerAiWazaDamageFunc; // 0x08
    FightTrainerAiWazaDamageFightPokemonFunc mFightTrainerAiWazaDamageFightPokemonFunc; // 0x0C
    FightWazaWzxTypeFunc mFightWazaWzxTypeFunc; // 0x10
    FightWazaWzxVariationFunc mFightWazaWzxVariationFunc; // 0x14
};

#ifdef __cplusplus
extern "C" {
#endif

extern struct WazaFuncData waza_func_data[];
extern u32 waza_func_data_number;

struct WazaData *wazaDataBiosGetPtr(u16 wazaID);
u32 wazaDataBiosGetName(struct WazaData *data);
u16 wazaDataBiosGetWazaFuncDataId(struct WazaData *data);
struct WazaFuncData *wazaFuncDataBiosGetPtr(u16 funcDataId);
u32 wazaGetWazaTypeIdName(u8 typeId);
u8 wazaIsWazaTypeId(u16 move, u8 typeId);
u8 wazaGetMaxPP(u16 move, u8 ppUpCount);

s32 wazaGetStatus(void *data, u16 wazaID, u16 fieldID, u16 index);
void wazaSetStatus(void *data, u16 wazaID, u16 fieldID, u16 index, s32 value);

u8 wazaCheckDarkWaza(u16 move);
u8 waza_GetZokuseiKindData(u16 move);

u8 wazaDB_GetCompatible(u16 wazaID);
s16 wazaDB_GetIryoku(u16 move);
u16 wazaDB_GetSeqId(u16 move);
u8 wazaDB_GetOujanosirusi(u16 move);
s32 wazaDB_GetName(u16 move);
u8 wazaDB_GetRangeId(u16 move);
u8 wazaDB_GetOumugaesi(u16 wazaDataId);
u8 wazaDB_GetZokuseiDataId(u16 move);
u8 wazaDB_GetRiskFlag(u16 move);
u32 wazaDB_GetWazawzxdataId(u16 move);
u8 wazaDB_GetTypeId(u16 move, u16 index);
u8 wazaDB_GetBouon(u16 move);
u8 wazaDB_GetYokodori(u16 move);
u8 wazaDB_GetMajikku(u16 move);
u8 wazaDB_GetMamoru(u16 move);
u8 wazaDB_GetDageki(u16 move);
u8 wazaDB_GetAddFightKoukaAvg(u16 move);
u8 wazaDB_GetAvg(u16 move);
s32 wazaDB_GetPri(u16 move);
u8 wazaDB_GetPp(u16 move);

FightWazaWzxVariationFunc wazaDB_GetFightWazaWzxVariationFuncPtr(u16 move);
FightWazaWzxTypeFunc wazaDB_GetFightWazaWzxTypeFuncPtr(u16 move);

u8 fightWazaWzxVariationFuncNull(u16 move, FightOutPokemon *attackPokemon,
    FightOutPokemon *defensePokemon, u16 invocation);
void fightWazaWzxTypeFuncNull(u16 move, FightOutPokemon *attackPokemon,
    FightOutPokemon *defensePokemon, u16 invocation, u8 variation);
void *wazaDB_GetFightTrainerAiWazaDamageFightPokemonFuncPtr(u16 move);
void *wazaDB_GetFightTrainerAiWazaDamageFuncPtr(u16 move);
void *wazaDB_GetFightTrainerAiWazaHitFuncPtr(u16 move);
void *wazaDB_GetFightTrainerAiWazaValueFuncPtr(u16 move);

void wazaDataBiosSetName(struct WazaData *data, u32 value);
void wazaDataBiosSetPp(struct WazaData *data, u8 value);
void wazaDataBiosSetZokuseiDataId(struct WazaData *data, u8 value);
void wazaDataBiosSetPri(struct WazaData *data, s8 value);
void wazaDataBiosSetRangeId(struct WazaData *data, u8 value);
void wazaDataBiosSetAvg(struct WazaData *data, u8 value);
void wazaDataBiosSetIryoku(struct WazaData *data, s16 value);
void wazaDataBiosSetFightKoukaDataId(struct WazaData *data, u16 value);
void wazaDataBiosSetSeqId(struct WazaData *data, u16 value);
void wazaDataBiosSetFightAttackTunagiMsgId(struct WazaData *data, u32 value);
void wazaDataBiosSetFightAttackMsgId(struct WazaData *data, u32 value);
void wazaDataBiosSetAddFightKoukaAvg(struct WazaData *data, u8 value);
void wazaDataBiosSetDageki(struct WazaData *data, u8 value);
void wazaDataBiosSetMamoru(struct WazaData *data, u8 value);
void wazaDataBiosSetMajikku(struct WazaData *data, u8 value);
void wazaDataBiosSetYokodori(struct WazaData *data, u8 value);
void wazaDataBiosSetOumugaesi(struct WazaData *data, u8 value);
void wazaDataBiosSetOujanosirusi(struct WazaData *data, u8 value);
void wazaDataBiosSetYubiwohuru(struct WazaData *data, u8 value);
void wazaDataBiosSetMonomane(struct WazaData *data, u8 value);
void wazaDataBiosSetNekonote(struct WazaData *data, u8 value);
void wazaDataBiosSetNegoto(struct WazaData *data, u8 value);
void wazaDataBiosSetBouon(struct WazaData *data, u8 value);
void wazaDataBiosSetPressure(struct WazaData *data, u8 value);
void wazaDataBiosSetHidenFlag(struct WazaData *data, u8 value);
void wazaDataBiosSetTypeId(struct WazaData *data, u16 index, u8 typeId);
void wazaDataBiosSetRiskFlag(struct WazaData *data, u8 value);
void wazaDataBiosSetFightTrainerAiWazaValueFuncPtr(struct WazaData *data, FightTrainerAiWazaValueFunc func);
void wazaDataBiosSetFightTrainerAiWazaHitFuncPtr(struct WazaData *data, FightTrainerAiWazaHitFunc func);
void wazaDataBiosSetFightTrainerAiWazaDamageFuncPtr(struct WazaData *data, FightTrainerAiWazaDamageFunc func);
void wazaDataBiosSetWazawzxdataId(struct WazaData *data, u32 value);
void wazaDataBiosSetFightWazaWzxTypeFuncPtr(struct WazaData *data, FightWazaWzxTypeFunc func);
void wazaDataBiosSetFightWazaWzxVariationFuncPtr(struct WazaData *data, FightWazaWzxVariationFunc func);
void wazaDataBiosSetDoc(struct WazaData *data, u32 value);
u8 wazaDataBiosGetPp(struct WazaData *data);
u8 wazaDataBiosGetZokuseiDataId(struct WazaData *data);
s8 wazaDataBiosGetPri(struct WazaData *data);
u8 wazaDataBiosGetRangeId(struct WazaData *data);
u8 wazaDataBiosGetAvg(struct WazaData *data);
s16 wazaDataBiosGetIryoku(struct WazaData *data);
u16 wazaDataBiosGetFightKoukaDataId(struct WazaData *data);
u16 wazaDataBiosGetSeqId(struct WazaData *data);
u32 wazaDataBiosGetFightAttackTunagiMsgId(struct WazaData *data);
u32 wazaDataBiosGetFightAttackMsgId(struct WazaData *data);
u8 wazaDataBiosGetAddFightKoukaAvg(struct WazaData *data);
u8 wazaDataBiosGetDageki(struct WazaData *data);
u8 wazaDataBiosGetMamoru(struct WazaData *data);
u8 wazaDataBiosGetMajikku(struct WazaData *data);
u8 wazaDataBiosGetYokodori(struct WazaData *data);
u8 wazaDataBiosGetOumugaesi(struct WazaData *data);
u8 wazaDataBiosGetOujanosirusi(struct WazaData *data);
u8 wazaDataBiosGetYubiwohuru(struct WazaData *data);
u8 wazaDataBiosGetMonomane(struct WazaData *data);
u8 wazaDataBiosGetNekonote(struct WazaData *data);
u8 wazaDataBiosGetNegoto(struct WazaData *data);
u8 wazaDataBiosGetBouon(struct WazaData *data);
u8 wazaDataBiosGetPressure(struct WazaData *data);
u8 wazaDataBiosGetHidenFlag(struct WazaData *data);
u8 wazaDataBiosGetTypeId(struct WazaData *data, u16 index);
u8 wazaDataBiosGetRiskFlag(struct WazaData *data);
FightTrainerAiWazaValueFunc wazaDataBiosGetFightTrainerAiWazaValueFuncPtr(struct WazaData *data);
FightTrainerAiWazaHitFunc wazaDataBiosGetFightTrainerAiWazaHitFuncPtr(struct WazaData *data);
FightTrainerAiWazaDamageFunc wazaDataBiosGetFightTrainerAiWazaDamageFuncPtr(struct WazaData *data);
u32 wazaDataBiosGetWazawzxdataId(struct WazaData *data);
FightWazaWzxTypeFunc wazaDataBiosGetFightWazaWzxTypeFuncPtr(struct WazaData *data);
FightWazaWzxVariationFunc wazaDataBiosGetFightWazaWzxVariationFuncPtr(struct WazaData *data);
u32 wazaDataBiosGetDoc(struct WazaData *data);
#ifdef __cplusplus
}
#endif

#endif
