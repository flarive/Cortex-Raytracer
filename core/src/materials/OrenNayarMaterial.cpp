#include "OrenNayarMaterial.h"

#include <glm/glm.hpp>

OrenNayarMaterial::OrenNayarMaterial(color color, float _roughness, float _emissivity, float _reflectivity, float _transparency, float _refractiveIndex, float _specularity, float _specularExponent) :
	surfaceColor(color), roughness(_roughness), emissivity(_emissivity), reflectivity(_reflectivity), transparency(_transparency), refractiveIndex(_refractiveIndex), specularity(_specularity), specularExponent(_specularExponent)
{
}

OrenNayarMaterial::OrenNayarMaterial(color color) :
	surfaceColor(color), roughness(0.5f), emissivity(0.0f), reflectivity(0.0f), transparency(0.0f), refractiveIndex(1.0f), specularity(0.0f), specularExponent(75.0f)
{
}

bool OrenNayarMaterial::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
	vector3 inDirection = -rec.normal;
	vector3 outDirection = -r_in.direction();
	vector3 normal = rec.normal;

	// just take the first light for the moment
	std::shared_ptr<light> mylight = std::dynamic_pointer_cast<light>(lights.objects[0]);
	if (mylight == nullptr)
	{
		// no light
		return false;
	}

	// Combine the surface color with the light's color/intensity
	color incomingIntensity = mylight->getColor() * mylight->getIntensity();




	assert(glm::length(inDirection) < 1.0f + 10.0f * FLT_EPSILON && glm::length(inDirection) > 1.0f - 10.0f * FLT_EPSILON);
	assert(glm::length(outDirection) < 1.0f + 10.0f * FLT_EPSILON && glm::length(outDirection) > 1.0f - 10.0f * FLT_EPSILON);
	// See https://en.wikipedia.org/wiki/Oren-Nayar_reflectance_model for more information.
		
	const float indot = glm::dot(-inDirection, normal);
	if (indot < FLT_EPSILON) {
		return false;
	}
		
	const float sigma2 = roughness * roughness;
		
	const float A = 1 - 0.5f * sigma2 / (sigma2 + 0.57f);
	const float B = 0.45f * sigma2 / (sigma2 + 0.09f);
		
	const float alphaInclination = acos(indot);
	const float betaInclination = acos(glm::dot(outDirection, normal));
		
	const float alpha = std::max(alphaInclination, betaInclination);
	const float beta = std::min(alphaInclination, betaInclination);
		
	const float C = glm::max<float>(0.0f, glm::dot(-inDirection, outDirection));
	const float D = (C > 0.0f) ? B * C * sin(alpha) * tan(beta) : 0.0f;
		
	color final_color = glm::max(0.0f, indot) * glm::min<float>((A + D), 1.0f) * (incomingIntensity * surfaceColor);


	srec.attenuation = final_color;
	//srec.pdf_ptr = nullptr;
	//srec.skip_pdf = true;
	//srec.skip_pdf_ray = ray(rec.hit_point, random_in_unit_sphere(), r_in.time());
	srec.pdf_ptr = std::make_shared<sphere_pdf>();
	srec.skip_pdf = false;

	return true;
}

double OrenNayarMaterial::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
	auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
	return cos_theta < 0 ? 0 : cos_theta / M_PI;
}

color OrenNayarMaterial::getSurfaceColor() const
{
	return surfaceColor;
}
