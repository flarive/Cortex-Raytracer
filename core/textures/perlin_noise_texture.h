#pragma once

#include "texture.h"
#include "../utilities/types.h"
#include "../misc/color.h"
#include "perlin.h"

/// <summary>
/// Perlin noise texture
/// </summary>
class perlin_noise_texture : public texture
{
public:
    perlin_noise_texture();
    perlin_noise_texture(double sc);

    color value(double u, double v, const point3& p) const override;

private:
    perlin noise;
    double scale = 0.0;
};