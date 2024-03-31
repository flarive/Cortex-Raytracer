#include "solid_color_texture.h"

solid_color_texture::solid_color_texture(color c) : color_value(c)
{

}

solid_color_texture::solid_color_texture(double red, double green, double blue) : solid_color_texture(color(red, green, blue))
{

}

color solid_color_texture::value(double u, double v, const point3& p) const
{
    return color_value;
}