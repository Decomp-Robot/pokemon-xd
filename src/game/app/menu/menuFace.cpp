#include <menuFace.h>
#include <menuFaceBios.h>
#include <menuCBPokemonEntry.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <fightPokemon.h>
#include <GSmathTypes.h>
#include <GSmem.h>
#include <GSres.h>
#include <GSfsys.h>
#include <GStexture.h>
#include <GSthread.h>
#include <string.h>
#include <dolphin/os/OSCache.h>
#include <dolphin/gx/GXTexture.h>

#define FACE_SIZE 42
#define ABS(x) ((x) < 0 ? -(x) : (x))

// Resource group the face textures are read into and registered under.
#define MENU_FACE_GROUP 1472

// Size and alignment of the buffer one loaded face texture is kept in.
#define MENU_FACE_TEX_SIZE 8032
#define MENU_FACE_TEX_ALIGN 32

// Load state of a cached face.
enum MENU_FACE_STATE {
    MENU_FACE_STATE_WAIT = 0, // listed, not read yet
    MENU_FACE_STATE_LOADING,  // read requested, waiting for the callback
    MENU_FACE_STATE_ENABLE    // texture loaded and registered
};

// One entry of the face cache. The list is kept in use order: the most
// recently requested face is at the end, the oldest one at the start.
struct MenuFaceList {
    u32 mTexID;                       // resource id of the face texture
    u16 mFaceID;
    u8 mState;
    s8 mTexIndex;                     // buffer of pMenuFaceTexList the entry owns
    MenuFaceLoadCallback mCallback;   // next face request, see menuFaceLoad
    void* mContext;
};

// Texture buffer a cache entry decodes its face into.
struct MenuFaceTexList {
    u32 _00;
    void* mBuffer;
};

s32 _menuFaceListNum;
MenuFaceList* pMenuFaceList;
MenuFaceTexList* pMenuFaceTexList;

static u8 _menuFaceLoadNormal;
static u8 _menuFaceDeoxysSel;

// Portrait IDs used by the menu, including alternate forms.
enum MENU_FACE_ID {
    MENU_FACE_ID_DEOXYS = 385,
    MENU_FACE_ID_DEOXYS_RARE = 771,
    MENU_FACE_ID_ANNON = 773,             // first Unown letter face, 28 in a row.
    MENU_FACE_ID_ANNON_RARE = 801,        // first shiny Unown letter face.
    MENU_FACE_ID_TAMAGO = 829,            // egg, also shown for an invalid pokemon.
    MENU_FACE_ID_DEOXYS_SPEED = 830,
    MENU_FACE_ID_DEOXYS_DEFENSE = 831,
    MENU_FACE_ID_DEOXYS_ATTACK = 832,
    MENU_FACE_ID_DEOXYS_SPEED_RARE = 834,
    MENU_FACE_ID_DEOXYS_DEFENSE_RARE = 835,
    MENU_FACE_ID_DEOXYS_ATTACK_RARE = 836,
    MENU_FACE_ID_DARK_RUGIA = 837         // Shadow Lugia.
};

// Deoxys form selection, as handed out by the GBA ROM version.
enum MENU_FACE_DEOXYS_SEL {
    MENU_FACE_DEOXYS_NORMAL = 0,
    MENU_FACE_DEOXYS_ATTACK = 1,
    MENU_FACE_DEOXYS_DEFENSE = 2,
    MENU_FACE_DEOXYS_SPEED = 3
};

#define ANNON_KATATI_MAX 28

void _menuFaceLoadCallBack(long fileMode, void* context, long argument);
s32 _menuFaceGetListNum(void);

static bool _unloadTexture(void* data, unsigned long group, unsigned long res)
{
    GStextureFree((GStexture*)data);
    return true;
}

