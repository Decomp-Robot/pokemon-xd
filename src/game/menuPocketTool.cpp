#include <types.h>

// Angle constants the direction helpers share.
extern const f32 zero;
extern const f32 twoPi;
extern const f32 invTwoPi;

// Wrap a degree value into the [-180, 180) range.
long degreeRefresh180(long degree)
{
    long result = degree % 360;

    if (result >= 180) {
        result -= 360;
    } else if (result <= -180) {
        result += 360;
    }

    return result;
}

// Wrap a degree value into the [0, 360) range.
long degreeRefresh360(long degree)
{
    long result = degree % 360;

    if (result < 0) {
        result += 360;
    }

    return result;
}

// Wrap a direction value into the [0, 2*PI) range.
float dirRefresh2PI(float dir)
{
    float result = dir - (float)(long)(invTwoPi * dir) * twoPi;

    if (result < zero) {
        result += twoPi;
    }

    return result;
}
