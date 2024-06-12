#pragma once

#include "../utilities/types.h"
#include "../misc/color.h"
#include "texture.h"

/// <summary>
/// Image texture
/// </summary>
class normal_texture : public texture
{
public:
    normal_texture(std::shared_ptr<texture> normal, double strength = 10.0);

    color value(double u, double v, const point3& p) const override;

    double getStrenth();
private:
    std::shared_ptr<texture> m_normal;
    double m_strength = 10.0;
};