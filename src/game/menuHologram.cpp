#include <types.h>
#include <CMenuGlobalState.h>
#include <GSmemory.h>
#include <menuHologram.h>

class CMenuHologram {
public:
    void reloadDatas(bool kickBgm);
    void unloadDatas();
    void main(CMenuGlobalState &state);
};

extern CMenuHologram *pMenuHologram;

void thisNew();

namespace {

void thisFree();
void thisExec(CMenuGlobalState &state);

}

void menuHologramReloadDatas()
{
    pMenuHologram->reloadDatas(false);
}

void menuHologramUnloadDatas()
{
    pMenuHologram->unloadDatas();
}

void menuHologramExecFromPcBox(CMenuGlobalState &state)
{
    thisExec(state);
}

namespace {

void thisExec(CMenuGlobalState &state)
{
    thisNew();
    pMenuHologram->main(state);
    thisFree();
}

void thisFree()
{
    if (pMenuHologram != NULL) {
        GSmemFree(pMenuHologram);
        pMenuHologram = NULL;
    }
}

}
