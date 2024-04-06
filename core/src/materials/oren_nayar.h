#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../lights/light.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

/// <summary>
/// Oren Nayar material
/// For natural surfaces, such as concrete, plaster, sand, etc.
/// </summary>
class oren_nayar : public material
{
public:
	oren_nayar(color color);

	oren_nayar(color color, float albedo, float roughness);

	bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override;
	double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;

	color getSurfaceColor() const;



public:
	//color& getColor();
	//double& getAmbient();
	//double& getDiffuse();
	//double& getSpecular();
	//double& getShininess();


private:
	color surfaceColor{};
	float roughness = 0.0f;
	float refractiveIndex = 0.0f;
	float reflectivity = 0.0f;
	float transparency = 0.0f;
	float emissivity = 0.0f;
	float specularity = 0.0f;
	float specularExponent = 0.0f;

	// ?????
	float albedo;
};