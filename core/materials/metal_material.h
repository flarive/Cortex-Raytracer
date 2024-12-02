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
    
    // Low m_fuzz : Surface has fewer imperfections, leading to focused and mirror - like reflections.
    // High m_fuzz : Surface has many imperfections, scattering light widely.
    //
    // Low Values(0.0 ≤ m_fuzz ≤ 0.3) :
    // Near - perfect reflection with slight roughness.Highlights are sharp, with minimal scattering.
    // Example : Polished metals like gold, silver, or a lightly brushed surface.
    //    
    // Moderate Values(0.3 < m_fuzz ≤ 0.7) :
    // Partially rough reflection.Highlights become blurred, and the material appears more diffuse.
    // Example : Brushed metals like aluminum, roughened steel, or weathered copper.
    //    
    // High Values(0.7 < m_fuzz ≤ 1.0) :
    // Highly rough or diffuse reflection.Highlights are heavily blurred, and the material resembles a matte surface.
    // Example : Rough metals, corroded surfaces, or frosted finishes.
    //
    // Polished Chrome	0.0	Perfectly reflective, like a mirror.
    // Polished Gold	0.0–0.1	Smooth with very slight imperfections.
    // Brushed Aluminum	0.2–0.4	Moderately rough, with directional scattering.
    // Weathered Copper	0.5–0.7	Rough with blurred reflections.
    // Rusted Steel	0.7–1.0	Almost fully diffuse with minimal reflection.
    // Frosted Metal	0.8–1.0	Very rough surface with highly scattered highlights.
    double m_fuzz = 0.0; // kind of blur amount (0 = none)


    // 0.0: No specular reflection.The material appears completely diffuse with no shiny highlights(e.g., matte surfaces like chalk or unpolished stone).
    // 1.0 : Full specular reflection.The material reflects light strongly, creating sharp and bright highlights(e.g., polished metals, mirrors).
    // 
    // Low Values(0.0 < m_specular_intensity <= 0.3) : Produces weak or subtle highlights.
    // Suitable for rough or diffuse materials like unpolished metals, paper, or cloth.
    //
    // Medium Values(0.3 < m_specular_intensity <= 0.7) : Produces noticeable but soft highlights.
    // Suitable for materials with moderate gloss, like satin finishes, waxed surfaces, or plastics.
    //
    // High Values(0.7 < m_specular_intensity <= 1.0) : Produces sharp and bright highlights.
    // Suitable for highly polished or shiny materials like mirrors, wet surfaces, or polished metals.
    // 
    // Polished Gold	0.9–1.0	Highly reflective with sharp highlights.
    // Brushed Aluminum	0.6–0.8	Moderate reflections with soft highlights.
    // Matte Paint	0.0–0.1	Virtually no specular reflection.
    // Wet Asphalt	0.5–0.7	Noticeable specular reflection due to surface moisture.
    // Plastic	0.3–0.6	Glossy but not as reflective as metals.
    // Frosted Glass	0.2–0.5	Subtle highlights due to roughness.
    double m_specular_intensity = 0.0; // 0.0 is neutral

    // Polished Silver	color(1.0, 1.0, 1.0)	Neutral reflection, untinted.
    // Polished Gold	color(1.0, 0.84, 0.0)	Strong gold reflection.
    // Brushed Copper	color(0.72, 0.45, 0.2)	Reddish copper hue with natural reflection.
    // Oxidized Brass	color(0.5, 0.7, 0.4)	Greenish - yellow tint.
    // Decorative Blue Metal	color(0.3, 0.5, 0.9)	Artistic stylization for coated or anodized surfaces.
    // Abstract Artistic Metal	color(0.9, 0.3, 0.4)	A strong custom tint for creative materials.
    color m_metal_tint{ 1.0, 1.0, 1.0 }; // white is neutral

    // 0.0: Isotropic reflection.The surface reflects light uniformly, like polished metal or smooth plastic.
    // 1.0 : Fully anisotropic reflection in the primary tangent direction(e.g., along a brushed metal grain).
    // - 1.0 : Fully anisotropic reflection in the secondary tangent direction(perpendicular to the primary tangent).
    // 
    // Polished Gold	0.0	Isotropic; uniform, mirror - like reflections.
    // Brushed Aluminum	0.5–1.0	Strong anisotropy along the brushing direction.
    // Satin or Silk	0.3–0.6	Moderate anisotropy for soft highlights along fabric grain.
    // Hair or Fibers	0.7–1.0	High anisotropy in the direction of strands.
    // Engineered Finishes - 0.3 to - 1.0	Stretched reflections perpendicular to grain(uncommon).
    double m_anisotropy = 0.0; // 0.0 (isotropic) is neutral

    // 0.0: No Fresnel effect. The material's reflectivity is constant and does not depend on the viewing angle.
    // This makes the surface look more "metallic" or artificial.
    // 1.0: Full Fresnel effect. Reflectivity varies strongly with the angle of incidence, as seen in real-world materials like metals, glass, or water.
    double m_fresnel_factor = 0.0; // 0 is neutral

    // 0.0: No heating is applied.The material retains its original base color.
    // 1.0: Full heating is applied. The material completely adopts the heated color.
    double m_heat = 0.0; // 0 is neutral

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
// m_heat = 1.0