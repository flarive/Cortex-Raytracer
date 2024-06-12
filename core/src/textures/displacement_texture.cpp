#include "displacement_texture.h"

displacement_texture::displacement_texture()
{
}

displacement_texture::displacement_texture(std::shared_ptr<texture> t, double strength)
    : m_displacement(t), m_strength(strength)
{
}

color displacement_texture::value(double u, double v, const point3& p) const
{
    // Sample the underlying texture to get the displacement map
    color displacement_map = m_displacement->value(u, v, p);

    return displacement_map;
}

point3 displacement_texture::displace(const point3& point) const
{
    // Displace the point based on the texture
    double displacement = m_displacement->value(0, 0, point).length() * m_strength;
    return point + vector3(0, 0, displacement);
}

double displacement_texture::getStrenth()
{
    return m_strength;
}
