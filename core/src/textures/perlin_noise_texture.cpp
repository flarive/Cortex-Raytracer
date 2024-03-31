#include "perlin_noise_texture.h"

perlin_noise_texture::perlin_noise_texture()
{

}

perlin_noise_texture::perlin_noise_texture(double sc) : scale(sc)
{

}

color perlin_noise_texture::value(double u, double v, const point3& p) const
{
    auto s = scale * p;
    return color(1, 1, 1) * 0.5 * (1 + sin(s.z + 10 * noise.turb(s)));
}