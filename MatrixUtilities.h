#ifndef MATRIXUTILS_H
#define MATRIXUTILS_H

#include "Matrix.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline float* matToArray(const Matrix<4,4>& m) {
    float* arr = new float[16];
    int idx = 0;
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
            arr[idx++] = m[row][col];
    return arr;
}

inline Matrix<4,4> makeIdentity() {
    Matrix<4,4> m;
    m[0][0]=1; 
    m[1][1]=1; 
    m[2][2]=1; 
    m[3][3]=1;
    return m;
}

inline Matrix<4,4> matMul(const Matrix<4,4>& a, const Matrix<4,4>& b) {
    Matrix<4,4> r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                r[i][j] += a[i][k] * b[k][j];
    return r;
}

inline Matrix<4,4> makeRotationX(float angleDeg) {
    Matrix<4,4> m = makeIdentity();
    float a = angleDeg * M_PI / 180.0f;
    m[1][1] =  cos(a); m[1][2] = -sin(a);
    m[2][1] =  sin(a); m[2][2] =  cos(a);
    return m;
}

inline Matrix<4,4> makeRotationY(float angleDeg) {
    Matrix<4,4> m = makeIdentity();
    float a = angleDeg * M_PI / 180.0f;
    m[0][0] =  cos(a); m[0][2] =  sin(a);
    m[2][0] = -sin(a); m[2][2] =  cos(a);
    return m;
}

inline Matrix<4,4> makeRotationZ(float angleDeg) {
    Matrix<4,4> m = makeIdentity();
    float a = angleDeg * M_PI / 180.0f;
    m[0][0] =  cos(a); m[0][1] = -sin(a);
    m[1][0] =  sin(a); m[1][1] =  cos(a);
    return m;
}

inline Matrix<4,4> makeTranslation(float tx, float ty, float tz) {
    Matrix<4,4> m = makeIdentity();
    m[0][3] = tx;
    m[1][3] = ty;
    m[2][3] = tz;
    return m;
}

inline Matrix<4,4> makePerspective(float fovDeg, float aspect, float nearP, float farP) {
    Matrix<4,4> m;
    float fov = fovDeg * M_PI / 180.0f;
    float f = 1.0f / tan(fov / 2.0f);
    m[0][0] = f / aspect;
    m[1][1] = f;
    m[2][2] = (farP + nearP) / (nearP - farP);
    m[2][3] = (2.0f * farP * nearP) / (nearP - farP);
    m[3][2] = -1.0f;
    m[3][3] = 0.0f;
    return m;
}

inline Matrix<4,4> makeView(float distance) {
    Matrix<4,4> m = makeIdentity();
    m[2][3] = -distance;
    return m;
}

#endif