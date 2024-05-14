#include "phong2.h"

#include "../primitives/hittable.h"
#include "../lights/light.h"
#include "../textures/solid_color_texture.h"

#include <glm/glm.hpp>


phong2::phong2(std::shared_ptr<texture> diffuseTexture, const color& ambientColor, const color& specularColor, double exponent) : material(diffuseTexture)
{
    m_ambientColor = ambientColor;
    m_specularColor = specularColor;
    m_exponent = exponent;
}

bool phong2::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const
{
    // Get the texture color at the hit point (assuming albedo texture)
    color diffuse_color = m_diffuse_texture->value(rec.u, rec.v, rec.hit_point);
    
    vector3 n, l, v, r;
    float nl;


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

    l = glm::normalize(dirToLight);
    n = glm::normalize(rec.normal);
    v = glm::normalize(-1.0 * (rec.hit_point - r_in.origin()));

    nl = maxDot3(n, l);
    r = glm::normalize((2.0 * nl * n) - l);

    color final_color = (diffuse_color * nl + m_specularColor * powf(maxDot3(v, r), m_exponent)) * lightColor;

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
