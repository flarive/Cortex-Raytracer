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

    int getWidth() const;
    int getHeight() const;
    unsigned char* get_data() const;
    float* get_data_float() const;
private:
    bitmap_image m_image;
};