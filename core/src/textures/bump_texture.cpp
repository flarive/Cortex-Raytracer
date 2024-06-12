#include "bump_texture.h"

#include "../utilities/randomizer.h"
#include "image_texture.h"

#include <glm/glm.hpp>

bump_texture::bump_texture()
{
}
	
bump_texture::bump_texture(std::shared_ptr<texture> bump, double strength) : m_bump(bump), m_strength(strength)
{
}

color bump_texture::value(double u, double v, const point3& p) const
{
    return m_bump->value(u, v, p);
}

vector3 bump_texture::perturb_normal(const vector3& normal, double u, double v, const vector3& p) const
{
    double m_bump_width = 0.0;
    double m_bump_height = 0.0;

    std::shared_ptr<image_texture> imageTex = std::dynamic_pointer_cast<image_texture>(m_bump);
    if (imageTex)
    {
        m_bump_width = imageTex->getWidth();
        m_bump_height = imageTex->getHeight();
    }
    
    double heightL = m_bump->value(u - 1.0 / m_bump_width, v, p).r();
    double heightR = m_bump->value(u + 1.0 / m_bump_width, v, p).r();
    double heightD = m_bump->value(u, v - 1.0 / m_bump_height, p).r();
    double heightU = m_bump->value(u, v + 1.0 / m_bump_height, p).r();

    // Scale the height differences using m_scale
    double scaledHeightL = m_strength * heightL;
    double scaledHeightR = m_strength * heightR;
    double scaledHeightD = m_strength * heightD;
    double scaledHeightU = m_strength * heightU;

    vector3 tangentSpaceNormal = glm::normalize(vector3(
        scaledHeightR - scaledHeightL,
        scaledHeightU - scaledHeightD,
        1.0f
    ));

    return convert_to_world_space(tangentSpaceNormal, normal);
}

vector3 bump_texture::convert_to_world_space(const vector3& tangentSpaceNormal, const vector3& originalNormal) const
{
    vector3 tangent, bitangent;
    compute_tangent_space(originalNormal, tangent, bitangent);

    vector3 worldNormal = glm::normalize(
        tangent * tangentSpaceNormal.x +
        bitangent * tangentSpaceNormal.y +
        originalNormal * tangentSpaceNormal.z
    );

    return worldNormal;
}

void bump_texture::compute_tangent_space(const vector3& normal, vector3& tangent, vector3& bitangent) const
{
    if (fabs(normal.x) > fabs(normal.y))
    {
        tangent = glm::normalize(glm::vec3(normal.z, 0.0f, -normal.x));
    }
    else
    {
        tangent = glm::normalize(glm::vec3(0.0f, -normal.z, normal.y));
    }
    bitangent = glm::normalize(glm::cross(normal, tangent));
}