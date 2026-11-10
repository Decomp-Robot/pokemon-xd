#include <types.h>
#include <menu.h>
#include <menuSub.h>
#include <winMsg.h>
#include <sound.h>
#include <floor.h>
#include <floorManager.h>
#include <GSmsg.h>
#include <GSlog.h>
#include <GSgfx.h>
#include <GSthread.h>
#include <GSres.h>
#include <GSmemman.h>
#include <GSgfxVideo.h>
#include <GSmodel.h>
#include <GSdvd.h>
#include <GSmovie.h>
#include <mailMain.h>

extern s32 g_hSysdolphineHeap;
extern u8 lbl_804E7F98;

#pragma force_active on

// Debug menu entries for the script, sound and movie tests, the TV mode and
// frame rate switches, the heap diagnostics and the on-screen debug overlays.

static s32 _dbgMenuDispAMemInfoFlag;

struct MovieResolution {
    u16 width;
    u16 height;
};

enum {
    PERF_BAR_RENDER_OFF = 0,
    PERF_BAR_RENDER_FULL = 1,
    PERF_BAR_RENDER_MINIMAL = 2
};

enum {
    FRAME_RATE_LOCK_NONE = 0,
    FRAME_RATE_LOCK_30 = 1,
    FRAME_RATE_LOCK_20 = 2,
    FRAME_RATE_LOCK_15 = 3
};

enum {
    MOVIE_TEST_FLOOR_ID = 406,
    MOVIE_PLAY_LOOP = 1,
    RESOURCE_ID_COLLISION_DEBUG_FLAGS = 2
};

extern "C" s32 debugMenuChangePerfBar(void)
{
    s32 mode = GSgfxPerfbarGetRenderMode();

    switch (mode) {
    case PERF_BAR_RENDER_OFF:
        mode = PERF_BAR_RENDER_MINIMAL;
        break;

    case PERF_BAR_RENDER_MINIMAL:
        mode = PERF_BAR_RENDER_FULL;
        break;

    case PERF_BAR_RENDER_FULL:
        mode = PERF_BAR_RENDER_OFF;
        break;
    }
    GSgfxPerfbarSetRenderMode(mode);
    return 0;
}

extern "C" s32 debugMenuChangeMemInfo(void)
{
    if (menuIsCheck(MENU_ID_DEBUG_MEMORY_INFO) != 0) {
        menuClose(MENU_ID_DEBUG_MEMORY_INFO);
    } else {
        menuOpenCustom(MENU_ID_DEBUG_MEMORY_INFO, 0, NULL, 0, 0, 0);
    }
    return 0;
}

extern "C" s32 debugMenuNodrawArea(void)
{
    if (menuIsCheck(MENU_ID_DEBUG_NODRAW_AREA) != 0) {
        menuClose(MENU_ID_DEBUG_NODRAW_AREA);
    } else {
        menuOpenCustom(MENU_ID_DEBUG_NODRAW_AREA, 0, NULL, 0, 0, 0);
    }
    return 0;
}

extern "C" s32 debugMenuShadowBorderDisp(void)
{
    static bool shadowBorderEnabled = false;

    shadowBorderEnabled = !shadowBorderEnabled;
    GSmodelSetShadowDebug(shadowBorderEnabled);
    return 0;
}

extern "C" s32 debugMenuColorBarDisp(void)
{
    lbl_804E7F98 = 0;
    menuOpenCustom(MENU_ID_DEBUG_COLOR_BAR, 0, NULL, 0, 1, 0);
    menuClose(MENU_ID_DEBUG_COLOR_BAR);
    lbl_804E7F98 = 1;
    return 0;
}

// Ids the debug menu dispatches to the collision display command: one per
// collision overlay the command toggles.
enum {
    MENU_ITEM_DEBUG_DRAW_COLLISION   = 305, // collision shapes
    MENU_ITEM_DEBUG_DRAW_LINE_CHECKS = 306  // collision line checks
};

extern "C" s32 dbgMenuColisionDisp(s32 index)
{
    struct CollisionDebugFlags {
        u8 drawCollision;
        u8 drawLineChecks;
    };
    CollisionDebugFlags *debugFlags;

    debugFlags = (CollisionDebugFlags *)GSresGetResource(0, RESOURCE_ID_COLLISION_DEBUG_FLAGS);
    if (debugFlags == NULL) {
        return 0;
    }
    switch (index) {
    case MENU_ITEM_DEBUG_DRAW_COLLISION:
        if (debugFlags->drawCollision != 0) {
            debugFlags->drawCollision = 0;
        } else {
            debugFlags->drawCollision = 1;
        }
        break;

    case MENU_ITEM_DEBUG_DRAW_LINE_CHECKS:
        if (debugFlags->drawLineChecks != 0) {
            debugFlags->drawLineChecks = 0;
        } else {
            debugFlags->drawLineChecks = 1;
        }
        break;
    }
    return 0;
}

