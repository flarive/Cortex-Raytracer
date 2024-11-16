#include "isotropic_material.h"

#include "../pdf/sphere_pdf.h"

isotropic_material::isotropic_material(color _color) : material(std::make_shared<solid_color_texture>(_color))
{
}

isotropic_material::isotropic_material(std::shared_ptr<texture> _albedo) : material(_albedo)
{
}

bool isotropic_material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const
{
    srec.attenuation = m_diffuse_texture->value(rec.u, rec.v, rec.hit_point);
    srec.pdf_ptr = std::make_shared<sphere_pdf>();
    srec.skip_pdf = false;

    return true;
}

double isotropic_material::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    return 1 / (4 * M_PI);
}