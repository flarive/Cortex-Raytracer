#include "phong2.h"

#include "../primitives/hittable.h"
#include "../lights/light.h"
#include "../textures/solid_color_texture.h"

#include <glm/glm.hpp>

phong2::phong2(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, const color& ambientColor, double shininess) : phong2(diffuseTexture, specularTexture, nullptr, ambientColor, shininess)
{
}

phong2::phong2(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, std::shared_ptr<texture> normalTexture, const color& ambientColor, double shininess) : material(diffuseTexture, specularTexture, normalTexture)
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

    color lightColor = mylight->getColor();


    // Check if a normal map texture is available
    if (m_normal_texture)
    {
        // Retrieve the normal map color at the hit point
        color normal_map_color = m_normal_texture->value(rec.u, rec.v, rec.hit_point);

        // Transform the normal map color (RGB values in [-1, 1]) to a perturbed normal
        vector3 tangent = rec.tangent;
        vector3 bitangent = rec.bitangent;
        vector3 perturbed_normal = glm::normalize(tangent * normal_map_color.r() + bitangent * normal_map_color.g() + normalv * normal_map_color.b());

        // Use the perturbed normal for shading instead of the geometric normal
        normalv = perturbed_normal;
    }


    vector3 n = glm::normalize(normalv);
    vector3 v = glm::normalize(-1.0 * (rec.hit_point - r_in.origin()));

    double nl = maxDot3(n, dirToLight);
    vector3 r = glm::normalize((2.0 * nl * n) - dirToLight);

    
    



    // Combine the surface color with the light's color/intensity
    color final_color = (diffuse_color * nl + specular_color * powf(maxDot3(v, r), m_shininess)) * lightColor;

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
float phong2::maxDot3(const vector3& v1, const vector3& v2)
{
    float dotProduct = 0.0;

    // Compute the dot product of the two vectors
    for (size_t i = 0; i < v1.length(); ++i) {
        dotProduct += v1[i] * v2[i];
    }

    // Return the maximum of the dot product and zero
    return std::max(dotProduct, 0.0f);
}
