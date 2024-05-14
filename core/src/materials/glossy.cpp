#include "glossy.h"

glossy::glossy(std::shared_ptr<texture>& a)
    : material(a), m_fuzz(nullptr)
{
}

glossy::glossy(std::shared_ptr<texture>& a, std::shared_ptr<texture>& f)
    : material(a), m_fuzz(f)
{
}

bool glossy::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const
{
    vector3 reflected = reflect(randomizer::unit_vector(r_in.direction()), rec.normal);
    srec.attenuation = m_diffuse_texture->value(rec.u, rec.v, rec.hit_point);

    srec.skip_pdf = true;

    double fuzz_factor = 0.0;
    if (m_fuzz)
    {
        double fuzz_factor = (m_fuzz->value(rec.u, rec.v, rec.hit_point)).length();
    }

    srec.skip_pdf_ray = ray(rec.hit_point, reflected + fuzz_factor * randomizer::random_in_unit_sphere(), r_in.time());

    srec.pdf_ptr = 0;
    return true;
}