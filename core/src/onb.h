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
    onb();

    vector3 operator[](int i) const;

    vector3 u() const;
    vector3 v() const;
    vector3 w() const;

    vector3 local(double a, double b, double c) const;


    vector3 local(const vector3& a) const;


    void build_from_w(const vector3& n);


private:
    vector3 axis[3];
};