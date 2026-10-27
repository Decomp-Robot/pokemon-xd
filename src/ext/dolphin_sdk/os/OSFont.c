#include <dolphin.h>
#include <dolphin/os.h>

#include "__os.h"

typedef char* (*ParseStringCallback)(u16, const char*, OSFontHeader**, int*);

static OSFontHeader* FontDataAnsi;
static OSFontHeader* FontDataSjis;
static ParseStringCallback ParseString;
static int FixedPitch;
static u16 FontEncode = 0xFFFF;

// prototypes
static char* ParseStringS(u16 encode, const char* string, OSFontHeader** pfont, int* pfontCode);

static u16 HankakuToCode[]
    = { 524, 525, 526, 527, 528, 529, 530, 531,
        532, 533, 534, 535, 536, 537, 538, 539,
        540, 541, 542, 543, 544, 545, 546, 547,
        548, 549, 550, 551, 552, 553, 554, 555,
        556, 557, 558, 559, 560, 561, 562, 563,
        564, 565, 566, 567, 568, 569, 570, 571,
        572, 573, 574, 575, 576, 577, 578, 579,
        580, 581, 582, 583, 584, 585, 586, 587,
        588, 589, 590, 591, 592, 593, 594, 595,
        596, 597, 598, 599, 600, 601, 602, 603,
        604, 605, 606, 607, 608, 609, 610, 611,
        612, 613, 614, 615, 616, 617, 618, 524,
        524, 524, 524, 524, 524, 524, 524, 524,
        524, 524, 524, 524, 524, 524, 524, 524,
        524, 524, 524, 524, 524, 524, 524, 524,
        524, 524, 524, 524, 524, 524, 524, 524,
        524, 619, 620, 621, 622, 623, 624, 625,
        626, 627, 628, 629, 630, 631, 632, 633,
        634, 635, 636, 637, 638, 639, 640, 641,
        642, 643, 644, 645, 646, 647, 648, 649,
        650, 651, 652, 653, 654, 655, 656, 657,
        658, 659, 660, 661, 662, 663, 664, 665,
        666, 667, 668, 669, 670, 671, 672, 673,
        674, 675, 676, 677, 678, 679, 680, 681,
    };

