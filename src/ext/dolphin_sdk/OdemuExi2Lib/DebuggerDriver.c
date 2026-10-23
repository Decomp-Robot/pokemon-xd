#include <dolphin.h>

#define REG_MAX 5
#define REG(chan, idx) (__EXIRegs[((chan) * REG_MAX) + (idx)])

#define DBG_MAILBOX_WRITE_OPCODE 0xC0000000
#define DBG_MAILBOX_PAYLOAD_MASK 0x1FFFFFFF
#define DBG_MAILBOX_READ_OPCODE 0x60000000
#define DBG_MEMORY_READ_OPCODE 0x20000000
#define DBG_MEMORY_WRITE_OPCODE 0xA0000000
#define DBG_MEMORY_ADDRESS_MASK 0x01FFFC00
#define DBG_STATUS_READ_OPCODE 0x40000000
#define DBG_STATUS_MAIL_PENDING 0x00000001
#define DBG_STATUS_TRANSMIT_BUSY 0x00000002
#define DBG_MAIL_TAG 0x1F000000
#define DBG_MAIL_LENGTH_MASK 0x00007FFF
#define DBG_RECEIVE_BUFFER_BASE 0x1E000
#define DBG_TRANSMIT_BUFFER_BASE 0x1C000
#define DBG_ALTERNATE_BUFFER_STRIDE 0x1000
#define DBG_MAIL_BUFFER_SELECT 0x00010000
#define PI_DEBUG_ACKNOWLEDGE 0x00001000
#define EXI_INTERRUPT_ENABLE_MASK 0x00000405
#define EXI_TRANSFER_START 0x00000001


// Callbacks and transfer state of the EXI2 debugger transport. The MetroTRK
// nub receives the input flag address through DBInitComm and polls it to see
// whether the debugger raised the PI_DEBUG interrupt.
static void (*MTRCallback)(s32 interrupt, OSContext* context);
static void (*DBGCallback)(s32 interrupt, OSContext* context);
static u32 SendMailData;
static s32 RecvDataLeng;
static u8* pEXIInputFlag;
static u8 EXIInputFlag;

// Send sequence of the debug mail messages; the low bit selects the alternating transfer buffer.
static u8 seqCount = 0x80;

void DBGHandler(s16 interrupt, OSContext* context);
void MWCallback(s32 interrupt, OSContext* context);

static inline BOOL DBGSelect(s32 chan, u32 dev, u32 freq) {
    u32 cpr = REG(chan, 0);
    cpr &= EXI_INTERRUPT_ENABLE_MASK;
    cpr |= (freq << 4) | ((1 << dev) << 7);
    REG(chan, 0) = cpr;
    return TRUE;
}

static inline BOOL DBGSync(s32 chan) {
    while (REG(chan, 3) & EXI_TRANSFER_START) {
    }
    return TRUE;
}

static inline BOOL DBGDeselect(s32 chan) {
    REG(chan, 0) &= EXI_INTERRUPT_ENABLE_MASK;
    return TRUE;
}

BOOL DBGEXIImm(void* buf, s32 len, u32 type) {
    u8* byteBuf = buf;
    u32 data;
    int i;

    if (type != EXI_READ) {
        data = 0;
        for (i = 0; i < len; i++) {
            data |= ((u8*)buf)[i] << ((3 - i) * 8);
        }
        REG(2, 4) = data;
    }
    REG(2, 3) = (type << 2 | EXI_TRANSFER_START) | (len - 1) << 4;
    while (REG(2, 3) & EXI_TRANSFER_START) {
    }
    if (type == EXI_READ) {
        data = REG(2, 4);
        for (i = 0; i < len; i++) {
            *byteBuf++ = data >> ((3 - i) * 8);
        }
    }
    return TRUE;
}

static inline BOOL WriteTransportMail(u32 mailData) {
    BOOL failed;
    u32 word;
    failed = FALSE;
    if (!DBGSelect(2, 0, EXI_FREQ_16M)) {
        return FALSE;
    }
    word = (mailData & DBG_MAILBOX_PAYLOAD_MASK) | DBG_MAILBOX_WRITE_OPCODE;
    failed |= !DBGEXIImm(&word, 4, EXI_WRITE);
    failed |= !DBGSync(2);
    failed |= !DBGDeselect(2);
    return !failed;
}

BOOL DBGReadMailbox(u32* mailbox) {
    BOOL failed = FALSE;
    u32 send;

    if (!DBGSelect(2, 0, EXI_FREQ_16M)) {
        return FALSE;
    }
    send = DBG_MAILBOX_READ_OPCODE;
    failed |= !DBGEXIImm(&send, 2, EXI_WRITE);
    failed |= !DBGSync(2);
    failed |= !DBGEXIImm(mailbox, 4, EXI_READ);
    failed |= !DBGSync(2);
    failed |= !DBGDeselect(2);
    return !failed;
}

