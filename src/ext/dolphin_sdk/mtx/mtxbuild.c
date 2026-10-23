#include <dolphin.h>
#include <dolphin/mtx.h>

/* Constants shared by the matrix construction routines below. They are
   defined at the end of this file. */
extern const f32 cMtxOneFloat;
extern const f32 cMtxZeroFloat;
extern const f32 cMtxHalfFloat;
extern const f32 cMtxThreeFloat;
extern const f32 cMtxTwoFloat;
extern const f32 cMtxNegOneFloat;
extern const f32 cMtxDegToRadFloat;

/* sinf/cosf of the angle, then hand the pair straight to PSMTXRotTrig. */
asm void PSMTXRotRad(register Mtx m, register char axis, register f32 rad) {
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x28(r1)
    stfd f31, 0x20(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    fmr f31, rad
    mr r30, m
    mr r31, axis
    fmr f1, f31
    bl sinf
    fmr f0, f1
    fmr f1, f31
    fmr f31, f0
    bl cosf
    fmr f0, f1
    mr r3, r30
    fmr f1, f31
    mr r4, r31
    fmr f2, f0
    bl PSMTXRotTrig
    lwz r0, 0x2c(r1)
    lfd f31, 0x20(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    addi r1, r1, 0x28
    mtlr r0
    blr
}

asm void PSMTXRotTrig(register Mtx m, register char axis, register f32 sinA, register f32 cosA) {
    frsp f5, sinA
    frsp f4, cosA
    lfs f0, cMtxZeroFloat
    lfs f1, cMtxOneFloat
    ori r0, axis, 0x20
    ps_neg f2, f5
    cmplwi r0, 'x'
    beq L_x
    cmplwi r0, 'y'
    beq L_y
    cmplwi r0, 'z'
    beq L_z
    b L_end
L_x:
    psq_st f1, 0(m), 1, 0
    psq_st f0, 4(m), 0, 0
    ps_merge00 f3, f5, f4
    psq_st f0, 12(m), 0, 0
    ps_merge00 f1, f4, f2
    psq_st f0, 28(m), 0, 0
    psq_st f0, 44(m), 1, 0
    psq_st f3, 36(m), 0, 0
    psq_st f1, 20(m), 0, 0
    b L_end
L_y:
    ps_merge00 f3, f4, f0
    ps_merge00 f1, f0, f1
    psq_st f0, 24(m), 0, 0
    psq_st f3, 0(m), 0, 0
    ps_merge00 f2, f2, f0
    ps_merge00 f0, f5, f0
    psq_st f3, 40(m), 0, 0
    psq_st f1, 16(m), 0, 0
    psq_st f0, 8(m), 0, 0
    psq_st f2, 32(m), 0, 0
    b L_end
L_z:
    psq_st f0, 8(m), 0, 0
    ps_merge00 f3, f5, f4
    ps_merge00 f2, f4, f2
    psq_st f0, 24(m), 0, 0
    psq_st f0, 32(m), 0, 0
    ps_merge00 f1, f1, f0
    psq_st f3, 16(m), 0, 0
    psq_st f2, 0(m), 0, 0
    psq_st f1, 40(m), 0, 0
L_end:
}

/* Rodrigues' rotation formula about an arbitrary axis; sinA and cosA are
   already computed. */
asm void __PSMTXRotAxisRadInternal(register Mtx m, register const Vec* axis,
                                   register f32 sinA, register f32 cosA) {
    lfs f10, cMtxHalfFloat
    lfs f9, cMtxThreeFloat
    frsp f11, cosA
    psq_l f2, 0(axis), 0, 0
    frsp f12, sinA
    lfs f3, 8(axis)
    ps_mul f4, f2, f2
    fadds f8, f10, f10
    ps_madd f5, f3, f3, f4
    fsubs f1, f10, f10
    ps_sum0 f6, f5, f3, f4
    fsubs f0, f8, f11
    frsqrte f7, f6
    fmuls f4, f7, f7
    fmuls f5, f7, f10
    fnmsubs f4, f4, f6, f9
    fmuls f7, f4, f5
    ps_merge00 f11, f11, f11
    ps_muls0 f2, f2, f7
    ps_muls0 f3, f3, f7
    ps_muls0 f6, f2, f0
    ps_muls0 f10, f2, f12
    ps_muls0 f7, f3, f0
    ps_muls1 f5, f6, f2
    ps_muls0 f4, f6, f2
    ps_muls0 f6, f6, f3
    fnmsubs f0, f3, f12, f5
    fmadds f8, f3, f12, f5
    ps_neg f2, f10
    ps_sum0 f9, f6, f1, f10
    ps_sum0 f4, f4, f0, f11
    ps_sum1 f5, f11, f8, f5
    ps_sum0 f0, f2, f1, f6
    psq_st f9, 8(m), 0, 0
    ps_sum0 f2, f6, f6, f2
    psq_st f4, 0(m), 0, 0
    ps_muls0 f7, f7, f3
    psq_st f5, 16(m), 0, 0
    ps_sum1 f6, f10, f2, f6
    psq_st f0, 24(m), 0, 0
    ps_sum0 f7, f7, f1, f11
    psq_st f6, 32(m), 0, 0
    psq_st f7, 40(m), 0, 0
}

void PSMTXRotAxisRad(Mtx m, const Vec* axis, f32 rad) {
    f32 sinA = sinf(rad);
    f32 cosA = cosf(rad);

    __PSMTXRotAxisRadInternal(m, axis, sinA, cosA);
}

asm void PSMTXTrans(register Mtx m, register f32 xT, register f32 yT, register f32 zT) {
    lfs f0, cMtxZeroFloat
    lfs f4, cMtxOneFloat
    stfs xT, 12(m)
    stfs yT, 28(m)
    psq_st f0, 4(m), 0, 0
    psq_st f0, 32(m), 0, 0
    stfs f0, 16(m)
    stfs f4, 20(m)
    stfs f0, 24(m)
    stfs f4, 40(m)
    stfs zT, 44(m)
    stfs f4, 0(m)
}

asm void PSMTXTransApply(register const Mtx src, register Mtx dst, register f32 xT,
                         register f32 yT, register f32 zT) {
    psq_l f4, 0(src), 0, 0
    frsp f1, xT
    psq_l f5, 8(src), 0, 0
    frsp f2, yT
    psq_l f7, 24(src), 0, 0
    frsp f3, zT
    psq_l f8, 40(src), 0, 0
    psq_st f4, 0(dst), 0, 0
    ps_sum1 f5, f1, f5, f5
    psq_l f6, 16(src), 0, 0
    psq_st f5, 8(dst), 0, 0
    ps_sum1 f7, f2, f7, f7
    psq_l f9, 32(src), 0, 0
    psq_st f6, 16(dst), 0, 0
    ps_sum1 f8, f3, f8, f8
    psq_st f7, 24(dst), 0, 0
    psq_st f9, 32(dst), 0, 0
    psq_st f8, 40(dst), 0, 0
}

asm void PSMTXScale(register Mtx m, register f32 xS, register f32 yS, register f32 zS) {
    lfs f0, cMtxZeroFloat
    stfs xS, 0(m)
    psq_st f0, 4(m), 0, 0
    psq_st f0, 12(m), 0, 0
    stfs yS, 20(m)
    psq_st f0, 24(m), 0, 0
    psq_st f0, 32(m), 0, 0
    stfs zS, 40(m)
    stfs f0, 44(m)
}

asm void PSMTXScaleApply(register const Mtx src, register Mtx dst, register f32 xS,
                         register f32 yS, register f32 zS) {
    frsp f1, xS
    psq_l f4, 0(src), 0, 0
    frsp f2, yS
    psq_l f5, 8(src), 0, 0
    frsp f3, zS
    ps_muls0 f4, f4, f1
    psq_l f6, 16(src), 0, 0
    ps_muls0 f5, f5, f1
    psq_l f7, 24(src), 0, 0
    ps_muls0 f6, f6, f2
    psq_l f8, 32(src), 0, 0
    psq_st f4, 0(dst), 0, 0
    ps_muls0 f7, f7, f2
    psq_l f2, 40(src), 0, 0
    psq_st f5, 8(dst), 0, 0
    ps_muls0 f8, f8, f3
    psq_st f6, 16(dst), 0, 0
    ps_muls0 f2, f2, f3
    psq_st f7, 24(dst), 0, 0
    psq_st f8, 32(dst), 0, 0
    psq_st f2, 40(dst), 0, 0
}

/* Build a rotation matrix from a quaternion. */
asm void PSMTXQuat(register Mtx m, register const Quaternion* q) {
    lfs f1, cMtxOneFloat
    psq_l f4, 0(q), 0, 0
    psq_l f5, 8(q), 0, 0
    fsubs f0, f1, f1
    fadds f2, f1, f1
    ps_mul f6, f4, f4
    ps_merge10 f9, f4, f4
    ps_madd f8, f5, f5, f6
    ps_mul f7, f5, f5
    ps_sum0 f3, f8, f8, f8
    ps_muls1 f10, f9, f5
    fres f11, f3
    ps_sum1 f8, f7, f8, f6
    ps_nmsub f3, f3, f11, f2
    ps_muls1 f7, f5, f5
    ps_mul f3, f11, f3
    ps_sum0 f6, f6, f6, f6
    fmuls f3, f3, f2
    ps_madd f11, f4, f9, f7
    ps_msub f7, f4, f9, f7
    psq_st f0, 12(m), 1, 0
    ps_nmsub f6, f6, f3, f1
    ps_nmsub f8, f8, f3, f1
    psq_st f0, 44(m), 1, 0
    ps_mul f11, f11, f3
    ps_mul f7, f7, f3
    psq_st f6, 40(m), 1, 0
    ps_madds0 f9, f4, f5, f10
    ps_merge00 f5, f11, f8
    ps_nmsub f10, f10, f2, f9
    ps_merge10 f4, f8, f7
    psq_st f5, 16(m), 0, 0
    ps_mul f9, f9, f3
    ps_mul f10, f10, f3
    psq_st f4, 0(m), 0, 0
    psq_st f9, 8(m), 1, 0
    ps_merge10 f7, f10, f0
    ps_merge01 f11, f10, f9
    psq_st f7, 24(m), 0, 0
    psq_st f11, 32(m), 0, 0
}

void C_MTXLookAt(Mtx m, const Point3d* camPos, const Vec* camUp, const Point3d* target) {
    Vec f, s, u;

    f.x = camPos->x - target->x;
    f.y = camPos->y - target->y;
    f.z = camPos->z - target->z;
    PSVECNormalize(&f, &f);

    PSVECCrossProduct(camUp, &f, &s);
    PSVECNormalize(&s, &s);
    PSVECCrossProduct(&f, &s, &u);

    m[0][0] = s.x;
    m[0][1] = s.y;
    m[0][2] = s.z;
    m[0][3] = -(camPos->x * s.x + camPos->y * s.y + camPos->z * s.z);

    m[1][0] = u.x;
    m[1][1] = u.y;
    m[1][2] = u.z;
    m[1][3] = -(camPos->x * u.x + camPos->y * u.y + camPos->z * u.z);

    m[2][0] = f.x;
    m[2][1] = f.y;
    m[2][2] = f.z;
    m[2][3] = -(camPos->x * f.x + camPos->y * f.y + camPos->z * f.z);
}

/* Light frustum: like C_MTXFrustum but with separate scale/translate on the
   translation row and no far plane. */
void C_MTXLightFrustum(register Mtx m, register f32 t, register f32 b, register f32 l,
                        register f32 r, register f32 n, register f32 scaleS,
                        register f32 scaleT, register f32 transS, f32 transT) {
    f32 tmpX = cMtxOneFloat / (r - l);
    f32 tmpY = cMtxTwoFloat * n;
    f32 tmpZ;

    m[0][0] = scaleS * (tmpY * tmpX);
    m[0][1] = cMtxZeroFloat;
    m[0][2] = (scaleS * (tmpX * (r + l))) - transS;
    m[0][3] = cMtxZeroFloat;
    tmpZ = cMtxOneFloat / (t - b);
    m[1][0] = cMtxZeroFloat;
    m[1][1] = scaleT * (tmpY * tmpZ);
    m[1][2] = (scaleT * (tmpZ * (t + b))) - transT;
    m[1][3] = cMtxZeroFloat;
    m[2][0] = cMtxZeroFloat;
    m[2][1] = cMtxZeroFloat;
    m[2][2] = cMtxNegOneFloat;
    m[2][3] = cMtxZeroFloat;
}

void C_MTXLightPerspective(Mtx m, f32 fovY, f32 aspect, f32 scaleS, f32 scaleT, f32 transS,
                           f32 transT) {
    f32 halfFovY = cMtxHalfFloat * fovY;
    f32 cot = cMtxOneFloat / tanf(cMtxDegToRadFloat * halfFovY);

    m[0][0] = scaleS * (cot / aspect);
    m[0][1] = cMtxZeroFloat;
    m[0][2] = -transS;
    m[0][3] = cMtxZeroFloat;
    m[1][0] = cMtxZeroFloat;
    m[1][1] = cot * scaleT;
    m[1][2] = -transT;
    m[1][3] = cMtxZeroFloat;
    m[2][0] = cMtxZeroFloat;
    m[2][1] = cMtxZeroFloat;
    m[2][2] = cMtxNegOneFloat;
    m[2][3] = cMtxZeroFloat;
}

/* Light orthographic counterpart of C_MTXLightFrustum. */
void C_MTXLightOrtho(register Mtx m, register f32 t, register f32 b, register f32 l,
                      register f32 r, register f32 scaleS, register f32 scaleT,
                      register f32 transS, register f32 transT) {
    f32 tmpZ;
    f32 tmpX;
    f32 twoX;
    f32 twoZ;
    tmpX = cMtxOneFloat / (r - l);
    twoX = cMtxTwoFloat * tmpX;
    m[0][0] = twoX * scaleS;
    m[0][1] = cMtxZeroFloat;
    m[0][2] = cMtxZeroFloat;
    m[0][3] = transS + ((tmpX * -(r + l)) * scaleS);
    m[1][0] = cMtxZeroFloat;
    tmpZ = cMtxOneFloat / (t - b);
    twoZ = cMtxTwoFloat * tmpZ;
    m[1][1] = twoZ * scaleT;
    m[1][2] = cMtxZeroFloat;
    m[1][3] = (scaleT * (tmpZ * -(t + b))) + transT;
    m[2][0] = cMtxZeroFloat;
    m[2][1] = cMtxZeroFloat;
    m[2][2] = cMtxZeroFloat;
    m[2][3] = cMtxOneFloat;
}

/* Matrix constants. */
const f32 cMtxOneFloat = 1.0f;
const f32 cMtxZeroFloat = 0.0f;
const f32 cMtxHalfFloat = 0.5f;
const f32 cMtxThreeFloat = 3.0f;
const f32 cMtxTwoFloat = 2.0f;
const f32 cMtxNegOneFloat = -1.0f;
const f32 cMtxDegToRadFloat = 0.017453292519943295f;
