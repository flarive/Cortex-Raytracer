#include "diffuse_spot_light.h"

#include "../utilities/randomizer.h"

#include <glm/glm.hpp>

diffuse_spot_light::diffuse_spot_light(std::shared_ptr<texture> emitTex, point3 pos, vector3 dir, double cutoff, double falloff, double blur, double intensity, bool invisible) :
    m_emit(emitTex), m_position(pos), m_direction(dir), m_intensity(intensity), m_cutoff(cutoff),
    m_falloff(falloff), m_blur(blur), m_invisible(invisible)
{
}


color diffuse_spot_light::emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
{
    /*if (m_directional && !rec.front_face)
    {
        return m_invisible ? color(0, 0, 0, 0) : color(1, 1, 1, 0);
    }

    vector3 light_dir = randomizer::unit_vector(m_direction);
    vector3 hit_to_light = randomizer::unit_vector(r_in.direction());

    double cos_theta = glm::dot(hit_to_light, light_dir);
    if (cos_theta < m_cutoff)
    {
        return color(1, 1, 1, 1);
    }

    double attenuation = glm::pow(cos_theta, m_falloff);
    return m_emit->value(u, v, p) * (m_intensity * attenuation);*/

    if (m_directional && !rec.front_face)
    {
        return m_invisible ? color(0, 0, 0, 0) : color(1, 1, 1, 0);
    }

    vector3 light_dir = randomizer::unit_vector(m_direction);
    vector3 hit_to_light = randomizer::unit_vector(r_in.direction());

    double cos_theta = glm::dot(hit_to_light, light_dir);
    if (cos_theta < m_cutoff)
    {
        return color(1, 1, 1, 1);
    }

    // Calculate the shadow factor based on the specified blur
    double shadow_factor = 1.0 - m_blur; // Adjust this value as needed
    double attenuation = glm::pow(cos_theta, m_falloff) * shadow_factor;

    return m_emit->value(u, v, p) * (m_intensity * attenuation);
}
