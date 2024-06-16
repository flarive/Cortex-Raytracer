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
    alpha_texture(std::shared_ptr<texture> alpha);

    color value(double u, double v, const point3& p) const;
    

private:
    std::shared_ptr<texture> m_alpha = nullptr;
};