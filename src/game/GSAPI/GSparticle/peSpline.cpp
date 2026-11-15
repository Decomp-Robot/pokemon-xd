#include <peSpline.h>
#include <dolphin/os/OSFastCast.h>

struct peColorSpline {
    u8 count;
    u8 segment;
    u8 reserved[2];
    GScolor value;
    f32 time;
};

static inline f32 ColorComponent(u8 component)
{
    f32 value;
    OSu8tof32(&component, &value);
    return value;
}
static inline u8 ColorByte(f32 value)
{
    u8 result;
    OSf32tou8(&value, &result);
    return result;
}
static inline void CopyRGB(GScolor *out, const GScolor *in)
{
    out->channels.r = in->channels.r;
    out->channels.g = in->channels.g;
    out->channels.b = in->channels.b;
}
static inline void InterpolateRGB(GScolor *out, GScolor *values, f32 time)
{
    f32 r0 = ColorComponent(values[0].channels.r);
    f32 g0 = ColorComponent(values[0].channels.g);
    f32 b0 = ColorComponent(values[0].channels.b);
    f32 r1 = ColorComponent(values[1].channels.r);
    f32 g1 = ColorComponent(values[1].channels.g);
    f32 b1 = ColorComponent(values[1].channels.b);
    out->channels.r = ColorByte(time * (r1 - r0) + r0);
    out->channels.g = ColorByte(time * (g1 - g0) + g0);
    out->channels.b = ColorByte(time * (b1 - b0) + b0);
}
extern "C" void peColorSplineGetValue(GScolor *color, peColorSpline *spline, f32 time)
{
    u8 count = spline->count;
    if (count < 2) return;
    u8 segment;
    GScolor *values = reinterpret_cast<GScolor *>(spline + 1);
    if (time <= 0.0f) { CopyRGB(color, values); return; }
    if (time >= 1.0f) { values += count - 1; CopyRGB(color, values); return; }
    f32 distance = spline->time - time;
    if (distance < 0.0f) distance = -distance;
    if (distance <= 0.00001f) { CopyRGB(color, &spline->value); return; }
    spline->time = time;
    if (count == 2) {
        InterpolateRGB(&spline->value, values, time);
        CopyRGB(color, &spline->value);
        return;
    }
    segment = spline->segment;
    f32 *times = reinterpret_cast<f32 *>(values + count);
    f32 *inverseLengths = times + count;
    if (time < times[segment]) { spline->segment = 0; segment = 0; }
    while (times[segment + 1] < time) {
        spline->segment = ++segment;
        if (segment >= count) return;
    }
    values += segment;
    f32 fraction = (time - times[segment]) * inverseLengths[segment];
    InterpolateRGB(&spline->value, values, fraction);
    CopyRGB(color, &spline->value);
}


enum { PE_SPLINE_FLAG_SIGNED = 0x4 };
enum { PE_SPLINE_SINGLE_SEGMENT_TANGENT = 4 };

extern "C" f32 peSplineGetValue(peSpline *spline, f32 time)
{
    int mode = spline->flags & PE_SPLINE_MODE_MASK;
    if (!mode)
        return 1.0f;
    u8 segment;
    u8 segmentCount = spline->knotCount - 1;
    if (segmentCount < 1)
        return 1.0f;
    f32 *values = reinterpret_cast<f32 *>(spline + 1);
    if (time <= 0.0f)
        return values[0];
    if (time >= 1.0f)
        return values[segmentCount];
    spline->time = time;
    if (segmentCount == 1) {
        switch (mode) {
        case PE_SPLINE_MODE_LINEAR:
        case PE_SPLINE_MODE_HERMITE:
            spline->value = values[0] + values[PE_SPLINE_SINGLE_SEGMENT_TANGENT] * time;
            break;
        case PE_SPLINE_MODE_STEP:
            spline->value = values[1];
            break;
        }
        return spline->value;
    }
    segment = 0;
    f32 *lengths = values + (segmentCount + 1);
    f32 length;
    while ((length = *lengths) < time) {
        ++segment;
        time -= length;
        ++lengths;
        if (segment >= segmentCount)
            return 1.0f;
    }
    f32 *inverseLengths = lengths + segmentCount;
    values += segment;
    time *= *inverseLengths;
    f32 *tangents = inverseLengths + segmentCount;
    switch (mode) {
    case PE_SPLINE_MODE_HERMITE: {
        f32 square = time * time;
        f32 cube = square * time;
        f32 endTangent = cube - square;
        f32 endValue = (square + square - cube) - endTangent;
        f32 startValue = 1.0f - endValue;
        f32 startTangent = time + (endTangent - square);
        f32 minimum = (spline->flags & PE_SPLINE_FLAG_SIGNED) ? -1.0f : 0.0f;
        f32 value = startValue * values[0];
        value += endValue * values[1];
        value += length * (startTangent * tangents[0] + endTangent * tangents[segmentCount]);
        if (value < minimum)
            value = minimum;
        else if (value > 1.0f)
            value = 1.0f;
        spline->value = value;
        break;
    }
    case PE_SPLINE_MODE_LINEAR:
        spline->value = values[0] + tangents[0] * time;
        break;
    case PE_SPLINE_MODE_STEP:
        spline->value = values[1];
        break;
    }
    return spline->value;
}
