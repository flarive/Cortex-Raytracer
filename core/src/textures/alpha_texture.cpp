#include "alpha_texture.h"

alpha_texture::alpha_texture(std::shared_ptr<texture> alpha) : m_alpha(alpha)
{
}

color alpha_texture::value(double u, double v, const point3& p) const
{
    return m_alpha->value(u, v, p);
}