BOOL DBGRead(u32 address, void* data, s32 len) {
    BOOL failed;
    u32* dataPtr = data;
    u32 word;
    u32 block;

    DBGSelect(2, 0, EXI_FREQ_16M);
    word = ((address << 8) & DBG_MEMORY_ADDRESS_MASK) | DBG_MEMORY_READ_OPCODE;
    failed = !DBGEXIImm(&word, 4, EXI_WRITE);
    failed |= !DBGSync(2);
    while (len != 0) {
        failed |= !DBGEXIImm(&block, 4, EXI_READ);
        failed |= !DBGSync(2);
        *dataPtr = block;
        dataPtr++;
        len -= 4;
        if (len < 0) {
            len = 0;
        }
    }
    failed |= !DBGDeselect(2);
    return !failed;
}

BOOL DBGWrite(u32 address, void* data, s32 len) {
    BOOL failed;
    u32* dataPtr = data;
    u32 word;

    DBGSelect(2, 0, EXI_FREQ_16M);
    word = ((address << 8) & DBG_MEMORY_ADDRESS_MASK) | DBG_MEMORY_WRITE_OPCODE;
    failed = !DBGEXIImm(&word, 4, EXI_WRITE);
    failed |= !DBGSync(2);
    while (len != 0) {
        u32 block = *dataPtr;

        dataPtr++;
        failed |= !DBGEXIImm(&block, 4, EXI_WRITE);
        failed |= !DBGSync(2);
        len -= 4;
        if (len < 0) {
            len = 0;
        }
    }
    failed |= !DBGDeselect(2);
    return !failed;
}

BOOL DBGReadStatus(u32* status) {
    BOOL failed = FALSE;
    u32 send;

    if (!DBGSelect(2, 0, EXI_FREQ_16M)) {
        return FALSE;
    }
    send = DBG_STATUS_READ_OPCODE;
    failed |= !DBGEXIImm(&send, 2, EXI_WRITE);
    failed |= !DBGSync(2);
    failed |= !DBGEXIImm(status, 4, EXI_READ);
    failed |= !DBGSync(2);
    failed |= !DBGDeselect(2);
    return !failed;
}

void MWCallback(s32 interrupt, OSContext* context) {
    EXIInputFlag = TRUE;
    if (MTRCallback != NULL) {
        MTRCallback(0, context);
    }
}

void DBGHandler(s16 interrupt, OSContext* context) {
    __PIRegs[0] = PI_DEBUG_ACKNOWLEDGE;
    if (DBGCallback != NULL) {
        DBGCallback(interrupt, context);
    }
}

void DBInitComm(u8** inputFlagPtr, void (*mtrCallback)(s32, OSContext*)) {
    BOOL enabled = OSDisableInterrupts();

    pEXIInputFlag = &EXIInputFlag;
    *inputFlagPtr = pEXIInputFlag;
    MTRCallback = mtrCallback;
    __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_2);
    REG(2, 0) = 0;
    OSRestoreInterrupts(enabled);
}

void DBInitInterrupts(void) {
    __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_2);
    __OSMaskInterrupts(OS_INTERRUPTMASK_PI_DEBUG);
    DBGCallback = MWCallback;
    __OSSetInterruptHandler(__OS_INTERRUPT_PI_DEBUG, DBGHandler);
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_DEBUG);
}

static inline void CheckMailBox(void) {
    u32 status;

    DBGReadStatus(&status);
    if (status & DBG_STATUS_MAIL_PENDING) {
        DBGReadMailbox(&status);
        status &= DBG_MAILBOX_PAYLOAD_MASK;
        if ((status & DBG_MAIL_TAG) == DBG_MAIL_TAG) {
            SendMailData = status;
            RecvDataLeng = status & DBG_MAIL_LENGTH_MASK;
            EXIInputFlag = TRUE;
        }
    }
}

s32 DBQueryData(void) {
    BOOL enabled;

    EXIInputFlag = FALSE;
    if (RecvDataLeng == 0) {
        enabled = OSDisableInterrupts();
        CheckMailBox();
        OSRestoreInterrupts(enabled);
    }
    return RecvDataLeng;
}

s32 DBRead(void* data, s32 len) {
    BOOL enabled = OSDisableInterrupts();

    DBGRead(DBG_RECEIVE_BUFFER_BASE + ((SendMailData & DBG_MAIL_BUFFER_SELECT) ? DBG_ALTERNATE_BUFFER_STRIDE : 0), data, (len + 3) & ~3);
    RecvDataLeng = 0;
    EXIInputFlag = FALSE;
    OSRestoreInterrupts(enabled);
    return 0;
}

s32 DBWrite(void* data, s32 len) {
    BOOL enabled;
    u32 status;
    u32 bufferOffset;
    u32 command;

    enabled = OSDisableInterrupts();
    do {
        DBGReadStatus(&status);
    } while (status & DBG_STATUS_TRANSMIT_BUSY);
    seqCount++;
    bufferOffset = (seqCount & 1) ? DBG_ALTERNATE_BUFFER_STRIDE : 0;
    do {
    } while (!DBGWrite(DBG_TRANSMIT_BUFFER_BASE | bufferOffset, data, (len + 3) & ~3));
    do {
        DBGReadStatus(&status);
    } while (status & DBG_STATUS_TRANSMIT_BUSY);
    command = (seqCount << 16) | DBG_MAIL_TAG | len;
    do {
    } while (!WriteTransportMail(command));
    do {
    } while (!DBGReadStatus(&status) || (status & DBG_STATUS_TRANSMIT_BUSY));
    OSRestoreInterrupts(enabled);
    return 0;
}
