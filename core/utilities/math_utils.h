#pragma once

#include "../constants.h"
#include "types.h"
#include "../misc/color.h"
#include <glm/glm.hpp>
#include <stdlib.h>
#include <ostream>

template<typename T>
T lerp(const T& a, const T& b, const T& x)
{
	// FMA friendly
	return x * b + (a - a * x);
}

/**
 * \brief Map a 3D point with a transformation.
 *        Convert the 3D vector to 4D homogeneous coordinates and back to 3D.
 * \param transformation A 4x4 transformation matrix
 * \param point A 3D point
 * \return The transformed 3D point
 */
static vector3 mapPoint(const matrix4& transformation, const vector3& point)
{
	const vector4 homogeneousPoint(point, 1.0);
	const auto homogeneousResult = transformation * homogeneousPoint;

	assert(homogeneousResult.w != 0.0);

	return vector3(homogeneousResult) / homogeneousResult.w;
}


/**
 * \brief Map a 3D vector with a transformation.
 *        Convert the 3D vector to 4D homogeneous coordinates and back to 3D.
 * \param transformation A 4x4 transformation matrix
 * \param vector A 3D vector
 * \return The transformed 3D vector
 */
static vector3 mapVector(const matrix4& transformation, const vector3& vector)
{
	const vector4 homogeneousVector(vector, 0.0);
	const auto homogeneousResult = transformation * homogeneousVector;
	// Conversion from vector4 to vector3
	return homogeneousResult;
}


static bool near_zero(vector3 v)
{
    // Return true if the vector is close to zero in all dimensions.
    auto s = 1e-8;
    return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}

inline double degrees_to_radians(double degrees)
{
    return degrees * M_PI / 180.0;
}

///// <summary>
///// The square of the length of this vector
///// </summary>
///// <param name="v"></param>
///// <returns></returns>
//static double vector_length_squared(vector3 v)
//{
//    return v.x * v.x + v.y * v.y + v.z * v.z;
//}

//static double vector_length(vector3 v)
//{
//    return sqrt(vector_length_squared(v));
//}

/// <summary>
/// Print a vector in stdout
/// </summary>
/// <param name="out"></param>
/// <param name="v"></param>
/// <returns></returns>
inline std::ostream& operator<<(std::ostream& out, const vector3& v)
{
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

// Overload for vector2
inline std::ostream& operator<<(std::ostream& out, const vector2& v)
{
    return out << v.x << ' ' << v.y;
}

// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
static void get_barycenter(point3 p, point3 a, point3 b, point3 c, double& u, double& v, double& w)
{
    vector3 v0 = b - a, v1 = c - a, v2 = p - a;
    double d00 = glm::dot(v0, v0);
    double d01 = glm::dot(v0, v1);
    double d11 = glm::dot(v1, v1);
    double d20 = glm::dot(v2, v0);
    double d21 = glm::dot(v2, v1);
    double denom = d00 * d11 - d01 * d01;

    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
}

/// <summary>
/// https://medium.com/@dbildibay/ray-tracing-adventure-part-iv-678768947371
/// </summary>
/// <param name="tan"></param>
/// <param name="bitan"></param>
/// <param name="normal"></param>
/// <param name="sampleNormal"></param>
/// <returns></returns>
static vector3 getTransformedNormal(const vector3& tan, const vector3& bitan, const vector3& normal, color& sample, double strength, bool useMatrix)
{
    // Adjust the sample values based on the strength
    sample = (sample - color(0.5, 0.5, 0.5)) * strength + color(0.5, 0.5, 0.5);

    if (useMatrix)
    {
        // Build a TNB matrix (Tangent/Normal/Bitangent matrix)
        glm::mat3x3 matTNB = glm::mat3x3(tan, bitan, normal);
        vector3 tmp = vector3(sample.r(), sample.g(), sample.b());

        // Apply TNB matrix transformation to the texture space normal
        vector3 transformed_normal = matTNB * tmp;

        // Normalize the transformed normal to ensure it's a unit vector
        return glm::normalize(transformed_normal);
    }
    else
    {
        // simplest method (often sufficient and easier to implement)
        vector3 transformed_normal = tan * sample.r() + bitan * sample.g() + normal * sample.b();

        // Normalize the transformed normal to ensure it's a unit vector
        return glm::normalize(transformed_normal);
    }
}

// Function to calculate the maximum of the dot product of two vectors and zero
static double maxDot3(const vector3& v1, const vector3& v2)
{
    double dotProduct = 0.0;

    // Compute the dot product of the two vectors
    for (auto i = 0; i < v1.length(); ++i)
    {
        dotProduct += v1[i] * v2[i];
    }

    // Return the maximum of the dot product and zero
    return std::max(dotProduct, 0.0);
}
