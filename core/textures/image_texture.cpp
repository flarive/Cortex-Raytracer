#include "image_texture.h"

#include "../utilities/interval.h"

image_texture::image_texture(const std::string filepath) : m_image(filepath.c_str())
{
}

image_texture::image_texture(unsigned char* filedata, size_t datasize) : m_image(filedata, datasize)
{

}


color image_texture::value(double u, double v, const point3& p) const
{
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (m_image.height() <= 0) return color(0, 1, 1);

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = interval(0, 1).clamp(u);
    v = 1.0 - interval(0, 1).clamp(v);  // Flip V to image coordinates

    auto i = static_cast<int>(u * m_image.width());
    auto j = static_cast<int>(v * m_image.height());
    
    double color_scale = 1.0 / 255.0;
    const unsigned char* pixel = m_image.pixel_data(i, j);
    return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
}

int image_texture::getWidth() const
{
    return m_image.width();
}

int image_texture::getHeight() const
{
    return m_image.height();
}

int image_texture::getChannels() const
{
    return m_image.channels();
}

unsigned char* image_texture::get_data() const
{
	return m_image.get_data();
}

float* image_texture::get_data_float() const
{
    return m_image.get_data_float();
}