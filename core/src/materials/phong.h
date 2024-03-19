#ifndef PHONG_H
#define PHONG_H

#include "../constants.h"
#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"

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
    phong(const color& color, double ambient, double diffuse, double specular, double shininess, double reflective, double transparency) :
        m_color(color::White()), m_ambient(0.1), m_diffuse(0.9), m_specular(0.9), m_shininess(200.0), reflective(0.0), transparency(0.0), refractiveIndex(1.0)
    {
    }


    //
    /// Karim phong code
    //
	//Color Material::lighting(const Shape& shape, const PointLight& light, const Point& point, const Vector& eyev, const Vector& normalv, bool inShadow) const
	//{
	//	Color color = m_color;

	//	if (pattern != nullptr)
	//		color = pattern->patternAtShape(shape, point);

	//	// Combine the surface color with the light's color/intensity
	//	Color effective_color = color * light.intensity();

	//	// Find the direction to the light source
	//	Vector lightv = (light.position() - point).normalize().asVector();

	//	// Compute the ambient contribution
	//	Color ambient = effective_color * m_ambient;

	//	if (inShadow)
	//		return ambient;

	//	// Light_dot_normal represents the cosine of the angle between the light vector and the normal vector. A negative number means the light is on the other side of the surface.
	//	double light_dot_normal = lightv.dot(normalv);
	//	Color diffuse{ 0, 0, 0 };
	//	Color specular{ 0, 0, 0 };
	//	if (light_dot_normal < 0) {
	//		diffuse = Color::Black();
	//		specular = Color::Black();
	//	}
	//	else {
	//		// Compute the diffuse contribution
	//		diffuse = effective_color * m_diffuse * light_dot_normal;

	//		// Reflect_dot_eye represents the cosine of the angle between the reflection vector and the eye vector. A negative number means the light reflects away from the eye.
	//		Tuple reflectv = (-lightv).reflect(normalv);
	//		double reflect_dot_eye = reflectv.dot(eyev);
	//		if (reflect_dot_eye <= 0) {
	//			specular = Color::Black();
	//		}
	//		else {
	//			// Compute the specular contribution
	//			double factor = pow(reflect_dot_eye, m_shininess);
	//			specular = light.intensity() * m_specular * factor;
	//		}
	//	}

	//	// Add the three contributions together to get the final shading
	//	return ambient + diffuse + specular;
	//}


    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override
    {
        // ???????????????????????????
        //srec.attenuation = ?

        //should return ambient + diffuse + specular;

        return true;
    }

    color& getColor()
    {
        return m_color;
    }

    double& getAmbient()
    {
        return m_ambient;
    }

    double& getDiffuse()
    {
        return m_diffuse;
    }

    double& getSpecular()
    {
        return m_specular;
    }

    double& getShininess()
    {
        return m_shininess;
    }

public:
    color& getColor();
    double& getAmbient();
    double& getDiffuse();
    double& getSpecular();
    double& getShininess();

    //APattern* pattern; // ?????????
    double reflective;
    double transparency;
    double refractiveIndex;

private:
    color m_color;
    double m_ambient;
    double m_diffuse;
    double m_specular;
    double m_shininess;
};

#endif