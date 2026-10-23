#include <darkPokemon.h>
#include <GSgfx.h>
#include <gamedata.h>
#include <savedata.h>
#include <memo.h>
#include <people.h>
#include <hero.h>
#include <heroMove.h>
#include <item.h>

namespace {

static const f32 kRadToDeg = 57.29577951f;

enum {
    HERO_OBJ_ID_STYLE0 = 0x16C90400,
    HERO_OBJ_ID_STYLE1 = 0x20250400,
    HERO_OBJ_ID_STYLE2 = 0x169F0400,
    HERO_OBJ_ID_STYLE0_FOLLOWER = 0x169E0400,
    HERO_OBJ_ID_STYLE1_FOLLOWER = 0x1EE20400,
    HERO_OBJ_ID_STYLE2_FOLLOWER = 0x16A00400,
};

enum {
    GAMEDATA_OPTION_RECORD = 1,
};

enum {
    CATCH_BALL_ENCOUNTER = 4,
};

enum {
    PEOPLE_SLOT_HERO = 0,
    PEOPLE_KIND_PLAYER_MODEL = 100,
};

enum {
    WAZA_EMPTY = 0x176,
};

} // namespace

bool Hero::itemSort(u8 kind)
{
    u16 num;
    u8 index = kind - 1;
    void *ary = heroItemGetItemKindToItemAryPtr(this, kind, &num, 0, 0, 0);

    if (index > 1) {
        return false;
    }
    itemSortAryByPriority(ary, num, kind);
    return true;
}

void Hero::setBattleResumeFloorIndex(u8 index)
{
    mBattleResumeFloorIndex = index;
}

u8 Hero::getBattleResumeFloorIndex() const
{
    return mBattleResumeFloorIndex;
}

void Hero::setBattleResumeFloorID(unsigned int floorID)
{
    mBattleResumeFloorID = floorID;
}

unsigned int Hero::getBattleResumeFloorID() const
{
    return mBattleResumeFloorID;
}

long Hero::getHeroObjID(long followerModelLevel, long heroStyle)
{
    long objID = HERO_OBJ_ID_STYLE0;

    if (heroStyle < 0) {
        heroStyle = getHeroStyle();
    }
    if (followerModelLevel < 0) {
        followerModelLevel = getFollowerModelLevel();
    }

    if (followerModelLevel == 1) {
        switch (heroStyle) {
        case 0:
            objID = HERO_OBJ_ID_STYLE0_FOLLOWER;
            break;
        case 1:
            objID = HERO_OBJ_ID_STYLE1_FOLLOWER;
            break;
        case 2:
            objID = HERO_OBJ_ID_STYLE2_FOLLOWER;
            break;
        }
    } else {
        switch (heroStyle) {
        case 0:
            objID = HERO_OBJ_ID_STYLE0;
            break;
        case 1:
            objID = HERO_OBJ_ID_STYLE1;
            break;
        case 2:
            objID = HERO_OBJ_ID_STYLE2;
            break;
        }
    }
    return objID;
}

void Hero::setHeroStyle(u8 style, bool readObject)
{
    if (mHeroStyle == style) {
        return;
    }
    mHeroStyle = style;
    if (!readObject) {
        return;
    }

    long objID = HERO_OBJ_ID_STYLE0;
    switch (style) {
    case 0:
        objID = HERO_OBJ_ID_STYLE0;
        break;
    case 1:
        objID = HERO_OBJ_ID_STYLE1;
        break;
    case 2:
        objID = HERO_OBJ_ID_STYLE2;
        break;
    }
    peopleReadObject(PEOPLE_SLOT_HERO, PEOPLE_KIND_PLAYER_MODEL, objID);
}

u8 Hero::getHeroStyle() const
{
    return mHeroStyle;
}

s16 Hero::pokemonGet(HeroPokemonGetParam *param, bool allowStorageFallback)
{
    Pokemon pokemon;
    s16 result;

    pokemonCreate(&pokemon, param->species, param->level, (GamedataAttest *)gamedataGetStatus(0, GAMEDATA_OPTION_RECORD));
    pokemon.setFriendLevel(param->friendLevel);
    pokemon.setCatchFloorId((u16)param->catchFloorId);
    pokemon.setCatchLevel(param->level);
    pokemon.setCatchBallId(CATCH_BALL_ENCOUNTER);
    pokemon.setCatchTrainerSex(param->sexDataId);
    pokemon.setCatchTrainerRnd(param->trainerId);
    pokemon.setCatchTrainerNamePtr(param->catchTrainerName);
    if (param->nickname != 0) {
        pokemon.setNicknamePtr(param->nickname);
    }
    pokemon.setExp(param->exp);

    for (u16 i = 0; i < 4; i++) {
        u16 waza = param->waza[i];
        if (waza != 0 && waza != WAZA_EMPTY) {
            pokemonWazaCreate(&pokemon, i, waza);
        }
    }

    s32 fit;

    if (param->shadowRecordId != 0) {
        fit = pokemonCreateRndFit(&pokemon, -1, -1, 0, param->trainerId);
    } else {
        fit = pokemonCreateRndFit(&pokemon, -1, -1, -1, param->trainerId);
    }
    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_PID, 0, fit);
    if (param->shadowRecordId != 0) {
        pokemonSetDarkPokemonStatus(&pokemon, param->shadowRecordId);
    }
    pokemonResetBasisStatus(&pokemon);
    memoDataSet(0, &pokemon);

    result = heroGetPokemon(this, &pokemon, allowStorageFallback);

    if (param->shadowRecordId != 0 && result >= 0) {
        DarkPokemon *dark = pokemon.getDarkPokemon();
        if (dark != 0) {
            dark->setPlace(DARK_POKEMON_WHERE_PARTY, (u8)result);
            dark->addStatus(DARK_POKEMON_SNATCHED);
            if (dark->getSnatchFromWho() == DARK_POKEMON_SNATCH_NONE) {
                dark->setSnatchFromWho(DARK_POKEMON_SNATCH_HERO);
            }
        }
    }
    return result;
}

