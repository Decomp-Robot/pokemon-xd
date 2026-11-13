#ifndef FLOOR_LAYER_CONFIG_H
#define FLOOR_LAYER_CONFIG_H

#include <GSgfx.h>

extern "C" {
GSgfxLayerID floorGetMenuLayerID();
void floorLayerConfig(unsigned long floorID, unsigned char floorKind);
}

#endif