extern "C" s32 dbgMenuSendMail(void)
{
    s32 index;

    while ((index = menuOpen(MENU_ID_NUMBER_INPUT, 1)) != -1) {
        if (mailChkReceiveMail(index) == 0 && mailAddMailbox(index) != 0) {
            mailMainReceiveStart();
        }
    }
    return 0;
}

extern "C" s32 dbgMenuSendAllMail(void)
{
    mailMainSendAllMail();
    return 0;
}

extern "C" s32 dbgMenuFrameRate60(void)
{
    GSgfxVideoSetLockFrameRate(FRAME_RATE_LOCK_NONE);
    return 0;
}

extern "C" s32 dbgMenuFrameRate30(void)
{
    GSgfxVideoSetLockFrameRate(FRAME_RATE_LOCK_30);
    return 0;
}

extern "C" s32 dbgMenuFrameRate20(void)
{
    GSgfxVideoSetLockFrameRate(FRAME_RATE_LOCK_20);
    return 0;
}

extern "C" s32 dbgMenuFrameRate15(void)
{
    GSgfxVideoSetLockFrameRate(FRAME_RATE_LOCK_15);
    return 0;
}

extern "C" s32 dbgMenuFrameLockToggle(void)
{
    GSgfxVideoSetLockFrameActive(!GSgfxVideoGetLockFrameActive());
    return 0;
}

extern "C" s32 dbgMenuScreenFilterOff(void)
{
    u8 filter[7] = { 0, 0, 21, 22, 21, 0, 0 };
    GSgfxVideoSetFilter(filter);
    return 0;
}

extern "C" s32 dbgMenuScreenFilterDefault(void)
{
    u8 filter[7] = { 8, 8, 10, 12, 10, 8, 8 };
    GSgfxVideoSetFilter(filter);
    return 0;
}

extern "C" s32 dbgMenuScreenFilterLow(void)
{
    u8 filter[7] = { 4, 6, 14, 16, 14, 6, 4 };
    GSgfxVideoSetFilter(filter);
    return 0;
}

extern "C" s32 dbgMenuScreenFilterMed(void)
{
    u8 filter[7] = { 9, 9, 9, 10, 9, 9, 9 };
    GSgfxVideoSetFilter(filter);
    return 0;
}

extern "C" s32 dbgMenuScreenFilterHi(void)
{
    u8 filter[7] = { 13, 9, 7, 6, 7, 9, 13 };
    GSgfxVideoSetFilter(filter);
    return 0;
}

extern "C" s32 dbgMenuGSmemCheck(void)
{
    u8 ok;

    GSlogWrite("check fsys heap\n");
    ok = GSmemCheckHeap(0);
    GSlogWrite("%s\n", ok ? "ok" : "error");
    GSlogWrite("check floor heap\n");
    ok = GSmemCheckHeap(floorGetLocalHeapHandle());
    GSlogWrite("%s\n", ok ? "ok" : "error");
    GSlogWrite("check system heap\n");
    ok = GSmemCheckHeap(g_hSysdolphineHeap);
    GSlogWrite("%s\n", ok ? "ok" : "error");
    return 0;
}

extern "C" s32 dbgMenuGSmemDispMap(void)
{
    GSmemDumpHeapInfo();
    GSmemDumpHeap(g_hSysdolphineHeap);
    GSmemDumpHeap(0);
    GSmemDumpPool(0);
    return 0;
}

extern "C" s32 dbgMenuGSmemOptimize(void)
{
    return 0;
}

extern "C" s32 dbgMenuSetPAL50(void)
{
    GSgfxVideoSetMode(GSGFX_VIDEO_MODE_PAL, GSGFX_VIDEO_FRAME_480, false,
                      GSGFX_VIDEO_FILTER_DEFLICKER, false);
    return 0;
}

extern "C" s32 dbgMenuSetPAL60(void)
{
    GSgfxVideoSetMode(GSGFX_VIDEO_MODE_EURGB60, GSGFX_VIDEO_FRAME_480, false,
                      GSGFX_VIDEO_FILTER_DEFLICKER, false);
    return 0;
}

