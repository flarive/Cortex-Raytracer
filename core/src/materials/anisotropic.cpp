#include "anisotropic.h"

anisotropic::anisotropic(color c, double roughness) : material(std::make_shared<solid_color_texture>(c)), m_roughness(roughness)
{
}

anisotropic::anisotropic(std::shared_ptr<texture> a, double roughness) : material(a), m_roughness(roughness)
{
}


bool anisotropic::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
    vector3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    // Adding some roughness to the reflected direction
    vector3 scatter_direction = reflected + m_roughness * random_in_unit_sphere();


    if (near_zero(scatter_direction))
        scatter_direction = rec.normal;


    srec.skip_pdf_ray = ray(rec.hit_point, scatter_direction);
    srec.attenuation = m_albedo->value(rec.u, rec.v, rec.hit_point);
    srec.skip_pdf = true;
    srec.pdf_ptr = nullptr; // Anisotropic materials don't use probability density functions for now
    return true;
}

double anisotropic::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    return 1 / (4 * M_PI);
}


