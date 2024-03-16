#ifndef PERLIN_NOISE_TEXTURE_H
#define PERLIN_NOISE_TEXTURE_H

#include "texture.h"
#include "../misc/color.h"
#include "perlin.h"

/// <summary>
/// Perlin noise texture
/// </summary>
class perlin_noise_texture : public texture
{
public:
    perlin_noise_texture() {}

    perlin_noise_texture(double sc) : scale(sc) {}

    color value(double u, double v, const point3& p) const override
    {
        auto s = scale * p;
        return color(1, 1, 1) * 0.5 * (1 + sin(s.z + 10 * noise.turb(s)));
    }

private:
    perlin noise;
    double scale = 0.0;
};

#endif