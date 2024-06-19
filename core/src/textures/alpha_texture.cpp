#include "alpha_texture.h"

alpha_texture::alpha_texture(std::shared_ptr<texture> alpha, bool double_sided = false) : m_alpha(alpha), m_double_sided(double_sided)
{
}

color alpha_texture::value(double u, double v, const point3& p) const
{
    return m_alpha->value(u, v, p);
}