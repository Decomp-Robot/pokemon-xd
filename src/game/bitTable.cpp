#include <types.h>

// Packed bit table accessors shared by the menu drawing code.
// getBitTable tests the bit at 'index'; the matching setter is setBitTable.
bool getBitTable(unsigned long *table, long index)
{
    return table[index / 32] & (1 << (index % 32));
}
