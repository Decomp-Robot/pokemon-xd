#include <floorLayerConfig.h>
#include <dbgMenu.h>

enum { FLOOR_KIND_FIELD = 1, FLOOR_KIND_FIGHT = 2, FLOOR_KIND_FIGHT_ALT = 4, FLOOR_KIND_MENU_EXTRA_A = 5, FLOOR_KIND_MENU_EXTRA_B = 6, FLOOR_KIND_MENU = 7 };

static char *g_layer_names[8] = {
    "background",
    "background distortion",
    "main",
    "main distortion",
    "secondary",
    "secondary distortion",
    "filter",
    "menu",
};

static _GSgfxLayerData _empty_layer_config[] = {
    {GSGFX_LAYER_DEFAULT, g_layer_names[GSGFX_LAYER_DEFAULT], GS_GFX_LAYER_NORMAL, false, false, false, true, 327680, 327680, 16},
    {GSGFX_LAYER_INVALID, 0, GS_GFX_LAYER_NORMAL, false, false, false, false, 0, 0, 0},
};

static _GSgfxLayerData _map_layer_config[] = {
    {GSGFX_LAYER_BASE, g_layer_names[GSGFX_LAYER_BASE], GS_GFX_LAYER_NORMAL, false, false, false, true, 393216, 196608, 16},
    {GSGFX_LAYER_DEFAULT, g_layer_names[GSGFX_LAYER_DEFAULT], GS_GFX_LAYER_NORMAL, false, false, false, true, 327680, 327680, 16},
    {GSGFX_LAYER_SECONDARY, g_layer_names[GSGFX_LAYER_SECONDARY], GS_GFX_LAYER_NORMAL, false, false, true, true, 65536, 131072, 16},
    {GSGFX_LAYER_FILTER, g_layer_names[GSGFX_LAYER_FILTER], GS_GFX_LAYER_NORMAL, false, false, false, true, 0, 8192, 1},
    {GSGFX_LAYER_MENU, g_layer_names[GSGFX_LAYER_MENU], GS_GFX_LAYER_NORMAL, false, false, false, true, 0, 98304, 8},
    {GSGFX_LAYER_INVALID, 0, GS_GFX_LAYER_NORMAL, false, false, false, false, 0, 0, 0},
};

static _GSgfxLayerData _fight_layer_config[] = {
    {GSGFX_LAYER_BASE, g_layer_names[GSGFX_LAYER_BASE], GS_GFX_LAYER_NORMAL, false, false, false, true, 262144, 262144, 16},
    {GSGFX_LAYER_BACKGROUND_DISTORTION, g_layer_names[GSGFX_LAYER_BACKGROUND_DISTORTION], GS_GFX_LAYER_DISTORTION, false, false, false, false, 0, 131072, 16},
    {GSGFX_LAYER_DEFAULT, g_layer_names[GSGFX_LAYER_DEFAULT], GS_GFX_LAYER_NORMAL, false, false, false, true, 524288, 327680, 16},
    {GSGFX_LAYER_MAIN_DISTORTION, g_layer_names[GSGFX_LAYER_MAIN_DISTORTION], GS_GFX_LAYER_DISTORTION, false, false, false, false, 0, 65536, 16},
    {GSGFX_LAYER_SECONDARY, g_layer_names[GSGFX_LAYER_SECONDARY], GS_GFX_LAYER_NORMAL, false, false, true, true, 131072, 542720, 16},
    {GSGFX_LAYER_SECONDARY_DISTORTION, g_layer_names[GSGFX_LAYER_SECONDARY_DISTORTION], GS_GFX_LAYER_DISTORTION, false, false, false, false, 0, 81920, 16},
    {GSGFX_LAYER_FILTER, g_layer_names[GSGFX_LAYER_FILTER], GS_GFX_LAYER_NORMAL, false, false, false, true, 0, 16384, 1},
    {GSGFX_LAYER_MENU, g_layer_names[GSGFX_LAYER_MENU], GS_GFX_LAYER_NORMAL, false, false, false, true, 0, 131072, 16},
    {GSGFX_LAYER_INVALID, 0, GS_GFX_LAYER_NORMAL, false, false, false, false, 0, 0, 0},
};

