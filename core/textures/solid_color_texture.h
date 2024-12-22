#pragma once

#include "../utilities/types.h"
#include "texture.h"
#include "../misc/color.h"

/// <summary>
/// Solid color texture
/// </summary>
class solid_color_texture : public texture
{
public:
    //solid_color_texture(color c);

    solid_color_texture(const color& c);

    solid_color_texture(double red, double green, double blue);

    color value(double u, double v, const point3& p) const override;

    color get_color() const;

private:
    color m_color_value{};
};