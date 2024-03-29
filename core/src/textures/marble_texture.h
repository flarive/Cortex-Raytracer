#pragma once

#include "texture.h"
#include "../misc/color.h"
#include "perlin.h"


class marble_texture : public texture
{
public:
    marble_texture() {}
    marble_texture(double sc) : scale(sc) {}

    virtual color value(double u, double v, const point3& p) const override
    {
        return color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z + 10 * noise.turb(p)));
    }
public:
    perlin noise;
    double scale;
};