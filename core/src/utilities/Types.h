#pragma once

#include <glm/glm.hpp>


#include <Eigen/Eigen/Core>
#include <Eigen/Eigen/StdVector>
#include <Eigen/Eigen/Geometry>

using vector2 = glm::dvec2;
using vector3 = glm::dvec3;
using vector4 = glm::dvec4;

using matrix3 = glm::dmat3;
using matrix4 = glm::dmat4;

using point2 = glm::dvec2;
using point3 = glm::dvec3;

typedef Eigen::Matrix<double, 5, 1> Vector5d;


inline double vector_multiply_to_double(const vector3& v1, const vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline vector3 vector_modulo_operator(const vector3& v1, const vector3& v2)
{
	return vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}