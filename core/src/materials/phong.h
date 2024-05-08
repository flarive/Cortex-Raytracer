#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../lights/light.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../textures/solid_color_texture.h"

/// <summary>
/// Phong material
/// </summary>
class phong : public material
{
public:
	phong();
	phong(const color& _color);
	phong(const color& _color, double _ambient, double _diffuse, double _specular, double _shininess);
	phong(const color& _color, double _ambient, double _diffuse, double _specular, double _shininess, double _transparency, double _refraction_index);

	phong(std::shared_ptr<texture> _albedo);
	phong(std::shared_ptr<texture> _albedo, double _ambient, double _diffuse, double _specular, double _shininess);
	phong(std::shared_ptr<texture> _albedo, double _ambient, double _diffuse, double _specular, double _shininess, double _transparency, double _refraction_index);

	bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, Random& random) const override;
	double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;
   

public:
	double& getAmbient();
	double& getDiffuse();
	double& getSpecular();
	double& getShininess();


private:
    double m_ambient;
    double m_diffuse;
    double m_specular;
    double m_shininess;

	static bool refract(const vector3& incident, const vector3& normal, double refractive_index, vector3& refracted);
};