#ifndef GSCOLSYS2_GEOMETRY_H
#define GSCOLSYS2_GEOMETRY_H

#include <GSvec.h>

/* Two-component operations for collision projections. */
static inline void GScolsys2SubtractXY(f32& outX, f32& outY,
                                      const GSvec& lhs, const GSvec& rhs)
{
    outX = lhs.x - rhs.x;
    outY = lhs.y - rhs.y;
}

static inline void GScolsys2SubtractXZ(f32& outX, f32& outZ,
                                      const GSvec& lhs, const GSvec& rhs)
{
    outX = lhs.x - rhs.x;
    outZ = lhs.z - rhs.z;
}

static inline f32 GScolsys2Cross2D(f32 lhsX, f32 lhsY, f32 rhsX, f32 rhsY)
{
    return lhsX * rhsY - lhsY * rhsX;
}

static inline void GScolsys2FillXY(GSvec& out, const f32& value)
{
    out.x = out.y = value;
}

static inline void GScolsys2FillXZ(GSvec& out, const f32& value)
{
    out.x = out.z = value;
}

#endif
