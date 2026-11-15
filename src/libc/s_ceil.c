static const double huge = 1.0e+300;

double ceil(double x)
{
    union {
        double value;
        long word[2];
    } bits;
    int highWord, lowWord;
    int exponent;
    unsigned int fractionMask;
    unsigned int increment, roundedLow;

    bits.value = x;
    highWord = bits.word[0];
    lowWord = bits.word[1];
    exponent = ((highWord >> 20) & 0x7ff) - 1023; /* exponent of x */

    if (exponent < 20) {
        if (exponent < 0) {
            /* |x| < 1.0 */
            if (huge + x > 0.0) {
                if (highWord < 0) {
                    highWord = 0x80000000;
                    lowWord = 0;
                } else if (highWord | lowWord) {
                    highWord = 0x3ff00000;
                    lowWord = 0;
                }
            }
        } else {
            fractionMask = (0x100000 - 1) >> exponent;
            if (((highWord & fractionMask) | lowWord) == 0) {
                return x; /* integral */
            }
            if (huge + x > 0.0) {
                if (highWord > 0) {
                    highWord += 0x100000 >> exponent;
                }
                highWord &= ~fractionMask;
                lowWord = 0;
            }
        }
    } else if (exponent > 51) {
        if (exponent == 1024) {
            return x + x; /* inf or NaN */
        }
        return x; /* integral */
    } else {
        fractionMask = 0xffffffff >> (exponent - 20);
        if ((lowWord & fractionMask) == 0) {
            return x; /* integral */
        }
        if (huge + x > 0.0) {
            if (highWord > 0) {
                if (exponent == 20) {
                    highWord += 1;
                } else {
                    increment = 1 << (52 - exponent);
                    roundedLow = lowWord + increment;
                    if (roundedLow < lowWord) {
                        highWord += 1;
                    }
                    lowWord = roundedLow;
                }
            }
            lowWord &= ~fractionMask;
        }
    }
    bits.word[0] = highWord;
    bits.word[1] = lowWord;
    return bits.value;
}
