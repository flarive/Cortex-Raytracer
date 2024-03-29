#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../lights/light.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

#include <glm/glm.hpp>

class oren_nayar : public material
{
public:
	oren_nayar() :
		m_color(color::White()), m_diffuse(0.9), m_roughness(0.1)
	{}

	oren_nayar(const color& _color, double _diffuse, double _roughness) :
		m_color(_color), m_diffuse(_diffuse), m_roughness(_roughness)
	{}

	bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override
	{
		// Compute eye vector and hit point
		vector3 eyev = -r_in.direction();
		point3 point = rec.hit_point;
		auto normalv = rec.normal;

		// Compute effective color (no ambient term in Oren-Nayar)
		color effective_color = m_color;

		// Compute diffuse reflection
		color diffuse{ 0, 0, 0 };
		std::shared_ptr<light> mylight = std::dynamic_pointer_cast<light>(lights.objects[0]);
		if (mylight != nullptr)
		{
			vector3 lightv = glm::normalize(mylight->getPosition() - point);
			double cos_theta_i = glm::dot(lightv, normalv);
			double cos_theta_o = glm::dot(-eyev, normalv);

			double max_cos = std::max(0.0, cos_theta_i * cos_theta_o);
			double sin_theta_i = sqrt(std::max(0.0, 1 - cos_theta_i * cos_theta_i));
			double sin_theta_o = sqrt(std::max(0.0, 1 - cos_theta_o * cos_theta_o));

			double alpha = std::max(sin_theta_i, sin_theta_o);
			double beta = std::min(sin_theta_i, sin_theta_o);

			double sigma = m_roughness;

			double A = 1 - 0.5 * (sigma * sigma / (sigma * sigma + 0.33));
			double B = 0.45 * (sigma * sigma / (sigma * sigma + 0.09));

			diffuse = effective_color * m_diffuse * max_cos * (A + B * std::max(0.0, cos_theta_i * cos_theta_o) * alpha / beta);
		}

		// No specular reflection in Oren-Nayar
		color specular{ 0, 0, 0 };

		// Final color is the sum of diffuse and specular
		color final_color = diffuse + specular;

		srec.attenuation = final_color;
		srec.pdf_ptr = nullptr;
		srec.skip_pdf = true;

		return true;
	}

	double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override
	{
		// No need to calculate for Oren-Nayar
		return 0.0;
	}

public:
	color& getColor() { return m_color; }
	double& getDiffuse() { return m_diffuse; }
	double& getRoughness() { return m_roughness; }

private:
	color m_color;
	double m_diffuse;
	double m_roughness;
};
