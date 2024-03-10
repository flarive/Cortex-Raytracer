#ifndef SOLID_COLOR_TEXTURE_H
#define SOLID_COLOR_TEXTURE_H

#include "../constants.h"
#include "texture.h"
#include "../misc/color.h"
#include "../misc/point3.h"
#include "../stb_image.h"

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

#endif