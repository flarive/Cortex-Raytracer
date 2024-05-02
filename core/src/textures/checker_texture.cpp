#include "checker_texture.h"

#include "solid_color_texture.h"

checker_texture::checker_texture(double _scale, std::shared_ptr<texture> _even, std::shared_ptr<texture> _odd)
    : inv_scale(1.0 / _scale), even(_even), odd(_odd)
{
}

checker_texture::checker_texture(double _scale, color c1, color c2)
    : inv_scale(1.0 / _scale),
    even(std::make_shared<solid_color_texture>(c1)),
    odd(std::make_shared<solid_color_texture>(c2))
{
}

color checker_texture::value(double u, double v, const point3& p) const
{
    auto xInteger = static_cast<int>(std::floor(inv_scale * p.x));
    auto yInteger = static_cast<int>(std::floor(inv_scale * p.y));
    auto zInteger = static_cast<int>(std::floor(inv_scale * p.z));

    bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

    return isEven ? even->value(u, v, p) : odd->value(u, v, p);
}