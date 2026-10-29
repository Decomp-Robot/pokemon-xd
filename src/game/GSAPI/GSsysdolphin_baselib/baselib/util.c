#include <dolphin/gx/GXStruct.h>
#include <hsd/assert.h>

GXPosNrmMtx HSD_Index2PosNrmMtx(u32 idx)
{
    switch (idx) {
    case 0:
        return GX_PNMTX0;
    case 1:
        return GX_PNMTX1;
    case 2:
        return GX_PNMTX2;
    case 3:
        return GX_PNMTX3;
    case 4:
        return GX_PNMTX4;
    case 5:
        return GX_PNMTX5;
    case 6:
        return GX_PNMTX6;
    case 7:
        return GX_PNMTX7;
    case 8:
        return GX_PNMTX8;
    case 9:
        return GX_PNMTX9;
    }
    __assert(__FILE__, 132, "0");
    return GX_PNMTX0;
}

u32 HSD_GetNbBits(u32 value)
{
    u32 nbits = 0;
    u32 i;

    for (i = 0; i < 32; i++) {
        if (value & (1 << i)) {
            nbits++;
        }
    }
    return nbits;
}

void HSD_MulColor(GXColor *src0, GXColor *src1, GXColor *dst)
{
    dst->r = (u32)src0->r * src1->r / 255;
    dst->g = (u32)src0->g * src1->g / 255;
    dst->b = (u32)src0->b * src1->b / 255;
    dst->a = (u32)src0->a * src1->a / 255;
}