static _GSgfxLayerData _menu_layer_config[] = {
    {GSGFX_LAYER_BASE, g_layer_names[GSGFX_LAYER_BASE], GS_GFX_LAYER_NORMAL, false, false, false, true, 262144, 262144, 32},
    {GSGFX_LAYER_AUTO, "menu extra", GS_GFX_LAYER_NORMAL, false, false, true, true, 131072, 262144, 16},
    {GSGFX_LAYER_DEFAULT, g_layer_names[GSGFX_LAYER_DEFAULT], GS_GFX_LAYER_NORMAL, false, false, false, true, 327680, 327680, 16},
    {GSGFX_LAYER_SECONDARY, g_layer_names[GSGFX_LAYER_SECONDARY], GS_GFX_LAYER_NORMAL, false, false, true, true, 65536, 262144, 16},
    {GSGFX_LAYER_FILTER, g_layer_names[GSGFX_LAYER_FILTER], GS_GFX_LAYER_NORMAL, false, false, false, true, 0, 8192, 1},
    {GSGFX_LAYER_MENU, g_layer_names[GSGFX_LAYER_MENU], GS_GFX_LAYER_NORMAL, false, false, false, true, 131072, 262144, 16},
    {GSGFX_LAYER_INVALID, 0, GS_GFX_LAYER_NORMAL, false, false, false, false, 0, 0, 0},
};

static _GSgfxLayerData _evolution_layer_config[] = {
    {GSGFX_LAYER_BASE, g_layer_names[GSGFX_LAYER_BASE], GS_GFX_LAYER_NORMAL, false, false, false, true, 262144, 262144, 32},
    {GSGFX_LAYER_DEFAULT, g_layer_names[GSGFX_LAYER_DEFAULT], GS_GFX_LAYER_NORMAL, false, false, false, true, 327680, 327680, 16},
    {GSGFX_LAYER_SECONDARY, g_layer_names[GSGFX_LAYER_SECONDARY], GS_GFX_LAYER_NORMAL, false, false, true, true, 262144, 262144, 16},
    {GSGFX_LAYER_FILTER, g_layer_names[GSGFX_LAYER_FILTER], GS_GFX_LAYER_NORMAL, false, false, false, true, 0, 8192, 1},
    {GSGFX_LAYER_MENU, g_layer_names[GSGFX_LAYER_MENU], GS_GFX_LAYER_NORMAL, false, false, false, true, 131072, 262144, 16},
    {GSGFX_LAYER_INVALID, 0, GS_GFX_LAYER_NORMAL, false, false, false, false, 0, 0, 0},
};

static _GSgfxLayerData _M6_layer_config[] = {
    {GSGFX_LAYER_BASE, g_layer_names[GSGFX_LAYER_BASE], GS_GFX_LAYER_NORMAL, false, false, false, true, 458752, 196608, 16},
    {GSGFX_LAYER_DEFAULT, g_layer_names[GSGFX_LAYER_DEFAULT], GS_GFX_LAYER_NORMAL, false, false, false, true, 327680, 327680, 16},
    {GSGFX_LAYER_SECONDARY, g_layer_names[GSGFX_LAYER_SECONDARY], GS_GFX_LAYER_NORMAL, false, false, true, true, 65536, 131072, 16},
    {GSGFX_LAYER_FILTER, g_layer_names[GSGFX_LAYER_FILTER], GS_GFX_LAYER_NORMAL, false, false, false, true, 0, 8192, 1},
    {GSGFX_LAYER_MENU, g_layer_names[GSGFX_LAYER_MENU], GS_GFX_LAYER_NORMAL, false, false, false, true, 0, 98304, 8},
    {GSGFX_LAYER_INVALID, 0, GS_GFX_LAYER_NORMAL, false, false, false, false, 0, 0, 0},
};

