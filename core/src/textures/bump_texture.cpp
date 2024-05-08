#include "bump_texture.h"

#include <glm/glm.hpp>

bump_texture::bump_texture()
{
}
	
bump_texture::bump_texture(std::shared_ptr<texture> diffuse, std::shared_ptr<texture> bump, double scale) : m_diffuse(diffuse), m_bump(bump), m_scale(scale)
{
}

color bump_texture::value(double u, double v, const point3& p) const
{
    // Sample the base texture color
    color base_color = m_diffuse->value(u, v, p);

    // Sample the bump map (grayscale)
    float bump_value = m_bump->value(u, v, p).r(); // Assuming grayscale

    // Compute the perturbed normal
    vector3 original_normal = computeSurfaceNormal(p); // Get the surface normal
    vector3 gradient = computeBumpGradient(u, v, p);  // Compute gradient from bump map
    vector3 perturbed_normal = glm::normalize(original_normal + m_scale * gradient);

    // Perform lighting calculations using perturbed_normal
    color result_color = base_color; // Start with the base color

    // Example: Simulate a directional light
    vector3 light_direction = glm::normalize(vector3(0.0f, 10.0f, -10.0f)); // Example light direction (adjust as needed)

    // Calculate diffuse lighting
    float diffuse_intensity = glm::max(glm::dot(glm::vec3(perturbed_normal), glm::vec3(light_direction)), 0.0f);
    color diffuse_color = base_color * diffuse_intensity;

    // Example: Simulate a specular highlight
    vector3 view_direction = glm::normalize(-vector3(p)); // View direction is towards the viewer (from surface point to origin)

    // Calculate halfway vector for specular reflection
    vector3 halfway = glm::normalize(light_direction + view_direction);

    // Calculate specular lighting
    float specular_intensity = glm::pow(glm::max(glm::dot(glm::vec3(perturbed_normal), glm::vec3(halfway)), 0.0f), 16.0f); // Example specular power of 16
    color specular_color(1.0f); // White specular highlight
    specular_color *= specular_intensity;

    // Combine diffuse and specular components
    result_color = diffuse_color + specular_color;

    return result_color;
}

vector3 bump_texture::computeSurfaceNormal(const point3& p) const
{
    // Compute the surface normal based on your object's geometry
    // For example, if you're dealing with a sphere centered at the origin:
    // return normalize(p - center);
    // Adjust this based on your actual scene objects.
    // Make sure to handle other surface types (e.g., triangles) accordingly.
    // Replace this placeholder implementation with your own logic.
    //return vector3(0, 1, 0); // Example: Upward-facing normal

    // For a sphere centered at the origin, the normal is simply the normalized point
    return glm::normalize(p);
}

vector3 bump_texture::computeBumpGradient(float u, float v, const point3& p) const
{
    // Implement this function to compute the gradient from the bump map
    // You'll need to sample the bump map and calculate partial derivatives.
    // Replace this placeholder implementation with your own logic.
    //return vector3(0, 0, 0); // Example: Zero gradient

    // Compute partial derivatives using central differences for bump mapping

    const float epsilon = 1e-4f; // Small epsilon value for finite differences

    // Sample bump map values around the current (u, v) coordinates
    float du = m_bump->value(u + epsilon, v, p).r() - m_bump->value(u - epsilon, v, p).r();
    float dv = m_bump->value(u, v + epsilon, p).r() - m_bump->value(u, v - epsilon, p).r();

    // Construct the gradient vector from the partial derivatives
    vector3 gradient(du, dv, 0.0f); // Assuming 2D bump map (no depth variation)

    // Normalize the gradient vector
    //gradient = glm::normalize(gradient);

    return gradient;
}
