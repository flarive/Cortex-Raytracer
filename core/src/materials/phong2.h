#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../lights/light.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../textures/solid_color_texture.h"
#include "../utilities/randomizer.h"

/// <summary>
/// Phong material
/// https://stackoverflow.com/questions/24132774/trouble-with-phong-shading
/// </summary>
class phong2 : public material
{
public:

    phong2(const color& diffuseColor, const color& specularColor, float exponent, const color& transparentColor, const color& reflectiveColor, float indexOfRefraction);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const override;
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;

private:
    color m_diffuseColor;
    color m_specularColor;
    float m_exponent;
    color m_reflectiveColor;
    color m_transparentColor;
    float m_indexOfRefraction;

    static float maxDot3(const vector3& v1, const vector3& v2);
};

