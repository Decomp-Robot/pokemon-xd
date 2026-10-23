#ifndef GSMSG_EX_H
#define GSMSG_EX_H

#include <types.h>
#include <GSgfx.h>
#include <GSmsg.h>

extern "C" {
long GSmsgIsCheck(unsigned long messageID);
long GSmsgExec(unsigned long messageID, u8 messageMode, u8 soundGroup);
void GSmsgKill(unsigned long messageID);
long GSmsgMain(GSgfxLayerID layer, unsigned long messageID, unsigned long keys, bool active);
}

#endif
