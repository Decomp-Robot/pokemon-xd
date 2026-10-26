#include <dolphin/card.h>
#include <dolphin/exi.h>

#include "os/__os.h"
#include "__card.h"

static u32 SectorSizeTable[8] = {
    8 * 1024, 16 * 1024, 32 * 1024, 64 * 1024, 128 * 1024, 256 * 1024, 0, 0,
};

static u32 LatencyTable[8] = {
    4, 8, 16, 32, 64, 128, 256, 512,
};

#define CARD_UNLOCKED_STATUS 0x40

#define CARD_MOUNT_STEP_IDENTIFY 0
#define CARD_MOUNT_STEP_UNLOCKED 1
#define CARD_MOUNT_STEP_READ_BASE 2

#define CARD_ID_SECTOR_SIZE_INDEX_MASK 0x00003800
#define CARD_ID_SECTOR_SIZE_INDEX_SHIFT 11
#define CARD_ID_LATENCY_INDEX_MASK 0x00000700
#define CARD_ID_LATENCY_INDEX_SHIFT 8
#define CARD_ID_SIZE_MASK 0x000000FC
#define CARD_ID_DEVICE_TYPE_MASK 0xFFFF0000
#define CARD_VENDOR_ID_UNSET 0xFFFF
#define CARD_VENDOR_ID_FAST_MODE 236
#define CARD_ID_NET_CARD 0x80000004
#define CARD_ACCESS_DISABLED 0x80
#define CARD_ID_INVALID_LOW_BITS_MASK 3

// prototypes
static s32 DoMount(s32 chan);
static void DoUnmount(s32 chan, s32 result);

static BOOL IsCard(u32 id) {
    u32 size;
    s32 sectorSize;
    if (id & CARD_ID_DEVICE_TYPE_MASK && (id != CARD_ID_NET_CARD || __CARDVendorID == CARD_VENDOR_ID_UNSET)) {
        return FALSE;
    }

    if ((id & CARD_ID_INVALID_LOW_BITS_MASK) != 0) {
        return FALSE;
    }

    size = id & CARD_ID_SIZE_MASK;
    switch (size) {
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
        break;
    default:
        return FALSE;
        break;
    }

    sectorSize = SectorSizeTable[(id & CARD_ID_SECTOR_SIZE_INDEX_MASK) >> CARD_ID_SECTOR_SIZE_INDEX_SHIFT];
    if (sectorSize == 0) {
        return FALSE;
    }

    if ((size * 1024 * 1024 / 8) / sectorSize < 8) {
        return FALSE;
    }

    return TRUE;
}

s32 CARDProbeEx(s32 chan, s32* memSize, s32* sectorSize) {
    u32 id;
    CARDControl* card;
    BOOL enabled;
    s32 result;
    int probe;

    if (chan < 0 || 2 <= chan)
        return CARD_RESULT_FATAL_ERROR;

    if (__gUnknown800030E3 & CARD_ACCESS_DISABLED) {
        return CARD_RESULT_NOCARD;
    }

    card = &__CARDBlock[chan];
    enabled = OSDisableInterrupts();

    probe = EXIProbeEx(chan);
    if (probe == -1)
        result = CARD_RESULT_NOCARD;
    else if (probe == 0)
        result = CARD_RESULT_BUSY;
    else if (card->attached) {
        if (card->mountStep < CARD_MOUNT_STEP_UNLOCKED)
            result = CARD_RESULT_BUSY;
        else {
            if (memSize)
                *memSize = card->size;

            if (sectorSize)
                *sectorSize = card->sectorSize;

            result = CARD_RESULT_READY;
        }
    }
    else if ((EXIGetState(chan) & EXI_STATE_ATTACHED))
        result = CARD_RESULT_WRONGDEVICE;
    else if (!EXIGetID(chan, 0, &id))
        result = CARD_RESULT_BUSY;
    else if (IsCard(id)) {
        if (memSize)
            *memSize = (s32)(id & CARD_ID_SIZE_MASK);
        
        if (sectorSize)
            *sectorSize = SectorSizeTable[(id & CARD_ID_SECTOR_SIZE_INDEX_MASK) >> CARD_ID_SECTOR_SIZE_INDEX_SHIFT];
        result = CARD_RESULT_READY;
    } else {
        result = CARD_RESULT_WRONGDEVICE;
    }

    OSRestoreInterrupts(enabled);
    return result;
}

