#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../lights/light.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

/// <summary>
/// Phong material
//ici ton material n'a que des informations sur la couleur 
//pas sur ses capacité de diffusion / reflexion
//https://github.com/kdridi/raytracer/blob/main/raytracer/src/material.cpp
//Color Material::lighting(const Shape & shape, const PointLight & light, const Point & point, const Vector & eyev, const Vector & normalv, bool inShadow) const
//return ambient + diffuse + specular;
//pour calculer la lumière il faut mixer ces trois composantes
/// </summary>
class phong : public material
{
public:
	phong();
	phong(const color& _color);
	phong(const color& _color, double _ambient, double _diffuse, double _specular, double _shininess);

	bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override;
	double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;
   

public:
	color& getColor();
	double& getAmbient();
	double& getDiffuse();
	double& getSpecular();
	double& getShininess();


private:
    color m_color;
    double m_ambient;
    double m_diffuse;
    double m_specular;
    double m_shininess;
};