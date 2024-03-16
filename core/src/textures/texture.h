#ifndef TEXTURE_H
#define TEXTURE_H

#include "../constants.h"
#include "../misc/color.h"

/// <summary>
/// Texture base class
/// </summary>
class texture
{
public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const Point3& p) const = 0;
};

#endif