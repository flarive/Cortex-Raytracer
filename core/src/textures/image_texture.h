#pragma once

#include "texture.h"
#include "../misc/color.h"
#include "../utilities/bitmap_image.h"

/// <summary>
/// Image texture
/// </summary>
class image_texture : public texture
{
public:
    image_texture(const char* filename);

    color value(double u, double v, const point3& p) const override;

    //vector3 interpolate(vector3 uv);


private:
    bitmap_image m_image;
};