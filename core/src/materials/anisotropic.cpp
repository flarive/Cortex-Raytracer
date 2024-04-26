#include "anisotropic.h"

#include "../lights/light.h"
#include "../textures/solid_color_texture.h"

#include <random>

anisotropic::anisotropic(color c, double roughness) : material(std::make_shared<solid_color_texture>(c)), m_roughness(roughness)
{
}

anisotropic::anisotropic(std::shared_ptr<texture> a, double roughness) : material(a), m_roughness(roughness)
{
}

bool anisotropic::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
    //vector3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    //// Adding some roughness to the reflected direction
    //vector3 scatter_direction = reflected + m_roughness * random_in_unit_sphere();


    //if (near_zero(scatter_direction))
    //    scatter_direction = rec.normal;


    //srec.skip_pdf_ray = ray(rec.hit_point, scatter_direction);
    //srec.attenuation = m_albedo->value(rec.u, rec.v, rec.hit_point);
    //srec.skip_pdf = true;
    //srec.pdf_ptr = nullptr; // Anisotropic materials don't use probability density functions for now
    //return true;

	// Construct a local coordinate system with the hit point as origin
	vector3 normal = unit_vector(rec.normal);
	vector3 tangent = unit_vector(random_unit_vector());
	vector3 bitangent = cross(normal, tangent);


	// just take the first light for the moment
	std::shared_ptr<light> mylight = std::dynamic_pointer_cast<light>(lights.objects[0]);
	if (mylight == nullptr)
	{
		// no light
		return false;
	}

	// Combine the surface color with the light's color/intensity
	color incomingIntensity = mylight->getColor() * mylight->getIntensity();



	// Create a random direction in tangent space using the roughness parameters
	vector3 direction = random_in_ellipse(m_roughness, m_roughness, tangent, bitangent);

	// Convert the direction from tangent space to world space
	direction = tangent * direction.x + bitangent * direction.y + normal * direction.z;

	srec.skip_pdf = true;
	srec.skip_pdf_ray = ray(rec.hit_point, direction, r_in.time());
	srec.attenuation = incomingIntensity * m_albedo->value(rec.u, rec.v, rec.hit_point);

	return true;}

// Generate a random point within an ellipse defined by the given radii
vector3 anisotropic::random_in_ellipse(double radiusX, double radiusY, const vector3& tangent, const vector3& bitangent)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dis(0.0, 1.0);

	// Generate polar coordinates
	double theta = 2 * M_PI * dis(gen);
	double r = sqrt(dis(gen)); // Square root for uniform distribution of points within the unit circle

	// Convert polar coordinates to Cartesian coordinates in tangent space
	vector3 point = tangent * (radiusX * r * cos(theta)) + bitangent * (radiusY * r * sin(theta));

	return point;
}

color anisotropic::emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
{
	return color::black();
}


double anisotropic::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    return 1 / (4 * M_PI);
}


/// Github copilot
//#include "material.h"
//
//class AnisotropicMaterial : public Material {
//public:
//	// Constructeur
//	AnisotropicMaterial(const Vec3& albedo, float fuzziness)
//		: albedo_(albedo), fuzziness_(fuzziness) {}
//
//	// Fonction de dispersion
//	bool scatter(const Ray& ray_in, const HitRecord& rec, ScatterRecord& srec) const override {
//		// Calcul de la direction réfléchie
//		Vec3 reflected = reflect(unit_vector(ray_in.direction()), rec.normal);
//		srec.specular_ray = Ray(rec.p, reflected + fuzziness_ * random_in_unit_sphere());
//		srec.attenuation = albedo_;
//		srec.is_specular = true;
//		srec.pdf_ptr = nullptr;
//		return true;
//	}
//
//private:
//	Vec3 albedo_;
//	float fuzziness_;
//};



