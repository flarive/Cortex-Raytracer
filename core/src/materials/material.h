#ifndef MATERIAL_H
#define MATERIAL_H

#include "../constants.h"
#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../pdf.h"


class scatter_record
{
public:
    color attenuation;
    shared_ptr<pdf> pdf_ptr;
    bool skip_pdf; // means specular
    ray skip_pdf_ray;
};



/// <summary>
/// Abstract class for materials
/// </summary>
class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const
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