static u16 Zenkaku2Code[]
    = { 0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55,
        56, 57, 58, 59, 60, 61, 62,
        63, 64, 65, 66, 67, 68, 69, 70,
        71, 72, 73, 74, 75, 76, 77, 78,
        79, 80, 81, 82, 83, 84, 85, 86,
        87, 88, 89, 90, 91, 92, 93, 94,
        95, 96, 97, 98, 99, 100, 101, 102,
        103, 104, 105, 106, 107, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        108, 109, 110, 111, 112, 113, 114, 115,
        0, 0, 0, 0, 0, 0, 0, 0,
        116, 117, 118, 119, 120, 121, 122, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 123, 124, 125, 126, 127, 128,
        129, 130, 131, 132, 133, 134, 135, 136,
        137, 0, 0, 0, 0, 0, 0, 0,
        138, 139, 140, 141, 142, 143, 144, 145,
        0, 0, 0, 0, 146,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 147,
        148, 149, 150, 151, 152, 153, 154, 155,
        156, 0, 0, 0, 0, 0, 0, 0,
        157, 158, 159, 160, 161, 162, 163, 164,
        165, 166, 167, 168, 169, 170, 171, 172,
        173, 174, 175, 176, 177, 178, 179, 180,
        181, 182, 0, 0, 0, 0, 0,
        0, 183, 184, 185, 186, 187, 188, 189,
        190, 191, 192, 193, 194, 195, 196, 197,
        198, 199, 200, 201, 202, 203, 204, 205,
        206, 207, 208, 0, 0, 0, 0, 209,
        210, 211, 212, 213, 214, 215, 216, 217,
        218, 219, 220, 221, 222, 223, 224, 225,
        226, 227, 228, 229, 230, 231, 232, 233,
        234, 235, 236, 237, 238, 239, 240, 241,
        242, 243, 244, 245, 246, 247, 248, 249,
        250, 251, 252, 253, 254, 255, 256, 257,
        258, 259, 260, 261, 262, 263, 264, 265,
        266, 267, 268, 269, 270, 271, 272, 273,
        274, 275, 276, 277, 278, 279, 280, 281,
        282, 283, 284, 285, 286, 287, 288, 289,
        290, 291, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        292, 293, 294, 295, 296, 297, 298, 299,
        300, 301, 302, 303, 304, 305, 306, 307,
        308, 309, 310, 311, 312, 313, 314, 315,
        316, 317, 318, 319, 320, 321, 322, 323,
        324, 325, 326, 327, 328, 329, 330, 331,
        332, 333, 334, 335, 336, 337, 338, 339,
        340, 341, 342, 343, 344, 345, 346, 347,
        348, 349, 350, 351, 352, 353, 354,
        355, 356, 357, 358, 359, 360, 361, 362,
        363, 364, 365, 366, 367, 368, 369, 370,
        371, 372, 373, 374, 375, 376, 377, 0,
        0, 0, 0, 0, 0, 0, 0, 378,
        379, 380, 381, 382, 383, 384, 385, 386,
        387, 388, 389, 390, 391, 392, 393, 394,
        395, 396, 397, 398, 399, 400, 401, 0,
        0, 0, 0, 0, 0, 0, 0, 402,
        403, 404, 405, 406, 407, 408, 409, 410,
        411, 412, 413, 414, 415, 416, 417, 418,
        419, 420, 421, 422, 423, 424, 425, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        426, 427, 428, 429, 430, 431, 432, 433,
        434, 435, 436, 437, 438, 439, 440, 441,
        442, 443, 444, 445, 446, 447, 448, 449,
        450, 451, 452, 453, 454, 455, 456, 457,
        458, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        459, 460, 461, 462, 463, 464, 465, 466,
        467, 468, 469, 470, 471, 472, 473,
        474, 475, 476, 477, 478, 479, 480, 481,
        482, 483, 484, 485, 486, 487, 488, 489,
        490, 491, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 492,
        493, 494, 495, 496, 497, 498, 499, 500,
        501, 502, 503, 504, 505, 506, 507, 508,
        509, 510, 511, 512, 513, 514, 515, 516,
        517, 518, 519, 520, 521, 522, 523, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 524,
        525, 526, 527, 528, 529, 530, 531, 532,
        533, 534, 535, 536, 537, 538, 539, 540,
        541, 542, 543, 544, 545, 546, 547, 548,
        549, 550, 551, 552, 553, 554, 555, 556,
        557, 558, 559, 560, 561, 562, 563, 564,
        565, 566, 567, 568, 569, 570, 571, 572,
        573, 574, 575, 576, 577, 578, 579, 580,
        581, 582, 583, 584, 585, 586, 587,
        588, 589, 590, 591, 592, 593, 594, 595,
        596, 597, 598, 599, 600, 601, 602, 603,
        604, 605, 606, 607, 608, 609, 610, 611,
        612, 613, 614, 615, 616, 617, 618, 619,
        620, 621, 622, 623, 624, 625, 626, 627,
        628, 629, 630, 631, 632, 633, 634, 635,
        636, 637, 638, 639, 640, 641, 642, 643,
        644, 645, 646, 647, 648, 649, 650, 651,
        652, 653, 654, 655, 656, 657, 658, 659,
        660, 661, 662, 663, 664, 665, 666, 667,
        668, 669, 670, 671, 672, 673, 674, 675,
        676, 677, 678, 679, 680, 681, 682, 683,
        684, 685, 686, 687, 688, 689, 690, 691,
        692, 693, 694, 695, 696, 697, 698, 699,
        700, 701, 702, 703, 704, 705, 706, 707,
        708, 709, 710, 711, 712,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        713, 714, 715, 716, 717, 718, 719, 720,
        721, 722, 723, 724, 725, 726, 727, 728,
        729, 730, 731, 732, 733, 734, 735, 736,
        737, 738, 739, 740, 741, 742, 0, 743,
        744, 745, 746, 747, 748, 749, 750, 751,
        752, 753, 754, 755, 756, 757, 758, 759,
        760, 761, 762, 763, 764, 765, 0, 0,
        0, 0, 0, 0, 0, 0, 766,
        767, 768, 769, 770, 771, 772, 773, 774,
        775, 776, 777, 778, 779, 780, 781, 782,
        783, 784, 785, 786, 787, 788, 789, 790,
        791, 792, 793, 794, 795, 0
    };

static inline BOOL IsSjisLeadByte(u8 c) {
    return (0x81 <= c && c <= 0x9F) || (0xE0 <= c && c <= 0xFC);
}

static inline BOOL IsSjisTrailByte(u8 c) {
    return (0x40 <= c && c <= 0xFC) && (c != 0x7F);
}

static const int SJIS_KANJI_GLYPH_BASE = 702;

