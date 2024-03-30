#pragma once

#include "utilities/types.h"
#include "utilities/math_utils.h"

/// <summary>
/// OrthoNormal Basis
/// An orthonormal basis (ONB) is a collection of three mutually orthogonal unit vectors
/// </summary>
class onb
{
public:
    onb() {}

    vector3 operator[](int i) const { return axis[i]; }

    vector3 u() const { return axis[0]; }
    vector3 v() const { return axis[1]; }
    vector3 w() const { return axis[2]; }

    vector3 local(double a, double b, double c) const
    {
        return a * u() + b * v() + c * w();
    }

    vector3 local(const vector3& a) const
    {
        return a.x * u() + a.y * v() + a.z * w();
    }

    void build_from_w(const vector3& n);


private:
    vector3 axis[3];
};