#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../utilities/randomizer2.h"

/// <summary>
/// Diffuse material
/// More accurate representation of real diffuse objects
/// Ray is randomly scattered using Lambertian distribution
/// https://en.wikipedia.org/wiki/Lambertian_reflectance
/// The smooth diffuse material (also referred to as 'Lambertian')
/// represents an ideally diffuse material with a user - specified amount of
/// reflectance.Any received illumination is scattered so that the surface
/// looks the same independently of the direction of observation.
/// https://www.hackification.io/blog/2008/07/18/experiments-in-ray-tracing-part-4-lighting/
/// </summary>
class lambertian : public material
{
public:

    lambertian(const color& _color);
    lambertian(const color& _color, double _transparency, double _refraction_index);

    lambertian(std::shared_ptr<texture> _albedo);
    lambertian(std::shared_ptr<texture> _albedo, double _transparency, double _refraction_index);

    

    /// <summary>
    /// Tells how ray should be reflected when hitting a lambertian diffuse object
    /// </summary>
    /// <param name="r_in"></param>
    /// <param name="rec"></param>
    /// <param name="attenuation"></param>
    /// <param name="scattered"></param>
    /// <returns></returns>
    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override;
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;
};