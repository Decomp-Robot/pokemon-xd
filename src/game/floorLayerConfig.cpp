#include <floorLayerConfig.h>

// Layer configuration data that GSgfxLayerConfig::Setup registers for a layer.
struct _GSgfxLayerData;

// One entry of the floor configuration list: the layer configuration used for
// a floor and the floor ID it was picked for.
struct FloorLayerConfigEntry {
    _GSgfxLayerData *config;
    unsigned long floorID;
};

extern FloorLayerConfigEntry _floor_layer_config_table[];
FloorLayerConfigEntry *g_floor_layer_config_list = _floor_layer_config_table;

// Number of entries in g_floor_layer_config_list.
unsigned long g_floor_layer_config_count = 3;

// ID of the extra layer the menus are drawn on.
extern GSgfxLayerID _menu_extra_layer_id;

extern "C" void floorLayerConfigLoad(void)
{
}

GSgfxLayerID floorGetMenuLayerID(void)
{
    return _menu_extra_layer_id;
}
