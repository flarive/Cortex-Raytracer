#include "metal.h"

metal::metal(const color& _color, double _fuzz) : material(std::make_shared<solid_color_texture>(_color)), m_fuzz(_fuzz < 1 ? _fuzz : 1)
{

}

bool metal::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, Random& random) const
{
    //srec.attenuation = m_color;
    srec.attenuation = m_albedo->value(rec.u, rec.v, rec.hit_point);
    srec.pdf_ptr = nullptr;
    srec.skip_pdf = true;
    vector3 reflected = glm::reflect(unit_vector(r_in.direction()), rec.normal);
    srec.skip_pdf_ray = ray(rec.hit_point, reflected + m_fuzz * random_in_unit_sphere(), r_in.time());
    return true;
}