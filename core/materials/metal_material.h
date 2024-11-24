#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../textures/solid_color_texture.h"
#include "../randomizers/randomizer.h"

/// <summary>
/// Metal material
/// For polished metals the ray won’t be randomly scattered
/// Ray is reflected 90°
/// color albedo -> reflective power of a surface (snow or mirror = 1, black object = 0)
/// </summary>
class metal_material : public material
{
public:
    metal_material(const color& _color, double _fuzz);

    metal_material(std::shared_ptr<texture> _texture, double _fuzz);


    metal_material(const color& _color, double _fuzz, double _specular_intensity, const color& _tint, double _anisotropy, double _fresnel_factor, double _heat);


    metal_material(std::shared_ptr<texture> _texture, double _fuzz, double _specular_intensity, const color& _tint, double _anisotropy, double _fresnel_factor, double _heat);



    /// <summary>
    /// Tells how ray should be reflected when hitting a metal object
    /// </summary>
    /// <param name="r_in"></param>
    /// <param name="rec"></param>
    /// <param name="attenuation"></param>
    /// <param name="scattered"></param>
    /// <returns></returns>
    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const override;


private:
    double m_fuzz = 0.0; // kind of blur amount (0 = none)
    double m_specular_intensity;
    color m_metal_tint;
    double m_anisotropy;
    double m_fresnel_factor;
    double m_heat;

    // Helper function for anisotropic fuzz
    vector3 anisotropic_fuzz(const vector3& normal, randomizer& rnd) const;

    // Helper function for Fresnel effect (Schlick's approximation)
    double fresnel_reflection(double cos_theta) const;

    // Helper function for heat effects
    color apply_heat(const color& base_color) const;
    double adjust_fuzz_for_heat() const;
};