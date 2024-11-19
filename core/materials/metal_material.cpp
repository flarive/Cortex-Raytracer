#include "metal_material.h"

#include "../misc/singleton.h"




metal_material::metal_material(const color& _color, double _fuzz)
    : material(std::make_shared<solid_color_texture>(_color)),
    m_fuzz(glm::clamp(_fuzz, 0.0, 1.0)),
    m_specular_intensity(0),
    m_metal_tint(color(0,0,0)),
    m_anisotropy(0.0),
    m_fresnel_factor(0.0),
    m_heat(0.0)
{
}

metal_material::metal_material(std::shared_ptr<texture> _texture, double _fuzz)
    : material(_texture),
    m_fuzz(glm::clamp(_fuzz, 0.0, 1.0)),
    m_specular_intensity(0),
    m_metal_tint(color(0, 0, 0)),
    m_anisotropy(0.0),
    m_fresnel_factor(0.0),
    m_heat(0.0)
{
}





metal_material::metal_material(const color& _color, double _fuzz, double _specular_intensity,
    const color& _tint, double _anisotropy,
    double _fresnel_factor, double _heat)
    : material(std::make_shared<solid_color_texture>(_color)),
    m_fuzz(glm::clamp(_fuzz, 0.0, 1.0)),
    m_specular_intensity(glm::clamp(_specular_intensity, 0.0, 1.0)),
    m_metal_tint(_tint),
    m_anisotropy(glm::clamp(_anisotropy, 0.0, 1.0)),
    m_fresnel_factor(glm::clamp(_fresnel_factor, 0.0, 1.0)),
    m_heat(glm::clamp(_heat, 0.0, 1.0))
{
}

metal_material::metal_material(std::shared_ptr<texture> _texture, double _fuzz,
    double _specular_intensity, const color& _tint,
    double _anisotropy, double _fresnel_factor, double _heat)
    : material(_texture),
    m_fuzz(glm::clamp(_fuzz, 0.0, 1.0)),
    m_specular_intensity(glm::clamp(_specular_intensity, 0.0, 1.0)),
    m_metal_tint(_tint),
    m_anisotropy(glm::clamp(_anisotropy, 0.0, 1.0)),
    m_fresnel_factor(glm::clamp(_fresnel_factor, 0.0, 1.0)),
    m_heat(glm::clamp(_heat, 0.0, 1.0))
{
}
//
//bool metal_material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const
//{
//    // Base color and heat adjustments
//    color base_color = m_diffuse_texture->value(rec.u, rec.v, rec.hit_point);
//    color heat_adjusted_color = apply_heat(base_color);
//
//    // Fresnel effect
//    double cos_theta = glm::dot(-glm::normalize(r_in.direction()), rec.normal);
//    double fresnel = fresnel_reflection(cos_theta);
//
//    // Attenuation combines color, heat, and Fresnel
//    srec.attenuation = heat_adjusted_color * m_metal_tint * fresnel;
//
//    // Reflect the incoming ray
//    vector3 reflected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
//
//    // Add anisotropic fuzz
//    double temp = adjust_fuzz_for_heat();
//    vector3 fuzzed_reflection = reflected + anisotropic_fuzz(rec.normal, rnd) * vector3(temp, temp, temp);
//    fuzzed_reflection = glm::normalize(fuzzed_reflection);
//
//    // Scale reflection intensity
//    fuzzed_reflection *= m_specular_intensity;
//
//    // Scattered ray
//    srec.skip_pdf_ray = ray(rec.hit_point, fuzzed_reflection, r_in.time());
//
//    // No specific PDF is used for this material
//    srec.pdf_ptr = nullptr;
//    srec.skip_pdf = true;
//
//    // Only scatter if reflected ray is in the same hemisphere as the surface normal
//    return glm::dot(srec.skip_pdf_ray.direction(), rec.normal) > 0;
//}
//
//vector3 metal_material::anisotropic_fuzz(const vector3& normal, randomizer& rnd) const
//{
//    // Create tangent and bitangent for anisotropic fuzz
//    vector3 tangent = glm::normalize(glm::cross(normal, vector3(1, 0, 0)));
//    if (glm::length(tangent) < 1e-6) { // Handle degenerate cases
//        tangent = glm::normalize(glm::cross(normal, vector3(0, 1, 0)));
//    }
//    vector3 bitangent = glm::cross(normal, tangent);
//
//    // Generate random fuzz in tangent space
//    return m_fuzz * (tangent * rnd.get_real(-1.0, 1.0) + bitangent * rnd.get_real(-1.0, 1.0) * m_anisotropy);
//}
//
//double metal_material::fresnel_reflection(double cos_theta) const
//{
//    return m_fresnel_factor + (1.0 - m_fresnel_factor) * pow(1.0 - cos_theta, 5.0);
//}
//
//color metal_material::apply_heat(const color& base_color) const
//{
//    // Heat effect changes color towards a "heated" look (e.g., yellowish-red)
//    color heat_color = color(1.0, 0.5, 0.2); // Example "heated" color
//
//    // glm::mix only works with vector3, not color
//    vector3 temp = glm::mix(vector3(base_color.r(), base_color.g(), base_color.b()), vector3(heat_color.r(), heat_color.g(), heat_color.b()), vector3(m_heat, m_heat, m_heat));
//
//    return color(temp.x, temp.y, temp.z);
//}
//
//double metal_material::adjust_fuzz_for_heat() const
//{
//    // Heat increases surface roughness
//    double heat_fuzz = 0.5; // Maximum fuzz due to heat
//    return glm::mix(m_fuzz, heat_fuzz, m_heat);
//}




bool metal_material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const
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