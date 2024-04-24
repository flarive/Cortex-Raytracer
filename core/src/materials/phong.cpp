#include "phong.h"

#include "../textures/texture.h"


#include <glm/glm.hpp>


phong::phong() 
	: material(std::make_shared<solid_color_texture>(color::white())), m_ambient(0.1), m_diffuse(0.1), m_specular(0.9), m_shininess(0.0)
{
}

phong::phong(const color& _color) 
	: material(std::make_shared<solid_color_texture>(_color)), m_ambient(0.1), m_diffuse(0.1), m_specular(0.9), m_shininess(0.0)
{
}

phong::phong(const color& _color, double _ambient, double _diffuse, double _specular, double _shininess) 
	: material(std::make_shared<solid_color_texture>(_color)), m_ambient(_ambient), m_diffuse(_diffuse), m_specular(_specular), m_shininess(_shininess)
{
}

phong::phong(const color& _color, double _ambient, double _diffuse, double _specular, double _shininess, double _transparency, double _refraction_index)
	: material(std::make_shared<solid_color_texture>(_color), _transparency, _refraction_index), m_ambient(_ambient), m_diffuse(_diffuse), m_specular(_specular), m_shininess(_shininess)
{
}

phong::phong(std::shared_ptr<texture> _albedo) 
	: material(_albedo), m_ambient(0.1), m_diffuse(0.1), m_specular(0.9), m_shininess(0.0)
{
}

phong::phong(std::shared_ptr<texture> _albedo, double _ambient, double _diffuse, double _specular, double _shininess) 
	: material(_albedo), m_ambient(_ambient), m_diffuse(_diffuse), m_specular(_specular), m_shininess(_shininess)
{
}

phong::phong(std::shared_ptr<texture> _albedo, double _ambient, double _diffuse, double _specular, double _shininess, double _transparency, double _refraction_index) : material(_albedo, _transparency, _refraction_index), m_ambient(_ambient), m_diffuse(_diffuse), m_specular(_specular), m_shininess(_shininess)
{
}

bool phong::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
	vector3 eyev = -r_in.direction();
	point3 point = rec.hit_point;
	vector3 normalv = rec.normal;
	color mycolor = m_albedo->value(rec.u, rec.v, rec.hit_point);

	
	color effective_color;

	

	// just take the first light for the moment
	if (lights.objects.size() == 0)
	{
		// no light
		return false;
	}

	std::shared_ptr<light> mylight = std::dynamic_pointer_cast<light>(lights.objects[0]);
	if (mylight == nullptr)
	{
		// no light
		return false;
	}


	// Combine the surface color with the light's color/intensity
	effective_color = mycolor * mylight->getColor() * mylight->getIntensity();

	// Find the direction to the light source
	vector3 lightv = glm::normalize(mylight->getPosition() - point);

	// Compute the ambient contribution
	color ambient = effective_color * m_ambient;


	// Light_dot_normal represents the cosine of the angle between the light vector and the normal vector.
	// A negative number means the light is on the other side of the surface.
	double light_dot_normal = glm::dot(lightv, normalv);
	color diffuse { 0, 0, 0 };
	color specular { 0, 0, 0 };

	if (light_dot_normal < 0)
	{
		diffuse = color::black();
		specular = color::black();
	}
	else
	{
		// Compute the diffuse contribution
		diffuse = effective_color * m_diffuse * light_dot_normal;

		// Reflect_dot_eye represents the cosine of the angle between the reflection vector and the eye vector.
		// A negative number means the light reflects away from the eye.
		vector3 reflectv = (-lightv) - normalv * vector3(2) * glm::dot(-lightv, normalv);
		double reflect_dot_eye = glm::dot(reflectv, eyev);
		if (reflect_dot_eye <= 0)
		{
			specular = color::black();
		}
		else
		{
			// Compute the specular contribution
			double factor = pow(reflect_dot_eye, m_shininess);
			specular = color(mylight->getColor() * mylight->getIntensity() * m_specular * factor);
		}
	}

	// Add the three contributions together to get the final shading
	color final_color = ambient + diffuse + specular;

	// Check if the material is transparent (e.g., glass)
	if (m_transparency > 0)
	{
		// Compute the refracted ray direction
		vector3 refracted_direction = glm::refract(r_in.direction(), normalv, m_refractiveIndex);
		//srec.attenuation = color(1.0, 1.0, 1.0); // Fully transparent
		srec.attenuation = final_color * color(m_transparency);
		srec.skip_pdf = true;
		srec.skip_pdf_ray = ray(rec.hit_point, refracted_direction, r_in.time());
		return true;

		// Total internal reflection (TIR)
		// Handle this case if needed
	}

	// No refraction, only reflection
	srec.attenuation = final_color;
	srec.pdf_ptr = std::make_shared<sphere_pdf>();
	srec.skip_pdf = false;

    return true;
}

bool phong::refract(const vector3& incident, const vector3& normal, double refractive_index, vector3& refracted)
{
	// Compute the cosine of the angle of incidence
	double cos_theta_i = glm::dot(-incident, normal);

	// Compute the discriminant
	double discriminant = 1.0 - (refractive_index * refractive_index) * (1.0 - cos_theta_i * cos_theta_i);

	if (discriminant > 0.0)
	{
		// Compute the refracted direction
		refracted = (refractive_index / cos_theta_i - std::sqrt(discriminant)) * normal;
		return true;
	}
	else
	{
		// Total internal reflection (TIR)
		return false;
	}
}

double phong::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
	auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
	return cos_theta < 0 ? 0 : cos_theta / M_PI;
}

double& phong::getAmbient()
{
	return m_ambient;
}

double& phong::getDiffuse()
{
	return m_diffuse;
}

double& phong::getSpecular()
{
	return m_specular;
}

double& phong::getShininess()
{
	return m_shininess;
}