#include "glossy.h"

glossy::glossy(std::shared_ptr<texture>& a, std::shared_ptr<texture>& f) : albedo(a), fuzz(f)
{

}

bool glossy::scatter(const ray& ray_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
    vector3 reflected = reflect(unit_vector(ray_in.direction()), rec.normal);
    srec.attenuation = albedo->value(rec.u, rec.v, rec.hit_point);

    srec.skip_pdf = true;
    double fuzz_factor = (fuzz->value(rec.u, rec.v, rec.hit_point)).length();
    srec.skip_pdf_ray = ray(rec.hit_point, reflected + fuzz_factor * random_in_unit_sphere(), ray_in.time());

    srec.pdf_ptr = 0;
    return true;
}
