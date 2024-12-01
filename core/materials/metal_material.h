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
    /// <summary>
    /// Gold Highly polished : fuzziness = 0.0 to 0.1 (Gold reflects light very smoothly in polished form).
    /// Gold Slightly rough : fuzziness = 0.1 to 0.3.
    /// Silver Highly polished : fuzziness = 0.0 to 0.05 (Silver is known for being one of the most reflective metals).
    /// Silver Slightly rough : fuzziness = 0.1 to 0.25.
    /// Bronze Highly polished : fuzziness = 0.1 to 0.2 (Bronze isn’t as reflective as gold or silver but still smooth when polished).
    /// Bronze Slightly rough : fuzziness = 0.3 to 0.5.
    /// Aluminum Highly polished : fuzziness = 0.0 to 0.1 (Polished aluminum is quite reflective, though not as much as silver).
    /// Aluminum Slightly rough : fuzziness = 0.2 to 0.4.
    /// </summary>
    double m_fuzz = 0.0; // kind of blur amount (0 = none)


    double m_specular_intensity = 1.0; // 1.0 is neutral


    color m_metal_tint{ 1.0, 1.0, 1.0 }; // white is neutral

    double m_anisotropy = 0.0; // 0.0 (isotropic) to 1.0 (fully anisotropic)

    double m_fresnel_factor = 0; // 0 is neutral

    double m_heat = 0; // 0 is neutral

    vector3 anisotropic_fuzz(const vector3& normal, randomizer& rnd) const;
    double fresnel_reflection(double cos_theta) const;
    color apply_heat(const color& base_color) const;
    double adjust_fuzz_for_heat() const;
};




// very nice buddha gold
// m_fresnel_factor = 0.54
// m_anisotropy = 1.0
// m_metal_tint{ 1.0, 1.0, 1.0 }
// m_specular_intensity = 1.0
// m_fuzz = 0.1