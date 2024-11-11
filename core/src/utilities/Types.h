#pragma once

#include <glm/glm.hpp>

#include <tuple>
#include <unordered_map>


#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Geometry>

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

inline double vector_length_squared(vector3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline double vector_length(vector3 v)
{
    return glm::sqrt(vector_length_squared(v));
}

inline vector3 unit_vector(vector3 v)
{
    double len = vector_length(v);
    return v / vector3(len, len, len);
}