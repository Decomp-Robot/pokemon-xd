#include <types.h>

/* Waza (move) debug sub menus and the small accessors that go with them. */
/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

/* Debug work block the waza debug menus share (0x38 bytes). */
struct DbgMenuWork {
    u8 pad0[0x16];
    u8 mPokemonSelectDirty;  /* 0x16 */
    u8 mWazaSelectDirty;     /* 0x17 */
    u8 pad18[0x30 - 0x18];
    s32 mDebugRunning;       /* 0x30 */
};

extern u8 lbl_804A1500[];

u8 menuIsOpen(s32 menuID);
void menuClose(s32 menuID);
s32 menuOpenCustom(s32 menuID, ...);
void menuSetPosition(s32 menuID, s32 x, s32 y);
void zz_0125ba4_(void);
void zz_0125bc4_(s32 id);
void GSthreadSwitch(void);

#pragma peephole off

s32 zz_000c0c4_(void) { return 0; }
s32 zz_000c0cc_(void) { return 0; }
s32 zz_000c0d4_(void) { return 0; }
s32 zz_000c0dc_(void) { return 0; }
s32 zz_000c0e4_(void) { return 0; }
s32 zz_000c0ec_(void) { return 0; }


int dbgMenuWazaDebugStop(void)
{
    if (((struct DbgMenuWork *)lbl_804A1500)->mDebugRunning != 0) {
        zz_0125ba4_();
    }
    return 0;
}

int dbgMenuPokemonDebugSelect(void)
{
    struct DbgMenuWork *work;

    if (menuIsOpen(0x26)) {
        menuClose(0x26);
    } else {
        work = (struct DbgMenuWork *)lbl_804A1500;
        work->mPokemonSelectDirty = 0;
        work->mWazaSelectDirty = 0;
        menuOpenCustom(0x26, 0, 0, 0, 1, 0);
        menuSetPosition(0x26, 0xC, 0xA);
    }
    return 0;
}

int dbgMenuWazaDebugSelect(void)
{
    if (menuIsOpen(0x15)) {
        menuClose(0x15);
    } else {
        menuOpenCustom(0x15, 0, 0, 0, 1, 0);
        menuSetPosition(0x15, 0xC, 0xA);
    }
    return 0;
}

int dbgMenuWazaDebugStart(void)
{
    if (menuIsOpen(0x11)) {
        menuClose(0x11);
    }
    if (((struct DbgMenuWork *)lbl_804A1500)->mDebugRunning == 0) {
        zz_0125bc4_(0x3E7);
        GSthreadSwitch();
    }
    return 0;
}

#pragma peephole reset


#pragma force_active reset
