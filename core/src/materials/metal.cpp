#include "metal.h"

#include "../misc/singleton.h"

//metal::metal(const color& _color, double _fuzz) : material(std::make_shared<solid_color_texture>(_color)), m_fuzz(_fuzz < 1 ? _fuzz : 1)
//{
//}
//
//metal::metal(std::shared_ptr<texture> _texture, double _fuzz) : material(_texture), m_fuzz(_fuzz < 1 ? _fuzz : 1)
//{
//}


// Constructors
metal::metal(const color& _color, double _fuzz)
    : material(std::make_shared<solid_color_texture>(_color)), m_fuzz(glm::clamp(_fuzz, 0.0, 1.0)) {}

metal::metal(std::shared_ptr<texture> _texture, double _fuzz)
    : material(_texture), m_fuzz(glm::clamp(_fuzz, 0.0, 1.0)) {}




bool metal::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const
{
    // Get the material's color at the hit point
    srec.attenuation = m_diffuse_texture->value(rec.u, rec.v, rec.hit_point);

    // No specific PDF is used for metal materials
    srec.pdf_ptr = nullptr;
    srec.skip_pdf = true;

    // Reflect the incoming ray and add fuzz
    vector3 reflected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
    vector3 fuzzed_reflection = reflected + m_fuzz * rnd.get_in_unit_sphere();

    // Create a scattered ray based on the reflection and fuzziness
    srec.skip_pdf_ray = ray(rec.hit_point, fuzzed_reflection, r_in.time());

    // Return true only if the scattered ray is in the same hemisphere as the surface normal
    return glm::dot(srec.skip_pdf_ray.direction(), rec.normal) > 0;
}