static s32 DoMount(s32 chan) {
    CARDControl* card;
    u32 id;
    u8 status;
    s32 result;
    OSSramEx* sram;
    int i;
    u8 checkSum;
    int step;

    ASSERTLINE(399, 0 <= chan && chan < 2);

    card = &__CARDBlock[chan];
    if (card->mountStep == CARD_MOUNT_STEP_IDENTIFY) {
        if (EXIGetID(chan, 0, &id) == 0) {
            result = CARD_RESULT_NOCARD;
        } else if (IsCard(id)) {
            result = CARD_RESULT_READY;
        } else {
            result = CARD_RESULT_WRONGDEVICE;
        }

        if (result < 0)
            goto error;

        card->cid = id;
        card->size = (u16)(id & CARD_ID_SIZE_MASK);
        ASSERTLINE(424, card->size);
        
        card->sectorSize = SectorSizeTable[(id & CARD_ID_SECTOR_SIZE_INDEX_MASK) >> CARD_ID_SECTOR_SIZE_INDEX_SHIFT];
        ASSERTLINE(426, card->sectorSize);

        card->cBlock = (u16)((card->size * 1024 * 1024 / 8) / card->sectorSize);
        ASSERTLINE(428, 8 <= card->cBlock);

        card->latency = LatencyTable[(id & CARD_ID_LATENCY_INDEX_MASK) >> CARD_ID_LATENCY_INDEX_SHIFT];

        result = __CARDReadVendorID(chan, &card->vendorID);
        if (result < 0)
            goto error;

        if (CARDGetFastMode() && (card->vendorID >> 8) == CARD_VENDOR_ID_FAST_MODE) {
            card->pageSize = 512;
        } else {
            card->pageSize = 128;
        }

        result = __CARDClearStatus(chan);
        if (result < 0)
            goto error;

        result = __CARDReadStatus(chan, &status);
        if (result < 0)
            goto error;

        if (!EXIProbe(chan)) {
            result = CARD_RESULT_NOCARD;
            goto error;
        }

        if (!(status & CARD_UNLOCKED_STATUS)) {
            result = __CARDUnlock(chan, card->id);
            if (result < 0)
                goto error;

            checkSum = 0;
            sram = __OSLockSramEx();
            for (i = 0; i < 12; i++) {
                sram->flashID[chan][i] = card->id[i];
                checkSum += card->id[i];
            }
            sram->flashIDCheckSum[chan] = (u8)~checkSum;
            __OSUnlockSramEx(TRUE);

            return result;
        } else {
            card->mountStep = CARD_MOUNT_STEP_UNLOCKED;

            checkSum = 0;
            sram = __OSLockSramEx();
            for (i = 0; i < 12; i++)
                checkSum += sram->flashID[chan][i];

            __OSUnlockSramEx(FALSE);
            if (sram->flashIDCheckSum[chan] != (u8)~checkSum) {
                result = CARD_RESULT_IOERROR;
                goto error;
            }
        }
    }

    if (card->mountStep == CARD_MOUNT_STEP_UNLOCKED) {
        if (card->cid == CARD_ID_NET_CARD) {
            u16 vendorID;

            sram = __OSLockSramEx();
            vendorID = *(u16*)sram->flashID[chan];
            __OSUnlockSramEx(FALSE);

            if (__CARDVendorID == CARD_VENDOR_ID_UNSET || vendorID != __CARDVendorID) {
                result = CARD_RESULT_WRONGDEVICE;
                goto error;
            }
        }

        card->mountStep = CARD_MOUNT_STEP_READ_BASE;

        result = __CARDEnableInterrupt(chan, TRUE);
        if (result < 0)
            goto error;

        EXISetExiCallback(chan, __CARDExiHandler);
        EXIUnlock(chan);
        DCInvalidateRange(card->workArea, CARD_WORKAREA_SIZE);
    }

    step = card->mountStep - CARD_MOUNT_STEP_READ_BASE;
    result = __CARDRead(chan, (u32)card->sectorSize * step, CARD_SYSTEM_BLOCK_SIZE,
                        (u8 *)card->workArea + (CARD_SYSTEM_BLOCK_SIZE * step), __CARDMountCallback);
    if (result < 0)
        __CARDPutControlBlock(card, result);
    return result;

error:
    EXIUnlock(chan);
    DoUnmount(chan, result);
    return result;
}

