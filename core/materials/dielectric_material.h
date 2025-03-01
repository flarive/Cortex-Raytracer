#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../randomizers/randomizer.h"

/// <summary>
/// Dielectric material
/// For water, glass, diamond...
/// Vacuum/air = 1.0, Water 1.333, Ice 1.31, Window glass 1.52, Diamond 2.42
/// https://viclw17.github.io/2018/08/05/raytracing-dielectric-materials
/// Ray is reflected and also refracted
/// </summary>
class dielectric_material : public material
{
public:
    dielectric_material(double index_of_refraction);
    dielectric_material(double index_of_refraction, std::shared_ptr<texture> texture_attenuation);
    dielectric_material(double index_of_refraction, const color& rgb);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const override;
   

private:
    double ir; // Index of Refraction (typically air = 1.0, glass = 1.3�1.7, diamond = 2.4)

    std::shared_ptr<texture> attenuation_texture; // Texture for attenuation

    // Static methods gets constructed only once no matter how many times the function is called.
    static double reflectance(double cosine, double ref_idx);
};