#ifndef GLOSSY_H
#define GLOSSY_H

#include "../constants.h"
#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

/// <summary>
/// Glossy material (NOT TESTED !!!)
/// https://github.com/Drummersbrother/raytracing-in-one-weekend/blob/master/material.h
/// </summary>
class glossy : public material
{
public:
    // Fuzz texture interpreted as the magnitude of the fuzz texture.
    glossy(std::shared_ptr<texture>& a, std::shared_ptr<texture>& f) : albedo(a), fuzz(f) {}

    virtual bool scatter(const ray& ray_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override
    {
        vector3 reflected = reflect(unit_vector(ray_in.direction()), rec.normal);
        srec.attenuation = albedo->value(rec.u, rec.v, rec.hit_point);

        srec.skip_pdf = true;
        double fuzz_factor = (fuzz->value(rec.u, rec.v, rec.hit_point)).length();
        srec.skip_pdf_ray = ray(rec.hit_point, reflected + fuzz_factor * random_in_unit_sphere(), ray_in.time());

        srec.pdf_ptr = 0;
        return true;
    }
public:
    std::shared_ptr<texture> albedo, fuzz;
};

#endif