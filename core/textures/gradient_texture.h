#pragma once

#include "texture.h"
#include "../misc/color.h"
#include "../utilities/types.h"


class gradient_texture : public texture
{
public:
    gradient_texture();
    gradient_texture(color c1, color c2, bool v, bool hsv2);

    virtual color value(double u, double v, const point3& p) const;

private:
    color gamma_color1{}, gamma_color2{};
    bool aligned_v = false;
    bool hsv = false;
};