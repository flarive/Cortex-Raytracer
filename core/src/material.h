#ifndef MATERIAL_H
#define MATERIAL_H

#include "constants.h"
#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "texture.h"
#include "hittable.h"

/// <summary>
/// Abstract class for materials
/// </summary>
class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

/// <summary>
/// Lambertian diffuse material
/// More accurate representation of real diffuse objects
/// Ray is randomly scattered using Lambertian distribution
/// https://en.wikipedia.org/wiki/Lambertian_reflectance
/// </summary>
class lambertian : public material
{
public:
    //lambertian(const color& a) : albedo(a) {}

    lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
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
        vec3 scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        // Lambertian scatter, bullet-proof
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p); // reflective power of a surface (snow or mirror = 1, black object = 0)
        return true;
    }

private:
    shared_ptr<texture> albedo;
};


/// <summary>
/// Metal material
/// For polished metals the ray won�t be randomly scattered
/// Ray is reflected 90�
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
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    color albedo; // reflective power of a surface (snow or mirror = 1, black object = 0)
    double fuzz; // kind of blur amount (0 = none)
};


/// <summary>
/// Dielectric material
/// For water, glass, diamond...
/// Ray is reflected and also refracted
/// </summary>
class dielectric : public material
{
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
        {
            direction = reflect(unit_direction, rec.normal);
        }
        else
        {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }

private:
    double ir; // Index of Refraction (typically air = 1.0, glass = 1.3�1.7, diamond = 2.4)

    // Static methods gets constructed only once no matter how many times the function is called.
    static double reflectance(double cosine, double ref_idx)
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

#endif