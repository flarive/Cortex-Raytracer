#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../randomizers/randomize.h"

/// <summary>
/// Oren Nayar material
/// For natural surfaces, such as concrete, plaster, sand, etc.
/// </summary>
class oren_nayar_material : public material
{
public:
	oren_nayar_material(color _color);
	oren_nayar_material(color _color, float _albedo_temp, float _roughness);

	oren_nayar_material(std::shared_ptr<texture> _albedo);
	oren_nayar_material(std::shared_ptr<texture> _albedo, float _albedo_temp, float _roughness);

	bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const override;
	double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;



private:
	float m_roughness = 0.0f;
	float m_refractiveIndex = 0.0f;
	float m_reflectivity = 0.0f;
	float m_transparency = 0.0f;
	float m_emissivity = 0.0f;
	float m_specularity = 0.0f;
	float m_specularExponent = 0.0f;

	// ?????
	double m_albedo_temp;
};