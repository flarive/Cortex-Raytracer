#pragma once

#include "../constants.h"
#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../textures/solid_color_texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../pdf.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

/// <summary>
/// Isotropic material
/// </summary>
class isotropic : public material
{
public:
    isotropic(color c);
    isotropic(std::shared_ptr<texture> a);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override;
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;

private:
    std::shared_ptr<texture> albedo;
};