static int GetFontCode(u16 encode, u16 code) {
    if (encode == OS_FONT_ENCODE_SJIS) {
        if (code >= 0x20 && code <= 0xDF) {
            return HankakuToCode[code - 0x20];
        }

        if (code > 0x889E && code <= 0x9872) {
            int i = ((code >> 8) - 0x88) * 188;
            int j = (code & 0xFF);

            if (!IsSjisTrailByte(j)) {
                return 0;
            }

            j -= 0x40;
            if (j >= 64) {
                j--;
            }

            return (i + j + SJIS_KANJI_GLYPH_BASE);
        }

        if (code >= 0x8140 && code < 0x879E) {
            int i  = ((code >> 8) - 0x81) * 188;
            int j = (code & 0xFF);

            if (!IsSjisTrailByte(j)) {
                return 0;
            }

            j -= 0x40;
            if (j >= 64) {
                j--;
            }

            return Zenkaku2Code[i + j];
        }
    } else if (code > 0x20 && code <= 0xFF) {
        return code - 0x20;
    }

    return 0;
}

static void Decode(u8* s, u8* d) {
    int i;
    int j;
    int k;
    int p;
    int q;
    int linkOfs;
    int litOfs;
    int cnt;
    int os;
    unsigned int flag;
    unsigned int code;

    os  = *(int*)(s + 0x4);
    linkOfs = *(int*)(s + 0x8);
    litOfs  = *(int*)(s + 0xC);

    q    = 0;
    flag = 0;
    p    = 16;

    do {
        // Get next mask
        if (flag == 0) {
            code = *(u32*)(s + p);
            p += sizeof(u32);
            flag = sizeof(u32) * 8;
        }

        // Non-linked chunk
        if (code & 0x80000000) {
            d[q++] = s[litOfs++];
        }
        // Linked chunk
        else {
            // Read offset from link table
            j = s[linkOfs] << 8 | s[linkOfs + 1];
            linkOfs += sizeof(u16);

            // Apply offset
            k = q - (j & 0x0FFF);
            cnt    = j >> 12;
            if (cnt == 0) {
                cnt = s[litOfs++] + 18;
            } else {
                cnt += 2;
            }

            // Copy chunk
            for (i = 0; i < cnt; i++, q++, k++) {
                d[q] = d[k - 1];
            }
        }

        // Prepare next mask bit
        code <<= 1;
        flag--;
    } while (q < os);
}

static inline u32 GetFontSize(u8* buf) {
    if (buf[0] == 'Y' && buf[1] == 'a' && buf[2] == 'y') {
        return *(u32*)(buf + 0x4);
    }

    return 0;
}

u16 OSGetFontEncode(void) {
    if (FontEncode != 0xFFFF) {
        return FontEncode;
    }

    switch (*(int*)OSPhysicalToCached(0xCC)) {
    case VI_NTSC:
        FontEncode = (__VIRegs[VI_DTV_STAT] & 2) ? OS_FONT_ENCODE_SJIS : OS_FONT_ENCODE_ANSI;
        break;
    case VI_PAL:
    case VI_MPAL:
    case VI_DEBUG:
    case VI_DEBUG_PAL:
    case VI_EURGB60:
    default:
        FontEncode = OS_FONT_ENCODE_ANSI;
    }

    ParseString = ParseStringS;
    return FontEncode;
}


static inline void ReadROM(void* buf, int length, int offset) {
    int len;
    while (length > 0) {
        len = (length <= 256) ? length : 256;
        length -= len;

        while (!__OSReadROM(buf, len, offset)) {
            ;
        }

        offset += len;
        (u8*)buf += len;
    }
}

static u32 ReadFont(void* img, u16 encode, void* fontData) {
    u32 size;

    if (encode == OS_FONT_ENCODE_SJIS) {
        ReadROM(img, OS_FONT_ROM_SIZE_SJIS, 0x1AFF00);
    } else {
        ReadROM(img, OS_FONT_ROM_SIZE_ANSI, 0x1FCF00);
    }

    size = GetFontSize(img);
    if (size == 0) {
        return 0;
    }

    Decode(img, fontData);
    if (encode == OS_FONT_ENCODE_SJIS) {
        OSFontHeader* font = (OSFontHeader*)fontData;
        int fontCode;
        u32 sheetSize;
        u8* imageSrc;
        int sheet;
        int numChars;
        int row;
        int column;
        int x;
        int y;
        u8* src;
        u16 imageT[4] = {0x2ABE, 0x003D, 0x003D, 0x003D};

        fontCode = GetFontCode(encode, 'T');
        sheet = fontCode / (font->sheetColumn * font->sheetRow);
        numChars = fontCode - (sheet * (font->sheetColumn * font->sheetRow)); 
        row = numChars / font->sheetColumn;
        column = numChars - (row * font->sheetColumn);
        row *= font->cellHeight;
        column *= font->cellWidth;

        sheetSize = font->sheetSize;
        imageSrc = (u8*)font + font->sheetImage;
        imageSrc += ((sheet * sheetSize) >> 1);

        for (y = 4; y < 8; y++) {
            x = 0;
            src = imageSrc + ((((font->sheetWidth / 8) << 5) / 2) * ((row + y) / 8));
            src += ((column + x) / 8) * 16;
            src += ((row + y) % 8) * 2;
            src += ((column + x) % 8) / 4;

            *(u16*)src = imageT[y - 4];
        }
    }

    return size;
}

