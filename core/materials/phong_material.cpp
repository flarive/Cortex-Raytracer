#include "phong_material.h"

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

phong_material::phong_material(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, const color& ambientColor, double shininess) : phong_material(diffuseTexture, specularTexture, nullptr, nullptr, nullptr, nullptr, nullptr, ambientColor, shininess)
{
}

phong_material::phong_material(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, std::shared_ptr<texture> bumpTexture, std::shared_ptr<texture> normalTexture, std::shared_ptr<texture> displaceTexture, std::shared_ptr<texture> alphaTexture, std::shared_ptr<texture> emissiveTexture, const color& ambientColor, double shininess) : material(diffuseTexture, specularTexture, normalTexture, bumpTexture, displaceTexture, alphaTexture, emissiveTexture)
{
    m_ambientColor = ambientColor;
    m_shininess = shininess;
}


//bool phong_material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const
//{
//    vector3 normalv = rec.normal;
//    vector3 hit_point = rec.hit_point;
//
//    color diffuse_color;
//    color specular_color;
//    color emissive_color(0, 0, 0); // Initialize emissive color to black
//
//    // Get the texture color at the hit point (assuming diffuse texture)
//    if (m_diffuse_texture)
//    {
//        diffuse_color = m_diffuse_texture->value(rec.u, rec.v, hit_point);
//    }
//
//    if (m_specular_texture)
//    {
//        specular_color = m_specular_texture->value(rec.u, rec.v, hit_point);
//    }
//
//    // just take the first light for the moment
//    if (lights.objects.size() == 0)
//    {
//        // no light
//        return false;
//    }
//
//    std::shared_ptr<light> mylight = std::dynamic_pointer_cast<light>(lights.objects[0]);
//    if (mylight == nullptr)
//    {
//        // no light
//        return false;
//    }
//
//    // Find the direction to the light source
//    vector3 dirToLight = glm::normalize(mylight->getPosition() - hit_point);
//
//    color lightColor = mylight->getColor() * mylight->getIntensity();
//
//    if (m_displacement_texture)
//    {
//        // not handled here ! see mesh_loader.cpp
//    }
//
//    if (m_bump_texture)
//    {
//        // Check if a bump map texture is available
//        std::shared_ptr<bump_texture> bumpTex = std::dynamic_pointer_cast<bump_texture>(m_bump_texture);
//        if (bumpTex)
//        {
//            normalv = bumpTex->perturb_normal(normalv, rec.u, rec.v, hit_point);
//        }
//    }
//    else if (m_normal_texture)
//    {
//        // Check if a normal map texture is available
//        std::shared_ptr<normal_texture> normalTex = std::dynamic_pointer_cast<normal_texture>(m_normal_texture);
//        if (normalTex)
//        {
//            // Sample the normal map texture to get the perturbed normal
//            color normal_map = m_normal_texture->value(rec.u, rec.v, hit_point);
//
//            // Transform the perturbed normal from texture space to world space
//            // Apply the normal strength factor to the perturbed normal
//            normalv = getTransformedNormal(rec.tangent, rec.bitangent, normalv, normal_map, normalTex->getStrenth(), false);
//        }
//    }
//
//    if (m_alpha_texture)
//    {
//        // Check if a alpha map texture is available
//        std::shared_ptr<alpha_texture> alphaTex = std::dynamic_pointer_cast<alpha_texture>(m_alpha_texture);
//        if (alphaTex)
//        {
//            srec.alpha_value = alphaTex->value(rec.u, rec.v, hit_point).r();
//        }
//    }
//
//    if (m_emissive_texture)
//    {
//        // Check if an emissive map texture is available
//        std::shared_ptr<emissive_texture> emissiveTex = std::dynamic_pointer_cast<emissive_texture>(m_emissive_texture);
//        if (emissiveTex)
//        {
//            emissive_color = emissiveTex->value(rec.u, rec.v, hit_point);
//        }
//    }
//
//    vector3 v = glm::normalize(-1.0 * (hit_point - r_in.origin()));
//    double nl = maxDot3(normalv, dirToLight);
//    vector3 r = glm::normalize((2.0 * nl * normalv) - dirToLight);
//
//    // Combine the surface color with the light's color/intensity
//    // Diffuse and specular reflection contributions
//    color lighting = (diffuse_color * nl + specular_color * pow(maxDot3(v, r), m_shininess)) * lightColor + emissive_color;
//
//    // Add ambient color contribution (scaled by diffuse color)
//    color ambient_contribution = m_ambientColor * diffuse_color;
//
//    // Final color considering ambient light
//    color final_color = lighting + ambient_contribution;
//
//    // No refraction, only reflection
//    srec.attenuation = final_color;
//    srec.pdf_ptr = std::make_shared<sphere_pdf>();
//    srec.skip_pdf = false;
//
//    return true;
//}


bool phong_material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const
{
    vector3 normalv = rec.normal;
    vector3 hit_point = rec.hit_point;

    color diffuse_color(0, 0, 0);
    color specular_color(0, 0, 0);
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

    if (m_emissive_texture)
    {
        std::shared_ptr<emissive_texture> emissiveTex = std::dynamic_pointer_cast<emissive_texture>(m_emissive_texture);
        if (emissiveTex)
        {
            emissive_color = emissiveTex->value(rec.u, rec.v, hit_point);
        }
    }

    if (m_bump_texture)
    {
        std::shared_ptr<bump_texture> bumpTex = std::dynamic_pointer_cast<bump_texture>(m_bump_texture);
        if (bumpTex)
        {
            normalv = bumpTex->perturb_normal(normalv, rec.u, rec.v, hit_point);
        }
    }
    else if (m_normal_texture)
    {
        std::shared_ptr<normal_texture> normalTex = std::dynamic_pointer_cast<normal_texture>(m_normal_texture);
        if (normalTex)
        {
            color normal_map = m_normal_texture->value(rec.u, rec.v, hit_point);
            normalv = getTransformedNormal(rec.tangent, rec.bitangent, normalv, normal_map, normalTex->getStrenth(), false);
        }
    }

    color total_light(0, 0, 0); // Accumulator for total light contribution

    for (const auto& obj : lights.objects)
    {
        std::shared_ptr<light> mylight = std::dynamic_pointer_cast<light>(obj);
        if (!mylight) continue; // Skip non-light objects

        // Compute light direction and intensity
        vector3 dirToLight = glm::normalize(mylight->getPosition() - hit_point);
        color lightColor = mylight->getColor() * mylight->getIntensity();

        // Lambertian (diffuse) term
        double nl = maxDot3(normalv, dirToLight);
        color diffuse_contribution = diffuse_color * nl;

        // Phong specular term
        vector3 v = glm::normalize(-1.0 * (hit_point - r_in.origin()));
        vector3 r = glm::normalize((2.0 * nl * normalv) - dirToLight);
        color specular_contribution = specular_color * pow(maxDot3(v, r), m_shininess);

        // Add this light's contribution
        total_light += (diffuse_contribution + specular_contribution) * lightColor;
    }

    // Add ambient color contribution (scaled by diffuse color)
    color ambient_contribution = m_ambientColor * diffuse_color;

    // Final color considering all lights and ambient light
    color final_color = total_light + ambient_contribution + emissive_color;

    // No refraction, only reflection
    srec.attenuation = final_color;
    srec.pdf_ptr = std::make_shared<sphere_pdf>();
    srec.skip_pdf = false;

    return true;
}



double phong_material::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
    return cos_theta < 0 ? 0 : cos_theta / M_PI;
}