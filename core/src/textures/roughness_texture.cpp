#include "roughness_texture.h"


roughness_texture::roughness_texture()
{
}

roughness_texture::roughness_texture(unsigned char* pixels, int A, int B, int nn) : data(pixels), nx(A), ny(B), channels(nn)
{

}

point2 roughness_texture::value(double u, double v) const
{
    constexpr double rescale = 1.f / 255.f;
    
    while (u < 0) u += 1;
    while (v < 0) v += 1;
    while (u > 1) u -= 1;
    while (v > 1) v -= 1;
    int i = static_cast<int>(u) * nx;
    int j = (1 - static_cast<int>(v)) * ny;
    if (i < 0) i = 0;
    if (j < 0) j = 0;
    if (i > nx - 1) i = nx - 1;
    if (j > ny - 1) j = ny - 1;
    double alphax = RoughnessToAlpha((double)data[channels * i + channels * nx * j] * rescale);
    double alphay = channels > 1 ? RoughnessToAlpha((double)data[channels * i + channels * nx * j + 1] * rescale) : alphax;
    return(point2(alphax * alphax, alphay * alphay));
}

double roughness_texture::RoughnessToAlpha(double roughness)
{
    roughness = std::fmax(roughness, (double)0.0001550155);
    double x = std::log(roughness);
    return(1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x + 0.000640711f * x * x * x * x);
}