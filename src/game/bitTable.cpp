#include <types.h>

void setBitTable(unsigned long *table, long index, bool flag)
{
    long pos = index / 32;
    long bit = index % 32;

    // A false flag leaves the table untouched.
    if (flag == 0) {
        return;
    }
    table[pos] |= (1 << bit);
}

bool getBitTable(unsigned long *table, long index)
{
    return table[index / 32] & (1 << (index % 32));
}
