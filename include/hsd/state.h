#ifndef HSD_STATE_H
#define HSD_STATE_H

#include <types.h>

enum {
    HSD_PE_COLOR_UPDATE = 0x01,
    HSD_PE_ALPHA_UPDATE = 0x02,
    HSD_PE_DST_ALPHA = 0x04,
    HSD_PE_BEFORE_TEX = 0x08,
    HSD_PE_Z_COMP = 0x10,
    HSD_PE_Z_UPDATE = 0x20,
    HSD_PE_DITHER = 0x40
};

typedef struct _HSD_PEDesc {
    u8 flags;
    u8 ref0;
    u8 ref1;
    u8 dstAlpha;
    u8 blendMode;
    u8 srcFactor;
    u8 dstFactor;
    u8 logicOp;
    u8 zComp;
    u8 alphaComp0;
    u8 alphaOp;
    u8 alphaComp1;
} HSD_PEDesc;

#endif
