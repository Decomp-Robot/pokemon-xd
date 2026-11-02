/*
 * GSflagStorage.c
 *
 * The flag store. GSflagGet and GSflagSet resolve a flag id to its record
 * and to the level array the record is assigned to;
 * _flagGet and _flagSet read and write the value inside that array using the
 * record's width and bit slot. GSflagClear resets one level's array,
 * GSflagInit registers the three level arrays, and GSflagInitBitPos
 * normalizes every catalog record's width and assigns it its bit position.
 */

#include <types.h>
#include <GSflag.h>
#include <GSlog.h>

u32 _flagGetBitLength(u32 value);
u32 _flagGet(u32 *bits, FlagEntry *entries, s32 flag);
void _flagSet(u32 *bits, FlagEntry *entries, s32 flag, u32 value);
void GSflagInitBitPos(FlagEntry *entries, u32 count, u32 size1, u32 size2, u32 size3);

/* Masks with the low n bits set, indexed by a flag's stored width. The
 * table spans every storable width. */
u32 _flagAND[33] = {
    0x00000000, 0x00000001, 0x00000003, 0x00000007,
    0x0000000F, 0x0000001F, 0x0000003F, 0x0000007F,
    0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF,
    0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF,
    0x0000FFFF, 0x0001FFFF, 0x0003FFFF, 0x0007FFFF,
    0x000FFFFF, 0x001FFFFF, 0x003FFFFF, 0x007FFFFF,
    0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF,
    0x0FFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF,
    0xFFFFFFFF
};

u32 GSflagGet(s32 flag)
{
    u32 level;

    level = flag_data[flag].level;
    return _flagGet(flag_level[level].buffer, flag_data, flag);
}

void GSflagSet(s32 flag, u32 value)
{
    u32 level;

    level = flag_data[flag].level;
    _flagSet(flag_level[level].buffer, flag_data, flag, value);
}

u32 _flagGet(u32 *bits, FlagEntry *entries, s32 flag)
{
    u32 width;
    u32 bitPosition;
    u32 word;
    u32 shift;
    u32 low;
    u32 high;

    if (bits == NULL) {
        GSlogWrite("ERROR[GSflagGet]:Initialization has not finished.\n");
        return 0;
    }
    width = entries[flag].width;
    bitPosition = entries[flag].bitPosition;
    word = bitPosition >> 5;
    shift = bitPosition & 0x1F;
    if (width > 1) {
        low = bits[word];
        high = bits[word + 1];
        low >>= shift;
        high <<= 32 - shift;
        return (high | low) & _flagAND[width];
    } else {
        return (bits[word] >> shift) & 1;
    }
}

void _flagSet(u32 *bits, FlagEntry *entries, s32 flag, u32 value)
{
    u32 width;
    u32 bitLength;
    u32 bitPosition;
    u32 word;
    u32 shift;
    u32 mask;
    u32 shifted;
    u32 high;
    u32 w0;
    u32 w1;
    u32 over;

    if (bits == NULL) {
        GSlogWrite("ERROR[GSflagSet]:Initialization has not finished.\n");
        return;
    }
    width = entries[flag].width;
    bitPosition = entries[flag].bitPosition;
    bitLength = _flagGetBitLength(value);
    if (bitLength > width) {
        GSlogWrite("ERROR[GSflagSet]:Overflow BitValue FlagID=%d Value=%d (%d BitLength=%d)>(Limit=%dBit).\n",
                   flag, value, value, bitLength, width);
        value &= _flagAND[width];
    }
    word = bitPosition >> 5;
    shift = bitPosition & 0x1F;
    if (width > 1) {
        shifted = value << shift;
        mask = _flagAND[width] << shift;
        w0 = bits[word];
        w0 &= ~mask;
        bits[word] = shifted | w0;
        if (width + shift >= 32) {
            over = width + shift - 32;
            high = value >> (width - over);
            w1 = bits[word + 1];
            w1 &= ~_flagAND[over];
            bits[word + 1] = high | w1;
        }
    } else if (value == 0) {
        bits[word] &= ~(1 << shift);
    } else {
        bits[word] |= 1 << shift;
    }
}

u32 _flagGetBitLength(u32 value)
{
    return 32 - __cntlzw(value);
}

void GSflagClear(s32 level)
{
    u32 *buffer;
    u32 size;
    u32 i;

    buffer = flag_level[level].buffer;
    if (buffer == NULL) {
        GSlogWrite("ERROR[GSflagClear]:Initialization has not finished.\n");
        return;
    }
    size = flag_level[level].size;
    for (i = 0; i < size; i++) {
        buffer[i] = 0;
    }
}

void GSflagInit(u32 *buffer1, u32 size1, u32 *buffer2, u32 size2, u32 *buffer3, u32 size3)
{
    GSflagInitBitPos(flag_data, flag_data_number->count, size1, size2, size3);
    flag_level[1].buffer = buffer1;
    flag_level[2].buffer = buffer2;
    flag_level[3].buffer = buffer3;
    flag_level[1].size = size1;
    flag_level[2].size = size2;
    flag_level[3].size = size3;
    GSflagClear(1);
    GSflagClear(2);
    GSflagClear(3);
}

void GSflagInitBitPos(FlagEntry *entries, u32 count, u32 size1, u32 size2, u32 size3)
{
    FlagEntry *entry = entries;
    u16 bitPos1 = 0;
    u16 bitPos2 = 0;
    u16 bitPos3 = 0;
    u32 i;

    for (i = 0; i < count; entry++, i++) {
        u32 width = entry->width;

        if (width > 32) {
            GSlogWrite("ERROR[GSflagInit]:Overflow BitLength FlagID=%d (Length=%d)>(Limit=%dBit).\n", i, width, 32);
            entry->width = 32;
        } else if (width == 0) {
            GSlogWrite("ERROR[GSflagInit]:Zero BitLength FlagID=%d (Length=%d)>(Limit=%dBit).\n", i, width, 32);
            entry->width = 1;
        }
        switch (entry->level) {
        case 1:
            entry->bitPosition = bitPos1;
            bitPos1 += entry->width;
            break;
        case 2:
            entry->bitPosition = bitPos2;
            bitPos2 += entry->width;
            break;
        case 3:
            entry->bitPosition = bitPos3;
            bitPos3 += entry->width;
            break;
        }
    }
    if (size1 <= (bitPos1 + 31) / 32) {
        GSlogWrite("ERROR[GSflagInit]:Overflow FlagLevel0 (Use Bytes=%d)>=(Limit Bytes=%d).\n", (bitPos1 + 31) / 32, size1);
    }
    if (size2 <= (bitPos2 + 31) / 32) {
        GSlogWrite("ERROR[GSflagInit]:Overflow FlagLevel1 (Use Bytes=%d)>=(Limit Bytes=%d).\n", (bitPos2 + 31) / 32, size2);
    }
    if (size3 <= (bitPos3 + 31) / 32) {
        GSlogWrite("ERROR[GSflagInit]:Overflow FlagLevel2 (Use Bytes=%d)>=(Limit Bytes=%d).\n", (bitPos3 + 31) / 32, size3);
    }
}