u16 *_dbgMenuMovieTestGetStr(u32 index)
{
    static const char *_test_movie_str[16] = {
        "tv_news_common", "tv_news_ren", "tv_music_chiril", "tv_deathgold_appear",
        "tv_agent_report", "tv_cyobin_comments", "tv_battle_demo", "tv_colosseum_demo",
        "tv_colosseum_demo_lugia", "tpc_logo", "gs_logo", "auto_demo", "ending",
        "staffroll", "relive_complete", "event_battle_192x144",
    };
    static u16 strbuf[32];

    if (index >= 16) {
        index = 15;
    }
    GScharMakeFromSJIS(strbuf, (char *)_test_movie_str[index]);
    return strbuf;
}

extern "C" s32 dbgMenuMovieTest(void)
{
    static s32 _test_movie[16] = {
        2285, 2286, 2287, 2288, 2289, 2290, 2291, 2548,
        2583, 2009, 2010, 2012, 2283, 2013, 2284, 2068,
};
    static MovieResolution _test_movie_wh[16] = {
        {192, 144}, {192, 144}, {192, 144}, {192, 144},
        {192, 144}, {192, 144}, {192, 144}, {192, 144},
        {192, 144}, {640, 480}, {640, 480}, {640, 480},
        {640, 480}, {640, 480}, {640, 480}, {192, 144},
};
    static u32 _test_movie_index = 0;
    struct GStexture *texture;
    u32 index;
    u32 i;

    for (i = 0; i < 16; i++) {
        s32 *movieID = &_test_movie[i];
        while (GSmoviePlayCheck(*movieID) == 1) {
            GSmovieStop(*movieID);
            GSthreadSwitch();
        }
    }
    if (menuSubOpenNumberInputID(_test_movie_index, &index, _dbgMenuMovieTestGetStr) == 0) {
        return 0;
    }
    if (index >= 16) {
        index = 15;
    }
    menuSubCloseNumberInput();
    if (GSfloorGetCurrent() == MOVIE_TEST_FLOOR_ID && _test_movie_wh[index].width == 192 &&
        _test_movie_wh[index].height == 144) {
        texture = floorGetActiveOffscreenTexture();
    } else {
        texture = NULL;
    }
    GSdvdFstRefresh();
    GSmoviePlay(_test_movie[index], 0, MOVIE_PLAY_LOOP, texture);
    _test_movie_index = index;
    return 0;
}

extern "C" s32 debugMenuChangeAMemInfo(void)
{
    static s32 _disp = 0;
    switch (_disp) {
    case 0:
    case 1:
        _dbgMenuDispAMemInfoFlag = _disp;
        menuOpenCustom(MENU_ID_DEBUG_ARAM_INFO, 0, NULL, 0, 0, 0);
        _disp++;
        break;

    default:
        if (menuIsCheck(MENU_ID_DEBUG_ARAM_INFO) != 0) {
            menuClose(MENU_ID_DEBUG_ARAM_INFO);
        }
        _disp = 0;
        break;
    }
    return 0;
}

extern "C" s32 dbgMenuScriptDispLine(void)
{
    if (floorIsDispScriptInfo() == 1) {
        floorSetDispScriptInfo(0);
    } else {
        floorSetDispScriptInfo(1);
    }
    return 0;
}

extern "C" s32 dbgMenuScriptMsgTest(void)
{
    static s32 msg_id = 0;
    while (menuSubOpenNumberInputDigit(msg_id, &msg_id) == 1) {
        winMsgOpenField(msg_id, 1, 0);
        winMsgClose(1);
    }
    return 0;
}

extern "C" s32 dbgMenuScriptYesNoMsgTest(void)
{
    static s32 msg_id = 0;
    while (menuSubOpenNumberInputDigit(msg_id, &msg_id) == 1) {
        winMsgOpenField(msg_id, 1, 0);
        menuSubOpenYesNoField();
        winMsgClose(1);
    }
    return 0;
}

extern "C" s32 dbgMenuScriptSoundTest(void)
{
    static s32 snd_id = 0;
    while (menuSubOpenNumberInputDigit(snd_id, &snd_id) == 1) {
        if (snd_id == soundGetBGMID() || snd_id == soundGetEnvID()) {
            soundStop(snd_id, 1000);
        } else {
            soundOpenPlay(snd_id, 0, 127);
        }
    }
    return 0;
}

extern "C" s32 dbgMenuScriptHumanColision(void)
{
    if (floorGetHumanColision() == 1) {
        floorSetHumanColision(0);
    } else {
        floorSetHumanColision(1);
    }
    return 0;
}
