#pragma once

#include <random>

#define _USE_MATH_DEFINES
#include <math.h>

#include "types.h"

#include <limits>

class Random
{
public:
	explicit Random(int addseed = 0);

	inline double getZeroOne()
	{		
		return uniformZeroOne(rng);
	}

	// not used yet anywhere, but it might be useful in the future
	inline double getGaussian()
	{
		return gaussian(rng);
	}

	//inline vector3 getRandomInUnitSphere()
	//{
	//	vector3 res;

	//	do
	//	{
	//		res = 2. * vector3(getZeroOne(), getZeroOne(), getZeroOne()) - vector3(1, 1, 1);
	//	} while (res * res >= 1);

	//	return res;
	//}

	//inline vector3 getRandomOnUnitSphere()
	//{
	//	return glm::normalize(getRandomInUnitSphere());
	//}

	//inline vector3 getRandomInHemisphere(const vector3& normal)
	//{
	//	vector3 res = getRandomInUnitSphere();
	//	if (res * normal > 0.0) 
	//		return -res;

	//	return res;
	//}

	//inline vector3 getRandomOnHemisphere(const vector3& normal)
	//{
	//	return getRandomInHemisphere(normal).Normalize();
	//}

	inline vector3 getRandomCosineDirection()
	{
		const double r1 = getZeroOne();
		const double r2 = getZeroOne();
		const double z = sqrt(1. - r2);
		const double phi = 2. * M_PI * r1;
		const double twosqrt = 2. * sqrt(r2);
		const double x = cos(phi) * twosqrt;
		const double y = sin(phi) * twosqrt; 

		return vector3(x, y, z);
	}

private:
	std::mt19937_64 rng;
	std::uniform_real_distribution<double> uniformZeroOne;
	std::normal_distribution<double> gaussian;
};