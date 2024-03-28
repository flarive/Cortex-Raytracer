#ifndef MATERIAL_H
#define MATERIAL_H

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../primitives/hittable.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

/// <summary>
/// Abstract class for materials
/// </summary>
class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
    {
        return false;
    }

    virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
    {
        return 0;
    }

    virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
    {
        return color(0, 0, 0);
    }
};

#endif