// Reverse the byte order of a 32-bit word.
unsigned long _swap32(unsigned long value)
{
    return (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | (value >> 24);
}
