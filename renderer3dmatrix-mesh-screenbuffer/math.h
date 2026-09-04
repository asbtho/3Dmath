#pragma once

#include <cmath>
#include <iostream>

struct Edge {
    int a, b;
};

struct Triangle {
    int a, b, c;
};

struct Vector2 {
    float x, y;
};

struct Vector3 {
    float x, y, z;
};

struct Vector4 {
    float x, y, z, w;
};

struct Matrix44 {
    float m[16];

    static Matrix44 identity();
    Matrix44 operator*(const Matrix44& o) const;
    static Matrix44 translation(float x, float y, float z);
    static Matrix44 scaling(float x, float y, float z);
    static Matrix44 rotationX(float angleRad);
    static Matrix44 rotationY(float angleRad);
    static Matrix44 perspective(float fovRad, float aspect, float nearP, float farP);
    Vector3 transform(const Vector3& v) const;
    Vector4 transformH(const Vector3& v) const;
};