void menuFaceInit(s32 num)
{
    s32 i;

    _menuFaceListNum = num;
    if (pMenuFaceList == NULL) {
        pMenuFaceList = (MenuFaceList*)GSmemAlloc(num * sizeof(MenuFaceList));
    }
    memset(pMenuFaceList, 0, _menuFaceListNum * sizeof(MenuFaceList));

    if (pMenuFaceTexList == NULL) {
        pMenuFaceTexList = (MenuFaceTexList*)GSmemAlloc(_menuFaceListNum * sizeof(MenuFaceTexList));
    }
    memset(pMenuFaceTexList, 0, _menuFaceListNum * sizeof(MenuFaceTexList));

    for (i = 0; i < _menuFaceListNum; i++) {
        if (pMenuFaceTexList[i].mBuffer == NULL) {
            pMenuFaceTexList[i].mBuffer = GSmemAllocAlign(MENU_FACE_TEX_SIZE, MENU_FACE_TEX_ALIGN);
        }
        memset(pMenuFaceTexList[i].mBuffer, 0, MENU_FACE_TEX_SIZE);
        pMenuFaceList[i].mTexIndex = i;
    }
}

void menuFaceAllocMemory(void)
{
}

s32 _menuFaceSearchList(u16 faceID)
{
    u32 texID = menuFaceGetTexID(faceID);
    MenuFaceList* list = pMenuFaceList;
    s32 i;

    for (i = 0; i < _menuFaceListNum; i++) {
        if (texID == list->mTexID) {
            return i;
        }
        list++;
    }
    return -1;
}

s32 _menuFaceGetListNum(void)
{
    MenuFaceList* list = pMenuFaceList;
    s32 i;

    for (i = 0; i < _menuFaceListNum; i++) {
        if (list->mTexID == 0) {
            return i;
        }
        list++;
    }
    return i;
}

s32 _menuFaceAddList(u16 faceID, MenuFaceLoadCallback callback, void* context)
{
    MenuFaceList work;
    u32 texID;
    s32 num;
    s32 index;
    s32 i;

    texID = menuFaceGetTexID(faceID);
    if (texID == 0) {
        return -2;
    }

    num = _menuFaceGetListNum();
    index = _menuFaceSearchList(faceID);
    if (index < 0) {
        if (num == _menuFaceListNum) {
            work = pMenuFaceList[0];
            if (pMenuFaceList[0].mState == MENU_FACE_STATE_ENABLE) {
                GSresFreeResource(MENU_FACE_GROUP, pMenuFaceList[0].mTexID);
            }
            for (i = 0; i < num - 1; i++) {
                pMenuFaceList[i] = pMenuFaceList[i + 1];
            }
            num = _menuFaceListNum - 1;
            pMenuFaceList[num] = work;
        }
        pMenuFaceList[num].mTexID = texID;
        pMenuFaceList[num].mFaceID = faceID;
        pMenuFaceList[num].mState = MENU_FACE_STATE_WAIT;
        pMenuFaceList[num].mCallback = callback;
        pMenuFaceList[num].mContext = context;
        return num;
    }

    work = pMenuFaceList[index];
    for (; index < num - 1; index++) {
        pMenuFaceList[index] = pMenuFaceList[index + 1];
    }
    pMenuFaceList[index] = work;
    return index;
}

s8 menuFaceSearchEnable(u16 faceID)
{
    s32 index = _menuFaceSearchList(faceID);

    if (index < 0) {
        return -1;
    }
    return pMenuFaceList[index].mState == MENU_FACE_STATE_ENABLE;
}

