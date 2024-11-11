#pragma once

#include "../utilities/types.h"
#include "texture.h"
#include "../misc/color.h"

/// <summary>
/// Roughness texture (NOT TESTED !!!!!!!!!)
/// </summary>
class roughness_texture : public texture
{
public:
    roughness_texture();
    roughness_texture(unsigned char* pixels, int A, int B, int nn);

    point2 value(double u, double v) const;

    static double RoughnessToAlpha(double roughness);

private:
    unsigned char* data = nullptr;
    int nx, ny, channels = 0;
    vector3 u_vec, v_vec;
};