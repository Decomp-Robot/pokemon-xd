#ifndef MENU_POCKET_TOOL_H
#define MENU_POCKET_TOOL_H

#include <PocketVec.h>
#include <types.h>

// Angle helpers the pocket menus share. The refresh helpers wrap a value
// into a fixed range and the subtraction helpers wrap the difference of two
// values, so callers can normalise a heading before comparing or drawing it.

// True when dir lies inside the clockwise region that runs from `from` to
// `to`; a region whose ends are equal covers the whole circle.
bool isDirRegionIn(f32 dir, f32 from, f32 to);

// Angle of the vector (x, y), with the screen-space y axis pointing down.
f32 xy2dir(f32 x, f32 y);

// Direction from center to point.
f32 xyxy2dir(const PocketVec<short> &point, const PocketVec<short> &center);

// Direction from center to point, rounded to the nearest degree.
long xyxy2dirDegree(const PocketVec<short> &point, const PocketVec<short> &center);

// Wrap a degree value into the [-180, 180) range.
long degreeRefresh180(long degree);

// Wrap a degree value into the [0, 360) range.
long degreeRefresh360(long degree);

// Wrap a direction value into the [0, 2*PI) range.
f32 dirRefresh2PI(f32 dir);

// Wrap a direction value into the [-PI, PI) range.
f32 dirRefreshPI(f32 dir);

// Wrap the difference of two degree values into [-180, 180).
long degreeSubHalf(long degree1, long degree2);

// Wrap the difference of two directions into [0, 2*PI).
f32 dirSubHalf(f32 dir1, f32 dir2);

// Wrap the difference of two directions into [-PI, PI).
f32 dirSubPI(f32 dir1, f32 dir2);

#endif
