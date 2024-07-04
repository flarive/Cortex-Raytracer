#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../textures/solid_color_texture.h"
#include "../utilities/randomizer.h"
#include "../utilities/randomizer2.h"

/// <summary>
/// Metal material
/// For polished metals the ray won’t be randomly scattered
/// Ray is reflected 90°
/// color albedo -> reflective power of a surface (snow or mirror = 1, black object = 0)
/// </summary>
class metal : public material
{
public:
    metal(const color& _color, double _fuzz);

    /// <summary>
    /// Tells how ray should be reflected when hitting a metal object
    /// </summary>
    /// <param name="r_in"></param>
    /// <param name="rec"></param>
    /// <param name="attenuation"></param>
    /// <param name="scattered"></param>
    /// <returns></returns>
    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer2& random) const override;


private:
    double m_fuzz; // kind of blur amount (0 = none)
};