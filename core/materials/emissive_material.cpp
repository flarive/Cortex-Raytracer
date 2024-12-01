#include "emissive_material.h"

#include "../textures/solid_color_texture.h"
#include "../pdf/cosine_pdf.h"

#include <glm/glm.hpp>

emissive_material::emissive_material(std::shared_ptr<texture> emissiveTex)
    : material(emissiveTex, nullptr, nullptr, nullptr, nullptr, nullptr, emissiveTex), m_intensity(1.0)
{
}

emissive_material::emissive_material(std::shared_ptr<texture> emissiveTex, double _intensity)
    : material(emissiveTex, nullptr, nullptr, nullptr, nullptr, nullptr, emissiveTex), m_intensity(_intensity)
{
}

emissive_material::emissive_material(color _c)
    : material(std::make_shared<solid_color_texture>(_c), nullptr, nullptr, nullptr, nullptr, nullptr, std::make_shared<solid_color_texture>(_c)), m_intensity(1.0)
{
}

emissive_material::emissive_material(color _c, double _intensity)
    : material(std::make_shared<solid_color_texture>(_c), nullptr, nullptr, nullptr, nullptr, nullptr, std::make_shared<solid_color_texture>(_c)), m_intensity(_intensity)
{
}



bool emissive_material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const
{
	srec.attenuation = m_diffuse_texture->value(rec.u, rec.v, rec.hit_point);
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
    return m_emissive_texture->value(u, v, p) * m_intensity;
}