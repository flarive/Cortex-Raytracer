#include "displacement_texture.h"
#include "../utilities/randomizer.h"


displacement_texture::displacement_texture()
{
}

displacement_texture::displacement_texture(std::shared_ptr<texture> t, double strength)
    : m_displacement(t), m_strength(strength)
{
}

color displacement_texture::value(double u, double v, const point3& p) const
{
    // Calculate displacement using a distance function
    double displacement = distance_function(p) * m_strength;
    vector3 displaced_point = p + displacement * normal_function(p);
    return m_displacement->value(u, v, displaced_point);
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

double displacement_texture::distance_function(const vector3& p) const
{
    // Example distance function (sphere distance function)
    vector3 center(0, 0, 0);
    double radius = 1.0;
    return (p - center).length() - radius;
}

vector3 displacement_texture::normal_function(const vector3& p) const
{
    // Calculate normal for displacement
    double epsilon = 1e-5;
    double dx = distance_function(p + vector3(epsilon, 0, 0)) - distance_function(p - vector3(epsilon, 0, 0));
    double dy = distance_function(p + vector3(0, epsilon, 0)) - distance_function(p - vector3(0, epsilon, 0));
    double dz = distance_function(p + vector3(0, 0, epsilon)) - distance_function(p - vector3(0, 0, epsilon));
    return unit_vector(vector3(dx, dy, dz));
}

vector3 displacement_texture::displace_point(const vector3& p, double displacement, const vector3& normal) const
{
    return p + displacement * normal;
}

//vector3 displacement_texture::calculate_normal(const vector3& p) const
//{
//    double epsilon = 1e-5;
//    double dx = distance_function(p + vec3(epsilon, 0, 0)) - distance_function(p - vec3(epsilon, 0, 0));
//    double dy = distance_function(p + vec3(0, epsilon, 0)) - distance_function(p - vec3(0, epsilon, 0));
//    double dz = distance_function(p + vec3(0, 0, epsilon)) - distance_function(p - vec3(0, 0, epsilon));
//    return unit_vector(vec3(dx, dy, dz));
//}