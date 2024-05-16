#pragma once

#include "../utilities/types.h"
#include "texture.h"
#include "../misc/color.h"
#include "../utilities/bitmap_image.h"

/// <summary>
/// Image texture
/// </summary>
class image_texture : public texture
{
public:
    image_texture(const std::string filepath);

    color value(double u, double v, const point3& p) const override;
private:
    bitmap_image m_image;
};