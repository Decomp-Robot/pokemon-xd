#include <GScolsys2Internal.h>
#include <GScolsys2Geometry.h>

/* Intersect the line through start (running toward end) with the plane
 * through org with the given normal. The crossing point goes to out, the
 * parameter along the line to t. Returns 0 when the line is parallel to the
 * plane, leaving the outputs untouched. */
extern "C" bool GScolsys2UtilGetCpPlaneLine(GSvec* out, f32* t, GSvec* normal,
                                          GSvec* org, GSvec* start, GSvec* end)
{
    GSvec dir;
    f32 denom;
    f32 param;

    PSVECSubtract((const Vec *)end, (const Vec *)start, (Vec *)&dir);
    if (0.0f == (denom = normal->x * dir.x + normal->y * dir.y + normal->z * dir.z)) {
        return 0;
    }
    param = (normal->x * (org->x - start->x) + normal->y * (org->y - start->y) + normal->z * (org->z - start->z)) / denom;
    PSVECScale((const Vec *)&dir, (Vec *)&dir, param);
    PSVECAdd((const Vec *)&dir, (const Vec *)start, (Vec *)out);
    *t = param;
    return 1;
}

/* Closest point on the segment a->b to p, written to out, with the
 * segment parameter returned. A degenerate segment collapses onto a. */
extern "C" f32 GScolsys2UtilGetCpLinePoint(GSvec* out, GSvec* a, GSvec* b,
                                           GSvec* p)
{
    GSvec direction;
    f32 lengthSquared;
    f32 parameter;
    f32 xSquared;
    f32 ySquared;
    f32 zSquared;

    PSVECSubtract((const Vec *)b, (const Vec *)a, (Vec *)&direction);
    xSquared = direction.x * direction.x;
    ySquared = direction.y * direction.y;
    zSquared = direction.z * direction.z;
    lengthSquared = zSquared + (xSquared + ySquared);
    if (0.0f == lengthSquared) {
        *out = *a;
        return 0.0f;
    } else {
        parameter = (direction.x * (p->x - a->x) + direction.y * (p->y - a->y) + direction.z * (p->z - a->z)) / lengthSquared;
        PSVECScale((const Vec *)&direction, (Vec *)&direction, parameter);
        PSVECAdd((const Vec *)&direction, (const Vec *)a, (Vec *)out);
        return parameter;
    }
}

static inline f32 projectedTriangleEdgeSide(const GSvec& point, const GSvec& vertex,
                                          const GSvec& nextVertex)
{
    f32 edgeDeltaX;
    f32 edgeDeltaY;
    f32 pointDeltaX;
    f32 pointDeltaY;

    GScolsys2SubtractXY(edgeDeltaX, edgeDeltaY, nextVertex, vertex);
    GScolsys2SubtractXY(pointDeltaX, pointDeltaY, point, vertex);
    return GScolsys2Cross2D(edgeDeltaX, edgeDeltaY, pointDeltaX, pointDeltaY);
}

/* Walk the ray from start toward end out to the given length, writing the
 * extended point to out. A zero-length segment leaves out untouched. */
extern "C" void GScolsy2UtilGetPointExtentionLine(GSvec* out, GSvec* start,
                                                  f32 len, GSvec* end)
{
    GSvec dir;
    f32 mag;

    PSVECSubtract((const Vec *)end, (const Vec *)start, (Vec *)&dir);
    mag = PSVECMag((const Vec *)&dir);
    if (0.0f != mag) {
        PSVECScale((const Vec *)&dir, (Vec *)&dir, len / mag);
        PSVECAdd((const Vec *)&dir, (const Vec *)start, (Vec *)out);
    }
}

/* Point-in-triangle test for a triangle in the plane described by normal.
 *
 * The triangle is projected onto the two axes with the *smallest* normal
 * magnitude, which is the projection that keeps the triangle largest on
 * screen and so least likely to be mis-tested at a shallow angle. The point
 * must fall inside the projected bounding box and on the inner side of all
 * three edges.
 */
