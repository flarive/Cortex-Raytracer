#pragma once

#include "types.h"

#include <array>
#include <cmath>

class matrix4x4
{
public:
    std::array<std::array<float, 4>, 4> m; // 4x4 matrix elements

    matrix4x4() {
        // Initialize as identity matrix
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    // Multiply two matrices
    matrix4x4 operator*(const matrix4x4& other) const {
        matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    // Multiply matrix by a 4D vector
    vector4 operator*(const vector4& v) const {
        return vector4(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
        );
    }

    // Static methods to create scaling, rotation, and translation matrices
    static matrix4x4 scaling(float sx, float sy, float sz) {
        matrix4x4 result;
        result.m[0][0] = sx;
        result.m[1][1] = sy;
        result.m[2][2] = sz;
        return result;
    }

    static matrix4x4 rotationX(float angle) {
        matrix4x4 result;
        float c = cos(angle);
        float s = sin(angle);
        result.m[1][1] = c;
        result.m[1][2] = -s;
        result.m[2][1] = s;
        result.m[2][2] = c;
        return result;
    }

    static matrix4x4 rotationY(float angle) {
        matrix4x4 result;
        float c = cos(angle);
        float s = sin(angle);
        result.m[0][0] = c;
        result.m[0][2] = s;
        result.m[2][0] = -s;
        result.m[2][2] = c;
        return result;
    }

    static matrix4x4 rotationZ(float angle) {
        matrix4x4 result;
        float c = cos(angle);
        float s = sin(angle);
        result.m[0][0] = c;
        result.m[0][1] = -s;
        result.m[1][0] = s;
        result.m[1][1] = c;
        return result;
    }

    static matrix4x4 translation(float tx, float ty, float tz) {
        matrix4x4 result;
        result.m[0][3] = tx;
        result.m[1][3] = ty;
        result.m[2][3] = tz;
        return result;
    }

    // Compute the inverse transpose for normal transformations
    matrix4x4 inverse() const;    // You can implement this using Gauss-Jordan elimination or libraries
    matrix4x4 transpose() const;  // Transpose the matrix (swap rows and columns)
};

