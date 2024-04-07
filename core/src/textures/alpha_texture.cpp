#include "alpha_texture.h"


alpha_texture::alpha_texture()
{

}

alpha_texture::alpha_texture(unsigned char* pixels, int _width, int _height, int _bit_per_pixel) : data(pixels), m_width(_width), m_height(_height), m_channels(_bit_per_pixel)
{

}

color alpha_texture::value(double u, double v, const point3& p) const
{
    while (u < 0) u += 1;
    while (v < 0) v += 1;
    while (u > 1) u -= 1;
    while (v > 1) v -= 1;
    int i = u * m_width;
    int j = (1 - v) * m_height;
    if (i < 0) i = 0;
    if (j < 0) j = 0;
    if (i > m_width - 1) i = m_width - 1;
    if (j > m_height - 1) j = m_height - 1;
    double aa = ((double)data[m_channels * i + m_channels * m_width * j + m_channels - 1] * rescale);

    double r = double(data[m_channels * i + m_channels * m_width * j]) *  rescale;
    double g = double(data[m_channels * i + m_channels * m_width * j + 1]) * rescale;
    double b = double(data[m_channels * i + m_channels * m_width * j + 2]) * rescale;
    double a = double(data[m_channels * i + m_channels * m_width * j + 3]) * rescale;

    color ccc = color(r, g, b, a);

    if (a < 1.0)
    {
        int aa = 0;
    }

    return ccc;

    //return color(1, 0, 0, 0);
}