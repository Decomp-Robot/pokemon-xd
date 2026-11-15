#include <types.h>
#include <Pokemon.h>
#include <darkPokemon.h>
#include <GSgfx.h>
#include <time.h>

f32 meterCount;

void _menuDrawReliveMeterSub(DarkPokemon *pokemon, long x, long y, long width, long height, u8 alpha,
                            GSgfxLayer *layer);
void _menuDrawReliveMeterSubDpValue(long initPoint, long darkPoint, long x, long y, long width, long height,
                                   u8 alpha, GSgfxLayer *layer);
extern "C" void menuDrawReliveMeter3(Pokemon *pokemon, long x, long y, long width, long height, u8 alpha,
                                    GSgfxLayerID layerID)
{
    GSgfxLayer *layer = GSgfxLayerFindByID(layerID);
    if (layer == 0) {
        return;
    }
    _menuDrawReliveMeterSub(pokemon->getDarkPokemon(), x, y, width, height, alpha, layer);
}

extern "C" void menuDrawReliveMeterByDarkPokemon(DarkPokemon *pokemon, long x, long y, long width, long height,
                                                 u8 alpha, GSgfxLayerID layerID)
{
    GSgfxLayer *layer = GSgfxLayerFindByID(layerID);
    if (layer == 0) {
        return;
    }
    _menuDrawReliveMeterSub(pokemon, x, y, width, height, alpha, layer);
}

extern "C" void menuDrawReliveMeter(Pokemon *pokemon, long width, long height, GSgfxLayerID layerID)
{
    if (!pokemon->isDarkPokemon()) {
        return;
    }
    GSgfxLayer *layer = GSgfxLayerFindByID(layerID);
    if (layer == 0) {
        return;
    }
    _menuDrawReliveMeterSub(pokemon->getDarkPokemon(), 0, 0, width, height, 255, layer);
}

extern "C" void menuDrawReliveUpdate(void)
{
    meterCount += timeGetLastFrameSec();
}

void _menuDrawReliveMeterSub(DarkPokemon *pokemon, long x, long y, long width, long height, u8 alpha,
                            GSgfxLayer *layer)
{
    long initPoint = pokemon->getInitDarkPointDirect();
    long darkPoint = pokemon->getDarkPointDirect();
    _menuDrawReliveMeterSubDpValue(initPoint, darkPoint, x, y, width, height, alpha, layer);
}
