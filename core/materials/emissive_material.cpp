#include "emissive_material.h"

#include "../textures/solid_color_texture.h"
#include "../pdf/cosine_pdf.h"

#include <glm/glm.hpp>

emissive_material::emissive_material(std::shared_ptr<texture> emissiveTex)
    : m_emissive(emissiveTex), m_intensity(1.0)
{
}

emissive_material::emissive_material(std::shared_ptr<texture> emissiveTex, double _intensity)
    : m_emissive(emissiveTex), m_intensity(_intensity)
{
}

emissive_material::emissive_material(color _c)
    : m_emissive(std::make_shared<solid_color_texture>(_c)), m_intensity(1.0)
{
}

emissive_material::emissive_material(color _c, double _intensity)
    : m_emissive(std::make_shared<solid_color_texture>(_c)), m_intensity(_intensity)
{
}



bool emissive_material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const
{
	//// Check if the material is transparent (e.g., glass)
	//if (m_transparency > 0)
	//{
	//	// Compute the refracted ray direction
	//	vector3 refracted_direction = glm::refract(r_in.direction(), rec.normal, m_refractiveIndex);
	//	//srec.attenuation = color(1.0, 1.0, 1.0); // Fully transparent
	//	srec.attenuation = m_diffuse_texture->value(rec.u, rec.v, rec.hit_point) * color(m_transparency);
	//	srec.skip_pdf = true;
	//	srec.skip_pdf_ray = ray(rec.hit_point, refracted_direction, r_in.time());
	//	return true;

	//	// Total internal reflection (TIR)
	//	// Handle this case if needed
	//}


	//srec.attenuation = m_diffuse_texture->value(rec.u, rec.v, rec.hit_point);
	srec.attenuation = m_emissive->value(rec.u, rec.v, rec.hit_point);
	srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal);
	srec.skip_pdf = false;

	return true;
}

double emissive_material::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
	auto cos_theta = glm::dot(rec.normal, unit_vector(scattered.direction()));
	return cos_theta < 0 ? 0 : cos_theta / M_PI;
}

color emissive_material::emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
{
    // Material emission
    return m_emissive->value(u, v, p) * m_intensity;
}