#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

/// <summary>
/// Dielectric material
/// For water, glass, diamond...
/// Ray is reflected and also refracted
/// </summary>
class dielectric : public material
{
public:
    dielectric(double index_of_refraction);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override;
   

private:
    double ir; // Index of Refraction (typically air = 1.0, glass = 1.3–1.7, diamond = 2.4)

    // Static methods gets constructed only once no matter how many times the function is called.
    static double reflectance(double cosine, double ref_idx);
};