#pragma once

#include "texture.h"
#include "../misc/color.h"
#include "../utilities/types.h"

/// <summary>
/// Procedural checker texture
/// </summary>
class checker_texture : public texture
{
public:
    checker_texture(double _scale, std::shared_ptr<texture> _even, std::shared_ptr<texture> _odd);
    checker_texture(double _scale, color c1, color c2);

    color value(double u, double v, const point3& p) const override;

private:
    double m_scale = 0.0;
    double m_inv_scale = 0.0;
    std::shared_ptr<texture> m_even;
    std::shared_ptr<texture> m_odd;
};