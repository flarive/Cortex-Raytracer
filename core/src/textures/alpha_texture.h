#pragma once

#include "texture.h"
#include "../misc/color.h"

static constexpr double rescale = 1.f / 255.f;

/// <summary>
/// Alpha texture (NOT TESTED !!!!!!!!!)
/// </summary>
class alpha_texture : public texture
{
public:
    alpha_texture();
    alpha_texture(unsigned char* pixels, int A, int B, int nn);

    color value(double u, double v, const point3& p) const;
    

private:
    unsigned char* data;
    int nx, ny, channels;
};