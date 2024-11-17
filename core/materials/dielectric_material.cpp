//#include "dielectric_material.h"
//
//#include "../misc/singleton.h"
//
//
//dielectric_material::dielectric_material(double index_of_refraction) : ir(index_of_refraction)
//{
//}
//
//bool dielectric_material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const
//{
//    srec.attenuation = color(1.0, 1.0, 1.0);
//    srec.pdf_ptr = nullptr;
//    srec.skip_pdf = true;
//    double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;
//
//    vector3 unit_direction = unit_vector(r_in.direction());
//    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
//    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
//
//    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
//    vector3 direction;
//
//    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > rnd.get_real(0, 1))
//        direction = glm::reflect(unit_direction, rec.normal);
//    else
//        direction = glm::refract(unit_direction, rec.normal, refraction_ratio);
//
//    srec.skip_pdf_ray = ray(rec.hit_point, direction, r_in.time());
//    return true;
//}
//
//
//// Static methods gets constructed only once no matter how many times the function is called.
//double dielectric_material::reflectance(double cosine, double ref_idx)
//{
//    // Use Schlick's approximation for reflectance.
//    auto r0 = (1 - ref_idx) / (1 + ref_idx);
//    r0 = r0 * r0;
//    return r0 + (1 - r0) * pow((1 - cosine), 5);
//}



#include "dielectric_material.h"

#include "../misc/singleton.h"

dielectric_material::dielectric_material(double index_of_refraction)
    : ir(index_of_refraction)
{
}

dielectric_material::dielectric_material(double index_of_refraction, std::shared_ptr<texture> texture_attenuation)
    : ir(index_of_refraction), attenuation_texture(texture_attenuation)
{
}

bool dielectric_material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const
{
    // Use texture or default color (white) for attenuation
    if (attenuation_texture) {
        srec.attenuation = attenuation_texture->value(rec.u, rec.v, rec.hit_point);
    }
    else {
        srec.attenuation = color(1.0, 1.0, 1.0);
    }

    srec.pdf_ptr = nullptr;
    srec.skip_pdf = true;

    double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

    vector3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    vector3 direction;

    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > rnd.get_real(0, 1))
        direction = glm::reflect(unit_direction, rec.normal);
    else
        direction = glm::refract(unit_direction, rec.normal, refraction_ratio);

    srec.skip_pdf_ray = ray(rec.hit_point, direction, r_in.time());
    return true;
}

double dielectric_material::reflectance(double cosine, double ref_idx)
{
    // Use Schlick's approximation for reflectance
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

