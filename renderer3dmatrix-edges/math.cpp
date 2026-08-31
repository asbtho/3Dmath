#include "math.h"

Matrix44 Matrix44::identity() {
    Matrix44 res = { {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    } };
    return res;
}

Matrix44 Matrix44::operator*(const Matrix44& o) const {
    Matrix44 res = { {0} };
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            res.m[c * 4 + r] =
                m[0 * 4 + r] * o.m[c * 4 + 0] +
                m[1 * 4 + r] * o.m[c * 4 + 1] +
                m[2 * 4 + r] * o.m[c * 4 + 2] +
                m[3 * 4 + r] * o.m[c * 4 + 3];
        }
    }
    return res;
}

Matrix44 Matrix44::translation(float x, float y, float z) {
    Matrix44 res = identity();
    res.m[12] = x;
    res.m[13] = y;
    res.m[14] = z;
    return res;
}

Matrix44 Matrix44::scaling(float x, float y, float z) {
    Matrix44 res = identity();
    res.m[0] = x;
    res.m[5] = y;
    res.m[10] = z;
    return res;
}

Matrix44 Matrix44::rotationX(float angleRad) {
    Matrix44 res = identity();
    float c = cosf(angleRad);
    float s = sinf(angleRad);
    res.m[5] = c;  res.m[9] = -s;
    res.m[6] = s;  res.m[10] = c;
    return res;
}

Matrix44 Matrix44::rotationY(float angleRad) {
    Matrix44 res = identity();
    float c = cosf(angleRad);
    float s = sinf(angleRad);
    res.m[0] = c;   res.m[8] = s;
    res.m[2] = -s;  res.m[10] = c;
    return res;
}

Matrix44 Matrix44::perspective(float fovRad, float aspect, float nearP, float farP) {
    Matrix44 res = { {0} };
    float tanHalfFov = tanf(fovRad / 2.0f);
    res.m[0] = 1.0f / (aspect * tanHalfFov);
    res.m[5] = 1.0f / tanHalfFov;
    res.m[10] = -(farP + nearP) / (farP - nearP);
    res.m[11] = -1.0f;
    res.m[14] = -(2.0f * farP * nearP) / (farP - nearP);
    return res;
}

Vector3 Matrix44::transform(const Vector3& v) const {
    float x = v.x * m[0] + v.y * m[4] + v.z * m[8]  + m[12];
    float y = v.x * m[1] + v.y * m[5] + v.z * m[9]  + m[13];
    float z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
    float w = v.x * m[3] + v.y * m[7] + v.z * m[11] + m[15];

    if (w != 0.0f) {
        x /= w; y /= w; z /= w;
    }

    return { x, y, z };
}

Vector4 Matrix44::transformH(const Vector3& v) const {
    float x = v.x * m[0] + v.y * m[4] + v.z * m[8]  + m[12];
    float y = v.x * m[1] + v.y * m[5] + v.z * m[9]  + m[13];
    float z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
    float w = v.x * m[3] + v.y * m[7] + v.z * m[11] + m[15];
    
    return { x, y, z, w };
}
