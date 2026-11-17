#include <flashmenu.h>
#include <GSgfx.h>
#include <GSgfxVideo.h>
#include <winSprite.h>
#include <time.h>

inline void FlashMenu::ResetDisplays()
{
    s32 i;
    FlashDisplayObject* display = rootDisplay;
    FlashMovieHeader* header = movie;
    for (i = 0; i < header->depthCapacity; i++) {
        display->morphRatio = 0.0f;
        display->unknownPlacement = NULL;
        display->placement = NULL;
        flcalcUnitCxform(&display->colorTransform);
        ++display;
    }
}

inline void FlashMenu::AdvanceFrame()
{
    u32 frame = playFrame;
    if (!pauseFlag) {
        u32 rate = GSgfxVideoGetVsyncRate();
        u32 elapsed = timeGetLastFrameTime();
        if (elapsed >= 10)
            elapsed = 1;
        if (elapsed == 0)
            playbackRate = rate;
        else
            playbackRate = rate / (f32)elapsed;
        frame += elapsed;
        if (frame > endFrame) {
            if (loopFlag) {
                frame = startFrame;
                ResetDisplays();
            } else {
                frame = endFrame;
            }
        }
        playFrame = frame;
    }
}

void FlashMenu::Draw(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    if (definitions) {
        GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);
        GSgfxLayerSetActive(layer);
        GSgfxLayerFindByID(layer);
        DrawSub(layer, window, sprite);
        AdvanceFrame();
        GSgfxLayerSetActive(GSGFX_LAYER_MENU);
    }
}

FlashMenu::FlashMenu()
{
    currentDepth = 0;
    fixPointers = 0;
    shapeCount = 0;
}

FlashMenu::~FlashMenu()
{
}

GStexture* FlashMenu::GetGStexture(s32 textureID)
{
    for (s32 i = 0; i < 30; i++) {
        if (textures[i].characterId == textureID)
            return textures[i].texture;
    }
    return NULL;
}
