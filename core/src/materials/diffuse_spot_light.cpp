#include "diffuse_spot_light.h"

#include "../utilities/randomizer.h"

#include <glm/glm.hpp>

diffuse_spot_light::diffuse_spot_light(std::shared_ptr<texture> emitTex, point3 pos, vector3 dir, double cosTotalWidth, double cosFalloffStart, double intensity, bool invisible) :
    m_emit(emitTex), m_position(pos), m_direction(dir), m_intensity(intensity), m_cutoff(cosTotalWidth),
    m_falloff(cosFalloffStart), m_invisible(invisible)
{
}


color diffuse_spot_light::emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
{
    if (m_directional && !rec.front_face)
    {
        return m_invisible ? color(0, 0, 0, 0) : color(1, 1, 1, 0);
    }

    vector3 light_dir = randomizer::unit_vector(m_direction);
    vector3 hit_to_light = randomizer::unit_vector(r_in.direction());

    double cos_theta = glm::dot(hit_to_light, light_dir);
    if (cos_theta < m_cutoff)
    {
        return color(0, 0, 0, 0);
    }

    double attenuation = glm::pow(cos_theta, m_falloff);
    return m_emit->value(u, v, p) * (m_intensity * attenuation);
}

//double diffuse_spot_light::falloff(const vector3& w) const
//{
//    double cosTheta = glm::dot(m_direction, randomizer::unit_vector(w));
//    if (cosTheta < m_cutoff) {
//        return(0);
//    }
//    if (cosTheta > m_falloff) {
//        return(1);
//    }
//    double delta = (cosTheta - m_cutoff) / (m_falloff - m_cutoff);
//    double aaa = ((delta * delta) * (delta * delta));
//
//    if (aaa > 0.0)
//    {
//        int a = 0;
//    }
//
//    return aaa;
//}

color diffuse_spot_light::get_albedo(const ray& r_in, const hit_record& rec) const
{
    return (m_emit->value(rec.u, rec.v, rec.hit_point) * m_intensity);
}

size_t diffuse_spot_light::getSize()
{
    return(sizeof(*this));
}