#include "phong2.h"

#include "../primitives/hittable.h"
#include "../lights/light.h"
#include "../textures/solid_color_texture.h"
#include "../textures/bump_texture.h"
#include "../textures/normal_texture.h"
#include "../utilities/math_utils.h"
#include "mtl_material.h"

#include <glm/glm.hpp>

phong2::phong2(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, const color& ambientColor, double shininess) : phong2(diffuseTexture, specularTexture, nullptr, nullptr, ambientColor, shininess)
{
}

phong2::phong2(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, std::shared_ptr<texture> normalTexture, std::shared_ptr<texture> bumpTexture, const color& ambientColor, double shininess) : material(diffuseTexture, specularTexture, normalTexture, bumpTexture)
{
    m_ambientColor = ambientColor;
    m_shininess = shininess;
}

bool phong2::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const
{
    vector3 normalv = rec.normal;

    color diffuse_color;
    color specular_color;
    
    // Get the texture color at the hit point (assuming diffuse texture)
    if (m_diffuse_texture)
    {
        diffuse_color = m_diffuse_texture->value(rec.u, rec.v, rec.hit_point);
    }

    if (m_specular_texture)
    {
        specular_color = m_specular_texture->value(rec.u, rec.v, rec.hit_point);
    }

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

    // Find the direction to the light source
    vector3 dirToLight = glm::normalize(mylight->getPosition() - rec.hit_point);

    color lightColor = mylight->getColor() * mylight->getIntensity();

    
    if (m_bump_texture)
    {
        // Check if a bump map texture is available
        std::shared_ptr<bump_texture> bumpTex = std::dynamic_pointer_cast<bump_texture>(m_bump_texture);
        if (bumpTex)
        {
            normalv = bumpTex->perturb_normal(normalv, rec.u, rec.v, rec.hit_point);
        }
    }
    else if (m_normal_texture)
    {
        // Check if a normal map texture is available
        std::shared_ptr<normal_texture> normalTex = std::dynamic_pointer_cast<normal_texture>(m_normal_texture);
        if (normalTex)
        {
            // Sample the normal map texture to get the perturbed normal
            color normal_map = m_normal_texture->value(rec.u, rec.v, rec.hit_point);

            // Transform the perturbed normal from texture space to world space
            // Apply the normal strength factor to the perturbed normal
            normalv = getTransformedNormal(rec.tangent, rec.bitangent, normalv, normal_map, normalTex->getStrenth(), false);
        }
    }

    vector3 v = glm::normalize(-1.0 * (rec.hit_point - r_in.origin()));
    double nl = maxDot3(normalv, dirToLight);
    vector3 r = glm::normalize((2.0 * nl * normalv) - dirToLight);


    // Combine the surface color with the light's color/intensity
    color final_color = (diffuse_color * nl + specular_color * pow(maxDot3(v, r), m_shininess)) * lightColor;

    // No refraction, only reflection
    srec.attenuation = final_color;
    srec.pdf_ptr = std::make_shared<sphere_pdf>();
    srec.skip_pdf = false;

    return true;
}

double phong2::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    auto cos_theta = dot(rec.normal, randomizer::unit_vector(scattered.direction()));
    return cos_theta < 0 ? 0 : cos_theta / M_PI;
}


// Function to calculate the maximum of the dot product of two vectors and zero
double phong2::maxDot3(const vector3& v1, const vector3& v2)
{
    double dotProduct = 0.0;

    // Compute the dot product of the two vectors
    for (auto i = 0; i < v1.length(); ++i)
    {
        dotProduct += v1[i] * v2[i];
    }

    // Return the maximum of the dot product and zero
    return std::max(dotProduct, 0.0);
}