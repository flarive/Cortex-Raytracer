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
	: material(std::make_shared<solid_color_texture>(_color), nullptr, _transparency, _refraction_index), m_ambient(_ambient), m_diffuse(_diffuse), m_specular(_specular), m_shininess(_shininess)
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

phong::phong(std::shared_ptr<texture> _albedo, double _ambient, double _diffuse, double _specular, double _shininess, double _transparency, double _refraction_index) : material(_albedo, nullptr, _transparency, _refraction_index), m_ambient(_ambient), m_diffuse(_diffuse), m_specular(_specular), m_shininess(_shininess)
{
}

phong::phong(std::shared_ptr<texture> _albedo, std::shared_ptr<texture> _normal, double _ambient, double _diffuse, double _specular, double _shininess, double _transparency, double _refraction_index) : material(_albedo, _normal, _transparency, _refraction_index), m_ambient(_ambient), m_diffuse(_diffuse), m_specular(_specular), m_shininess(_shininess)
{
}


bool phong::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const
{
	vector3 eyev = -r_in.direction();
	point3 point = rec.hit_point;
	vector3 normalv = rec.normal;


	// Get the texture color at the hit point (assuming albedo texture)
	color albedo_color = m_albedo->value(rec.u, rec.v, rec.hit_point);

	// Check if a normal map texture is available
	if (m_normal)
	{
		// Retrieve the normal map color at the hit point
		color normal_map_color = m_normal->value(rec.u, rec.v, rec.hit_point);

		// Transform the normal map color (RGB values in [-1, 1]) to a perturbed normal
		vector3 tangent = rec.tangent;
		vector3 bitangent = rec.bitangent;
		vector3 perturbed_normal = glm::normalize(tangent * normal_map_color.r() + bitangent * normal_map_color.g() + normalv * normal_map_color.b());

		// Use the perturbed normal for shading instead of the geometric normal
		normalv = perturbed_normal;
	}


	
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
	effective_color = albedo_color * mylight->getColor() * mylight->getIntensity();

	// Find the direction to the light source
	vector3 lightv = glm::normalize(mylight->getPosition() - point);

	// Compute the ambient contribution
	color ambient = effective_color * m_ambient;


	// Light_dot_normal represents the cosine of the angle between the light vector and the normal vector.
	// A negative number means the light is on the other side of the surface.



	// Compute the diffuse contribution
	double light_dot_normal = glm::dot(lightv, normalv);
	color diffuse = color::black();
	if (light_dot_normal > 0.0)
		diffuse = effective_color * m_diffuse * light_dot_normal;

	// Reflect_dot_eye represents the cosine of the angle between the reflection vector and the eye vector.
	// A negative number means the light reflects away from the eye.


	// Compute the specular contribution
	vector3 reflectv = glm::reflect(-lightv, normalv);
	double reflect_dot_eye = glm::dot(reflectv, eyev);
	color specular = color::black();
	if (reflect_dot_eye > 0.0) {
		double specular_factor = std::pow(reflect_dot_eye, m_shininess);
		specular = mylight->getColor() * mylight->getIntensity() * m_specular * specular_factor;
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
	auto cos_theta = dot(rec.normal, randomizer::unit_vector(scattered.direction()));
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