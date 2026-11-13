#include <types.h>
#include <GSlog.h>

// One record of the loaded portrait table.
struct PokeFaceData {
    u8 rareFlag : 1; // rare portrait variation.
    u8 : 7;
    u8 _01[3];       // unknown.
    u32 texID;       // portrait texture id.
};

extern "C" {

// The loaded portrait table: the number of records, and the records
// themselves.
extern u32* poke_face_number;
extern PokeFaceData* poke_face;

}

static PokeFaceData* _menuFaceBiosGetPtr(u16 faceID);

// Whether the portrait is the rare variation of its face.
u8 menuFaceBiosGetRareFlag(u16 faceID) {
    PokeFaceData* face = _menuFaceBiosGetPtr(faceID);

    if (face == 0) {
        return 0;
    }
    return face->rareFlag != 0;
}

// The texture the portrait is drawn from.
u32 menuFaceBiosGetTexID(u16 faceID) {
    PokeFaceData* face = _menuFaceBiosGetPtr(faceID);

    if (face == 0) {
        return 0;
    }
    return face->texID;
}

// The record of faceID. A face id outside the loaded count is reported
// instead of reading past the table.
static PokeFaceData* _menuFaceBiosGetPtr(u16 faceID) {
    if (faceID >= *poke_face_number) {
        GSlogWrite("[%s] 範囲外アクセスです\n", __FUNCTION__);
        return 0;
    }
    return &poke_face[faceID];
}
