#include "color.h"

color::color() : c{ 0,0,0,1 }
{
}

color::color(double c0) : c{ c0, c0, c0, 1 }
{
}

color::color(double c0, double c1, double c2) : c{ c0, c1, c2, 1 }
{
}

color::color(double c0, double c1, double c2, double c3) : c{ c0, c1, c2, c3 }
{
}

double color::r() const { return c[0]; }
double color::g() const { return c[1]; }
double color::b() const { return c[2]; }
double color::a() const { return c[3]; }

void color::r(double r) { c[0] = r; }
void color::g(double g) { c[1] = g; }
void color::b(double b) { c[2] = b; }
void color::a(double a) { c[3] = a; }

color color::operator-() const
{
    return color(-c[0], -c[1], -c[2]);
}

double color::operator[](int i) const
{
    return c[i];
}

double& color::operator[](int i)
{
    return c[i];
}

color& color::operator+=(const color& v)
{
    if (v.c[3] == 0)
    {
        return *this;
    }
        
    c[0] += v.c[0];
    c[1] += v.c[1];
    c[2] += v.c[2];
    c[3] += v.c[3];

    if (c[3] > 1)
    {
        c[3] = 1;
    }
    return *this;
}

color& color::operator+=(double t)
{
    c[0] += t;
    c[1] += t;
    c[2] += t;
    return *this;
}

color& color::operator*=(double t)
{
    c[0] *= t;
    c[1] *= t;
    c[2] *= t;
    return *this;
}

color& color::operator/=(double t)
{
    return *this *= 1 / t;
}

double color::length() const
{
    return sqrt(length_squared());
}

double color::length_squared() const
{
    return c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
}

color color::White()
{
    return color(1, 1, 1);
}

color color::Black()
{
    return color(0, 0, 0);
}


color color::random()
{
    return color(random_double(), random_double(), random_double());
}

color color::random(double min, double max)
{
    return color(random_double(min, max), random_double(min, max), random_double(min, max));
}


double color::linear_to_gamma(double linear_component)
{
    return sqrt(linear_component);
}

color color::RGBtoHSV(color rgb)
{
    double max_val = ffmax(ffmax(rgb.r(), rgb.g()), rgb.b());
    double min_val = ffmin(ffmin(rgb.r(), rgb.g()), rgb.b());
    double delta_val = max_val - min_val;
    color hsv;

    if (delta_val > 0) {
        if (max_val == rgb.r()) {
            hsv.r(60 * (fmod(((rgb.g() - rgb.b()) / delta_val), 6)));
        }
        else if (max_val == rgb.g()) {
            hsv.r(60 * (((rgb.b() - rgb.r()) / delta_val) + 2));
        }
        else if (max_val == rgb.b()) {
            hsv.r(60 * (((rgb.r() - rgb.g()) / delta_val) + 4));
        }
        if (max_val > 0) {
            hsv.g(delta_val / max_val);
        }
        else {
            hsv.g(0);
        }
        hsv.b(max_val);
    }
    else {
        hsv.r(0);
        hsv.g(0);
        hsv.b(max_val);
    }
    if (hsv.r() < 0) {
        hsv.r(360 + hsv.r());
    }
    return(hsv);
}


color color::HSVtoRGB(color hsv)
{
    double chroma = hsv.b() * hsv.g();
    double fHPrime = fmod(hsv.r() / 60.0, 6);
    double x_val = chroma * (1 - std::fabs(fmod(fHPrime, 2) - 1));
    double m_val = hsv.b() - chroma;

    if (0 <= fHPrime && fHPrime < 1) {
        color rgb(chroma, x_val, 0);
        rgb += m_val;
        return(rgb);
    }
    else if (1 <= fHPrime && fHPrime < 2) {
        color rgb(x_val, chroma, 0);
        rgb += m_val;
        return(rgb);
    }
    else if (2 <= fHPrime && fHPrime < 3) {
        color rgb(0, chroma, x_val);
        rgb += m_val;
        return(rgb);
    }
    else if (3 <= fHPrime && fHPrime < 4) {
        color rgb(0, x_val, chroma);
        rgb += m_val;
        return(rgb);
    }
    else if (4 <= fHPrime && fHPrime < 5) {
        color rgb(x_val, 0, chroma);
        rgb += m_val;
        return(rgb);
    }
    else if (5 <= fHPrime && fHPrime < 6) {
        color rgb(chroma, 0, x_val);
        rgb += m_val;
        return(rgb);
    }
    else {
        color rgb(0, 0, 0);
        rgb += m_val;
        return(rgb);
    }
}



/// <summary>
/// Write pixel color to the output stream
/// </summary>
/// <param name="out"></param>
/// <param name="pixel_color"></param>
void color::write_color(std::ostream& out, color pixel_color)
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
void color::write_color(std::ostream& out, color pixel_color, int samples_per_pixel)
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