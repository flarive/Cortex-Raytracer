#include "phong.h"

#include "../primitives/hittable.h"
#include "../lights/light.h"
#include "../textures/solid_color_texture.h"
#include "../textures/bump_texture.h"
#include "../textures/normal_texture.h"
#include "../textures/displacement_texture.h"
#include "../textures/alpha_texture.h"
#include "../textures/emissive_texture.h"
#include "../utilities/math_utils.h"
#include "../pdf/sphere_pdf.h"

#include <glm/glm.hpp>

phong::phong(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, const color& ambientColor, double shininess) : phong(diffuseTexture, specularTexture, nullptr, nullptr, nullptr, nullptr, nullptr, ambientColor, shininess)
{
}

phong::phong(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, std::shared_ptr<texture> bumpTexture, std::shared_ptr<texture> normalTexture, std::shared_ptr<texture> displaceTexture, std::shared_ptr<texture> alphaTexture, std::shared_ptr<texture> emissiveTexture, const color& ambientColor, double shininess) : material(diffuseTexture, specularTexture, normalTexture, bumpTexture, displaceTexture, alphaTexture, emissiveTexture)
{
    m_ambientColor = ambientColor;
    m_shininess = shininess;
}

bool phong::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
{
    vector3 normalv = rec.normal;

    vector3 hit_point = rec.hit_point;

    color diffuse_color;
    color specular_color;
    color emissive_color(0, 0, 0); // Initialize emissive color to black

    // Get the texture color at the hit point (assuming diffuse texture)
    if (m_diffuse_texture)
    {
        diffuse_color = m_diffuse_texture->value(rec.u, rec.v, hit_point);
    }

    if (m_specular_texture)
    {
        specular_color = m_specular_texture->value(rec.u, rec.v, hit_point);
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
    vector3 dirToLight = glm::normalize(mylight->getPosition() - hit_point);

    color lightColor = mylight->getColor() * mylight->getIntensity();

    //if (m_displacement_texture)
    //{
    //    // Check if a displacement map texture is available
    //    std::shared_ptr<displacement_texture> displacementTex = std::dynamic_pointer_cast<displacement_texture>(m_displacement_texture);
    //    if (displacementTex)
    //    {
    //        // Displace the hit point using the displacement texture
    //        double displacement_value = displacementTex->value(rec.u, rec.v, hit_point).r(); // Assuming grayscale texture for displacement
    //        hit_point = displacementTex->displace_point(hit_point, displacement_value, rec.normal);

    //        // Recalculate normal at the displaced point
    //        normalv = displacementTex->normal_function(hit_point);
    //    }
    //}
    
    if (m_bump_texture)
    {
        // Check if a bump map texture is available
        std::shared_ptr<bump_texture> bumpTex = std::dynamic_pointer_cast<bump_texture>(m_bump_texture);
        if (bumpTex)
        {
            normalv = bumpTex->perturb_normal(normalv, rec.u, rec.v, hit_point);
        }
    }
    else if (m_normal_texture)
    {
        // Check if a normal map texture is available
        std::shared_ptr<normal_texture> normalTex = std::dynamic_pointer_cast<normal_texture>(m_normal_texture);
        if (normalTex)
        {
            // Sample the normal map texture to get the perturbed normal
            color normal_map = m_normal_texture->value(rec.u, rec.v, hit_point);

            // Transform the perturbed normal from texture space to world space
            // Apply the normal strength factor to the perturbed normal
            normalv = getTransformedNormal(rec.tangent, rec.bitangent, normalv, normal_map, normalTex->getStrenth(), false);
        }
    }

    if (m_alpha_texture)
    {
        // Check if a alpha map texture is available
        std::shared_ptr<alpha_texture> alphaTex = std::dynamic_pointer_cast<alpha_texture>(m_alpha_texture);
        if (alphaTex)
        {
            // good idea ?
            srec.alpha_value = alphaTex->value(rec.u, rec.v, hit_point).r();
        }
    }

    if (m_emissive_texture)
    {
        // Check if a emissive map texture is available
        std::shared_ptr<emissive_texture> emissiveTex = std::dynamic_pointer_cast<emissive_texture>(m_emissive_texture);
        if (emissiveTex)
        {
            emissive_color = emissiveTex->value(rec.u, rec.v, hit_point);
        }
    }

    vector3 v = glm::normalize(-1.0 * (hit_point - r_in.origin()));
    double nl = maxDot3(normalv, dirToLight);
    vector3 r = glm::normalize((2.0 * nl * normalv) - dirToLight);


    // Combine the surface color with the light's color/intensity
    //color final_color = (diffuse_color * nl + specular_color * pow(maxDot3(v, r), m_shininess)) * lightColor;
    color final_color = (diffuse_color * nl + specular_color * pow(maxDot3(v, r), m_shininess)) * lightColor + emissive_color;


    // No refraction, only reflection
    srec.attenuation = final_color;
    srec.pdf_ptr = std::make_shared<sphere_pdf>();
    srec.skip_pdf = false;

    return true;
}

double phong::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
    return cos_theta < 0 ? 0 : cos_theta / M_PI;
}