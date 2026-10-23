#include <dolphin.h>
#include <dolphin/dvd.h>

#include "__dvd.h"

static u8 bb2Buf[63];

static u32 status;
static DVDBB2* bb2;
static DVDDiskID* idTmp;

// prototypes
static void cb(s32 result, DVDCommandBlock* block);

static void cb(s32 result, DVDCommandBlock* block) {
    if (result > 0) {
        switch(status) {
        case 0:
            status = 1;
            DVDReadAbsAsyncForBS(block, bb2, 0x20, 0x420, cb);
            return;
        case 1:
            status = 2;
            DVDReadAbsAsyncForBS(block, bb2->FSTAddress, (bb2->FSTLength + 0x1F) & 0xFFFFFFE0, bb2->FSTPosition, cb);
        default:
            return;
        }
    }

    if (result == -1) {
        return;
    } else if (result == -4) {
        status = 0;
        DVDReset();
        DVDReadDiskID(block, idTmp, cb);
    }
}

static char gameName[0x70] = {
    // "  Game Name ... %c%c%c%c\n"
    0x20, 0x20, 0x47, 0x61, 0x6D, 0x65, 0x20, 0x4E, 0x61, 0x6D, 0x65, 0x20, 0x2E, 0x2E, 0x2E, 0x20,
    0x25, 0x63, 0x25, 0x63, 0x25, 0x63, 0x25, 0x63, 0x0A, 0x00,
    // lbl_803D256A
    0x00, 0x00, 0x20, 0x20, 0x43, 0x6F, 0x6D, 0x70, 0x61, 0x6E, 0x79, 0x20, 0x2E, 0x2E, 0x2E, 0x2E,
    0x2E, 0x20, 0x25, 0x63, 0x25, 0x63, 0x0A, 0x00, 0x00, 0x00, 0x20, 0x20, 0x44, 0x69, 0x73, 0x6B,
    0x20, 0x23, 0x20, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x20, 0x25, 0x64, 0x0A, 0x00, 0x20, 0x20,
    0x47, 0x61, 0x6D, 0x65, 0x20, 0x76, 0x65, 0x72, 0x20, 0x2E, 0x2E, 0x2E, 0x2E, 0x20, 0x25, 0x64,
    0x0A, 0x00, 0x20, 0x20, 0x53, 0x74, 0x72, 0x65, 0x61, 0x6D, 0x69, 0x6E, 0x67, 0x20, 0x2E, 0x2E,
    0x2E, 0x20, 0x25, 0x73, 0x0A, 0x00
};

void __fstLoad(void) {
    OSBootInfo* bootInfo;
    DVDDiskID* id;
    u8 idTmpBuf[63];
    s32 state;
    void* arenaHi;
    static DVDCommandBlock block;

    arenaHi = OSGetArenaHi();
    bootInfo = (void*)OSPhysicalToCached(0);
    idTmp = (void*)OSRoundUp32B(idTmpBuf);
    bb2 = (void*)OSRoundUp32B(bb2Buf);

    DVDReset();
    DVDReadDiskID(&block, idTmp, cb);

    while (1) {
        state = DVDGetDriveStatus();
        if (state == 0) {
            break;
        }

        // weird switch that seemingly wont do anything but break out of its own switch. What was this for? Early DVD development pre-hardware?
        switch(state) {
        case DVD_STATE_FATAL_ERROR: break;
        case DVD_STATE_BUSY: break;
        case DVD_STATE_WAITING: break;
        case DVD_STATE_COVER_CLOSED: break;
        case DVD_STATE_NO_DISK: break;
        case DVD_STATE_COVER_OPEN: break;
        case DVD_STATE_MOTOR_STOPPED: break;
        }

    }

    bootInfo->FSTLocation = (void*)bb2->FSTAddress;
    bootInfo->FSTMaxLength = bb2->FSTMaxLength;
    id = &bootInfo->DVDDiskID;
    memcpy(id, idTmp, 0x20);
    OSReport("\n");
    OSReport(gameName, id->gameName[0], id->gameName[1], id->gameName[2], id->gameName[3]);
    OSReport(gameName + 0x1c, id->company[0], id->company[1]);
    OSReport(gameName + 0x34, id->diskNumber);
    OSReport(gameName + 0x48, id->gameVersion);
    OSReport(gameName + 0x5c, !(id->streaming) ? "OFF" : "ON");
    OSReport("\n");
    OSSetArenaHi(bb2->FSTAddress);
}
