#include "isotropic.h"


isotropic::isotropic(color c) : albedo(make_shared<solid_color_texture>(c))
{

}

isotropic::isotropic(shared_ptr<texture> a) : albedo(a)
{

}

bool isotropic::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
    srec.attenuation = albedo->value(rec.u, rec.v, rec.hit_point);
    srec.pdf_ptr = make_shared<sphere_pdf>();
    srec.skip_pdf = false;
    return true;
}

double isotropic::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    return 1 / (4 * M_PI);
}