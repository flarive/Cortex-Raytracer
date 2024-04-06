#pragma once

#include "../constants.h"
#include "types.h"
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

/// <summary>
/// The square of the Length of this vector
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
static double vector_length_squared(vector3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static double vector_length(vector3 v)
{
    return sqrt(vector_length_squared(v));
}



static double random_double()
{
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

static double random_double(double min, double max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

static vector3 random()
{
    return vector3(random_double(), random_double(), random_double());
}

static vector3 random(double min, double max)
{
    return vector3(random_double(min, max), random_double(min, max), random_double(min, max));
}

static int random_int(int min, int max)
{
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max + 1));
}

inline vector3 unit_vector(vector3 v)
{
    return v / vector3(vector_length(v), vector_length(v), vector_length(v));
}

inline vector3 random_cosine_direction()
{
    auto r1 = random_double();
    auto r2 = random_double();

    auto phi = 2 * M_PI * r1;
    auto x = cos(phi) * sqrt(r2);
    auto y = sin(phi) * sqrt(r2);
    auto z = sqrt(1 - r2);

    return vector3(x, y, z);
}

///// <summary>
///// Generate random point inside unit disk
///// </summary>
///// <returns></returns>
inline vector3 random_in_unit_disk()
{
    while (true) {
        auto p = vector3(random_double(-1, 1), random_double(-1, 1), 0);
        if (vector_length_squared(p) < 1)
            return p;
    }
}

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

///// <summary>
///// Find a random vector in the unit sphere
///// </summary>
///// <returns></returns>
static vector3 random_in_unit_sphere()
{
    while (true)
    {
        auto p = random(-1, 1);
        if (vector_length_squared(p) < 1)
            return p;
    }
}

///// <summary>
///// Find and normalize a random vector in the unit sphere (Figure 12)
///// </summary>
///// <returns></returns>
static vector3 random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

///// <summary>
///// Check if a random unit vector is in the correct hemisphere (Figure 13)
///// </summary>
///// <param name="normal"></param>
///// <returns></returns>
static vector3 random_on_hemisphere(const vector3& normal)
{
    vector3 on_unit_sphere = random_unit_vector();

    // In the same hemisphere as the normal
    if (glm::dot(on_unit_sphere, normal) > 0.0)
    {
        // good hemisphere facing normal
        return on_unit_sphere;
    }
    else
    {
        // wrong hemisphere not facing normal
        // invert the vector to make it be in the correct hemisphere
        return -on_unit_sphere;
    }
}

static void get_sphere_uv(const point3& p, double& u, double& v)
{
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    auto theta = acos(-p.y);
    auto phi = atan2(-p.z, p.x) + M_PI;

    u = phi / (2 * M_PI);
    v = theta / M_PI;
}

static vector3 random_to_sphere(double radius, double distance_squared)
{
    auto r1 = random_double();
    auto r2 = random_double();
    auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

    auto phi = 2 * M_PI * r1;
    auto x = cos(phi) * sqrt(1 - z * z);
    auto y = sin(phi) * sqrt(1 - z * z);

    return vector3(x, y, z);
}