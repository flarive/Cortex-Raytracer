#include "anisotropic.h"

#include "../lights/light.h"
#include "../textures/solid_color_texture.h"
#include "../utilities/randomizer.h"


anisotropic::anisotropic(double Nu, double Nv, const std::shared_ptr<texture>& diffuseTexture, const std::shared_ptr<texture>& specularTexture, const std::shared_ptr<texture>& exponentTexture)
	: m_diffuse(diffuseTexture), m_specular(specularTexture), m_exponent(exponentTexture), m_nu(Nu), m_nv(Nv)
{
}

bool anisotropic::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const
{
	srec.skip_pdf = true;
	srec.attenuation = srec.diffuseColor = m_diffuse->value(rec.u, rec.v, rec.hit_point);
	srec.specularColor = m_specular->value(rec.u, rec.v, rec.hit_point);

	if (m_exponent)
	{
		const color c = m_exponent->value(rec.u, rec.v, rec.hit_point);
		const double e = (c.r() + c.g() + c.b()) / 3.;

		if (e > 0)
			srec.pdf_ptr = std::make_shared<AnisotropicPhong_pdf>(r_in.direction(), rec.normal, e * m_nu, e * m_nv);
		else
			srec.pdf_ptr = std::make_shared<AnisotropicPhong_pdf>(r_in.direction(), rec.normal, m_nu, m_nv);
	}
	else
	{
		srec.pdf_ptr = std::make_shared<AnisotropicPhong_pdf>(r_in.direction(), rec.normal, m_nu, m_nv);
	}


	vector3 dir(srec.pdf_ptr->generate(random, srec));


	while (vector_multiply_to_double(dir, rec.normal) < 0)
	{
		dir = srec.pdf_ptr->generate(random, srec);
	}
	srec.skip_pdf_ray = ray(rec.hit_point + epsilon * rec.normal, dir);

	return true;
}

color anisotropic::emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
{
	return color::black();
}


double anisotropic::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
	const double cosine = vector_multiply_to_double(rec.normal, scattered.direction());

	if (cosine < 0) return 0;

	return cosine * M_1_PI;
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