u16 _menuFaceGetIDSub(Pokemon* pokemon, u8 deoxysSel, s8 rare)
{
    static u16 unknown_face_id_tbl[ANNON_KATATI_MAX][2] = {
        { 773, 801 }, { 774, 802 }, { 775, 803 }, { 776, 804 },
        { 777, 805 }, { 778, 806 }, { 779, 807 }, { 780, 808 },
        { 781, 809 }, { 782, 810 }, { 783, 811 }, { 784, 812 },
        { 785, 813 }, { 786, 814 }, { 787, 815 }, { 788, 816 },
        { 789, 817 }, { 790, 818 }, { 791, 819 }, { 792, 820 },
        { 793, 821 }, { 794, 822 }, { 795, 823 }, { 796, 824 },
        { 797, 825 }, { 798, 826 }, { 799, 827 }, { 800, 828 },
    };
    u16 species;
    u8 rareFlag;
    s32 katati;
    u16 faceID;

    if (pokemon == NULL) {
        return 0;
    }
    if (!pokemonCheckValid(pokemon)) {
        return MENU_FACE_ID_TAMAGO;
    }
    if (pokemonBiosGetTamagoFlag(pokemon)) {
        return MENU_FACE_ID_TAMAGO;
    }

    rareFlag = pokemonCheckRare2(pokemon);
    if (rare >= 0) {
        rareFlag = (rare != 0);
    }

    species = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
    if (species == POKEMON_SPECIES_DEOXYS) {
        if (rareFlag == 0) {
            if (deoxysSel == MENU_FACE_DEOXYS_ATTACK) {
                faceID = MENU_FACE_ID_DEOXYS_ATTACK;
            } else if (deoxysSel == MENU_FACE_DEOXYS_DEFENSE) {
                faceID = MENU_FACE_ID_DEOXYS_DEFENSE;
            } else if (deoxysSel == MENU_FACE_DEOXYS_SPEED) {
                faceID = MENU_FACE_ID_DEOXYS_SPEED;
            } else {
                faceID = pokemonGetStatus(NULL, species, POKEMON_BIOS_FIELD_FACE_ID, 0);
            }
        } else {
            if (deoxysSel == MENU_FACE_DEOXYS_ATTACK) {
                faceID = MENU_FACE_ID_DEOXYS_ATTACK_RARE;
            } else if (deoxysSel == MENU_FACE_DEOXYS_DEFENSE) {
                faceID = MENU_FACE_ID_DEOXYS_DEFENSE_RARE;
            } else if (deoxysSel == MENU_FACE_DEOXYS_SPEED) {
                faceID = MENU_FACE_ID_DEOXYS_SPEED_RARE;
            } else {
                faceID = pokemonGetStatus(NULL, species, POKEMON_BIOS_FIELD_FACE_ID, 1);
            }
        }
    } else if (species == POKEMON_SPECIES_RUGIA) {
        if (pokemonIsDarkPokemon(pokemon)) {
            return MENU_FACE_ID_DARK_RUGIA;
        }
        if (rareFlag == 0) {
            faceID = pokemonGetStatus(NULL, species, POKEMON_BIOS_FIELD_FACE_ID, 0);
        } else {
            faceID = pokemonGetStatus(NULL, species, POKEMON_BIOS_FIELD_FACE_ID, 1);
        }
    } else if (species == POKEMON_SPECIES_ANNON) {
        katati = pokemonGetAnnonKatati(pokemonBiosGetRnd(pokemon));
        if (rareFlag == 0) {
            faceID = unknown_face_id_tbl[katati][0];
        } else {
            faceID = unknown_face_id_tbl[katati][1];
        }
    } else {
        if (rareFlag == 0) {
            faceID = pokemonGetStatus(NULL, species, POKEMON_BIOS_FIELD_FACE_ID, 0);
        } else {
            faceID = pokemonGetStatus(NULL, species, POKEMON_BIOS_FIELD_FACE_ID, 1);
        }
    }
    return faceID;
}

u16 menuFaceGetID(Pokemon* pokemon)
{
    return _menuFaceGetIDSub(pokemon, _menuFaceDeoxysSel, -1);
}

u16 menuFaceGetID2(Pokemon* pokemon, u8 deoxysSel)
{
    return _menuFaceGetIDSub(pokemon, deoxysSel, -1);
}

u16 menuFaceGetID3(Pokemon* pokemon, u8 deoxysSel, s8 rare)
{
    return _menuFaceGetIDSub(pokemon, deoxysSel, rare);
}

