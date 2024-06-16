#include "alpha_texture.h"

alpha_texture::alpha_texture(std::shared_ptr<texture> alpha) : m_alpha(alpha)
{
}

color alpha_texture::value(double u, double v, const point3& p) const
{
    color maskColor = m_alpha->value(u, v, p);


    if (maskColor.r() < 1.0)
    {
        int xx = 0;
    }

    return maskColor;
}