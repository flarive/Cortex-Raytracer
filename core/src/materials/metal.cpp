#include "metal.h"


metal::metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1)
{

}


bool metal::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
    srec.attenuation = albedo;
    srec.pdf_ptr = nullptr;
    srec.skip_pdf = true;
    vector3 reflected = glm::reflect(unit_vector(r_in.direction()), rec.normal);
    srec.skip_pdf_ray = ray(rec.hit_point, reflected + fuzz * random_in_unit_sphere(), r_in.time());
    return true;
}