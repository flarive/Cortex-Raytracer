#ifndef MATERIAL_H
#define MATERIAL_H

#include "../constants.h"
#include "../vec3.h"
#include "../ray.h"
#include "../color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"

/// <summary>
/// Abstract class for materials
/// </summary>
class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;

    virtual color emitted(double u, double v, const point3& p) const
    {
        return color(0, 0, 0);
    }
};

#endif