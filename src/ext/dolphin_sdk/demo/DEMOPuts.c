#include <dolphin.h>
#include <dolphin/gx.h>
#include <dolphin/demo.h>
#include <dolphin/mtx.h>

#include "__demo.h"

static GXTexObj fontTexObj;

static s32 fontShift;
static OSFontHeader* FontData;
static void* LastSheet;
static s16 FontSize;
static s16 FontSpace;

// prototypes
static void DrawFontChar(int x, int y, int z, int xChar, int yChar);
static void LoadSheet(void* image, GXTexMapID texMapID);

// UNUSED: the game never calls this; dead-stripped at link.
void DEMOSetFontType(s32 attr) {
    switch(attr) {
    case DM_FT_RVS:
        GXSetBlendMode(GX_BM_LOGIC, GX_BL_ZERO, GX_BL_ZERO, GX_LO_INVCOPY);
        break;
    case DM_FT_XLU:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_CLEAR);
        break;
    case DM_FT_OPQ:
    default:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
        break;
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
void DEMOLoadFont(GXTexMapID texMap, GXTexMtx texMtx, DMTexFlt texFlt) {
    Mtx fontTMtx;
    u16 width;
    u16 height;

    width = 64;
    height = 0x1800 / width;
    GXInitTexObj(&fontTexObj, (void*)DEMOFontBitmap, width, (u16)height, GX_TF_I4, GX_CLAMP, GX_CLAMP, 0);

    if (texFlt == DMTF_POINTSAMPLE) {
        GXInitTexObjLOD(&fontTexObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
        fontShift = 0;
    } else {
        fontShift = 1;
    }

    GXLoadTexObj(&fontTexObj, texMap);
    MTXScale(fontTMtx, 1.0f / width, 1.0f / height, 1.0f);
    GXLoadTexMtxImm(fontTMtx, texMtx, GX_MTX2x4);
    GXSetNumTexGens(1);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, texMtx);
}

// UNUSED: the game never calls this; dead-stripped at link.
void DEMOSetupScrnSpc(s32 width, s32 height, f32 depth) {
    Mtx44 pMtx;
    Mtx mMtx;
    f32 top;

    if (DEMOGetRenderModeObj()->field_rendering && !VIGetNextField()) {
        top = -0.667f;
    } else {
        top = 0.0f;
    }

    MTXOrtho(pMtx, top, height, 0.0f, width, 0.0f, -depth);
    GXSetProjection(pMtx, GX_ORTHOGRAPHIC);
    MTXIdentity(mMtx);
    GXLoadPosMtxImm(mMtx, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
}

// UNUSED: the game never calls this; dead-stripped at link.
void DEMOInitCaption(s32 font_type, s32 width, s32 height) {
    DEMOSetupScrnSpc(width, height, 100.0f);
    GXSetZMode(GX_ENABLE, GX_ALWAYS, GX_ENABLE);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    DEMOLoadFont(GX_TEXMAP0, GX_TEXMTX0, DMTF_POINTSAMPLE);
    DEMOSetFontType(font_type);
}

// UNUSED: the game never calls this; dead-stripped at link.
void DEMOPuts(s16 x, s16 y, s16 z, char* string) {
    char* str;
    s32 s;
    s32 t;
    s32 c;
    s32 w;
    s32 len;
    s32 i;

    str = string;
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 1);

    // calc len
    len = 0;
    while (1) {
        c = *(str++);
        if ((c >= 0x20) && (c <= 0x7F)) {
            len++;
            continue;
        }

        if (len > 0) {
            GXBegin(GX_QUADS, GX_VTXFMT0, len * 4);
            for(i = 0; i < len; i++) {
                w = string[i] - 0x20;
                s = fontShift + ((w % 8) * 0x10);
                t = fontShift + ((w / 8) * 0x10);
                GXPosition3s16(x + (i * 8), y, z);
                GXTexCoord2s16(s, t);
                GXPosition3s16(x + (i * 8) + 8, y, z);
                GXTexCoord2s16(s + 0x10, t);
                GXPosition3s16(x + (i * 8) + 8, y + 8, z);
                GXTexCoord2s16(s + 0x10, t + 0x10);
                GXPosition3s16(x + (i * 8), y + 8, z);
                GXTexCoord2s16(s, t + 0x10);
            }
            GXEnd();
            len = 0;
        }

        string = str;
        if (c == 0xA) {
            y += 0x8;
        } else {
            break;
        }
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
void DEMOPrintf(s16 x, s16 y, s16 z, char* fmt, ...) {
    va_list vlist;
    char buf[256];

    va_start(vlist, fmt);
    vsprintf(buf, fmt, vlist);
    DEMOPuts(x, y, z, buf);
    va_end(vlist);
}

// UNUSED: the game never calls this; dead-stripped at link.
OSFontHeader* DEMOInitROMFont(void) {
    switch (OSGetFontEncode()) {
    case OS_FONT_ENCODE_SJIS:
        FontData = OSAlloc(OS_FONT_SIZE_SJIS);
        break;
    case OS_FONT_ENCODE_ANSI:
        FontData = OSAlloc(OS_FONT_SIZE_ANSI);
        break;
    default:
        FontData = OSAlloc(0x141020);
        break;
    }

    if (!FontData) {
        OSPanic(__FILE__, 446, "Ins. memory to load ROM font.");
    }

    if (OSInitFont(FontData) == 0) {
        OSPanic(__FILE__, 450, "ROM font is available in boot ROM ver 0.8 or later.");
    }

    FontSize = FontData->cellWidth * 16;
    FontSpace = -16;
    return FontData;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
static void DrawFontChar(int x, int y, int z, int xChar, int yChar) {
    s16 posLeft = x;
    s16 posRight = posLeft + FontSize;
    s16 posTop = y - (FontData->ascent * FontSize / FontData->cellWidth);
    s16 posBottom = y + (FontData->descent * FontSize / FontData->cellWidth);

    s16 texLeft = xChar;
    s16 texRight = (xChar + FontData->cellWidth);
    s16 texTop = yChar;
    s16 texBottom = (yChar + FontData->cellHeight);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3s16(posLeft, posTop, z);
    GXTexCoord2s16(texLeft, texTop);
    GXPosition3s16(posRight, posTop, z);
    GXTexCoord2s16(texRight, texTop);
    GXPosition3s16(posRight, posBottom, z);
    GXTexCoord2s16(texRight, texBottom);
    GXPosition3s16(posLeft, posBottom, z);
    GXTexCoord2s16(texLeft, texBottom);
    GXEnd();
}

// UNUSED: the game never calls this; dead-stripped at link.
static void LoadSheet(void* image, GXTexMapID texMapID) {
    Mtx mtx;
    GXTexObj texObj;

    if (LastSheet != image) {
        LastSheet = image;
        GXInitTexObj(&texObj, image, FontData->sheetWidth, FontData->sheetHeight, FontData->sheetFormat, 0, 0, 0);
        GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
        GXLoadTexObj(&texObj, texMapID);
        MTXScale(mtx, 1.0f / FontData->sheetWidth, 1.0f / FontData->sheetHeight, 1.0f);
        GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
        GXSetNumTexGens(1);
        GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
int DEMORFPuts(s16 x, s16 y, s16 z, char* string) {
    s32 cx;
    void* image;
    s32 xChar;
    s32 yChar;
    s32 width;

    ASSERTLINE(583, FontData);
    LastSheet = NULL;

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 4);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0);

    x *= 16;
    y *= 16;
    z *= 16;

    width = 0;
    while (*string != 0) {
        if (*string == '\n') {
            width = 0;
            y += FontData->leading * FontSize / FontData->cellWidth;
            string++;
        } else if (*string == '\t') {
            width += 8 * (FontSize + FontSpace);
            width -= width % (8 * (FontSize + FontSpace));
            string++;
        } else {
            string = OSGetFontTexture(string, &image, &xChar, &yChar, &cx);
            LoadSheet(image, GX_TEXMAP0);
            DrawFontChar(x + width, y, z, xChar, yChar);
            width = FontSpace + ((FontSize * cx) / FontData->cellWidth) + width;
        }
    }

    return (width + 15) / 16;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
char* DEMODumpROMFont(char* string) {
    u32 image[288];
    void* temp;
    int i;
    int j;
    s32 width;

    ASSERTLINE(724, FontData);

    switch (OSGetFontEncode()) {
    case OS_FONT_ENCODE_SJIS:
        temp = (u8*)FontData + OS_FONT_SIZE_SJIS - OS_FONT_ROM_SIZE_SJIS;
        break;
    case OS_FONT_ENCODE_ANSI:
        temp = (u8*)FontData + OS_FONT_SIZE_ANSI - OS_FONT_ROM_SIZE_ANSI;
        break;
    default:
        temp = (u8*)FontData + 0xF4020;
        break;
    }

    temp = (void*)OSRoundDown32B(temp);
    OSLoadFont(FontData, temp);

    memset(image, 0, sizeof(image));
    string = OSGetFontTexel(string, &image[0], 0, 0xC, &width);
    for (i = 0; i < 0x30; i++) {
        j = (i % 8) + ((i / 8) * 0x30);
        OSReport("%08x%08x%08x%08x%08x%08x\n", image[j], image[j+8], image[j+0x10], image[j+0x18], image[j+0x20], image[j+0x28]);
    }

    OSReport("\nwidth %d\n", width);
    OSInitFont(FontData);
    return string;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