static _GSgfxLayerData _floor_542_layer_config[] = {
    {GSGFX_LAYER_BASE, g_layer_names[GSGFX_LAYER_BASE], GS_GFX_LAYER_NORMAL, false, false, false, true, 327680, 262144, 16},
    {GSGFX_LAYER_BACKGROUND_DISTORTION, g_layer_names[GSGFX_LAYER_BACKGROUND_DISTORTION], GS_GFX_LAYER_DISTORTION, false, false, false, false, 0, 131072, 16},
    {GSGFX_LAYER_DEFAULT, g_layer_names[GSGFX_LAYER_DEFAULT], GS_GFX_LAYER_NORMAL, false, false, false, true, 524288, 327680, 16},
    {GSGFX_LAYER_MAIN_DISTORTION, g_layer_names[GSGFX_LAYER_MAIN_DISTORTION], GS_GFX_LAYER_DISTORTION, false, false, false, false, 0, 65536, 16},
    {GSGFX_LAYER_SECONDARY, g_layer_names[GSGFX_LAYER_SECONDARY], GS_GFX_LAYER_NORMAL, false, false, true, true, 131072, 542720, 16},
    {GSGFX_LAYER_SECONDARY_DISTORTION, g_layer_names[GSGFX_LAYER_SECONDARY_DISTORTION], GS_GFX_LAYER_DISTORTION, false, false, false, false, 0, 81920, 16},
    {GSGFX_LAYER_FILTER, g_layer_names[GSGFX_LAYER_FILTER], GS_GFX_LAYER_NORMAL, false, false, false, true, 0, 16384, 1},
    {GSGFX_LAYER_MENU, g_layer_names[GSGFX_LAYER_MENU], GS_GFX_LAYER_NORMAL, false, false, false, true, 0, 131072, 16},
    {GSGFX_LAYER_INVALID, 0, GS_GFX_LAYER_NORMAL, false, false, false, false, 0, 0, 0},
};

_GSgfxLayerData *g_empty_layer_config = _empty_layer_config;
_GSgfxLayerData *g_map_layer_config = _map_layer_config;
_GSgfxLayerData *g_fight_layer_config = _fight_layer_config;
_GSgfxLayerData *g_menu_layer_config = _menu_layer_config;

struct FloorLayerConfigEntry {
    _GSgfxLayerData *config;
    unsigned long floorID;
};

static FloorLayerConfigEntry _floor_layer_config_table[] = {
    {_M6_layer_config, 153},
    {_floor_542_layer_config, 542},
    {_evolution_layer_config, 902},
};

FloorLayerConfigEntry *g_floor_layer_config_list = _floor_layer_config_table;
unsigned long g_floor_layer_config_count = 3;

GSgfxLayerID _menu_extra_layer_id;

extern "C" void floorLayerConfigLoad(void)
{
}

GSgfxLayerID floorGetMenuLayerID(void)
{
    return _menu_extra_layer_id;
}

extern "C" void floorLayerConfig(unsigned long floorID, unsigned char floorKind)
{
    unsigned long i;

    for (i = 0; i < g_floor_layer_config_count; i++) {
        if (g_floor_layer_config_list[i].floorID == floorID) {
            GSgfxLayerConfig::Setup(g_floor_layer_config_list[i].config);
            dbgMenuSetLayerID(GSgfxLayerFindByName_C("debug menu"));
            return;
        }
    }

    switch (floorKind) {
    case FLOOR_KIND_MENU_EXTRA_A:
    case FLOOR_KIND_MENU_EXTRA_B:
        GSgfxLayerConfig::Setup(g_menu_layer_config);
        _menu_extra_layer_id = GSgfxLayerFindByName_C("menu extra");
        break;
    case FLOOR_KIND_MENU:
        GSgfxLayerConfig::Setup(g_menu_layer_config);
        _menu_extra_layer_id = GSgfxLayerFindByName_C("menu");
        break;
    case FLOOR_KIND_FIELD: {
        GSgfxLayerConfig::Setup(g_map_layer_config);
        GSgfxLayer *backgroundLayer = GSgfxLayerFindByID(GSGFX_LAYER_BASE);
        GSgfxLayer *mainLayer = GSgfxLayerFindByID(GSGFX_LAYER_DEFAULT);
        backgroundLayer->SetPassRedirect(GSGFX_LAYER_PASS_XLU, true, mainLayer,
                                         GSGFX_LAYER_PASS_OPAQUE, GS_LAYER_INSERT_AFTER);
        break;
    }
    case FLOOR_KIND_FIGHT:
    case FLOOR_KIND_FIGHT_ALT:
        GSgfxLayerConfig::Setup(g_fight_layer_config);
        break;
    default:
        GSgfxLayerConfig::Setup(g_empty_layer_config);
        break;
    }

    dbgMenuSetLayerID(GSgfxLayerFindByName_C("debug menu"));
}
