#include <types.h>
#include <GSmem.h>
#include <GSlog.h>
#include <GSvec.h>

/* Interpolation modes accepted by GSsplineCreate. */
enum {
    GSSPLINE_LINEAR = 0,
    GSSPLINE_CUBIC = 1,
    GSSPLINE_CUBIC_ALT = 2,
    GSSPLINE_LAGRANGE = 3
};

/* Value types stored by a spline. */
enum {
    GSSPLINE_FLOAT = 0,
    GSSPLINE_VECTOR = 1
};

typedef struct GSspline {
    s32 interpolation;
    s32 dataType;
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

/* Diagnostics reported by the GSspline module. */
static const char splineVectorNotInitialised[] = "GSsplineGetVectorValue: spline parameters not initialised!";
static const char splineVectorBadType[] = "GSsplineGetVectorValue: wrong data type!";
static const char splineFloatNotInitialised[] = "GSsplineGetFloatValue: spline parameters not initialised!";
static const char splineFloatBadType[] = "GSsplineGetFloatValue: wrong data type!";
static const char controlVectorTimeWentBack[] = "GSsplineAddControlVectorValue: Warning! We are going back in Time!";
static const char controlVectorFull[] = "GSsplineAddControlVectorValue: cannot add control point!";
static const char controlVectorBadType[] = "GSsplineAddControlVectorValue: wrong data type!";
static const char controlFloatTimeWentBack[] = "GSsplineAddControlFloatValue: Warning! We are going back in Time!";
static const char controlFloatFull[] = "GSsplineAddControlFloatValue: cannot add control point!";
static const char controlFloatBadType[] = "GSsplineAddControlFloatValue: wrong data type!";
static const char initialiseBadPointCount[] = "GSsplineInitialise: invalid number of control points!";
static const char createTooFewPoints[] = "GSsplineCreate: too few control points; minimum is 2!";
static const char createForceLinear[] = "GSsplineCreate: invalid number of control points. Forcing Linear interpolation!";
static const char createForceLagrange[] = "GSsplineCreate: invalid number of control points. Forcing Lagrance interpolation!";
static const char createTwoPointLinear[] = "GSsplineCreate: spline has 2 control points; forcing Linear interpolation!";

void GSsplineFree(GSspline *spline)
{
    if (spline) {
        GSmemFree(spline->controlAllocation);
        GSmemFree(spline->allocation);
    }
}

void GSsplineAddControlVectorValue(GSspline *spline, struct GSvec *value, f32 time)
{
    if (spline) {
        if (spline->dataType == GSSPLINE_VECTOR) {
            if (spline->count < spline->capacity) {
                u8 addTime = 1;
                struct GSvec *control;

                if ((spline->interpolation == GSSPLINE_CUBIC || spline->interpolation == GSSPLINE_CUBIC_ALT) &&
                    spline->count % 3 != 0)
                    addTime = 0;

                control = &((struct GSvec *)spline->values)[spline->count];
                control->x = value->x;
                control->y = value->y;
                control->z = value->z;
                spline->times[spline->count] = time;
                spline->count++;

                if (addTime) {
                    u8 index = spline->timeCount;
                    spline->times[index] = time;
                    if (index == 0)
                        spline->startTime = time;
                    else if (time < spline->times[index - 1])
                        GSlogWrite(controlVectorTimeWentBack);
                    if (spline->count == spline->capacity)
                        spline->endTime = time;
                    spline->timeCount++;
                }
            } else {
                GSlogWrite(controlVectorFull);
            }
        } else {
            GSlogWrite(controlVectorBadType);
        }
    }
}

GSspline *GSsplineCreate(s32 interpolation, s32 dataType, u8 count)
{
    GSspline *spline = 0;
    void *data;

    if (count < 2) {
        GSlogWrite(createTooFewPoints);
        return 0;
    }

    if ((interpolation == GSSPLINE_CUBIC || interpolation == GSSPLINE_CUBIC_ALT) && (count - 1) % 3 != 0) {
        if (count == 2) {
            GSlogWrite(createForceLinear);
            interpolation = GSSPLINE_LINEAR;
        } else {
            GSlogWrite(createForceLagrange);
            interpolation = GSSPLINE_LAGRANGE;
        }
    } else if (count == 2 && interpolation != GSSPLINE_LINEAR) {
        interpolation = GSSPLINE_LINEAR;
        GSlogWrite(createTwoPointLinear);
    }

    data = GSmemAlloc(sizeof(GSspline));
    if (data) {
        spline = data;
        spline->allocation = data;
        spline->interpolation = interpolation;
        spline->dataType = dataType;
        spline->count = 0;
        spline->timeCount = 0;

        switch (interpolation) {
        case GSSPLINE_LINEAR:
            if (dataType == GSSPLINE_VECTOR) {
                data = GSmemAlloc(count * (sizeof(struct GSvec) + sizeof(f32)));
                if (data) {
                    spline->controlAllocation = data;
                    spline->values = data;
                    data = (struct GSvec *)data + count;
                    spline->auxiliary = 0;
                    spline->times = (f32 *)data;
                    spline->capacity = count;
                    spline->dataType = GSSPLINE_VECTOR;
                } else {
                    GSmemFree(spline->allocation);
                    spline = 0;
                }
            } else {
                data = GSmemAlloc(count * 2 * sizeof(f32));
                if (data) {
                    spline->controlAllocation = data;
                    spline->values = data;
                    data = (f32 *)data + count;
                    spline->auxiliary = 0;
                    spline->times = (f32 *)data;
                    spline->capacity = count;
                    spline->dataType = GSSPLINE_FLOAT;
                } else {
                    GSmemFree(spline->allocation);
                    spline = 0;
                }
            }
            break;
        case GSSPLINE_CUBIC:
        case GSSPLINE_CUBIC_ALT:
            if (dataType == GSSPLINE_VECTOR) {
                data = GSmemAlloc(count * sizeof(struct GSvec) + ((count + 2) / 3) * sizeof(f32));
                if (data) {
                    spline->controlAllocation = data;
                    spline->values = data;
                    data = (struct GSvec *)data + count;
                    spline->auxiliary = 0;
                    spline->times = (f32 *)data;
                    spline->capacity = count;
                    spline->dataType = GSSPLINE_VECTOR;
                } else {
                    GSmemFree(spline->allocation);
                    spline = 0;
                }
            } else {
                data = GSmemAlloc((count + (count + 2) / 3) * sizeof(f32));
                if (data) {
                    spline->controlAllocation = data;
                    spline->values = data;
                    data = (f32 *)data + count;
                    spline->auxiliary = 0;
                    spline->times = (f32 *)data;
                    spline->capacity = count;
                    spline->dataType = GSSPLINE_FLOAT;
                } else {
                    GSmemFree(spline->allocation);
                    spline = 0;
                }
            }
            break;
        default:
            if (dataType == GSSPLINE_VECTOR) {
                data = GSmemAlloc(count * (sizeof(struct GSvec) + 2 * sizeof(f32)));
                if (data) {
                    spline->controlAllocation = data;
                    spline->values = data;
                    spline->auxiliary = (f32 *)((struct GSvec *)data + count);
                    spline->times = (f32 *)((struct GSvec *)data + count) + count;
                    spline->capacity = count;
                    spline->dataType = GSSPLINE_VECTOR;
                } else {
                    GSmemFree(spline->allocation);
                    spline = 0;
                }
            } else {
                data = GSmemAlloc(count * 3 * sizeof(f32));
                if (data) {
                    u8 *cursor;
                    u32 offset;

                    spline->controlAllocation = data;
                    spline->values = data;
                    offset = (u32)count * sizeof(f32);
                    cursor = (u8 *)data + offset;
                    spline->auxiliary = (f32 *)cursor;
                    cursor += offset;
                    spline->times = (f32 *)cursor;
                    spline->capacity = count;
                    spline->dataType = GSSPLINE_FLOAT;
                } else {
                    GSmemFree(spline->allocation);
                    spline = 0;
                }
            }
            break;
        }
    }
    return spline;
}
