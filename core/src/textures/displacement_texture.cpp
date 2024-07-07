#include "displacement_texture.h"

#include "image_texture.h"

displacement_texture::displacement_texture()
{
}

displacement_texture::displacement_texture(std::shared_ptr<texture> t, double strength)
    : m_displacement(t), m_strength(strength)
{
    std::shared_ptr<image_texture> imageTex = std::dynamic_pointer_cast<image_texture>(m_displacement);
    if (imageTex)
    {
        m_width = imageTex->getWidth();
        m_height = imageTex->getHeight();
        m_data = imageTex->get_data_float();
        m_channels = imageTex->getChannels();
    }
}

color displacement_texture::value(double u, double v, const point3& p) const
{
    // Calculate displacement using a distance function
    //double displacement = distance_function(p) * m_strength;
    //vector3 displaced_point = p + displacement * normal_function(p);
    //return m_displacement->value(u, v, displaced_point);

    return color::red();
}

//point3 displacement_texture::displace(const point3& point) const
//{
//    // Displace the point based on the texture
//    double displacement = m_displacement->value(0, 0, point).length() * m_strength;
//    return point + vector3(0, 0, displacement);
//}

double displacement_texture::getStrenth()
{
    return m_strength;
}

//double displacement_texture::distance_function(const vector3& p) const
//{
//    // Example distance function (sphere distance function)
//    vector3 center(0, 0, 0);
//    double radius = 1.0;
//    return (p - center).length() - radius;
//}
//
//vector3 displacement_texture::normal_function(const vector3& p) const
//{
//    // Calculate normal for displacement
//    double epsilon = 1e-5;
//    double dx = distance_function(p + vector3(epsilon, 0, 0)) - distance_function(p - vector3(epsilon, 0, 0));
//    double dy = distance_function(p + vector3(0, epsilon, 0)) - distance_function(p - vector3(0, epsilon, 0));
//    double dz = distance_function(p + vector3(0, 0, epsilon)) - distance_function(p - vector3(0, 0, epsilon));
//    return unit_vector(vector3(dx, dy, dz));
//}
//
//vector3 displacement_texture::displace_point(const vector3& p, double displacement, const vector3& normal) const
//{
//    return p + displacement * normal;
//}

//vector3 displacement_texture::calculate_normal(const vector3& p) const
//{
//    double epsilon = 1e-5;
//    double dx = distance_function(p + vec3(epsilon, 0, 0)) - distance_function(p - vec3(epsilon, 0, 0));
//    double dy = distance_function(p + vec3(0, epsilon, 0)) - distance_function(p - vec3(0, epsilon, 0));
//    double dz = distance_function(p + vec3(0, 0, epsilon)) - distance_function(p - vec3(0, 0, epsilon));
//    return unit_vector(vec3(dx, dy, dz));
//}

float displacement_texture::getDisplacement(float u, float v) const
{
    // Clamp u and v to [0, 1]
    u = std::fmod(u, 1.0f);
    v = std::fmod(v, 1.0f);

    // Map u and v to texture coordinates
    int x = static_cast<int>(u * m_width) % m_width;
    int y = static_cast<int>(v * m_height) % m_height;

    // Compute the index in the data array
    int index = (y * m_width + x) * m_channels;

    // Calculate the displacement value based on the number of channels
    if (m_channels == 1) { // Grayscale
        return m_data[index] / m_strength;
    }
    else if (m_channels == 3) { // RGB
        float r = m_data[index] / m_strength;
        float g = m_data[index + 1] / m_strength;
        float b = m_data[index + 2] / m_strength;
        // Using the average of the RGB values as the displacement
        return (r + g + b) / 3.0f;
    }
    else if (m_channels == 4) { // RGBA
        float r = m_data[index] / m_strength;
        float g = m_data[index + 1] / m_strength;
        float b = m_data[index + 2] / m_strength;
        // Ignore the alpha channel for displacement
        return (r + g + b) / 3.0f;
    }
    else {
        throw std::runtime_error("Unsupported number of channels");
    }
}