#pragma once

#include "texture.h"
#include "../misc/color.h"
#include "perlin.h"


class marble_texture : public texture
{
public:
    marble_texture();
    marble_texture(double sc);

    virtual color value(double u, double v, const point3& p) const override;

public:
    perlin noise;
    double scale;
};