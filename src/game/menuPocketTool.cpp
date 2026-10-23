#include <menuPocketTool.h>

// Angle constants the direction helpers share.
extern const f32 zero;
extern const f32 twoPi;
extern const f32 invTwoPi;
extern const f32 pi;
extern const f32 half;
extern const f32 degHalfTurn;
extern const f32 lbl_804EC094;

// MSL routine that returns the angle of the vector (x, y).
extern "C" double atan2(double y, double x);

// True when dir lies inside the clockwise region that runs from `from` to
// `to`; a region whose ends are equal covers the whole circle.
bool isDirRegionIn(float dir, float from, float to)
{
    float range = (to == from) ? twoPi : dirSubHalf(to, from);

    return dirSubHalf(dir, from) <= range;
}

// Angle of the vector (x, y), with the screen-space y axis pointing down.
float xy2dir(float x, float y)
{
    return atan2(x, -y);
}

// Angle of the vector from center to point.
float xyxy2dir(const PocketVec<short> &point, const PocketVec<short> &center)
{
    float dx = point.x - center.x;
    float dy = -(point.y - center.y);

    return atan2(dx, dy);
}

// Angle of the vector from center to point, rounded to the nearest degree.
long xyxy2dirDegree(const PocketVec<short> &point, const PocketVec<short> &center)
{
    float dir = xyxy2dir(point, center);

    return degreeRefresh180((long)(degHalfTurn * dir / pi + half));
}

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

    if (result > pi) {
        result -= twoPi;
    } else if (result < lbl_804EC094) {
        result += twoPi;
    }

    return result;
}
