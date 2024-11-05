#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../randomizers/randomize.h"

/// <summary>
/// Anisotropic material
/// Anisotropic materials show different properties in different directions.
/// Wood, composite materials, all crystals (except cubic crystal) are examples of anisotropic materials.
/// Is a material (shader) that represents surfaces with grooves, such as a CD, feathers, or fabrics like velvet or satin
/// </summary>
class anisotropic : public material
{
public:
    anisotropic(double Nu, double Nv, const std::shared_ptr<texture>& diffuseTexture, const std::shared_ptr<texture>& specularTexture, const std::shared_ptr<texture>& exponentTexture);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const override;
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;
    

private:
    const double epsilon = 1E-5;

    std::shared_ptr<texture> m_diffuse;
    std::shared_ptr<texture> m_specular;
    std::shared_ptr<texture> m_exponent;

    // exponents - for nu=nv it's similar with Phong
    // The Phong exponent defines the "tightness" of the highlight.
    // A higher exponent results in a smaller, tighter highlight while a lower exponent results in a broader flatter one.
    double m_nu = 0.0;
    double m_nv = 0.0;


    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const;
};