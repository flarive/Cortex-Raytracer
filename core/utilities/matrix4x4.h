#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp> // For additional transformations
#include <glm/gtx/matrix_operation.hpp> // For matrix utilities

class matrix4x4
{
public:
    glm::dmat4 m; // GLM 4x4 matrix

    matrix4x4() : m(glm::dmat4(1.0)) {} // Identity matrix by default

    // Multiply two matrices
    matrix4x4 operator*(const matrix4x4& other) const {
        matrix4x4 result;
        result.m = m * other.m;
        return result;
    }

    // Multiply matrix by a 4D vector
    glm::vec4 operator*(const glm::vec4& v) const {
        return m * v;
    }

    // Static methods to create scaling, rotation, and translation matrices
    static matrix4x4 scaling(float sx, float sy, float sz) {
        matrix4x4 result;
        result.m = glm::scale(glm::vec3(sx, sy, sz));
        return result;
    }

    static matrix4x4 rotationX(float angle) {
        matrix4x4 result;
        result.m = glm::rotate(angle, glm::vec3(1.0f, 0.0f, 0.0f));
        return result;
    }

    static matrix4x4 rotationY(float angle) {
        matrix4x4 result;
        result.m = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
        return result;
    }

    static matrix4x4 rotationZ(float angle) {
        matrix4x4 result;
        result.m = glm::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));
        return result;
    }

    static matrix4x4 translation(float tx, float ty, float tz) {
        matrix4x4 result;
        result.m = glm::translate(glm::vec3(tx, ty, tz));
        return result;
    }

    // Transpose the matrix
    matrix4x4 transpose() const {
        matrix4x4 result;
        result.m = glm::transpose(m);
        return result;
    }

    // Compute the inverse of the matrix
    matrix4x4 inverse() const {
        matrix4x4 result;
        result.m = glm::inverse(m);
        return result;
    }
};
