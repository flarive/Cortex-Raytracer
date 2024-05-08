#pragma once

#include "utilities/types.h"
#include "utilities/math_utils.h"

/// <summary>
/// Ortho Normal Basis
/// An orthonormal basis (ONB) is a collection of three mutually orthogonal unit vectors
/// </summary>
class onb
{
public:
	onb() = default;

	// this will have passed the normal to the surface
	explicit onb(const vector3& v) { BuildFrom(v); };
	// the normal and the incident ray vector
	onb(const vector3& n, const vector3& i) { BuildFrom(n, i); };

	// normal, tangent, bitangent - in case it's also used for the TBN matrix
	onb(const vector3& n, const vector3& t, const vector3& b) { BuildFrom(n, t, b); };

	~onb() = default;

    vector3 operator[](int i) const;

    vector3 u() const;
    vector3 v() const;
    vector3 w() const;

    vector3 local(double a, double b, double c) const;


    vector3 local(const vector3& a) const;


    void build_from_w(const vector3& n);

	inline vector3 Normal() const
	{
		return w();
	}

	inline vector3 LocalToGlobal(double X, double Y, double Z) const
	{
		return X * axis[0] + Y * axis[1] + Z * axis[2];
	}

	inline vector3 LocalToGlobal(const vector3& vect) const
	{
		return vect.x * axis[0] + vect.y * axis[1] + vect.z * axis[2];
	}

	inline vector3 GlobalToLocal(const vector3& vect) const
	{
		return vector3(
			axis[0].x * vect.x + axis[1].x * vect.y + axis[2].x * vect.z,
			axis[0].y * vect.x + axis[1].y * vect.y + axis[2].y * vect.z,
			axis[0].z * vect.x + axis[1].z * vect.y + axis[2].z * vect.z
		);
	}


	inline void BuildFrom(const vector3& v)
	{
		//basis[2] = v.Normalize(); // don't normalize, pass it as normalized already, so avoid useless computations
		axis[2] = v;

		if (abs(axis[2].x) > 0.9) axis[1] = vector3(0, 1, 0);
		else axis[1] = vector3(1, 0, 0);

		axis[1] = glm::normalize(vector_modulo_operator(axis[1], axis[2]));
		axis[0] = vector_modulo_operator(axis[1], axis[2]); // x = y % z
	}

	inline void BuildFrom(const vector3& n, const vector3& i)
	{
		const double cosine = glm::abs(vector_multiply_to_double(n, i));

		if (cosine > 0.99999999)
		{
			BuildFrom(n);
		}
		else
		{
			axis[2] = n; // z
			axis[0] = glm::normalize(vector_modulo_operator(i, n)); // x
			axis[1] = vector_modulo_operator(axis[2], axis[0]); // x = z % x
		}
	}

	inline void BuildFrom(const vector3& n, const vector3& t, const vector3& b)
	{
		axis[2] = n; // z
		axis[0] = t;
		axis[1] = b;
	}



private:
    vector3 axis[3];
};