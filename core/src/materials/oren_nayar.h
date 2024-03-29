#pragma once

#include <algorithm>

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

#include <glm/glm.hpp>

/// <summary>
/// Oren Nayar material
/// </summary>
class oren_nayar : public material
{
public:
	oren_nayar(color color, float _roughness, float _emissivity, float _reflectivity, float _transparency, float _refractiveIndex, float _specularity, float _specularExponent) :
		surfaceColor(color), roughness(_roughness), emissivity(_emissivity), reflectivity(_reflectivity), transparency(_transparency), refractiveIndex(_refractiveIndex), specularity(_specularity), specularExponent(_specularExponent)
	{
	}

	oren_nayar(color color) : 
		surfaceColor(color), roughness(0.5f), emissivity(0.0f), reflectivity(0.0f), transparency(0.0f), refractiveIndex(1.0f), specularity(0.0f), specularExponent(75.0f)
	{
	}

	bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override
	{
		//	assert(glm::length(inDirection) < 1.0f + 10.0f * FLT_EPSILON && glm::length(inDirection) > 1.0f - 10.0f * FLT_EPSILON);
		//	assert(glm::length(outDirection) < 1.0f + 10.0f * FLT_EPSILON && glm::length(outDirection) > 1.0f - 10.0f * FLT_EPSILON);
		//	// See https://en.wikipedia.org/wiki/Oren-Nayar_reflectance_model for more information.
		//
		//	const float indot = glm::dot(-inDirection, normal);
		//	if (indot < FLT_EPSILON) {
		//		return glm::vec3(0);
		//	}
		//
		//	const float sigma2 = roughness * roughness;
		//
		//	const float A = 1 - 0.5f * sigma2 / (sigma2 + 0.57f);
		//	const float B = 0.45f * sigma2 / (sigma2 + 0.09f);
		//
		//	const float alphaInclination = acos(indot);
		//	const float betaInclination = acos(glm::dot(outDirection, normal));
		//
		//	const float alpha = std::max(alphaInclination, betaInclination);
		//	const float beta = std::min(alphaInclination, betaInclination);
		//
		//	const float C = glm::max<float>(0.0f, glm::dot(-inDirection, outDirection));
		//	const float D = (C > 0.0f) ? B * C * sin(alpha) * tan(beta) : 0.0f;
		//
		//	return glm::max(0.0f, indot) * glm::min<float>((A + D), 1.0f) * (incomingRadiance * surfaceColor);

		return true;
	}

	color getSurfaceColor() const
	{
		return surfaceColor;
	}

private:
	color surfaceColor{};
	float roughness = 0.0f;
	float refractiveIndex = 0.0f;
	float reflectivity = 0.0f;
	float transparency = 0.0f;
	float emissivity = 0.0f;
	float specularity = 0.0f;
	float specularExponent = 0.0f;
};



//glm::vec3 OrenNayarMaterial::CalculateDiffuseLighting(const glm::vec3& inDirection,
//	const glm::vec3& outDirection,
//	const glm::vec3& normal,
//	const glm::vec3& incomingRadiance) const {
//	assert(glm::length(inDirection) < 1.0f + 10.0f * FLT_EPSILON && glm::length(inDirection) > 1.0f - 10.0f * FLT_EPSILON);
//	assert(glm::length(outDirection) < 1.0f + 10.0f * FLT_EPSILON && glm::length(outDirection) > 1.0f - 10.0f * FLT_EPSILON);
//	// See https://en.wikipedia.org/wiki/Oren-Nayar_reflectance_model for more information.
//
//	const float indot = glm::dot(-inDirection, normal);
//	if (indot < FLT_EPSILON) {
//		return glm::vec3(0);
//	}
//
//	const float sigma2 = roughness * roughness;
//
//	const float A = 1 - 0.5f * sigma2 / (sigma2 + 0.57f);
//	const float B = 0.45f * sigma2 / (sigma2 + 0.09f);
//
//	const float alphaInclination = acos(indot);
//	const float betaInclination = acos(glm::dot(outDirection, normal));
//
//	const float alpha = std::max(alphaInclination, betaInclination);
//	const float beta = std::min(alphaInclination, betaInclination);
//
//	const float C = glm::max<float>(0.0f, glm::dot(-inDirection, outDirection));
//	const float D = (C > 0.0f) ? B * C * sin(alpha) * tan(beta) : 0.0f;
//
//	return glm::max(0.0f, indot) * glm::min<float>((A + D), 1.0f) * (incomingRadiance * surfaceColor);
//}