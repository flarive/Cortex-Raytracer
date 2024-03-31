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
	oren_nayar();
	oren_nayar(const color& _color, double _diffuse, double _roughness);

	bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override;
	double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;


public:
	color& getColor();
	double& getDiffuse();
	double& getRoughness();

private:
	color m_color;
	double m_diffuse;
	double m_roughness;
};