u32 OSLoadFont(OSFontHeader* fontData, void* tmp) {
    u16 encode;
    u32 size;

    encode = OSGetFontEncode();
    switch (encode) {
    case OS_FONT_ENCODE_ANSI:
        FontDataAnsi = fontData;
        size = ReadFont(tmp, OS_FONT_ENCODE_ANSI, FontDataAnsi);
        break;
    case OS_FONT_ENCODE_SJIS:
        FontDataSjis = fontData;
        size = ReadFont(tmp, OS_FONT_ENCODE_SJIS, FontDataSjis);
        break;
    case 3:
    case 4:
    case 5:
        FontDataAnsi = fontData;
        size = ReadFont(tmp, OS_FONT_ENCODE_ANSI, FontDataAnsi);
        if (size != 0) {
            FontDataSjis = (OSFontHeader*)((u8*)FontDataAnsi + size);
            size += ReadFont(tmp, OS_FONT_ENCODE_SJIS, FontDataSjis);
        }
        break;
    case 2:
    default:
        size = 0;
        break;
    }

    return size;
}

static char* ParseStringS(u16 encode, const char* string, OSFontHeader** pfont, int* pfontCode) {
    OSFontHeader* font;
    u16 code = 0;

    switch (encode) {
    case OS_FONT_ENCODE_ANSI:
        font = FontDataAnsi;
        code = (u8)*string;
        if (code != 0) {
            string++;
        }
        break;
    case OS_FONT_ENCODE_SJIS:
        font = FontDataSjis;
        code = (u8)*string;
        if (code == 0) {
            break;
        }
        string++;

        if (IsSjisLeadByte(code) && IsSjisTrailByte((u8)*string)) {
            code = (code << 8 | (u8)*string++);
        }
        break;
    }

    *pfont = font;
    *pfontCode = GetFontCode(encode, code);

    return (char*)string;
}


char* OSGetFontTexel(const char* string, void* image, s32 pos, s32 stride, s32* width) {
    u16 encode;
    OSFontHeader* font;
    u8* src;
    u8* dst;
    int fontCode;
    int sheet;
    int numChars;
    int row;
    int column;
    int x;
    int y;
    int offsetSrc;
    int offsetDst;
    u8* colorIndex;
    u8* imageSrc;

    encode = OSGetFontEncode();
    string = ParseString(encode, string, &font, &fontCode);
    colorIndex = &font->c0;
    ASSERTLINE(828, font->sheetFormat == GX_TF_I4);
    
    sheet = fontCode / (font->sheetColumn * font->sheetRow);
    numChars = fontCode - (sheet * (font->sheetColumn * font->sheetRow)); 
    row = numChars / font->sheetColumn;
    column = numChars - (row * font->sheetColumn);
    row *= font->cellHeight;
    column *= font->cellWidth;

    imageSrc = (u8*)font + font->sheetImage;
    imageSrc += ((sheet * font->sheetSize) >> 1);

    for (y = 0; y < font->cellHeight; y++) {
        for (x = 0; x < font->cellWidth; x++) {
            src = imageSrc + (((font->sheetWidth / 8) * 32) / 2) * ((row + y) / 8);
            src += ((column + x) / 8) * 16;
            src += ((row + y) % 8) * 2; 
            src += ((column + x) % 8) / 4;

            offsetSrc = (column + x) % 4;

            dst = (u8*)image + ((y / 8) * (((stride * 4) / 8) * 32));
            dst += (((pos + x) / 8) * 32);
            dst += ((y % 8) * 4);
            dst += ((pos + x) % 8) / 2;

            offsetDst = (pos + x) % 2;

            *dst |= colorIndex[*src >> (6 - (offsetSrc * 2)) & 3] & ((offsetDst != 0) ? 0x0F : 0xF0);
        }
    }
    
    if (width != 0) {
        *width = ((u8*)font + font->widthTable)[fontCode];
    }

    return (char*)string;
}





