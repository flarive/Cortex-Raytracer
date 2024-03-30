#pragma once

#include "../utilities/interval.h"
#include "../utilities/math_utils.h"
#include <iostream>

class color
{
public:
    double c[4];

    color() : c{ 0,0,0,1 } {}
    color(double c0) : c{ c0, c0, c0, 1 } {}
    color(double c0, double c1, double c2) : c{ c0, c1, c2, 1 } {}
    color(double c0, double c1, double c2, double c3) : c{ c0, c1, c2, c3 } {}

    double r() const;
    double g() const;
    double b() const;
    double a() const;

    void r(double r);
    void g(double g);
    void b(double b);
    void a(double a);

    color operator-() const;
    double operator[](int i) const;
    double& operator[](int i);

    color& operator+=(const color& v);
    color& operator+=(double t);
    color& operator*=(double t);
    color& operator/=(double t);


    double length() const;
    double length_squared() const;
    static color random();
    static color random(double min, double max);

    static color White();
    static color Black();


    /// <summary>
    /// Write pixel color to the output stream with pixel sampling (antialiasing)
    /// </summary>
    /// <param name="out"></param>
    /// <param name="pixel_color"></param>
    /// <param name="samples_per_pixel"></param>
    static void write_color(std::ostream& out, color pixel_color, int samples_per_pixel);

    /// <summary>
    /// Write pixel color to the output stream
    /// </summary>
    /// <param name="out"></param>
    /// <param name="pixel_color"></param>
    static void write_color(std::ostream& out, color pixel_color);


    static color RGBtoHSV(color rgb);
    static color HSVtoRGB(color hsv);

    static double linear_to_gamma(double linear_component);
};


// Color Utility Functions

inline std::ostream& operator<<(std::ostream& out, const color& v)
{
    return out << v.c[0] << ' ' << v.c[1] << ' ' << v.c[2];
}

inline color operator+(const color& u, const color& v)
{
    return color(u.c[0] + v.c[0], u.c[1] + v.c[1], u.c[2] + v.c[2]);
}

inline color operator-(const color& u, const color& v)
{
    return color(u.c[0] - v.c[0], u.c[1] - v.c[1], u.c[2] - v.c[2]);
}

inline color operator*(const color& u, const color& v)
{
    return color(u.c[0] * v.c[0], u.c[1] * v.c[1], u.c[2] * v.c[2]);
}

inline color operator*(double t, const color& v)
{
    return color(t * v.c[0], t * v.c[1], t * v.c[2]);
}

inline color operator*(const color& v, double t)
{
    return t * v;
}

inline color operator/(color v, double t)
{
    return (1 / t) * v;
}





template<class T>
inline T ffmin(T a, T b) { return(a < b ? a : b); }

template<class T>
inline T ffmax(T a, T b) { return(a > b ? a : b); }