void __CARDMountCallback(s32 chan, s32 result) {
    CARDControl* card;
    CARDCallback callback;

    ASSERTLINE(570, 0 <= chan && chan < 2);

    card = &__CARDBlock[chan];
    switch (result) {
    case CARD_RESULT_READY:
        if (++card->mountStep < CARD_MAX_MOUNT_STEP) {
            result = DoMount(chan);
            if (0 <= result)
                return;
        } else
            result = __CARDVerify(card);
        break;
    case CARD_RESULT_UNLOCKED:
        card->unlockCallback = __CARDMountCallback;
        if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
            return;
        }
        card->unlockCallback = 0;

        result = DoMount(chan);
        if (result >= 0)
            return;
        break;
    case CARD_RESULT_IOERROR:
    case CARD_RESULT_NOCARD:
        DoUnmount(chan, result);
        break;
    }

    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    ASSERTLINE(620, callback);
    callback(chan, result);
}

s32 CARDMountAsync(s32 chan, void* workArea, CARDCallback detachCallback, CARDCallback attachCallback) {
    CARDControl* card;
    BOOL enabled;

    ASSERTLINE(652, workArea && ((u32) workArea % 32 == 0));
    ASSERTLINE(653, 0 <= chan && chan < 2);

    if (chan < 0 || 2 <= chan)
        return CARD_RESULT_FATAL_ERROR;

    if (__gUnknown800030E3 & CARD_ACCESS_DISABLED) {
        return CARD_RESULT_NOCARD;
    }

    card = &__CARDBlock[chan];

    enabled = OSDisableInterrupts();
    if (card->result == CARD_RESULT_BUSY) {
        OSRestoreInterrupts(enabled);
        return CARD_RESULT_BUSY;
    }

    if (!card->attached && (EXIGetState(chan) & EXI_STATE_ATTACHED)) {
        OSRestoreInterrupts(enabled);
        return CARD_RESULT_WRONGDEVICE;
    }

    card->result = CARD_RESULT_BUSY;
    card->workArea = workArea;
    card->extCallback = detachCallback;
    card->apiCallback = attachCallback ? attachCallback : __CARDDefaultApiCallback;
    card->exiCallback = 0;

    if (!card->attached && !EXIAttach(chan, __CARDExtHandler)) {
        card->result = CARD_RESULT_NOCARD;
        OSRestoreInterrupts(enabled);
        return CARD_RESULT_NOCARD;
    }

    card->mountStep = CARD_MOUNT_STEP_IDENTIFY;
    card->attached = TRUE;
    EXISetExiCallback(chan, 0);
    OSCancelAlarm(&card->alarm);

    card->currentDir = 0;
    card->currentFat = 0;

    OSRestoreInterrupts(enabled);

    card->unlockCallback = __CARDMountCallback;
    if (!EXILock(chan, 0, __CARDUnlockedHandler))
        return CARD_RESULT_READY;

    card->unlockCallback = 0;
    return DoMount(chan);
}

static void DoUnmount(s32 chan, s32 result) {
    CARDControl* card;
    BOOL enabled;

    ASSERTLINE(758, 0 <= chan && chan < 2);

    card = &__CARDBlock[chan];
    enabled = OSDisableInterrupts();
    if (card->attached) {
        EXISetExiCallback(chan, 0);
        EXIDetach(chan);
        OSCancelAlarm(&card->alarm);
        card->attached = FALSE;
        card->result = result;
        card->mountStep = CARD_MOUNT_STEP_IDENTIFY;
    }
    OSRestoreInterrupts(enabled);
}

s32 CARDUnmount(s32 chan) {
    CARDControl* card;
    s32 result;

    ASSERTLINE(793, 0 <= chan && chan < 2);

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0)
        return result;

    DoUnmount(chan, CARD_RESULT_NOCARD);
    return CARD_RESULT_READY;
}
