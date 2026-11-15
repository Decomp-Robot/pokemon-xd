static const double two54 = 1.80143985094819840000e+16; /* 2^54 */

double frexp(double x, int *eptr)
{
    int highWord, magnitudeHigh, lowWord;

    highWord = *(int *)&x;
    magnitudeHigh = highWord & 0x7fffffff;
    lowWord = *((int *)&x + 1);
    *eptr = 0;
    if (magnitudeHigh >= 0x7ff00000 || ((magnitudeHigh | lowWord) == 0)) {
        return x; /* zero, infinity or NaN */
    }
    if (magnitudeHigh < 0x00100000) { /* subnormal */
        x *= two54;
        *eptr = -54;
        highWord = *(int *)&x;
        magnitudeHigh = highWord & 0x7fffffff;
    }
    *eptr += (magnitudeHigh >> 20) - 1022;
    highWord = (highWord & 0x800fffff) | 0x3fe00000;
    *(int *)&x = highWord;
    return x;
}
