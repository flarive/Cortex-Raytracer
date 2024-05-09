#pragma once

#include "texture.h"
#include "../misc/color.h"
#include "../utilities/bitmap_image.h"

/// <summary>
/// Image texture
/// </summary>
class normal_texture : public texture
{
public:
    normal_texture(std::shared_ptr<texture> normal);

    color value(double u, double v, const point3& p) const override;
private:
    std::shared_ptr<texture> m_normal;
};

