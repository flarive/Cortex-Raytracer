#pragma once

#include "texture.h"
#include "../misc/color.h"

/// <summary>
/// Solid color texture
/// </summary>
class solid_color_texture : public texture
{
public:
    solid_color_texture(color c) : color_value(c) {}

    solid_color_texture(double red, double green, double blue) : solid_color_texture(color(red, green, blue)) {}

    color value(double u, double v, const point3& p) const override
    {
        return color_value;
    }

private:
    color color_value;
};