extern "C" u8 GScolsy2UtilChkInTri(f32* p, f32* tri, GSvec* normal)
{
    f32 normalAbsZ;
    f32 normalAbsX;
    f32 normalAbsY;
    f32 normalX;
    f32 normalY;
    f32 normalZ;
    f32 projectionFacing;
    GSvec minimum;
    GSvec maximum;
    GSvec point;
    GSvec projected[3];
    s32 axisA;
    s32 axisB;
    s32 edge;

    projectionFacing = 0.0f;
    normalX = normal->x;
    if (normalX > 0.0f) {
        normalAbsX = normalX;
    } else {
        normalAbsX = -normalX;
    }
    normalY = normal->y;
    if (normalY > 0.0f) {
        normalAbsY = normalY;
    } else {
        normalAbsY = -normalY;
    }
    normalZ = normal->z;
    if (normalZ > 0.0f) {
        normalAbsZ = normalZ;
    } else {
        normalAbsZ = -normalZ;
    }
    if (normalAbsX < normalAbsY) {
        if (normalAbsY < normalAbsZ) {
            projectionFacing = normalZ;
            axisA = 0;
            axisB = 1;
        } else {
            projectionFacing = normalY;
            axisA = 2;
            axisB = 0;
        }
    } else {
        if (normalAbsX > normalAbsZ) {
            projectionFacing = -normalX;
            axisA = 2;
            axisB = 1;
        } else {
            projectionFacing = normalZ;
            axisA = 0;
            axisB = 1;
        }
    }
    if (projectionFacing < 0.0f) {
        projected[0].x = tri[axisA];
        projected[0].y = tri[axisB];
        projected[1].x = tri[axisA + 3];
        projected[1].y = tri[axisB + 3];
        projected[2].x = tri[axisA + 6];
        projected[2].y = tri[axisB + 6];
    } else {
        projected[0].x = tri[axisA + 6];
        projected[0].y = tri[axisB + 6];
        projected[1].x = tri[axisA + 3];
        projected[1].y = tri[axisB + 3];
        projected[2].x = tri[axisA];
        projected[2].y = tri[axisB];
    }
    point.x = p[axisA];
    point.y = p[axisB];
    const f32 initialMinimum = 1000000.0f;
    const f32 initialMaximum = -1000000.0f;
    GScolsys2FillXY(minimum, initialMinimum);
    GScolsys2FillXY(maximum, initialMaximum);
    #define CHKBOUNDS(v) \
    if (minimum.x > (v).x) { minimum.x = (v).x; } \
    if (minimum.y > (v).y) { minimum.y = (v).y; } \
    if (maximum.x < (v).x) { maximum.x = (v).x; } \
    if (maximum.y < (v).y) { maximum.y = (v).y; }

    CHKBOUNDS(projected[0])
    CHKBOUNDS(projected[1])
    CHKBOUNDS(projected[2])
#undef CHKBOUNDS
    if (minimum.x > point.x || minimum.y > point.y || maximum.x < point.x || maximum.y < point.y) {
        return 0;
    }
    for (edge = 0; edge < 3; edge++) {
        s32 nextEdge = edge + 1;

        if (nextEdge >= 3) {
            nextEdge = 0;
        }
        f32 cross = projectedTriangleEdgeSide(point, projected[edge], projected[nextEdge]);

        if (cross > 0.0f) {
            return 0;
        }
    }
    return 1;
}

/* Project pos onto the plane through tri with the given normal. */
extern "C" void GScolsy2UtilGetCpPlanePoint(GSvec* cp, GSvec* normal, GSvec* tri,
                                            GSvec* pos)
{
    f32 t = normal->x * (tri->x - pos->x) + normal->y * (tri->y - pos->y) + normal->z * (tri->z - pos->z);
    f32 len2 = normal->x * normal->x + normal->y * normal->y + normal->z * normal->z;
    PSVECScale((const Vec *)normal, (Vec *)cp, t / len2);
    PSVECAdd((const Vec *)cp, (const Vec *)pos, (Vec *)cp);
}
