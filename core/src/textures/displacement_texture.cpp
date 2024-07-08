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
    double value = 0.0;
    
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
        value = m_data[index] / m_strength;
    }
    else if (m_channels == 3) { // RGB
        double r = m_data[index] / m_strength;
        double g = m_data[index + 1] / m_strength;
        double b = m_data[index + 2] / m_strength;
        // Using the average of the RGB values as the displacement
        value = (r + g + b) / 3.0f;
    }
    else if (m_channels == 4) { // RGBA
        double r = m_data[index] / m_strength;
        double g = m_data[index + 1] / m_strength;
        double b = m_data[index + 2] / m_strength;
        // Ignore the alpha channel for displacement
        value = (r + g + b) / 3.0f;
    }

    // return a single double as a color
    return color(value, value, value);
}

double displacement_texture::getStrenth()
{
    return m_strength;
}