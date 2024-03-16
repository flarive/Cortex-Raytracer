#pragma once

#include <glm/glm.hpp>

#include "../constants.h"
#include "types.h"

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
Vec3 mapPoint(const Mat4& transformation, const Vec3& point)
{
	const Vec4 homogeneousPoint(point, 1.0);
	const auto homogeneousResult = transformation * homogeneousPoint;

	assert(homogeneousResult.w != 0.0);

	return Vec3(homogeneousResult) / homogeneousResult.w;
}


/**
 * \brief Map a 3D vector with a transformation.
 *        Convert the 3D vector to 4D homogeneous coordinates and back to 3D.
 * \param transformation A 4x4 transformation matrix
 * \param vector A 3D vector
 * \return The transformed 3D vector
 */
Vec3 mapVector(const Mat4& transformation, const Vec3& vector)
{
	const Vec4 homogeneousVector(vector, 0.0);
	const auto homogeneousResult = transformation * homogeneousVector;
	// Conversion from Vec4 to Vec3
	return homogeneousResult;
}


static bool near_zero(Vec3 v)
{
    // Return true if the vector is close to zero in all dimensions.
    auto s = 1e-8;
    return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}

// Utility Functions

inline double degrees_to_radians(double degrees)
{
    return degrees * M_PI / 180.0;
}

static double length_squared(Vec3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static double length2(Vec3 v)
{
    return sqrt(length_squared(v));
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

static Vec3 random()
{
    return Vec3(random_double(), random_double(), random_double());
}
//
static Vec3 random(double min, double max)
{
    return Vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

static int random_int(int min, int max)
{
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max + 1));
}

inline Vec3 unit_vector(Vec3 v)
{
    return v / Vec3(length2(v), length2(v), length2(v));
}

//
///// <summary>
///// Generate random point inside unit disk
///// </summary>
///// <returns></returns>
inline Vec3 random_in_unit_disk()
{
    while (true) {
        auto p = Vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (length_squared(p) < 1)
            return p;
    }
}


inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
    return out << v.x << ' ' << v.y << ' ' << v.z;
}


///// <summary>
///// Find a random vector in the unit sphere
///// </summary>
///// <returns></returns>
Vec3 random_in_unit_sphere()
{
    while (true)
    {
        auto p = random(-1, 1);
        if (length_squared(p) < 1)
            return p;
    }
}

///// <summary>
///// Find and normalize a random vector in the unit sphere (Figure 12)
///// </summary>
///// <returns></returns>
Vec3 random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

inline double dot2(const Vec3& u, const Vec3& v)
{
    return u.x * v.x
        + u.y * v.y
        + u.z * v.z;
}

inline Vec3 cross2(const Vec3& u, const Vec3& v)
{
    return Vec3(u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x);
}


//
///// <summary>
///// Check if a random unit vector is in the correct hemisphere (Figure 13)
///// </summary>
///// <param name="normal"></param>
///// <returns></returns>
Vec3 random_on_hemisphere(const Vec3& normal)
{
    Vec3 on_unit_sphere = random_unit_vector();

    // In the same hemisphere as the normal
    if (dot2(on_unit_sphere, normal) > 0.0)
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



