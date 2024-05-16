#pragma once

#include "../utilities/types.h"
#include "texture.h"
#include "../misc/color.h"

static constexpr double rescale = 1.f / 255.f;

/// <summary>
/// Alpha texture (NOT TESTED !!!!!!!!!)
/// </summary>
class alpha_texture : public texture
{
public:
    alpha_texture();
    alpha_texture(unsigned char* pixels, int _width, int _height, int _bit_per_pixel);

    color value(double u, double v, const point3& p) const;
    

private:
    unsigned char* data;
    int m_width = 0, m_height = 0, m_channels = 0;
};