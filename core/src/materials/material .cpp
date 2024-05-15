#include "material.h"

material::material()
    : material(nullptr, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse)
    : material(_diffuse, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular)
    : material(_diffuse, _specular, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal)
    : material(_diffuse, _specular, _normal, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal, double transparency, double refractive_index)
    : m_diffuse_texture(_diffuse), m_specular_texture(_specular), m_normal_texture(_normal), m_transparency(transparency), m_refractiveIndex(refractive_index)
{
}

bool material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const
{
    return false;
}

double material::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    return 0;
}

color material::emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
{
    return color(0, 0, 0);
}