u32 menuFaceGetTexID(u16 faceID)
{
    return menuFaceBiosGetTexID(faceID);
}

u8 _menuFaceDispFromID(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, u16 faceID, GSvec2* pos)
{
    static tagSPRITE_WORK sprite_work;
    u32 texID;
    GStexture* texture;

    if (menuFaceSearchEnable(faceID) != 1) {
        return 0;
    }
    texID = menuFaceGetTexID(faceID);
    if (texID == 0) {
        return 0;
    }
    texture = (GStexture*)GSresGetResourceAllGroup(texID);
    if (texture == NULL) {
        return 0;
    }

    sprite_work = *sprite;
    sprite_work.mSpriteValue = texID;
    if (pos != NULL) {
        sprite_work.mX = pos->x;
        sprite_work.mY = pos->y;
    } else {
        sprite_work.mX = 0;
        sprite_work.mY = 0;
    }
    sprite_work.mSpriteX = 0;
    sprite_work.mSpriteY = 0;
    sprite_work.mTextureWidth = FACE_SIZE;
    sprite_work.mTextureHeight = FACE_SIZE;
    if (menuFaceBiosGetRareFlag(faceID) && GStextureGetYsize(texture) > FACE_SIZE) {
        sprite_work.mSpriteY = FACE_SIZE;
    }
    // A negative width asks for the face to look the other way.
    if (sprite_work.mWidth < 0 && GStextureGetXsize(texture) > FACE_SIZE) {
        sprite_work.mSpriteX = FACE_SIZE;
        sprite_work.mWidth = ABS(sprite_work.mWidth);
    }
    winSpriteDrawTexture(window, &sprite_work);
    return 1;
}

u8 _menuFaceDispFromID3(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, u16 faceID, GSivec2* size, GSivec2* pos)
{
    static tagSPRITE_WORK sprite_work;
    u32 texID;
    GStexture* texture;

    if (menuFaceSearchEnable(faceID) != 1) {
        return 0;
    }
    texID = menuFaceGetTexID(faceID);
    if (texID == 0) {
        return 0;
    }
    texture = (GStexture*)GSresGetResourceAllGroup(texID);
    if (texture == NULL) {
        return 0;
    }

    sprite_work = *sprite;
    sprite_work.mSpriteValue = texID;
    if (pos != NULL) {
        sprite_work.mX = pos->x;
        sprite_work.mY = pos->y;
    } else {
        sprite_work.mX = 0;
        sprite_work.mY = 0;
    }
    sprite_work.mSpriteX = 0;
    sprite_work.mSpriteY = 0;
    sprite_work.mTextureWidth = FACE_SIZE;
    sprite_work.mTextureHeight = FACE_SIZE;
    if (menuFaceBiosGetRareFlag(faceID) && GStextureGetYsize(texture) > FACE_SIZE) {
        sprite_work.mSpriteY = FACE_SIZE;
    }
    sprite_work.mWidth = size->x;
    sprite_work.mHeight = size->y;
    // A negative width asks for the face to look the other way.
    if (size->x < 0 && GStextureGetXsize(texture) > FACE_SIZE) {
        sprite_work.mSpriteX = FACE_SIZE;
        sprite_work.mWidth = ABS(size->x);
    }
    winSpriteDrawTexture(window, &sprite_work);
    return 1;
}

u8 menuFaceDispFromID(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, u16 faceID)
{
    return _menuFaceDispFromID(window, sprite, faceID, NULL);
}

u8 menuFaceDispFromID2(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, u16 faceID, GSvec2* pos)
{
    return _menuFaceDispFromID(window, sprite, faceID, pos);
}

u8 menuFaceDispFromID3(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, u16 faceID, GSivec2* size, GSivec2* pos)
{
    return _menuFaceDispFromID3(window, sprite, faceID, size, pos);
}

