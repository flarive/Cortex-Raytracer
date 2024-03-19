#ifndef ALPHA_TEXTURE_H
#define ALPHA_TEXTURE_H

#include "texture.h"
#include "../misc/color.h"

static constexpr double rescale = 1.f / 255.f;

/// <summary>
/// Alpha texture (NOT TESTED !!!!!!!!!)
/// </summary>
class alpha_texture : public texture
{
public:
    alpha_texture() {}
    alpha_texture(unsigned char* pixels, int A, int B, int nn) : data(pixels), nx(A), ny(B), channels(nn) {}


    color value(double u, double v, const point3& p) const
    {
        while (u < 0) u += 1;
        while (v < 0) v += 1;
        while (u > 1) u -= 1;
        while (v > 1) v -= 1;
        int i = u * nx;
        int j = (1 - v) * ny;
        if (i < 0) i = 0;
        if (j < 0) j = 0;
        if (i > nx - 1) i = nx - 1;
        if (j > ny - 1) j = ny - 1;
        //return((double)data[channels * i + channels * nx * j + channels - 1] * rescale);

        //return((Float)data[channels * i + channels * nx * j] * rescale);

        double intensity = 1.0;

        double bu = double(data[channels * (i + 1) + channels * nx * j] - data[channels * (i - 1) + channels * nx * j]) / 2 * rescale;
        double bv = double(data[channels * i + channels * nx * (j + 1)] - data[channels * i + channels * nx * (j - 1)]) / 2 * rescale;
        return(color(intensity * bu, intensity * bv, 0));
    }

private:
    unsigned char* data;
    int nx, ny, channels;
};

#endif