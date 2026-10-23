#include <string.h>
#include "floor.h"
#include "GSmodel.h"
#include "GSres.h"


extern "C" {
GStexture *GStextureCreate(u16 width, u16 height, u32 format, u32 tlut, u32 additionalMipLevels);
void GSmoviePlay(u32 res, u32 preloadMovie, u32 loop, GStexture *texture);
void GSmoviePlayWait(u32 res);
void GSmovieStop(u32 res);

u32 getCurrentMapGroupId();

_GSmodel *floorOpenObjectRes(u32 group, u32 res);
}

// Offscreen state of the open floor. The room data owns the entry list; the
// movie entry below is the one entry this module owns itself.
FloorOffscreenEntry _offscreenMovie;
FloorOffscreenEntry *floorOffscreenTable;
u32 floorOffscreenCount;
FloorOffscreenEntry *floorOffscreenEntry;
GStexture *floorOffscreenTexture;
_GSmodel *floorOffscreenModel;
void *floorOffscreenExtraRes;
s32 floorOffscreenIndex = -1;

// Play a movie on the offscreen screen. The movie entry is the module's own
// entry, so it is selected directly instead of through the room's list.
u8 floorMoviePlay(u32 movieRes, u32 preloadMovie, u32 loop, u32 modelRes,
                  u16 width, u16 height)
{
    _floorFreeOffscreenData();
    floorOffscreenEntry = &_offscreenMovie;
    floorOffscreenEntry->mMode = FloorOffscreenMovie;
    floorOffscreenEntry->mIndex = FLOOR_OFFSCREEN_MOVIE_INDEX;
    floorOffscreenEntry->mTextureReceiverModelRes = modelRes;
    floorOffscreenEntry->mModelRes = movieRes;
    floorOffscreenEntry->mPlaybackParameter = loop;
    floorOffscreenEntry->mWidth = width;
    floorOffscreenEntry->mHeight = height;
    floorOffscreenEntry->mExtraRes = 0;

    floorOffscreenTexture = GStextureCreate(floorOffscreenEntry->mWidth,
                                            floorOffscreenEntry->mHeight,
                                            FLOOR_OFFSCREEN_TEX_FORMAT, 0, 0);
    if (floorOffscreenTexture == 0) {
        floorOffscreenEntry = 0;
        return 0;
    }

    GSmoviePlay(floorOffscreenEntry->mModelRes, preloadMovie, loop,
                floorOffscreenTexture);
    GSmodelSetTextureOverride((_GSmodel *)GSresGetResource(getCurrentMapGroupId(),
                                       floorOffscreenEntry->mTextureReceiverModelRes),
                      floorOffscreenTexture);
    floorOffscreenIndex = FLOOR_OFFSCREEN_MOVIE_INDEX;
    return 1;
}

// Wait for the offscreen movie of the current floor to finish. The floor has a model
// instead of a movie when its entry plays an animation, so there is nothing to wait for.
void floorOffscreenPlayWait()
{
    if (floorOffscreenEntry == 0 || floorOffscreenModel != 0) {
        return;
    }
    GSmoviePlayWait(floorOffscreenEntry->mModelRes);
}

// Screen texture the selected offscreen entry draws on.
GStexture *floorGetOffscreenTexture()
{
    return floorOffscreenTexture;
}

// Select the offscreen entry with the given index and start it: it creates the
// screen texture and either starts the entry's model animation or plays its
// movie. Returns 0 when the index is unknown or the entry cannot be set up.
u8 floorSetOffscreenIndex(s32 index)
{
    u32 i;
    u8 found;

    if (floorOffscreenIndex == index) {
        return 1;
    }
    _floorFreeOffscreenData();
    floorOffscreenEntry = floorOffscreenTable;
    found = 0;
    for (i = 0; i != floorOffscreenCount; i++) {
        if (floorOffscreenEntry->mIndex == index) {
            found = 1;
            break;
        }
        floorOffscreenEntry++;
    }
    if (found == 0) {
        floorOffscreenEntry = 0;
        return 0;
    }

    floorOffscreenTexture = GStextureCreate(floorOffscreenEntry->mWidth,
                                            floorOffscreenEntry->mHeight,
                                            FLOOR_OFFSCREEN_TEX_FORMAT, 0, 0);
    if (floorOffscreenTexture == 0) {
        floorOffscreenEntry = 0;
        return 0;
    }

    if (floorOffscreenEntry->mMode == FloorOffscreenModelAnimation) {
        floorOffscreenModel = floorOpenObjectRes(getCurrentMapGroupId(),
                                                 floorOffscreenEntry->mModelRes);
        GSmodelSetVisibility(floorOffscreenModel, 0);
        GSmodelLinkTexAnimToAnim(floorOffscreenModel, 1);
        GSmodelSetAnimIndex(floorOffscreenModel, floorOffscreenEntry->mPlaybackParameter);
        GSmodelStartAnimation(floorOffscreenModel);
    } else {
        GSmoviePlay(floorOffscreenEntry->mModelRes, 0,
                    floorOffscreenEntry->mPlaybackParameter != 0, floorOffscreenTexture);
    }

    floorOffscreenExtraRes = GSresGetResource(getCurrentMapGroupId(),
                                              floorOffscreenEntry->mExtraRes);
    GSmodelSetTextureOverride((_GSmodel *)GSresGetResource(getCurrentMapGroupId(),
                                       floorOffscreenEntry->mTextureReceiverModelRes),
                      floorOffscreenTexture);
    floorOffscreenIndex = index;
    return 1;
}

// Drop the room's offscreen entry list and free what was playing on it.
void floorClearOffscreen()
{
    _floorFreeOffscreenData();
    floorOffscreenTable = 0;
    floorOffscreenCount = 0;
}

// Advance the model of the selected entry; the movie entries and the floors without an
// offscreen entry have nothing to advance.
void floorUpdateOffscreen()
{
    if (floorOffscreenEntry == 0 || floorOffscreenModel == 0) {
        return;
    }
    GSmodelForceAnimTransformUpdate(floorOffscreenModel);
}

// Take the offscreen entry list of the room data. The movie entry is cleared
// so a later floorMoviePlay starts from an empty entry.
void floorSetOffscreenTable(GSfloor_dd_ *floor)
{
    if (floor->mOffscreenRes == 0) {
        return;
    }
    floorOffscreenTable = floor->mOffscreenRes->mList->mEntries;
    if (floorOffscreenTable == 0) {
        return;
    }
    floorOffscreenCount = *floor->mOffscreenRes->mList->mCount;
    if (floorOffscreenCount == 0) {
        return;
    }
    memset(&_offscreenMovie, 0, sizeof(_offscreenMovie));
}

// Release the offscreen entry: stop its movie, restore its receiving model's
// textures, free its texture and model and clear the selection.
void _floorFreeOffscreenData()
{
    if (floorOffscreenEntry == 0) {
        return;
    }
    if (floorOffscreenEntry->mMode == FloorOffscreenMovie) {
        GSmovieStop(floorOffscreenEntry->mModelRes);
    }
    GSmodelResetTextureOverride((_GSmodel *)GSresGetResource(getCurrentMapGroupId(),
                                            floorOffscreenEntry->mTextureReceiverModelRes));
    if (floorOffscreenTexture != 0) {
        GStextureFree(floorOffscreenTexture);
        floorOffscreenTexture = 0;
    }
    if (floorOffscreenModel != 0) {
        GSmodelFree(floorOffscreenModel);
        floorOffscreenModel = 0;
    }
    floorOffscreenExtraRes = 0;
    floorOffscreenEntry = 0;
    floorOffscreenIndex = -1;
}