u8 menuFaceDisp(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, Pokemon* pokemon)
{
    return menuFaceDispFromID(window, sprite, menuFaceGetID(pokemon));
}

void _menuFaceLoadCallBack(long fileMode, void* context, long argument)
{
    u16 faceID = argument;
    MenuFaceList* list;
    GStexture* texture;
    u32 texID;
    s32 index;

    texID = menuFaceGetTexID(faceID);
    index = _menuFaceSearchList(faceID);
    if (index < 0) {
        if (texID != 0) {
            GSresFreeResource(MENU_FACE_GROUP, texID);
        }
        return;
    }

    list = &pMenuFaceList[index];
    texture = (GStexture*)GSresGetResource(MENU_FACE_GROUP, texID);

    // Turn the image and palette pointers back into file offsets so
    // GStextureLoad can resolve them against the copy.
    texture->mStateDirty = FALSE;
    texture->mImage[0] = (void*)((u32)texture->mImage[0] - (u32)texture);
    if (texture->mFormat <= GS_TEXTURE_FORMAT_CI8) {
        texture->mTlut = (void*)((u32)texture->mTlut - (u32)texture);
    }
    memcpy(pMenuFaceTexList[list->mTexIndex].mBuffer, texture, MENU_FACE_TEX_SIZE);
    GSresFreeResource(MENU_FACE_GROUP, texID);

    texture = GStextureLoad(pMenuFaceTexList[list->mTexIndex].mBuffer);
    GSresRegisterResource(texture, MENU_FACE_GROUP, texID, _unloadTexture);
    DCFlushRange(texture, MENU_FACE_TEX_SIZE);
    GXInvalidateTexAll();
    list->mState = MENU_FACE_STATE_ENABLE;

    if (_menuFaceLoadNormal) {
        menuFaceLoadFromID(0, list->mCallback, list->mContext);
    } else {
        menuFaceLoadFromID_Tool(0, list->mCallback, list->mContext);
    }
}

u8 menuFaceCheckLoading(void)
{
    s32 num;
    s32 i;

    num = _menuFaceGetListNum();
    for (i = 0; i < num; i++) {
        if (pMenuFaceList[i].mState == MENU_FACE_STATE_LOADING) {
            return 1;
        }
    }
    return 0;
}

u8 menuFaceLoadFromID(u16 faceID, MenuFaceLoadCallback callback, void* context)
{
    u32 texID;
    s32 num;
    s32 i;
    s32 index;

    while (TRUE) {
        if (faceID == 0) {
            Pokemon* pokemon = NULL;
            if (callback != NULL) {
                pokemon = callback(context);
            }
            if (pokemon != NULL) {
                faceID = menuFaceGetID(pokemon);
            }
        }
        if (faceID == 0) {
            if (menuFaceCheckLoading()) {
                return 0;
            }
            num = _menuFaceGetListNum();
            for (i = 0; i < num; i++) {
                if (pMenuFaceList[i].mState == MENU_FACE_STATE_WAIT) {
                    faceID = pMenuFaceList[i].mFaceID;
                    break;
                }
            }
            if (i == num) {
                GSfsysFree(MENU_FACE_GROUP);
                return 0;
            }
        } else if (_menuFaceSearchList(faceID) >= 0) {
            _menuFaceAddList(faceID, callback, context);
            faceID = 0;
            continue;
        } else {
            _menuFaceAddList(faceID, callback, context);
        }

        texID = menuFaceGetTexID(faceID);
        if (texID == 0) {
            faceID = 0;
            continue;
        }
        if (menuFaceCheckLoading()) {
            return 1;
        }
        index = _menuFaceSearchList(faceID);
        pMenuFaceList[index].mState = MENU_FACE_STATE_LOADING;
        _menuFaceLoadNormal = TRUE;
        GSfsysReadIDEX(MENU_FACE_GROUP, texID, _menuFaceLoadCallBack, NULL, faceID);
        return 1;
    }
}

