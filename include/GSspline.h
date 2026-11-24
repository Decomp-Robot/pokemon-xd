#ifndef GS_SPLINE_H
#define GS_SPLINE_H

#include <types.h>
#include <GSvec.h>

enum GSsplineInterpolation {
    GSSPLINE_LINEAR = 0,
    GSSPLINE_CUBIC = 1,
    GSSPLINE_CUBIC_ALT = 2,
    GSSPLINE_LAGRANGE = 3
};

enum GSsplineDataType {
    GSSPLINE_FLOAT = 0,
    GSSPLINE_VECTOR = 1
};

typedef struct GSspline {
    enum GSsplineInterpolation interpolation;
    enum GSsplineDataType dataType;
    u8 capacity;
    u8 count;
    u8 reserved;
    u8 timeCount;
    void *values;
    f32 *auxiliary;
    f32 *times;
    f32 startTime;
    f32 endTime;
    void *allocation;
    void *controlAllocation;
} GSspline;

#ifdef __cplusplus
extern "C" {
#endif

GSspline *GSsplineCreate(enum GSsplineInterpolation interpolation, enum GSsplineDataType dataType, u8 count);
void GSsplineAddControlVectorValue(GSspline *spline, struct GSvec *value, f32 time);
void GSsplineFree(GSspline *spline);

#ifdef __cplusplus
}
#endif

#endif
