#pragma once

#include <iostream>

class color
{
public:
    double c[4];

    color();
    color(double c0);
    color(double c0, double c1, double c2);
    color(double c0, double c1, double c2, double c3);

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
    color& operator*=(const color& v);
    color& operator/=(double t);


    double length() const;
    double length_squared() const;

    static const color white();
    static const color black();
    static const color red();
    static const color green();
    static const color blue();
    static const color yellow();
    static const color undefined();

    /// <summary>
    /// Write pixel color to the output stream with pixel sampling (antialiasing) and gamma correction
    /// </summary>
    /// <param name="out"></param>
    /// <param name="pixel_color"></param>
    /// <param name="samples_per_pixel"></param>
    static color prepare_pixel_color(int x, int y, color pixel_color, int samples_per_pixel, bool gamma_correction);

    static color RGBtoHSV(color rgb);
    static color HSVtoRGB(color hsv);

    static double linear_to_gamma(double linear_component);

    static color blend_colors(const color& front, const color& back, double alpha);

    static color blend_with_background(const color& background, const color& object_color, float alpha);

    bool isValidColor();
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