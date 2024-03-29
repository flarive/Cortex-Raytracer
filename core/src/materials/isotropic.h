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
    isotropic(color c) : albedo(make_shared<solid_color_texture>(c)) {}

    isotropic(shared_ptr<texture> a) : albedo(a) {}

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override
    {
        srec.attenuation = albedo->value(rec.u, rec.v, rec.hit_point);
        srec.pdf_ptr = make_shared<sphere_pdf>();
        srec.skip_pdf = false;
        return true;
    }

    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override
    {
        return 1 / (4 * M_PI);
    }

private:
    shared_ptr<texture> albedo;
};