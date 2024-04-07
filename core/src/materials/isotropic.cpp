#include "isotropic.h"


isotropic::isotropic(color _color) : material(_color, std::make_shared<solid_color_texture>(_color))
{

}

isotropic::isotropic(std::shared_ptr<texture> _albedo) : material(color::undefined(), _albedo)
{

}

bool isotropic::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
    srec.attenuation = m_albedo->value(rec.u, rec.v, rec.hit_point);
    srec.pdf_ptr = std::make_shared<sphere_pdf>();
    srec.skip_pdf = false;
    return true;
}

double isotropic::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    return 1 / (4 * M_PI);
}