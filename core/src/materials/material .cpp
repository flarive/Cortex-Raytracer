#include "material.h"

material::material()
    : material(nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _albedo)
    : material(_albedo, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _albedo, std::shared_ptr<texture> _normal, double transparency, double refractive_index)
    : m_albedo(_albedo), m_normal(_normal), m_transparency(transparency), m_refractiveIndex(refractive_index)
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