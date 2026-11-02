#ifndef GSCOLSYS2_INTERNAL_H
#define GSCOLSYS2_INTERNAL_H

#include <GScolsys2.h>
#include <GScamera.h>
#include <dolphin/mtx.h>

/* Current floor and debug drawing resources. */
struct GScolsys2State {
    CCD_FILEHEAD* mFile; /* The loaded CCD collision file head. */
    GSCOLSYS2_FLOOR mFloor[1]; /* Floor collision data, indexed by mTableIndex. */
    s32 mTableIndex; /* Floor entry the collision checks use. */
    GSgfxVF* mVf; /* Vertex format the debug display lists render with. */
    GSgfxDL* mFixedDisplayList; /* Display list drawing the fixed collision objects. */
};

extern GScolsys2State GScolsys2;

extern "C" {
CCD_FILEHEAD* GScolsys2GetCCDFileHead(void);
}

#endif
