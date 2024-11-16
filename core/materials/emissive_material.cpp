#include "emissive_material.h"

#include "../textures/solid_color_texture.h"

#include <glm/glm.hpp>

emissive_material::emissive_material(std::shared_ptr<texture> a)
    : m_emit(a), m_intensity(1.0)
{
}

emissive_material::emissive_material(std::shared_ptr<texture> a, double _intensity)
    : m_emit(a), m_intensity(_intensity)
{
}

emissive_material::emissive_material(color _c)
    : m_emit(std::make_shared<solid_color_texture>(_c)), m_intensity(1.0)
{
}

emissive_material::emissive_material(color _c, double _intensity)
    : m_emit(std::make_shared<solid_color_texture>(_c)), m_intensity(_intensity)
{
}

color emissive_material::emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
{
    // Material emission
    return m_emit->value(u, v, p) * m_intensity;
}