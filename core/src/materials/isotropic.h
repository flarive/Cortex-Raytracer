#pragma once

#include "../constants.h"
#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../textures/solid_color_texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../utilities/randomizer.h"

/// <summary>
/// Isotropic material
/// Isotropic materials show the same properties in all directions.
/// Glass, crystals with cubic symmetry, diamonds, metals are examples of isotropic materials.
/// </summary>
class isotropic : public material
{
public:
    isotropic(color _color);
    isotropic(std::shared_ptr<texture> _albedo);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const override;
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;
};