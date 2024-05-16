#pragma once

#include "../utilities/types.h"
#include "../constants.h"
#include "../misc/color.h"

/// <summary>
/// Texture base class
/// </summary>
class texture
{
public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
};