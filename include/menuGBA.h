#ifndef MENUGBA_H
#define MENUGBA_H
#include <types.h>

class Hero;

// Cartridge identifiers reported by the GBA link program.
enum GBA_ROM_VERSION {
    GBA_ROM_VERSION_UNKNOWN = 0,
    GBA_ROM_VERSION_RUBY = 1,
    GBA_ROM_VERSION_SAPPHIRE = 2,
    GBA_ROM_VERSION_FIRE_RED = 3,
    GBA_ROM_VERSION_LEAF_GREEN = 4,
    GBA_ROM_VERSION_EMERALD = 5
};

enum { GBA_ROM_ENTRY_ALL = 0xFFFF };

extern "C" u16 menuGBACGetRomVer(u16 entry);
extern "C" long openMenuGBA(long menu, long mode, Hero** heroes);
extern "C" void menuGBACSetRomVer(unsigned short version, unsigned short entry);
// Link progress of one controller port while menuGBAC talks to the GBAs.
enum _GBA_PHASE {
    GBA_PHASE_NONE = 0,         // port takes no part in the transfer.
    GBA_PHASE_WAIT = 1,         // waiting for its turn.
    GBA_PHASE_CONNECT = 2,      // looking for the GBA on the port.
    GBA_PHASE_UPLOAD = 3,       // sending the multiboot program.
    GBA_PHASE_TRANSFER = 4,     // reading the party from the GBA.
    GBA_PHASE_COMPLETE = 5,     // party received.
    GBA_PHASE_ERROR = 6,        // the transfer failed.
    GBA_PHASE_DISCONNECTED = 7, // the GBA was unplugged after completing.
    GBA_PHASE_SAVE_LOAD = 8,    // the GameCube save is used for this port.
    GBA_PHASE_SAVE_COMPLETE = 10,
    GBA_PHASE_SAVE_ERROR = 11
};

enum { GBA_CONNECT_RESULT_DISCONNECTED = 8 };

// What the connect menu shows in each controller-port slot.
enum {
    GBA_SLOT_NONE = 0,     // no slot drawn.
    GBA_SLOT_SAVE = 1,     // port 1 reads the GameCube save, no GBA.
    GBA_SLOT_GBA = 2,      // a GBA is connected to this port.
    GBA_SLOT_GBA_HOST = 3, // port 1 GBA while it is being linked.
    GBA_SLOT_SAVE_GBA = 4  // port 1 reads the save, a GBA may be linked later.
};

struct _GBA_Connect_WORK {
    /* 0x00 */ _GBA_PHASE mPhase[4];
    /* 0x10 */ _GBA_PHASE mPhaseBackup[4];
    /* 0x20 */ u8 _20;
    /* 0x21 */ s8 mPort;
    /* 0x22 */ u8 _22[2];
    /* 0x24 */ u32 mFlags;
    /* 0x28 */ u32 mResult;
    /* 0x2C */ s32 mType;
    /* 0x30 */ s8 mSlot[4];
    /* 0x34 */ s16 mIconX[4];
    /* 0x3C */ s16 mIconY[4];
    /* 0x44 */ s8 mShakeCount[4];
    /* 0x48 */ u8 mIconPosReset[4];
};

extern _GBA_Connect_WORK gbaConectGBA_Connect_WORK;


#endif
