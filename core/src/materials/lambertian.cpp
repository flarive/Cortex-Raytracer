#include "lambertian.h"

#include "../textures/solid_color_texture.h"
#include "../pdf/cosine_pdf.h"

#include <glm/glm.hpp>


lambertian::lambertian(const color& _color)
	: material(std::make_shared<solid_color_texture>(_color))
{
}

lambertian::lambertian(const color& _color, double _transparency, double _refraction_index)
	: material(std::make_shared<solid_color_texture>(_color), nullptr, nullptr, nullptr, nullptr, _transparency, _refraction_index)
{
}

lambertian::lambertian(std::shared_ptr<texture> _albedo)
	: material(_albedo)
{
}

lambertian::lambertian(std::shared_ptr<texture> _albedo, double _transparency, double _refraction_index)
	: material(_albedo, nullptr, nullptr, nullptr, nullptr, _transparency, _refraction_index)
{
}

bool lambertian::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const
{
	//if (rec.is_shadowed)
	//{
	//	srec.attenuation = color::green();
	//	srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal);
	//	srec.skip_pdf = false;
	//	return true;
	//}
	
	// Check if the material is transparent (e.g., glass)
	if (m_transparency > 0)
	{
		// Compute the refracted ray direction
		vector3 refracted_direction = glm::refract(r_in.direction(), rec.normal, m_refractiveIndex);
		//srec.attenuation = color(1.0, 1.0, 1.0); // Fully transparent
		srec.attenuation = m_diffuse_texture->value(rec.u, rec.v, rec.hit_point) * color(m_transparency);
		srec.skip_pdf = true;
		srec.skip_pdf_ray = ray(rec.hit_point, refracted_direction, r_in.time());
		return true;

		// Total internal reflection (TIR)
		// Handle this case if needed
	}

	
	srec.attenuation = m_diffuse_texture->value(rec.u, rec.v, rec.hit_point);
    srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal);
    srec.skip_pdf = false;


	// ???????????????????????????????
	//if (srec.attenuation.a() < 1.0)
	//{
	//    return false;
	//}

    return true;
}

double lambertian::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    auto cos_theta = glm::dot(rec.normal, randomizer::unit_vector(scattered.direction()));
    return cos_theta < 0 ? 0 : cos_theta / M_PI;
}