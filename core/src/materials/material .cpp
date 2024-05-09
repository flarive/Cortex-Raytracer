#include "material.h"

material::material() : m_albedo(nullptr)
{
}

material::material(std::shared_ptr<texture> _albedo)
    : m_albedo(_albedo)
{
}

material::material(std::shared_ptr<texture> _albedo, double transparency, double refractive_index)
    : m_albedo(_albedo), m_transparency(transparency), m_refractiveIndex(refractive_index)
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