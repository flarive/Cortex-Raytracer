#include "color.h"
#include "../utilities/interval.h"

color::color() : c{ 0, 0, 0, 1 } {}

color::color(double c0) : c{ c0, c0, c0, 1 } {}

color::color(double c0, double c1, double c2) : c{ c0, c1, c2, 1 } {}

color::color(double c0, double c1, double c2, double c3) : c{ c0, c1, c2, c3 } {}

double color::r() const { return c[0]; }
double color::g() const { return c[1]; }
double color::b() const { return c[2]; }
double color::a() const { return c[3]; }

void color::r(double r) { c[0] = r; }
void color::g(double g) { c[1] = g; }
void color::b(double b) { c[2] = b; }
void color::a(double a) { c[3] = a; }

color color::operator-() const {
    return color(-c[0], -c[1], -c[2], c[3]);
}

double color::operator[](int i) const {
    return c[i];
}

double& color::operator[](int i) {
    return c[i];
}

color& color::operator+=(const color& v) {
    if (v.c[3] == 0) return *this;

    c[0] += v.c[0];
    c[1] += v.c[1];
    c[2] += v.c[2];
    c[3] = std::min(c[3] + v.c[3], 1.0);

    return *this;
}

color& color::operator+=(double t) {
    c[0] += t;
    c[1] += t;
    c[2] += t;
    return *this;
}

color& color::operator*=(double t) {
    c[0] *= t;
    c[1] *= t;
    c[2] *= t;
    return *this;
}

color& color::operator*=(const color& v) {
    c[0] *= v[0];
    c[1] *= v[1];
    c[2] *= v[2];
    return *this;
}

color& color::operator/=(double t) {
    return *this *= 1.0 / t;
}

double color::length() const {
    return std::sqrt(length_squared());
}

double color::length_squared() const {
    return c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
}

const color color::white() {
    return color(1, 1, 1);
}

const color color::black() {
    return color(0, 0, 0);
}

const color color::red() {
    return color(1, 0, 0);
}

const color color::green() {
    return color(0, 1, 0);
}

const color color::blue() {
    return color(0, 0, 1);
}

const color color::yellow() {
    return color(1, 1, 0);
}

const color color::undefined() {
    return color(-1, -1, -1);
}

double color::linear_to_gamma(double linear_component) {
    return std::sqrt(linear_component);
}

color color::blend_colors(const color& front, const color& back, double alpha) {
    return alpha * front + (1.0 - alpha) * back;
}

color color::blend_with_background(const color& background, const color& object_color, float alpha) {
    return (1.0f - alpha) * background + alpha * object_color;
}

color color::RGBtoHSV(color rgb) {
    double max_val = ffmax(ffmax(rgb.r(), rgb.g()), rgb.b());
    double min_val = ffmin(ffmin(rgb.r(), rgb.g()), rgb.b());
    double delta_val = max_val - min_val;
    color hsv(0, delta_val > 0 ? delta_val / max_val : 0, max_val);

    if (delta_val > 0) {
        if (max_val == rgb.r()) {
            hsv.r(60 * std::fmod((rgb.g() - rgb.b()) / delta_val, 6));
        }
        else if (max_val == rgb.g()) {
            hsv.r(60 * ((rgb.b() - rgb.r()) / delta_val + 2));
        }
        else if (max_val == rgb.b()) {
            hsv.r(60 * ((rgb.r() - rgb.g()) / delta_val + 4));
        }
        if (hsv.r() < 0) hsv.r(hsv.r() + 360);
    }

    return hsv;
}

color color::HSVtoRGB(color hsv) {
    double chroma = hsv.b() * hsv.g();
    double fHPrime = std::fmod(hsv.r() / 60.0, 6);
    double x_val = chroma * (1 - std::fabs(std::fmod(fHPrime, 2) - 1));
    double m_val = hsv.b() - chroma;

    color rgb;

    if (0 <= fHPrime && fHPrime < 1) {
        rgb = color(chroma, x_val, 0);
    }
    else if (1 <= fHPrime && fHPrime < 2) {
        rgb = color(x_val, chroma, 0);
    }
    else if (2 <= fHPrime && fHPrime < 3) {
        rgb = color(0, chroma, x_val);
    }
    else if (3 <= fHPrime && fHPrime < 4) {
        rgb = color(0, x_val, chroma);
    }
    else if (4 <= fHPrime && fHPrime < 5) {
        rgb = color(x_val, 0, chroma);
    }
    else if (5 <= fHPrime && fHPrime < 6) {
        rgb = color(chroma, 0, x_val);
    }
    else {
        rgb = color(0, 0, 0);
    }

    rgb += m_val;
    return rgb;
}

bool color::isValidColor() {
    return c[0] >= 0 && c[1] >= 0 && c[2] >= 0 && c[3] >= 0;
}

color color::prepare_pixel_color(int x, int y, color pixel_color, int samples_per_pixel, bool gamma_correction) {
    double r = std::isnan(pixel_color.r()) ? 0.0 : pixel_color.r();
    double g = std::isnan(pixel_color.g()) ? 0.0 : pixel_color.g();
    double b = std::isnan(pixel_color.b()) ? 0.0 : pixel_color.b();

    if (samples_per_pixel > 0) {
        double scale = 1.0 / samples_per_pixel;
        r *= scale;
        g *= scale;
        b *= scale;
    }

    if (gamma_correction) {
        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);
    }

    return color(r, g, b);
}
