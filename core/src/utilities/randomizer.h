#pragma once

#include "types.h"
#include "math_utils.h"

#include <limits>
#include <random>

class randomizer
{
public:
	explicit randomizer(int addseed = 0);

	inline double getZeroOne()
	{		
		return uniformZeroOne(rng);
	}

	// not used yet anywhere, but it might be useful in the future
	inline double getGaussian()
	{
		return gaussian(rng);
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

	static vector3 random_vector()
	{
		return vector3(random_double(), random_double(), random_double());
	}

	static vector3 random_vector(double min, double max)
	{
		return vector3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

	static int random_int(int min, int max)
	{
		// Returns a random integer in [min,max].
		return static_cast<int>(random_double(min, max + 1));
	}

	//static int random_int2(int min, int max)
	//{
	//	return min + (random_int() % (max - min + 1));
	//}

	static float random_float()
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> distribution(0.0, 1.0);
		return distribution(gen);
	}

	static float random_float(float min, float max)
	{
		return min + (max - min) * random_float();
	}

	static int random_int()
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_int_distribution<int> distribution(0, std::numeric_limits<int>::max());
		return distribution(gen);
	}

	static vector3 unit_vector(vector3 v)
	{
		return v / vector3(vector_length(v), vector_length(v), vector_length(v));
	}

	static vector3 random_cosine_direction()
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
	static vector3 random_in_unit_disk()
	{
		while (true) {
			auto p = vector3(random_double(-1, 1), random_double(-1, 1), 0);
			if (vector_length_squared(p) < 1)
				return p;
		}
	}

	///// <summary>
	///// Find a random vector in the unit sphere (getRandomInUnitSphere)
	///// </summary>
	///// <returns></returns>
	static vector3 random_in_unit_sphere()
	{
		while (true)
		{
			auto p = random_vector(-1, 1);
			if (vector_length_squared(p) < 1)
				return p;
		}
	}

	///// <summary>
	///// Find and normalize a random vector in the unit sphere (Figure 12) (getRandomOnUnitSphere)
	///// </summary>
	///// <returns></returns>
	static vector3 random_unit_vector()
	{
		return randomizer::unit_vector(random_in_unit_sphere());
	}


	///// <summary>
	///// Check if a random unit vector is in the correct hemisphere (Figure 13)
	///// </summary>
	///// <param name="normal"></param>
	///// <returns></returns>
	static vector3 random_on_hemisphere(const vector3& normal)
	{
		vector3 on_unit_sphere = randomizer::random_unit_vector();

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