u8 menuFaceLoadFromID_Tool(u16 faceID, MenuFaceLoadCallback callback, void* context)
{
    u32 texID;
    s32 num;
    s32 i;
    s32 index;

    while (TRUE) {
        if (faceID == 0) {
            Pokemon* pokemon = NULL;
            if (callback != NULL) {
                pokemon = callback(context);
            }
            if (pokemon != NULL) {
                faceID = uToolPokemon_faceId;
            }
        }
        if (faceID == 0) {
            if (menuFaceCheckLoading()) {
                return 0;
            }
            num = _menuFaceGetListNum();
            for (i = 0; i < num; i++) {
                if (pMenuFaceList[i].mState == MENU_FACE_STATE_WAIT) {
                    faceID = pMenuFaceList[i].mFaceID;
                    break;
                }
            }
            if (i == num) {
                GSfsysFree(MENU_FACE_GROUP);
                return 0;
            }
        } else if (_menuFaceSearchList(faceID) >= 0) {
            _menuFaceAddList(faceID, callback, context);
            faceID = 0;
            continue;
        } else {
            _menuFaceAddList(faceID, callback, context);
        }

        texID = menuFaceGetTexID(faceID);
        if (texID == 0) {
            faceID = 0;
            continue;
        }
        if (menuFaceCheckLoading()) {
            return 1;
        }
        index = _menuFaceSearchList(faceID);
        pMenuFaceList[index].mState = MENU_FACE_STATE_LOADING;
        _menuFaceLoadNormal = FALSE;
        GSfsysReadIDEX(MENU_FACE_GROUP, texID, _menuFaceLoadCallBack, NULL, faceID);
        return 1;
    }
}

u8 menuFaceLoad(Pokemon* pokemon, MenuFaceLoadCallback callback, void* context)
{
    return menuFaceLoadFromID(menuFaceGetID(pokemon), callback, context);
}

u8 menuFaceLoad_Tool(Pokemon* pokemon, MenuFaceLoadCallback callback, void* context)
{
    return menuFaceLoadFromID_Tool(menuFaceGetID2(pokemon, MENU_FACE_DEOXYS_NORMAL), callback, context);
}

u8 menuFaceLoadFightPokemon(FightPokemon* fightPokemon, MenuFaceLoadCallback callback, void* context)
{
    Pokemon* pokemon;
    u8 deoxysSel;
    u16 faceID;

    pokemon = (Pokemon*)pokemonGetStatus(fightPokemon, 0, POKEMON_QUERY_PARTY_RECORD, 0);
    deoxysSel = pokemonGetFightPokemonDeoxysSel(fightPokemon);
    faceID = menuFaceGetID2(pokemon, deoxysSel);
    menuFaceLoadFromID(faceID, callback, context);
    while (TRUE) {
        if (menuFaceSearchEnable(faceID)) {
            break;
        }
        GSthreadSwitch();
    }

    if (faceID == MENU_FACE_ID_DEOXYS_ATTACK || faceID == MENU_FACE_ID_DEOXYS_DEFENSE ||
        faceID == MENU_FACE_ID_DEOXYS_SPEED) {
        faceID = MENU_FACE_ID_DEOXYS;
        menuFaceLoadFromID(faceID, callback, context);
        while (TRUE) {
            if (menuFaceSearchEnable(faceID)) {
                break;
            }
            GSthreadSwitch();
        }
    }
    if (faceID == MENU_FACE_ID_DEOXYS_ATTACK_RARE || faceID == MENU_FACE_ID_DEOXYS_DEFENSE_RARE ||
        faceID == MENU_FACE_ID_DEOXYS_SPEED_RARE) {
        faceID = MENU_FACE_ID_DEOXYS_RARE;
        menuFaceLoadFromID(faceID, callback, context);
        while (TRUE) {
            if (menuFaceSearchEnable(faceID)) {
                break;
            }
            GSthreadSwitch();
        }
    }
    return 1;
}
