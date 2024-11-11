#pragma once

#include "../utilities/types.h"
#include "texture.h"
#include "../misc/color.h"

/// <summary>
/// Alpha texture
/// </summary>
class alpha_texture : public texture
{
public:
    alpha_texture(std::shared_ptr<texture> alpha, bool double_sided);

    color value(double u, double v, const point3& p) const;

    bool is_double_sided() const;
    

private:
    std::shared_ptr<texture> m_alpha = nullptr;
    bool m_double_sided = false;
};