#include "mtl_material.h"

#include "../materials/lambertian.h"
#include "../materials/glossy.h"
#include "../materials/diffuse_light.h"
#include "../textures/roughness_from_sharpness_texture.h"

mtl_material::mtl_material(
    std::shared_ptr<texture> diffuse_a,
    std::shared_ptr<texture> specular_a,
    std::shared_ptr<texture> emissive_a,
    std::shared_ptr<texture> transparency_map,
    std::shared_ptr<texture> sharpness_map,
    int illum) :
    emissive_text(emissive_a),
    diffuse_text(diffuse_a),
    specular_text(specular_a),
    transparency_text(transparency_map),
    roughness_text(make_shared<roughness_from_sharpness_texture>(sharpness_map, 1, 10000))
{
    diffuse_mat = std::make_shared<lambertian>(diffuse_text);
    specular_mat = std::make_shared<glossy>(specular_text, roughness_text);
    emissive_mat = std::make_shared<diffuse_light>(emissive_text);
}


bool mtl_material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
    double transp_prob = transparency_prob(rec.u, rec.v, rec.hit_point);
    if (transp_prob > random_double())
    {
        srec.attenuation = transparency_text->value(rec.u, rec.v, rec.hit_point);
        srec.skip_pdf = true;
        // Continue in the same direction, starting from hitpoint
        srec.skip_pdf_ray = ray(rec.hit_point, r_in.direction(), r_in.time());

        return false;
    }

    return choose_mat(rec.u, rec.v, rec.hit_point)->scatter(r_in, lights, rec, srec);
}

color mtl_material::emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
{
    return emissive_mat->emitted(r_in, rec, u, v, p);
}

double mtl_material::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    // We don't need to care about the transparent case, this only integrates over scattered rays (note specular are scatterd, but not diffuse)
    double diff_prob = diffuse_prob(rec.u, rec.v, rec.hit_point);

    return diff_prob * (diffuse_mat->scattering_pdf(r_in, rec, scattered)) + (1 - diff_prob) * specular_mat->scattering_pdf(r_in, rec, scattered);
}

inline double mtl_material::transparency_prob(double u, double v, const point3& p) const
{
    double diff = diffuse_text->value(u, v, p).length();
    double spec = specular_text->value(u, v, p).length();
    double transp = transparency_text->value(u, v, p).length();
    return transp / (transp + diff + spec + 0.00001);
}

inline double mtl_material::diffuse_prob(double u, double v, const point3& p) const
{
	double diff = diffuse_text->value(u, v, p).length();
	double spec = specular_text->value(u, v, p).length();
	return diff / (diff + spec + 0.00001);
}

inline std::shared_ptr<material> mtl_material::choose_mat(double u, double v, const point3& p) const
{
	if (diffuse_prob(u, v, p) > random_double()) {
		return diffuse_mat;
	}
	else {
		return specular_mat;
	}
}
