#ifndef MENU_HOLOGRAM_H
#define MENU_HOLOGRAM_H

class CMenuGlobalState;

void menuHologramExecFromPcBox(CMenuGlobalState &state);
extern "C" void menuHologramReloadDatas();
extern "C" void menuHologramUnloadDatas();

#endif
