#pragma once

#include "texture.h"
#include "../misc/color.h"
#include "../utilities/types.h"


class gradient_texture : public texture
{
public:
    gradient_texture() {}

    gradient_texture(color c1, color c2, bool v, bool hsv2) :
        aligned_v(v) {
        gamma_color1 = hsv2 ? color::RGBtoHSV(c1) : c1;
        gamma_color2 = hsv2 ? color::RGBtoHSV(c2) : c2;
        hsv = hsv2;
    }

    virtual color value(double u, double v, const point3& p) const
    {
        color final_color = aligned_v ? gamma_color1 * (1 - u) + u * gamma_color2 : gamma_color1 * (1 - v) + v * gamma_color2;

        return (hsv ? color::HSVtoRGB(final_color) : final_color);
    }

private:
    color gamma_color1, gamma_color2;
    bool aligned_v;
    bool hsv;
};