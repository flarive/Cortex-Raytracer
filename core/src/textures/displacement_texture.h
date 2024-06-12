#pragma once

#include "../utilities/types.h"
#include "texture.h"
#include "../misc/color.h"
#include "../utilities/bitmap_image.h"

/// <summary>
/// Displacement texture
/// </summary>
class displacement_texture : public texture
{
public:
    displacement_texture();

    displacement_texture(std::shared_ptr<texture> t, double strength);

    color value(double u, double v, const point3& p) const override;

    double getStrenth();

    // Modify the hit point based on the displacement map
    point3 displace(const point3& point) const;

private:
    std::shared_ptr<texture> m_displacement = nullptr;
    double m_strength = 10.0;
};