#include <dolphin.h>
#include <dolphin/mtx.h>
#include "fake_tgmath.h"

void PSQUATMultiply(const register Quaternion* p, const register Quaternion* q, register Quaternion* pq) {
    register f32 pxy, pzw;
    register f32 qxy, qzw;
    register f32 pnxy, pnzw, pnxny, pnznw;
    register f32 rxy, rzw;
    register f32 sxy, szw;

    asm {
        psq_l pxy, 0x0(p), 0, 0
        psq_l pzw, 0x8(p), 0, 0
        psq_l qxy, 0x0(q), 0, 0
        ps_neg pnxny, pxy
        psq_l qzw, 0x8(q), 0, 0
        ps_neg pnznw, pzw
        ps_merge01 pnxy, pnxny, pxy
        ps_muls0 rxy, pzw, qxy
        ps_muls0 rzw, pnxny, qxy
        ps_merge01 pnzw, pnznw, pzw
        ps_muls1 szw, pnxy, qxy
        ps_madds0 rxy, pnxy, qzw, rxy
        ps_muls1 sxy, pnzw, qxy
        ps_madds0 rzw, pnzw, qzw, rzw
        ps_madds1 szw, pnznw, qzw, szw
        ps_merge10 rxy, rxy, rxy
        ps_madds1 sxy, pxy, qzw, sxy
        ps_merge10 rzw, rzw, rzw
        ps_add rxy, rxy, sxy
        psq_st rxy, 0x0(pq), 0, 0
        ps_sub rzw, rzw, szw
        psq_st rzw, 0x8(pq), 0, 0
    }
}

void PSQUATInverse(const register Quaternion* src, register Quaternion* inv) {
    register f32 sxy, szw;
    register f32 izz, iww;
    register f32 mag, nmag;
    register f32 norminv, nninv;
    register f32 nwork0;
    register f32 c_two;
    register f32 c_zero;
    register f32 c_one = 1.0f;

    asm {
        psq_l sxy, 0x0(src), 0, 0
        ps_mul mag, sxy, sxy
        ps_sub c_zero, c_one, c_one
        psq_l szw, 0x8(src), 0, 0
        ps_madd mag, szw, szw, mag
        ps_add c_two, c_one, c_one
        ps_sum0 mag, mag, mag, mag
        fcmpu cr0, mag, c_zero
        beq L_00000948
        fres norminv, mag
        ps_neg nmag, mag
        ps_nmsub nwork0, mag, norminv, c_two
        ps_mul norminv, norminv, nwork0
        b L_0000094C
    L_00000948:
        fmr norminv, c_one
    L_0000094C:
        ps_neg nninv, norminv
        ps_muls1 iww, norminv, szw
        ps_muls0 sxy, sxy, nninv
        psq_st iww, 0xc(inv), 1, 0
        ps_muls0 izz, szw, nninv
        psq_st sxy, 0x0(inv), 0, 0
        psq_st izz, 0x8(inv), 1, 0
    }
}

void C_QUATRotAxisRad(Quaternion* r, const Vec* axis, f32 rad) {
    f32 half, sh, ch;
    Vec nAxis;

    ASSERTMSGLINE(758, r, "QUATRotAxisRad():  NULL QuaternionPtr 'r' ");
    ASSERTMSGLINE(759, axis, "QUATRotAxisRad():  NULL VecPtr 'axis' ");

    VECNormalize(axis, &nAxis);

    half = rad * 0.5f;
    sh = sinf(half);
    ch = cosf(half);

    r->x = sh * nAxis.x;
    r->y = sh * nAxis.y;
    r->z = sh * nAxis.z;
    r->w = ch;
}

void C_QUATMtx(Quaternion* r, const Mtx m) {
    f32 tr,s;
    s32 i, j, k;
    s32 nxt[3] = {1, 2, 0};
    f32 q[3];

    ASSERTMSGLINE(791, r, "QUATMtx():  NULL QuaternionPtr 'r' ");
    ASSERTMSGLINE(792, m, "QUATMtx():  NULL MtxPtr 'm' ");

    tr = m[0][0] + m[1][1] + m[2][2];
    if (tr > 0.0f) {
        s = sqrtf(tr + 1.0f);
        r->w = s * 0.5f;
        s = 0.5f / s;

        r->x = (m[2][1] - m[1][2]) * s;
        r->y = (m[0][2] - m[2][0]) * s;
        r->z = (m[1][0] - m[0][1]) * s;
    } else  {
        i = 0;
        if (m[1][1] > m[0][0]) {
            i = 1;
        }

        if (m[2][2] > m[i][i]) {
            i = 2;
        }

        j = nxt[i];
        k = nxt[j];

        s = sqrtf((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);
        q[i] = s * 0.5f;

        if (s != 0.0f) {
            s = 0.5f / s;
        }

        r->w = (m[k][j] - m[j][k]) * s;
        q[j] = (m[i][j] + m[j][i]) * s;
        q[k] = (m[i][k] + m[k][i]) * s;

        r->x = q[0];
        r->y = q[1];
        r->z = q[2];
    }
}

void C_QUATLerp(const Quaternion* p, const Quaternion* q, Quaternion* r, f32 t) {
    ASSERTMSGLINE(842, p, "QUATLerp():  NULL QuaternionPtr 'p' ");
    ASSERTMSGLINE(843, q, "QUATLerp():  NULL QuaternionPtr 'q' ");
    ASSERTMSGLINE(844, r, "QUATLerp():  NULL QuaternionPtr 'r' ");

    r->x = t * (q->x - p->x) + p->x;
    r->y = t * (q->y - p->y) + p->y;
    r->z = t * (q->z - p->z) + p->z;
    r->w = t * (q->w - p->w) + p->w;
}

void C_QUATSlerp(const Quaternion* p, const Quaternion* q, Quaternion* r, f32 t) {
    f32 theta, sin_th, cos_th;
    f32 tp, tq;

    ASSERTMSGLINE(869, p, "QUATSlerp():  NULL QuaternionPtr 'p' ");
    ASSERTMSGLINE(870, q, "QUATSlerp():  NULL QuaternionPtr 'q' ");
    ASSERTMSGLINE(871, r, "QUATSlerp():  NULL QuaternionPtr 'r' ");

    cos_th = p->x * q->x + p->y * q->y + p->z * q->z + p->w * q->w;
    tq = 1.0f;

    if (cos_th < 0.0f) {
        cos_th = -cos_th;
        tq = -tq;
    }

    if (cos_th <= 0.99999f) {
        theta = acosf(cos_th);
        sin_th = sinf(theta);

        tp = sinf((1.0f - t) * theta) / sin_th;
        tq *= sinf(t * theta) / sin_th;
    } else {
        tp = 1.0f - t;
        tq *= t;
    }

    r->x = (tp * p->x) + (tq * q->x);
    r->y = (tp * p->y) + (tq * q->y);
    r->z = (tp * p->z) + (tq * q->z);
    r->w = (tp * p->w) + (tq * q->w);
}
