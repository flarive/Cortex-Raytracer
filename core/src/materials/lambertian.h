#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "../constants.h"
#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../textures/solid_color_texture.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

/// <summary>
/// Lambertian diffuse material
/// More accurate representation of real diffuse objects
/// Ray is randomly scattered using Lambertian distribution
/// https://en.wikipedia.org/wiki/Lambertian_reflectance
/// The smooth diffuse material (also referred to as ``Lambertian'')
/// represents an ideally diffuse material with a user - specified amount of
/// reflectance.Any received illumination is scattered so that the surface
/// looks the same independently of the direction of observation.
/// </summary>
class lambertian : public material
{
public:

    lambertian(const color& a) : albedo(make_shared<solid_color_texture>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {}

    /// <summary>
    /// Tells how ray should be reflected when hitting a lambertian diffuse object
    /// </summary>
    /// <param name="r_in"></param>
    /// <param name="rec"></param>
    /// <param name="attenuation"></param>
    /// <param name="scattered"></param>
    /// <returns></returns>
    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override
    {
        srec.attenuation = albedo->value(rec.u, rec.v, rec.hit_point);
        srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
        srec.skip_pdf = false;
        return true;
    }




    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override
    {
        auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
        return cos_theta < 0 ? 0 : cos_theta / M_PI;
    }

private:
    shared_ptr<texture> albedo;
};

#endif