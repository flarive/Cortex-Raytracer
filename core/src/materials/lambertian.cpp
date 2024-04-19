#include "lambertian.h"


lambertian::lambertian(const color& _color) : material(std::make_shared<solid_color_texture>(_color))
{
}

lambertian::lambertian(std::shared_ptr<texture> _albedo) : material(_albedo)
{
}

bool lambertian::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
    srec.attenuation = m_albedo->value(rec.u, rec.v, rec.hit_point);
    srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal);
    srec.skip_pdf = false;

    return true;


	// transparency ????????????? to test !!!!
	//srec.is_specular = false;
	//srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal);
	//srec.skip_pdf = false;

	//if (alpha < 1.0 && alpha > 0.0) {
	//	// If the material is partially transparent, the ray will be transmitted
	//	srec.specular_ray = ray(rec.hit_point, refract(unit_vector(r_in.direction()), rec.normal, 1.0 / refraction_index));
	//	srec.attenuation = color(1.0, 1.0, 1.0); // Fully transparent
	//}
	//else {
	//	// If the material is opaque, perform Lambertian reflection
	//	srec.attenuation = m_albedo->value(rec.u, rec.v, rec.hit_point);
	//}

	//return true;
}

double lambertian::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
    return cos_theta < 0 ? 0 : cos_theta / M_PI;
}