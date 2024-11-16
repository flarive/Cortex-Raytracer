#include "emissive_texture.h"

#include "image_texture.h"

#include <glm/glm.hpp>

// Default constructor
emissive_texture::emissive_texture()
    : m_emissive(nullptr), m_strength(10.0)
{
}

// Parameterized constructor
emissive_texture::emissive_texture(std::shared_ptr<texture> emissive, double strength)
    : m_emissive(emissive), m_strength(strength)
{
}

// Value function
color emissive_texture::value(double u, double v, const point3& p) const
{
    if (m_emissive)
    {
        // Scale the emissive base texture value by the strength
        return m_strength * m_emissive->value(u, v, p);
    }
    // Default emissive color (e.g., black if no base texture)
    return color(0, 0, 0);
}
