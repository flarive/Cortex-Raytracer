#pragma once

#include <glm/glm.hpp>

#include "Types.h"

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
//Vec3 mapPoint(const Mat4& transformation, const Vec3& point);
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
//Vec3 mapVector(const Mat4& transformation, const Vec3& vector);
Vec3 mapVector(const Mat4& transformation, const Vec3& vector)
{
	const Vec4 homogeneousVector(vector, 0.0);
	const auto homogeneousResult = transformation * homogeneousVector;
	// Conversion from Vec4 to Vec3
	return homogeneousResult;
}

