#ifndef METAL_H
#define METAL_H

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"

/// <summary>
/// Metal material
/// For polished metals the ray won’t be randomly scattered
/// Ray is reflected 90°
/// </summary>
class metal : public material
{
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    /// <summary>
    /// Tells how ray should be reflected when hitting a metal object
    /// </summary>
    /// <param name="r_in"></param>
    /// <param name="rec"></param>
    /// <param name="attenuation"></param>
    /// <param name="scattered"></param>
    /// <returns></returns>
    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override
    {
        srec.attenuation = albedo;
        srec.pdf_ptr = nullptr;
        srec.skip_pdf = true;
        vector3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        srec.skip_pdf_ray = ray(rec.hit_point, reflected + fuzz * random_in_unit_sphere(), r_in.time());
        return true;
    }

private:
    color albedo; // reflective power of a surface (snow or mirror = 1, black object = 0)
    double fuzz; // kind of blur amount (0 = none)
};

#endif