s32 Hero::getLegendPokemonSize() const
{
    s32 i = 0;
    s32 count = 0;

    for (; i < 6; i++) {
        const Pokemon *pokemon = getPokemon(i);
        if (pokemon->checkValid() != 0 && pokemon->isLegend()) {
            count++;
        }
    }
    return count;
}

u16 Hero::itemQuantity(u16 itemID)
{
    u16 num = 0;
    u16 max = 0;
    u8 sortMode;
    void *ary;

    ary = heroItemGetItemDataIdToItemAryPtr(this, itemID, &num, &max, &sortMode, 0);
    if (ary == 0) {
        return 0;
    }
    return itemGetQuantityInAry(ary, num, itemID, max);
}

Hero *Hero::getHeroPtr()
{
    return (Hero *)savedataGetStatus(0, TRAINER_DATA_HERO);
}

void Hero::getRestertPos(GSvec *pos, s16 &angle) const
{
    pos->x = mRestertX;
    pos->y = mRestertY;
    pos->z = mRestertZ;
    angle = mRestertAngle;
}

void Hero::setRestertPos()
{
    GSvec pos;
    GSvec rot;

    heroMoveGetHeroPos(&pos);
    heroMoveGetHeroRot(&rot);

    mRestertX = pos.x;
    mRestertY = pos.y;
    mRestertZ = pos.z;
    mRestertAngle = rot.y * kRadToDeg;
}

bool Hero::deletePokemon(long index)
{
    Pokemon *pokemon = getPokemon(index);

    if (pokemon == 0 || pokemon->checkValid() == 0) {
        return false;
    }
    pokemon->clear();
    return true;
}

bool Hero::setPokemon(const Pokemon *src, long index)
{
    Pokemon *pokemon = getPokemon(index);

    // A party slot is only written while it reads as empty.
    if (pokemon == 0 || pokemon->checkValid() != 0) {
        return false;
    }

    pokemonBiosCopy(pokemon, src);
    DarkPokemon *dark = pokemon->getDarkPokemon();
    if (dark != 0) {
        dark->setPlace(DARK_POKEMON_WHERE_PARTY, 0);
    }
    return true;
}

long Hero::addPokemon(const Pokemon *pokemon)
{
    for (u8 i = 0; i < 6; i++) {
        if (getPokemon(i)->checkValid() == 0) {
            setPokemon(pokemon, i);
            return i;
        }
    }
    return -1;
}

Pokemon *Hero::getPokemon(long index, bool &isHatena)
{
    Pokemon *pokemon = getPokemon(index);

    isHatena = false;
    if (pokemon == 0) {
        return 0;
    }
    if (pokemon->getPokemonDataId() == 0) {
        return 0;
    }
    if (gamedataAttestCheckValid(pokemon->getAttest()) == 0) {
        return 0;
    }
    if (pokemon->isFuseiFlag() != 0) {
        return 0;
    }
    isHatena = pokemon->isHatena();
    return pokemon;
}

const Pokemon *Hero::getPokemon(long index) const
{
    if (index < 0 || index >= 6) {
        return 0;
    }
    return &mPokemon[index];
}

Pokemon *Hero::getPokemon(long index)
{
    if (index < 0 || index >= 6) {
        return 0;
    }
    return &mPokemon[index];
}

void Hero::setMeetDarkPokemonCount(u8 count)
{
    mMeetDarkPokemonCount = count;
}

u8 Hero::getMeetDarkPokemonCount() const
{
    return mMeetDarkPokemonCount;
}

void *Hero::getDisk()
{
    return mDisk;
}

void Hero::addFootStep(unsigned long step)
{
    u32 total = mFootStep + step;

    mFootStep = (mFootStep > total) ? (u32)-1 : total;
}

u32 Hero::getFootStep() const
{
    return mFootStep;
}
