#ifndef TEXTURE_H
#define TEXTURE_H

#include "../constants.h"
#include "../color.h"
#include "../point3.h"
#include "../stb_image.h"
#include "../perlin.h"


/// <summary>
/// Texture base class
/// </summary>
class texture
{
public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
};










#endif