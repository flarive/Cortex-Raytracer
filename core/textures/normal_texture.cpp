#include "normal_texture.h"

#include "../misc/color.h"

#include <glm/glm.hpp>

normal_texture::normal_texture(std::shared_ptr<texture> normal, double strength) : m_normal(normal), m_strength(strength)
{
}

color normal_texture::value(double u, double v, const point3& p) const
{
    // Sample the underlying texture to get the normal map
    color normal_map = m_normal->value(u, v, p);

    // Scale from [0, 1] to [-1, 1]
    vector3 normal;
    normal.x = 2.0 * normal_map.r() - 1.0;
    normal.y = 2.0 * normal_map.g() - 1.0;
    normal.z = 2.0 * normal_map.b() - 1.0;

    // Normalize the resulting vector
    normal = glm::normalize(normal);

    // Convert normalized vector back to color for output
    return color(normal.x, normal.y, normal.z);
}

double normal_texture::getStrenth()
{
    return m_strength;
}
