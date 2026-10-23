#ifndef MENU_POCKET_TOOL_H
#define MENU_POCKET_TOOL_H

#include <types.h>

// Angle helpers the pocket menus share. The refresh helpers wrap a value
// into a fixed range and the subtraction helpers wrap the difference of two
// values, so callers can normalise a heading before comparing or drawing it.

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
