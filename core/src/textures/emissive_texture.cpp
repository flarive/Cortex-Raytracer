#include "emissive_texture.h"

#include "image_texture.h"

#include <glm/glm.hpp>

emissive_texture::emissive_texture()
{
}

emissive_texture::emissive_texture(std::shared_ptr<texture> bump, double strength) : m_emissive(bump), m_strength(strength)
{
}

color emissive_texture::value(double u, double v, const point3& p) const
{
	return m_emissive->value(u, v, p);
}