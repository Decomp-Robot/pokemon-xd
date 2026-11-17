#include <fightSeq.h>
#include <status.h>

/*
 * Status accesses of the battle script. A status instruction carries the
 * status kind and the operands of the generic status system: the address of
 * the status object pointer, the address of the data id byte, the field id and
 * the address of the index byte, followed by the address of the value the read
 * or write works on. The handlers forward those operands and step over the
 * whole instruction.
 */
static const u32 STATUS_OPCODE_SIZE = 20;

enum STATUS_OPCODE_OFFSET
{
    // Operand offsets within the status instruction.
    STATUS_OPCODE_KIND = 1,
    STATUS_OPCODE_STATUS = 2,
    STATUS_OPCODE_DATA_ID = 6,
    STATUS_OPCODE_FIELD = 10,
    STATUS_OPCODE_INDEX = 12,
    STATUS_OPCODE_VALUE = 16,
};

extern "C" void WS_STATUS_GET8(void)
{
    u8 *script = fightSeq_GetWazaSeq();
    void **statusAddress = *(void ***)(script + STATUS_OPCODE_STATUS);
    u8 kind = script[STATUS_OPCODE_KIND];
    u8 *dataIdAddress = *(u8 **)(script + STATUS_OPCODE_DATA_ID);
    u16 fieldId = *(u16 *)(script + STATUS_OPCODE_FIELD);
    u8 *indexAddress = *(u8 **)(script + STATUS_OPCODE_INDEX);
    u8 *value = *(u8 **)(script + STATUS_OPCODE_VALUE);
    void *status;
    u16 dataId;
    u16 index;

    if (statusAddress == 0) {
        status = 0;
    } else {
        status = *statusAddress;
    }
    if (dataIdAddress == 0) {
        dataId = 0;
    } else {
        dataId = *dataIdAddress;
    }
    if (indexAddress == 0) {
        index = 0;
    } else {
        index = *indexAddress;
    }
    *value = statusGetStatus(kind, status, dataId, fieldId, index);
    fightSeq_NextWazaSeq(STATUS_OPCODE_SIZE);
}

extern "C" void WS_STATUS_GET16(void)
{
    u8 *script = fightSeq_GetWazaSeq();
    void **statusAddress = *(void ***)(script + STATUS_OPCODE_STATUS);
    u8 kind = script[STATUS_OPCODE_KIND];
    u8 *dataIdAddress = *(u8 **)(script + STATUS_OPCODE_DATA_ID);
    u16 fieldId = *(u16 *)(script + STATUS_OPCODE_FIELD);
    u8 *indexAddress = *(u8 **)(script + STATUS_OPCODE_INDEX);
    u16 *value = *(u16 **)(script + STATUS_OPCODE_VALUE);
    void *status;
    u16 dataId;
    u16 index;

    if (statusAddress == 0) {
        status = 0;
    } else {
        status = *statusAddress;
    }
    if (dataIdAddress == 0) {
        dataId = 0;
    } else {
        dataId = *dataIdAddress;
    }
    if (indexAddress == 0) {
        index = 0;
    } else {
        index = *indexAddress;
    }
    *value = statusGetStatus(kind, status, dataId, fieldId, index);
    fightSeq_NextWazaSeq(STATUS_OPCODE_SIZE);
}

extern "C" void WS_STATUS_GET32(void)
{
    u8 *script = fightSeq_GetWazaSeq();
    void **statusAddress = *(void ***)(script + STATUS_OPCODE_STATUS);
    u8 kind = script[STATUS_OPCODE_KIND];
    u8 *dataIdAddress = *(u8 **)(script + STATUS_OPCODE_DATA_ID);
    u16 fieldId = *(u16 *)(script + STATUS_OPCODE_FIELD);
    u8 *indexAddress = *(u8 **)(script + STATUS_OPCODE_INDEX);
    s32 *value = *(s32 **)(script + STATUS_OPCODE_VALUE);
    void *status;
    u16 dataId;
    u16 index;

    if (statusAddress == 0) {
        status = 0;
    } else {
        status = *statusAddress;
    }
    if (dataIdAddress == 0) {
        dataId = 0;
    } else {
        dataId = *dataIdAddress;
    }
    if (indexAddress == 0) {
        index = 0;
    } else {
        index = *indexAddress;
    }
    *value = statusGetStatus(kind, status, dataId, fieldId, index);
    fightSeq_NextWazaSeq(STATUS_OPCODE_SIZE);
}

