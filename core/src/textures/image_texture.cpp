#include "image_texture.h"

image_texture::image_texture(const char* filename) : m_image(filename)
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
    auto pixel = m_image.pixel_data(i, j);

    auto color_scale = 1.0 / 255.0;
    return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
}


//vector3 image_texture::interpolate(vector3 uv)
//{
//    float u = uv[0] - (float)std::floor(uv[0]);
//    float v = uv[1] - (float)std::floor(uv[1]);
//    u *= m_image.width() - 1;
//    v *= m_image.height() - 1;
//
//    auto rrr = m_image.pixel_data((int)u, (int)v);
//
//    //color cuv = m_image.GetPixel((int)u, (int)v);
//
//    //return vector3(cuv.r, cuv.g, cuv.b);
//
//    return vector3();
//}