#if DEBUG

#include <stdio.h>

#include <dolphin/gx.h>

#include "__gx.h"

static char __data_0[] = "RGB multisample";
static char _305[] = "GX_TEVPREV(color)";
static char _306[] = "GX_TEVPREV(alpha)";
static char _307[] = "GX_TEVREG0(color)";
static char _308[] = "GX_TEVREG0(alpha)";
static char _309[] = "GX_TEVREG1(color)";
static char _310[] = "GX_TEVREG1(alpha)";
static char _311[] = "GX_TEVREG2(color)";
static char _312[] = "GX_TEVREG2(alpha)";

static char* TevRegNames[8] = {0};

#define SOME_GET_REG_MACRO(reg, size, shift) ((u32)((reg) << (shift)) & ((1 << (size)) - 2))
#define SOME_GET_REG_MACRO2(reg, size, shift) ((u32)((reg) >> (shift)) & ((1 << (size)) - 2))

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

#define SOMEINDEX(index) (index & 3) + ((index * 8) & ~0x1F)

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// UNUSED: the game never calls this; dead-stripped at link.

#if DEBUG
static char _521[] = "A";
static char _522[] = "B";
static char _523[] = "C";
static char _524[] = "D";
// UNUSED: the game never calls this; dead-stripped at link.
asm void __GXVerifyTEV(void) {
    nofralloc
#include "../../nonmatchings/__GXVerifyTEV.s"
}
#pragma peephole on
#else
void __GXVerifyTEV(void) {
    u32 i; // r31
    u32 nTev; // r29
    u32 nCol; // r28
    u32 enabled; // r30
    u32 color; // r27
    u32 Clh[4]; // r1+0x38
    u32 Alh[4]; // r1+0x28
    u32 Cwritten[4]; // r1+0x18
    u32 Awritten[4]; // r1+0x8

    nTev = GET_REG_FIELD(__gxVerif->rasRegs[0], 4, 10) + 1;
    nCol = GET_REG_FIELD(__gxVerif->rasRegs[0], 3, 4);
    nCol;

    for (i = 0; i < 4; i++) {
        Clh[i] = 0;
        Alh[i] = 0;
        Cwritten[i] = 0;
        Awritten[i] = 0;
    }

    for (i = 0; i < nTev; i++) {
        if (__gxVerif->verifyLevel >= GX_WARN_SEVERE
         && (((u32) ((__gxVerif->rasRegs[(i * 2) + 0xC0] & 0xFF000000) + 0x01000000) == 0U) || ((u32) ((__gxVerif->rasRegs[(i * 2) + 0xC1] & 0xFF000000) + 0x01000000) == 0U))) {
            sprintf(__gxvDummyStr, __gxvWarnings[32], i);
            __gxVerif->cb(1, 0x20U, __gxvDummyStr);
        }

        if (i & 1) {
            color = GET_REG_FIELD(__gxVerif->rasRegs[(i >> 1U) + 0x28], 3, 19);
        } else {
            color = GET_REG_FIELD(__gxVerif->rasRegs[(i >> 1U) + 0x28], 3, 7);
        }

        if (__gxVerif->verifyLevel >= GX_WARN_MEDIUM && ((color == 0 && nCol < 1) || (color == 1 && nCol < 2))) {
            sprintf(__gxvDummyStr, __gxvWarnings[33], i);
            __gxVerif->cb(1, 0x21U, __gxvDummyStr);
        }

        if (i & 1) {
            enabled = GET_REG_FIELD(__gxVerif->rasRegs[(i >> 1U) + 0x28], 1, 18);
        } else {
            enabled = GET_REG_FIELD(__gxVerif->rasRegs[(i >> 1U) + 0x28], 1, 6);
        }

        if (__gxVerif->verifyLevel >= GX_WARN_SEVERE) {
            if (!enabled && ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 12) == 8 || (u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 12) == 9)) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x22], "A", i);
                __gxVerif->cb(1, 0x22U, __gxvDummyStr);
            }

            if (!enabled && ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 8) == 8 || (u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 8) == 9)) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x22], "B", i);
                __gxVerif->cb(1, 0x22U, __gxvDummyStr);
            }

            if (!enabled && ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 4) == 8 || (u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 4) == 9)) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x22], "C", i);
                __gxVerif->cb(1, 0x22U, __gxvDummyStr);
            }

            if (!enabled && ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 0) == 8 || (u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 0) == 9)) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x22], "D", i);
                __gxVerif->cb(1, 0x22U, __gxvDummyStr);
            }
        
            if (!enabled && (u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 13) == 4) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x23], "A", i);
                __gxVerif->cb(1, 0x23U, __gxvDummyStr);
            }

            if (!enabled && (u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 10) == 4) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x23], "B", i);
                __gxVerif->cb(1, 0x23U, __gxvDummyStr);
            }

            if (!enabled && (u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 7) == 4) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x23], "C", i);
                __gxVerif->cb(1, 0x23U, __gxvDummyStr);
            }

            if (!enabled && (u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 4) == 4) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x23], "D", i);
                __gxVerif->cb(1, 0x23U, __gxvDummyStr);
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 12) <= 7 && ((__gxVerif->rasRegs[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 12) + 0xE1] & 0xFF000000) + 0x01000000) == 0U) {
                if (GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 12) ? !Awritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 13)] : !Cwritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 13)]) {
                    sprintf(__gxvDummyStr, __gxvWarnings[0x24], "A", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 0xCU) ? "alpha" : "color", (__gxVerif->rasRegs[(i * 2) + 0xC0] >> 0xDU) & 7);
                    __gxVerif->cb(1, 0x24U, __gxvDummyStr);
                }
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 8) <= 7 && ((__gxVerif->rasRegs[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 8) + 0xE1] & 0xFF000000) + 0x01000000) == 0U) {
                if (GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 8) ? !Awritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 9)] : !Cwritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 9)]) {
                    sprintf(__gxvDummyStr, __gxvWarnings[0x24], "B", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 8U) ? "alpha" : "color", (__gxVerif->rasRegs[(i * 2) + 0xC0] >> 9U) & 7);
                    __gxVerif->cb(1, 0x24U, __gxvDummyStr);
                }
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 4) <= 7 && ((__gxVerif->rasRegs[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 4) + 0xE1] & 0xFF000000) + 0x01000000) == 0U) {
                if (GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 4) ? !Awritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 5)] : !Cwritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 5)]) {
                    sprintf(__gxvDummyStr, __gxvWarnings[0x24], "C", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 4U) ? "alpha" : "color", (__gxVerif->rasRegs[(i * 2) + 0xC0] >> 5U) & 7);
                    __gxVerif->cb(1, 0x24U, __gxvDummyStr);
                }
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 0) <= 7 && ((__gxVerif->rasRegs[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 0) + 0xE1] & 0xFF000000) + 0x01000000) == 0U) {
                if (GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 0) ? !Awritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 1)] : !Cwritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 1)]) {
                    sprintf(__gxvDummyStr, __gxvWarnings[0x24], "D", i, (__gxVerif->rasRegs[(i * 2) + 0xC0] & 1) ? "alpha" : "color", (__gxVerif->rasRegs[(i * 2) + 0xC0] >> 1U) & 7);
                    __gxVerif->cb(1, 0x24U, __gxvDummyStr);
                }
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 13) <= 3 && ((__gxVerif->rasRegs[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 4, 14) + 0xE0] & 0xFF000000) + 0x01000000) == 0U && Awritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 13)] == 0U) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x25], "A", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 13));
                __gxVerif->cb(1, 0x25U, __gxvDummyStr);
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 10) <= 3 && ((__gxVerif->rasRegs[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 11) + 0xE0] & 0xFF000000) + 0x01000000) == 0U && Awritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 10)] == 0U) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x25], "B", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 10));
                __gxVerif->cb(1, 0x25U, __gxvDummyStr);
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 7) <= 3 && ((__gxVerif->rasRegs[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 8) + 0xE0] & 0xFF000000) + 0x01000000) == 0U && Awritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 7)] == 0U) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x25], "C", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 7));
                __gxVerif->cb(1, 0x25U, __gxvDummyStr);
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 4) <= 3 && ((__gxVerif->rasRegs[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 3) + 0xE0] & 0xFF000000) + 0x01000000) == 0U && Awritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 4)] == 0U) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x25], "D", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 4));
                __gxVerif->cb(1, 0x25U, __gxvDummyStr);
            }
        }

        if (__gxVerif->verifyLevel >= GX_WARN_ALL) {
            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 12) <= 7) {
                if (GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 12) ? Alh[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 13)] : Clh[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 13)]) {
                    sprintf(__gxvDummyStr, __gxvWarnings[0x26], "A", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 12) ? "alpha" : "color", GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 13));
                    __gxVerif->cb(3, 0x26U, __gxvDummyStr);
                }
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 8) <= 7) {
                if (GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 8) ? Alh[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 9)] : Clh[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 9)]) {
                    sprintf(__gxvDummyStr, __gxvWarnings[0x26], "B", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 8) ? "alpha" : "color", GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 9));
                    __gxVerif->cb(3, 0x26U, __gxvDummyStr);
                }
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 4, 4) <= 7) {
                if (GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 4) ? Alh[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 5)] : Clh[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 5)]) {
                    sprintf(__gxvDummyStr, __gxvWarnings[0x26], "C", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 4) ? "alpha" : "color", GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 3, 5));
                    __gxVerif->cb(3, 0x26U, __gxvDummyStr);
                }
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 13) <= 3 && (u32)Alh[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 13)] != 0) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x27], "A", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 0xDU));
                __gxVerif->cb(3, 0x27U, __gxvDummyStr);
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 10) <= 3 && (u32)Alh[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 10)] != 0) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x27], "B", i, ((__gxVerif->rasRegs[(i * 2) + 0xC1] >> 0xAU) & 7));
                __gxVerif->cb(3, 0x27U, __gxvDummyStr);
            }

            if ((u32)GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 7) <= 3 && (u32)Alh[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 7)] != 0) {
                sprintf(__gxvDummyStr, __gxvWarnings[0x27], "C", i, GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 3, 7U));
                __gxVerif->cb(3, 0x27U, __gxvDummyStr);
            }
        }
        Cwritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 2, 22)] = 1;
        Awritten[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 2, 22)] = 1;
        Clh[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 2, 22)] = (!GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 2, 0) && !GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC0], 1, 19));
        Alh[GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 2, 22)] = (!GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 2, 0) && !GET_REG_FIELD(__gxVerif->rasRegs[(i * 2) + 0xC1], 1, 19));
    }

    for (i = 0; i < 4; i++) {
        if (Cwritten[i] != 0U) {
            __gxVerif->rasRegs[(i * 2) + 0xE1] = (__gxVerif->rasRegs[(i * 2) + 0xE1] & 0xFFFFFF) | 0xFF000000;
        }
        if (Awritten[i] != 0U) {
            __gxVerif->rasRegs[(i * 2) + 0xE0] = (__gxVerif->rasRegs[(i * 2) + 0xE0] & 0xFFFFFF) | 0xFF000000;
        }
    }

    if (GET_REG_FIELD(__gxVerif->rasRegs[0xF5], 2, 2) && __gxVerif->verifyLevel >= 1) {
        if ((u32) ((__gxVerif->rasRegs[0xF4] & 0xFF000000) + 0x01000000) == 0U) {
            __gxVerif->cb(1, 0x28U, __gxvWarnings[0x28]);
        }
        if (!enabled) {
            __gxVerif->cb(1, 0x29U, __gxvWarnings[0x29]);
        }
    }

    if (__gxVerif->verifyLevel >= GX_WARN_MEDIUM) {
        if (GET_REG_FIELD(__gxVerif->rasRegs[((nTev - 1) * 2) + 0xC0], 2, 22)) {
            __gxVerif->cb(2, 0x2AU, __gxvWarnings[0x2A]);
        }
        if (GET_REG_FIELD(__gxVerif->rasRegs[((nTev - 1) * 2) + 0xC1], 2, 22)) {
            __gxVerif->cb(2, 0x2BU, __gxvWarnings[0x2B]);
        }
    }

    if (__gxVerif->verifyLevel >= GX_WARN_ALL) {
        if (!GET_REG_FIELD(__gxVerif->rasRegs[((nTev - 1) * 2) + 0xC1], 2, 0) && !GET_REG_FIELD(__gxVerif->rasRegs[((nTev - 1) * 2) + 0xC0], 1, 19)) {
            __gxVerif->cb(3, 0x2CU, __gxvWarnings[0x2C]);
        }
        if (!GET_REG_FIELD(__gxVerif->rasRegs[((nTev - 1) * 2) + 0xC1], 2, 0) && !GET_REG_FIELD(__gxVerif->rasRegs[((nTev - 1) * 2) + 0xC1], 1, 19)) {
            __gxVerif->cb(3, 0x2DU, __gxvWarnings[0x2D]);
        }
    }

    if (__gxVerif->verifyLevel >= GX_WARN_MEDIUM && GET_REG_FIELD(__gxVerif->rasRegs[0x43], 1, 6) && (GET_REG_FIELD(__gxVerif->rasRegs[0xF3], 2, 22) || ((u32) GET_REG_FIELD(__gxVerif->rasRegs[0xF3], 3, 16) != 7) || ((u32) GET_REG_FIELD(__gxVerif->rasRegs[0xF3], 3, 19) != 7))) {
        __gxVerif->cb(2, 0x2EU, __gxvWarnings[0x2E]);
    }
}
#endif

// UNUSED: the game never calls this; dead-stripped at link.

#endif
