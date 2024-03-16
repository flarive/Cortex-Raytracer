#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "../constants.h"
#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../textures/solid_color_texture.h"

/// <summary>
/// Lambertian diffuse material
/// More accurate representation of real diffuse objects
/// Ray is randomly scattered using Lambertian distribution
/// https://en.wikipedia.org/wiki/Lambertian_reflectance
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
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        vector3 scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        // Lambertian scatter, bullet-proof
        if (near_zero(scatter_direction))
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p); // reflective power of a surface (snow or mirror = 1, black object = 0)
        return true;
    }

private:
    shared_ptr<texture> albedo;
};

#endif