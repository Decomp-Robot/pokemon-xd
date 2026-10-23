#ifndef GSMSG_EX_H
#define GSMSG_EX_H

#include <types.h>
#include <GSgfx.h>

extern "C" {
long GSmsgIsCheck(unsigned long messageID);
long GSmsgExec(unsigned long messageID, u8 messageMode, u8 soundGroup);
void GSmsgKill(unsigned long messageID);
long GSmsgMain(GSgfxLayerID layer, unsigned long messageID, unsigned long keys, bool active);
void GSmsgPrint2(GSgfxLayerID layer, long x, long y, unsigned long color, unsigned long messageID);
}

#endif
