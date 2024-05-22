#include "bump_texture.h"

#include "../utilities/randomizer.h"

#include <glm/glm.hpp>

bump_texture::bump_texture()
{
}
	
bump_texture::bump_texture(std::shared_ptr<texture> bump, double scale) : m_bump(bump), m_scale(scale)
{
}

color bump_texture::value(double u, double v, const point3& p) const
{
    color c = m_bump->value(u, v, p);
    float bump = m_scale * (c.r() + c.g() + c.b()) / 3.0; // Simple bump value based on average color
    return color(bump, bump, bump);
}

vector3 bump_texture::perturb_normal(const vector3& normal, float u, float v, const vector3& p)
{
    color bumpValue = value(u, v, p);
    vector3 perturbedNormal = normal + vector3(bumpValue.r(), bumpValue.g(), bumpValue.b());
    return randomizer::unit_vector(perturbedNormal);
}