extern "C" void WS_STATUS_SET8(void)
{
    u8 *script = fightSeq_GetWazaSeq();
    void **statusAddress = *(void ***)(script + STATUS_OPCODE_STATUS);
    u8 kind = script[STATUS_OPCODE_KIND];
    u8 *dataIdAddress = *(u8 **)(script + STATUS_OPCODE_DATA_ID);
    u16 fieldId = *(u16 *)(script + STATUS_OPCODE_FIELD);
    u8 *indexAddress = *(u8 **)(script + STATUS_OPCODE_INDEX);
    u8 *value = *(u8 **)(script + STATUS_OPCODE_VALUE);
    void *status;
    u16 dataId;
    u16 index;

    if (statusAddress == 0) {
        status = 0;
    } else {
        status = *statusAddress;
    }
    if (dataIdAddress == 0) {
        dataId = 0;
    } else {
        dataId = *dataIdAddress;
    }
    if (indexAddress == 0) {
        index = 0;
    } else {
        index = *indexAddress;
    }
    statusSetStatus(kind, status, dataId, fieldId, index, *value);
    fightSeq_NextWazaSeq(STATUS_OPCODE_SIZE);
}

extern "C" void WS_STATUS_SET16(void)
{
    u8 *script = fightSeq_GetWazaSeq();
    void **statusAddress = *(void ***)(script + STATUS_OPCODE_STATUS);
    u8 kind = script[STATUS_OPCODE_KIND];
    u8 *dataIdAddress = *(u8 **)(script + STATUS_OPCODE_DATA_ID);
    u16 fieldId = *(u16 *)(script + STATUS_OPCODE_FIELD);
    u8 *indexAddress = *(u8 **)(script + STATUS_OPCODE_INDEX);
    u16 *value = *(u16 **)(script + STATUS_OPCODE_VALUE);
    void *status;
    u16 dataId;
    u16 index;

    if (statusAddress == 0) {
        status = 0;
    } else {
        status = *statusAddress;
    }
    if (dataIdAddress == 0) {
        dataId = 0;
    } else {
        dataId = *dataIdAddress;
    }
    if (indexAddress == 0) {
        index = 0;
    } else {
        index = *indexAddress;
    }
    statusSetStatus(kind, status, dataId, fieldId, index, *value);
    fightSeq_NextWazaSeq(STATUS_OPCODE_SIZE);
}

extern "C" void WS_STATUS_SET32(void)
{
    u8 *script = fightSeq_GetWazaSeq();
    void **statusAddress = *(void ***)(script + STATUS_OPCODE_STATUS);
    u8 kind = script[STATUS_OPCODE_KIND];
    u8 *dataIdAddress = *(u8 **)(script + STATUS_OPCODE_DATA_ID);
    u16 fieldId = *(u16 *)(script + STATUS_OPCODE_FIELD);
    u8 *indexAddress = *(u8 **)(script + STATUS_OPCODE_INDEX);
    s32 *value = *(s32 **)(script + STATUS_OPCODE_VALUE);
    void *status;
    u16 dataId;
    u16 index;

    if (statusAddress == 0) {
        status = 0;
    } else {
        status = *statusAddress;
    }
    if (dataIdAddress == 0) {
        dataId = 0;
    } else {
        dataId = *dataIdAddress;
    }
    if (indexAddress == 0) {
        index = 0;
    } else {
        index = *indexAddress;
    }
    statusSetStatus(kind, status, dataId, fieldId, index, *value);
    fightSeq_NextWazaSeq(STATUS_OPCODE_SIZE);
}
