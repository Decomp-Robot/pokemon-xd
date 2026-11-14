#include <floor.h>
#include <relglobal.h>
#include <GSmodel.h>
#include <GStexture.h>
#include <GSmovie.h>
#include <GSres.h>
#include <string.h>

enum {
    OFFSCREEN_MODEL = 0,
    OFFSCREEN_MOVIE = 1,
    OFFSCREEN_USER_INDEX = 1000
};

struct OffscreenData {
    u8 isMovie;
    u8 animationIndex;
    u16 index;
    u16 width;
    u16 height;
    u32 targetModelResourceID;
    u32 resourceID;
    u32 cameraResourceID;
};

struct FloorOffscreenResources {
    u32 *count;
    OffscreenData *data;
};

static OffscreenData _userOffData;
static s32 _lastOffIdx = -1;
static OffscreenData *_offRootDataPtr;
static u32 _offRootDataCnt;
static OffscreenData *_offData;
static GStexture *_offTex;
static GSmodel *_offMdl;
static void *_offCam;

static void _floorFreeOffscreenData(void);

s32 floorMoviePlay(u32 movieID, s32 flags, s32 loop, u32 targetModelResourceID, u16 width, u16 height)
{
    _floorFreeOffscreenData();
    _offData = &_userOffData;
    _offData->isMovie = OFFSCREEN_MOVIE;
    _offData->index = OFFSCREEN_USER_INDEX;
    _offData->targetModelResourceID = targetModelResourceID;
    _offData->resourceID = movieID;
    _offData->animationIndex = loop;
    _offData->width = width;
    _offData->height = height;
    _offData->cameraResourceID = 0;
    _offTex = GStextureCreate(_offData->width, _offData->height, GS_TEXTURE_FORMAT_RGB565, 0, 0);
    if (_offTex == 0) {
        _offData = 0;
        return 0;
    }
    GSmoviePlay(_offData->resourceID, flags, loop, _offTex);
    u32 groupID = floorGetCurrentGroupID();
    GSmodelSetTextureChange((GSmodel *)GSresGetResource(groupID, _offData->targetModelResourceID), _offTex);
    _lastOffIdx = OFFSCREEN_USER_INDEX;
    return 1;
}

void floorOffscreenPlayWait(void)
{
    if (_offData == 0 || _offMdl != 0) {
        return;
    }
    GSmoviePlayWait(_offData->resourceID);
}

GStexture *floorGetActiveOffscreenTexture(void)
{
    return _offTex;
}

s32 floorSetOffscreenIndex(s32 index)
{
    if (_lastOffIdx == index) {
        return 1;
    }
    _floorFreeOffscreenData();
    _offData = _offRootDataPtr;
    u8 found = 0;
    for (u32 count = _offRootDataCnt; count != 0; --count) {
        if (_offData->index == index) {
            found = 1;
            break;
        }
        _offData++;
    }
    if (found == 0) {
        _offData = 0;
        return 0;
    }
    _offTex = GStextureCreate(_offData->width, _offData->height, GS_TEXTURE_FORMAT_RGB565, 0, 0);
    if (_offTex == 0) {
        _offData = 0;
        return 0;
    }
    if (_offData->isMovie == OFFSCREEN_MODEL) {
        _offMdl = floorOpenModel(floorGetCurrentGroupID(), _offData->resourceID);
        GSmodelSetVisibility(_offMdl, false);
        GSmodelLinkTexAnimToAnim(_offMdl, 1);
        GSmodelSetAnimIndex(_offMdl, _offData->animationIndex);
        GSmodelStartAnimation(_offMdl);
    } else {
        u8 loop = _offData->animationIndex != 0;
        GSmoviePlay(_offData->resourceID, 0, loop, _offTex);
    }
    _offCam = GSresGetResource(floorGetCurrentGroupID(), _offData->cameraResourceID);
    GSmodelSetTextureChange((GSmodel *)GSresGetResource(floorGetCurrentGroupID(), _offData->targetModelResourceID), _offTex);
    _lastOffIdx = index;
    return 1;
}

void floorReleaseOffscreen(void)
{
    _floorFreeOffscreenData();
    _offRootDataPtr = 0;
    _offRootDataCnt = 0;
}

void floorUpdateOffscreen(void)
{
    if (_offData == 0 || _offMdl == 0) {
        return;
    }
    GSmodelForceAnimTransformUpdate(_offMdl);
}

void floorInitOffscreen(GSfloor_dd_ *floor)
{
    if (floor->offscreen != 0) {
        _offRootDataPtr = floor->offscreen[0]->data;
        if (_offRootDataPtr != 0) {
            _offRootDataCnt = *floor->offscreen[0]->count;
            if (_offRootDataCnt != 0) {
                memset(&_userOffData, 0, sizeof(_userOffData));
            }
        }
    }
}

static void _floorFreeOffscreenData(void)
{
    if (_offData != 0) {
        if (_offData->isMovie == OFFSCREEN_MOVIE) {
            GSmovieStop(_offData->resourceID);
        }
        u32 groupID = floorGetCurrentGroupID();
        GSmodelResetTextureChange((GSmodel *)GSresGetResource(groupID, _offData->targetModelResourceID));
        if (_offTex != 0) {
            GStextureFree(_offTex);
            _offTex = 0;
        }
        if (_offMdl != 0) {
            GSmodelFree(_offMdl);
            _offMdl = 0;
        }
        _offCam = 0;
        _offData = 0;
        _lastOffIdx = -1;
    }
}

void floorOffscreenMainInit(void)
{
}
