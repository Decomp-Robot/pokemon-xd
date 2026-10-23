/* Byte-order helpers for the GBA link code.  The GameCube and the Game Boy
 * Advance disagree on word order, so values read from or written to a link
 * packet are passed through these to translate between the two. */

// Reverse the byte order of a 32-bit word.
unsigned long _swap32(unsigned long value)
{
	return (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | (value >> 24);
}

// Reverse the byte order of a 16-bit halfword.
unsigned short _swap16(unsigned short value)
{
	return (value << 8) | (value >> 8);
}
