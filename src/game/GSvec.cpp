#include <GSvec.h>

extern "C" {
double asin(double value);
double atan(double value);
}

void GSvecMakeFromQuat(GSvec *euler, const f32 *quat) {
    f32 x = quat[0];
    f32 y = quat[1];
    f32 z = quat[2];
    f32 w = quat[3];

    f32 xx = x * x;
    f32 yy = y * y;
    f32 zz = z * z;
    f32 ww = w * w;
    euler->x = (f32)asin(-2.0f * (x * z - w * y));
    euler->y = (f32)atan(2.0f * (x * y + w * z) / (ww + xx - yy - zz));
    euler->z = (f32)atan(2.0f * (w * x + y * z) / (ww - xx - yy + zz));
}

/* Rotate `value` by the quaternion (x, y, z, w) of `quat`. */
void GSvecTransformQuat(GSvec *out, const f32 *quat, const GSvec *value) {
    f32 rotationTerms[4];

    rotationTerms[0] = value->x * quat[3] + quat[1] * value->z - value->y * quat[2];
    rotationTerms[1] = value->y * quat[3] + quat[2] * value->x - value->z * quat[0];
    rotationTerms[2] = value->z * quat[3] + quat[0] * value->y - value->x * quat[1];
    rotationTerms[3] = value->x * quat[0] + quat[1] * value->y + value->z * quat[2];

    out->x = rotationTerms[0] * quat[3] + quat[0] * rotationTerms[3]
           + quat[1] * rotationTerms[2] - rotationTerms[1] * quat[2];
    out->y = rotationTerms[1] * quat[3] + quat[1] * rotationTerms[3]
           + quat[2] * rotationTerms[0] - rotationTerms[2] * quat[0];
    out->z = rotationTerms[2] * quat[3] + quat[2] * rotationTerms[3]
           + quat[0] * rotationTerms[1] - rotationTerms[0] * quat[1];
}

GSvec xAxisVector(1.0f, 0.0f, 0.0f);
GSvec yAxisVector(0.0f, 1.0f, 0.0f);
GSvec zAxisVector(0.0f, 0.0f, 1.0f);
GSvec zeroVector(0.0f, 0.0f, 0.0f);
GSvec unitScaleVector(1.0f, 1.0f, 1.0f);
