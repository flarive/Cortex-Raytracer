#ifndef VEC2_H
#define VEC2_H

#include "../constants.h"

#include <cmath>
#include <iostream>

using std::sqrt;

class vec2
{
public:
	double e[2];

	vec2() : e{ 0,0 } {}
	vec2(double e0, double e1) : e{ e0, e1 } {}

	double u() const { return e[0]; }
	double v() const { return e[1]; }
	

	vec2 operator-() const { return vec2(-e[0], -e[1]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	vec2& operator+=(const vec2& v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		return *this;
	}

	vec2& operator*=(double t)
	{
		e[0] *= t;
		e[1] *= t;
		return *this;
	}

	vec2& operator/=(double t)
	{
		return *this *= 1 / t;
	}

	double length() const
	{
		return sqrt(length_squared());
	}

	double length_squared() const
	{
		return e[0] * e[0] + e[1] * e[1];
	}

	static vec2 random()
	{
		return vec2(random_double(), random_double());
	}

	static vec2 random(double min, double max)
	{
		return vec2(random_double(min, max), random_double(min, max));
	}
};



// Vector Utility Functions

//inline std::ostream& operator<<(std::ostream& out, const vec3& v)
//{
//	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
//}
//
//inline vec3 operator+(const vec3& u, const vec3& v)
//{
//	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
//}
//
//inline vec3 operator-(const vec3& u, const vec3& v)
//{
//	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
//}
//
//inline vec3 operator*(const vec3& u, const vec3& v)
//{
//	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
//}
//
//inline vec3 operator*(double t, const vec3& v)
//{
//	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
//}
//
//inline vec3 operator*(const vec3& v, double t)
//{
//	return t * v;
//}
//
//inline vec3 operator/(vec3 v, double t)
//{
//	return (1 / t) * v;
//}
//
//inline double dot(const vec3& u, const vec3& v)
//{
//	return u.e[0] * v.e[0]
//		+ u.e[1] * v.e[1]
//		+ u.e[2] * v.e[2];
//}
//
//inline vec3 cross(const vec3& u, const vec3& v)
//{
//	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
//		u.e[2] * v.e[0] - u.e[0] * v.e[2],
//		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
//}
//
//inline vec3 unit_vector(vec3 v)
//{
//	return v / v.length();
//}

#endif