#include "mtl_material.h"

#include "../materials/lambertian.h"
#include "../materials/glossy.h"
#include "../materials/diffuse_light.h"
#include "../textures/roughness_from_sharpness_texture.h"

mtl_material::mtl_material(
    std::shared_ptr<texture> diffuse_a,
    std::shared_ptr<texture> specular_a,
    std::shared_ptr<texture> bump_a,
    std::shared_ptr<texture> normal_a,
    std::shared_ptr<texture> emissive_a,
    std::shared_ptr<texture> transparency_map,
    std::shared_ptr<texture> sharpness_map,
    int illum) :
    emissive_text(emissive_a),
    diffuse_text(diffuse_a),
    specular_text(specular_a),
    bump_text(bump_a),
    normal_text(normal_a),
    transparency_text(transparency_map),
    roughness_text(make_shared<roughness_from_sharpness_texture>(sharpness_map, 1, 10000))
{
    diffuse_mat = std::make_shared<lambertian>(diffuse_text);
    specular_mat = std::make_shared<glossy>(specular_text);// , roughness_text);
    emissive_mat = std::make_shared<diffuse_light>(emissive_text);
}

bool mtl_material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const
{
    double transp_prob = transparency_prob(rec.u, rec.v, rec.hit_point);
    if (transp_prob > randomizer::random_double())
    {
        srec.attenuation = transparency_text->value(rec.u, rec.v, rec.hit_point);
        srec.skip_pdf = true;
        // Continue in the same direction, starting from hitpoint
        srec.skip_pdf_ray = ray(rec.hit_point, r_in.direction(), r_in.time());

        return false;
    }

    // without normal map
    if (!normal_text)
    {
        return choose_mat(rec.u, rec.v, rec.hit_point)->scatter(r_in, lights, rec, srec, random);
    }


    // with normal map computation
    std::shared_ptr<material> mat = choose_mat(rec.u, rec.v, rec.hit_point);

    // Get the surface normal from the hit record
    vector3 normal = rec.normal;

    // Compute tangent and bitangent vectors
    vector3 tangent = rec.tangent;
    vector3 bitangent = rec.bitangent;
        
        

    // Sample the normal map texture to get the perturbed normal
    color normal_map = normal_text->value(rec.u, rec.v, rec.hit_point);

    // Convert RGB values ([0, 1]) to normal components in range [-1, 1]
    normal_map = 2.0f * normal_map - color(1, 1, 1);

    // Transform the perturbed normal from texture space to world space
    //normal = glm::normalize(tangent * normal_map.r() + bitangent * normal_map.g() + rec.normal * normal_map.b());




    // or this one better ????????
    vector3 tmp = vector3(normal_map.r(), normal_map.g(), normal_map.b());
    normal = glm::normalize(getTransformedNormal(tangent, bitangent, normal, tmp));


    // Pass the perturbed normal along with the hit record to the scatter function of the selected material
    hit_record hr;
    hr.hit_point = rec.hit_point;
    hr.normal = normal;
    hr.u = rec.u;
    hr.v = rec.v;
    hr.t = rec.t;
    hr.front_face = rec.front_face;

    return mat->scatter(r_in, lights, hr, srec, random);
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

double mtl_material::transparency_prob(double u, double v, const point3& p) const
{
    double diff = diffuse_text->value(u, v, p).length();
    double spec = specular_text->value(u, v, p).length();
    double transp = transparency_text->value(u, v, p).length();
    return transp / (transp + diff + spec + 0.00001);
}

double mtl_material::diffuse_prob(double u, double v, const point3& p) const
{
	double diff = diffuse_text->value(u, v, p).length();
	double spec = specular_text->value(u, v, p).length();
	return diff / (diff + spec + 0.00001);
}

std::shared_ptr<material> mtl_material::choose_mat(double u, double v, const point3& p) const
{
	if (diffuse_prob(u, v, p) > randomizer::random_double())
    {
		return diffuse_mat;
	}
	else
    {
		return specular_mat;
	}
}

/// <summary>
/// https://medium.com/@dbildibay/ray-tracing-adventure-part-iv-678768947371
/// </summary>
/// <param name="tan"></param>
/// <param name="bitan"></param>
/// <param name="normal"></param>
/// <param name="sampleNormal"></param>
/// <returns></returns>
vector3 mtl_material::getTransformedNormal(vector3& tan, vector3& bitan, vector3& normal, vector3& sampleNormal) const
{
    // Build a TNB matrix (Tangent/Normal/Bitangent matrix)
    glm::mat3x3 matTNB = glm::mat3x3(
        glm::vec3(tan.x, bitan.x, normal.x),
        glm::vec3(tan.y, bitan.y, normal.y),
        glm::vec3(tan.z, bitan.z, normal.z)
    );

    glm::mat3x3 normalVec(glm::vec3(sampleNormal.x, 0.0f, 0.0f),
        glm::vec3(sampleNormal.y, 0.0f, 0.0f),
        glm::vec3(sampleNormal.z, 0.0f, 0.0f));

    glm::mat3x3 newNormalMat = matTNB * normalVec;

    return vector3(newNormalMat[0][0], newNormalMat[1][0], newNormalMat[2][0]);
}