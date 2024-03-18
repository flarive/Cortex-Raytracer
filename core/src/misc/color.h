#ifndef COLOR_H
#define COLOR_H

#include "../interval.h"

#include "../utilities/math_utils.h"

#include <iostream>

class color
{
public:
    double c[3];

    color() : c{ 0,0,0 } {}
    color(double c0, double c1, double c2) : c{ c0, c1, c2 } {}

    double r() const { return c[0]; }
    double g() const { return c[1]; }
    double b() const { return c[2]; }

    color operator-() const { return color(-c[0], -c[1], -c[2]); }
    double operator[](int i) const { return c[i]; }
    double& operator[](int i) { return c[i]; }

    color& operator+=(const color& v)
    {
        c[0] += v.c[0];
        c[1] += v.c[1];
        c[2] += v.c[2];
        return *this;
    }

    color& operator*=(double t)
    {
        c[0] *= t;
        c[1] *= t;
        c[2] *= t;
        return *this;
    }

    color& operator/=(double t)
    {
        return *this *= 1 / t;
    }

    double length() const
    {
        return sqrt(length_squared());
    }

    double length_squared() const
    {
        return c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
    }


    static color random()
    {
        return color(random_double(), random_double(), random_double());
    }

    static color random(double min, double max)
    {
        return color(random_double(min, max), random_double(min, max), random_double(min, max));
    }

    static color White()
    {
        return color(0,0,0);
    }

    

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

inline double linear_to_gamma(double linear_component)
{
    return sqrt(linear_component);
}

/// <summary>
/// Write pixel color to the output stream
/// </summary>
/// <param name="out"></param>
/// <param name="pixel_color"></param>
void write_color(std::ostream& out, color pixel_color)
{
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color.r()) << ' '
        << static_cast<int>(255.999 * pixel_color.g()) << ' '
        << static_cast<int>(255.999 * pixel_color.b()) << '\n';
}


/// <summary>
/// Write pixel color to the output stream with pixel sampling (antialiasing)
/// </summary>
/// <param name="out"></param>
/// <param name="pixel_color"></param>
/// <param name="samples_per_pixel"></param>
void write_color(std::ostream& out, color pixel_color, int samples_per_pixel)
{
    double r = pixel_color.r();
    double g = pixel_color.g();
    double b = pixel_color.b();

    // Replace NaN components with zero.
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    // Divide the color by the number of samples.
    double scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Gamma correction
    // Apply the linear to gamma transform
    // Helps to have a much more consistent ramp from darkness to lightness in the final image
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Write the translated [0,255] value of each color component.
    // Static Variable gets constructed only once no matter how many times the function is called.
    static const interval intensity(0.000, 0.999);

    out << static_cast<int>(256 * intensity.clamp(r)) << ' '
        << static_cast<int>(256 * intensity.clamp(g)) << ' '
        << static_cast<int>(256 * intensity.clamp(b)) << '\n';
}

#endif