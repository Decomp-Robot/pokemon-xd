#include <flashsystem.h>
#include <flashmenu.h>
#include <GSthread.h>

struct FlashMovie {
    FlashMenu* menu;
    u32 resourceID;
};

enum { FLASH_MOVIE_COUNT = 10 };
static FlashMovie _flashmenu[FLASH_MOVIE_COUNT];

static inline s32 findMovie(u32 resourceID)
{
    for (s32 i = 0; i < FLASH_MOVIE_COUNT; ++i) {
        if (_flashmenu[i].menu && _flashmenu[i].resourceID == resourceID)
            return i;
    }
    return -1;
}

static inline FlashMovie* getMovie(u32 resourceID)
{
    s32 index = findMovie(resourceID);
    if (index >= 0) {
        FlashMovie* movie = &_flashmenu[index];
        if (movie->menu)
            return movie;
        return 0;
    }
    return 0;
}

static inline FlashMovie* getFreeMovie()
{
    for (s32 i = 0; i < FLASH_MOVIE_COUNT; ++i) {
        if (!_flashmenu[i].menu)
            return &_flashmenu[i];
    }
    return 0;
}

s32 FlashSystemInitAll()
{
    for (s32 i = 0; i < FLASH_MOVIE_COUNT; ++i) {
        _flashmenu[i].menu = 0;
        _flashmenu[i].resourceID = 0;
    }
    return 1;
}

s32 FlashSystemInit(u32 flash, void* resource)
{
    FlashMovie* movie = getFreeMovie();
    if (movie) {
        movie->menu = new FlashMenu;
        s32 result = movie->menu->Init(resource, flash);
        movie->resourceID = flash;
        return result;
    }
    return 0;
}

void FlashSystemPlay(u32 flash)
{
    FlashMovie* movie = getMovie(flash);
    if (movie)
        movie->menu->Update();
}

void FlashSystemDraw(u32 flash, tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    FlashMovie* movie = getMovie(flash);
    if (movie)
        movie->menu->Draw(window, sprite);
}

void FlashSystemRelease(u32 flash)
{
    FlashMovie* movie = getMovie(flash);
    if (movie) {
        movie->menu->Release();
        delete movie->menu;
        movie->menu = 0;
        movie->resourceID = 0;
    }
}

GStexture* FlashSystemGetGStexture(u32 flash, s32 textureID)
{
    FlashMovie* movie = getMovie(flash);
    if (movie)
        return movie->menu->GetGStexture(textureID);
    return 0;
}

u32 FlashSystemGetNowFrame(u32 flash)
{
    FlashMovie* movie = getMovie(flash);
    if (movie)
        return movie->menu->GetPlayFrame();
    return 0;
}

u32 FlashSystemGetEndFrame(u32 flash)
{
    FlashMovie* movie = getMovie(flash);
    if (movie)
        return movie->menu->endFrame;
    return 0;
}

s32 FlashSystemSetNowFrame(u32 flash, u32 frame)
{
    FlashMovie* movie = getMovie(flash);
    if (movie)
        movie->menu->SetPlayFrame(frame);
    return 0;
}

void FlashSystemSetLoopFlag(u32 flash, u8 loop)
{
    FlashMovie* movie = getMovie(flash);
    if (movie)
        movie->menu->loopFlag = loop;
}

s32 FlashSystemSetPlayFrameLabel(u32 flash, const char* start, const char* end)
{
    FlashMovie* movie = getMovie(flash);
    if (movie) {
        movie->menu->startFrame = movie->menu->GetFrameFromLabel(const_cast<char*>(start));
        movie->menu->endFrame = movie->menu->GetFrameFromLabel(const_cast<char*>(end));
    }
    return 1;
}

s32 FlashSystemSetNowFrameLabel(u32 flash, const char* label)
{
    FlashMovie* movie = getMovie(flash);
    if (movie)
        movie->menu->SetPlayFrame(movie->menu->GetFrameFromLabel(const_cast<char*>(label)));
    return 0;
}

u32 FlashSystemGetFrameFromLabel(u32 flash, const char* label)
{
    FlashMovie* movie = getMovie(flash);
    if (movie)
        return movie->menu->GetFrameFromLabel(const_cast<char*>(label));
    return 0;
}

static inline u8 getLoopFlag(u32 flash)
{
    FlashMovie* movie = getMovie(flash);
    if (movie)
        return movie->menu->loopFlag;
    return 0;
}

u8 FlashSystemIsLastFrame(u32 flash)
{
    u8 lastFrame = 0;
    if (!getLoopFlag(flash)) {
        if (FlashSystemGetNowFrame(flash) >= FlashSystemGetEndFrame(flash))
            lastFrame = 1;
    }
    return lastFrame;
}

void FlashSystemPlayAndWait(u32 flash, const char* start, const char* end)
{
    FlashSystemSetPlayFrameLabel(flash, start, end);
    FlashSystemSetLoopFlag(flash, 0);
    FlashSystemSetNowFrameLabel(flash, start);
    u32 endFrame = FlashSystemGetFrameFromLabel(flash, end);
    u32 nowFrame;
    do {
        nowFrame = FlashSystemGetNowFrame(flash);
        GSthreadSwitch();
    } while (nowFrame < endFrame);
}
