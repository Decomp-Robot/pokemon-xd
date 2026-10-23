#include <menuPocketTool.h>

// Angle constants the direction helpers share.
extern const f32 zero;
extern const f32 twoPi;
extern const f32 invTwoPi;
extern const f32 lbl_804EC040;
extern const f32 lbl_804EC094;

// Subtract two directions and wrap the difference into [0, 2*PI).
float dirSubHalf(float dir1, float dir2)
{
    return dirRefresh2PI(dir1 - dir2);
}

// Subtract two directions and wrap the difference into [-PI, PI).
float dirSubPI(float dir1, float dir2)
{
    return dirRefreshPI(dir1 - dir2);
}

// Subtract two degree values and wrap the difference into [-180, 180).
long degreeSubHalf(long degree1, long degree2)
{
    return degreeRefresh180(degree1 - degree2);
}

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

// Wrap a direction value into the [-PI, PI) range.
float dirRefreshPI(float dir)
{
    float result = dir - (float)(long)(invTwoPi * dir) * twoPi;

    if (result > lbl_804EC040) {
        result -= twoPi;
    } else if (result < lbl_804EC094) {
        result += twoPi;
    }

    return result;
}
