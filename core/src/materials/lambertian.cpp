#include "lambertian.h"


lambertian::lambertian(const color& a) : albedo(std::make_shared<solid_color_texture>(a))
{
}

lambertian::lambertian(std::shared_ptr<texture> a) : albedo(a)
{

}

bool lambertian::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
    srec.attenuation = albedo->value(rec.u, rec.v, rec.hit_point);
    srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal);
    srec.skip_pdf = false;
    return true;
}

double lambertian::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
    return cos_theta < 0 ? 0 : cos_theta / M_PI;
}