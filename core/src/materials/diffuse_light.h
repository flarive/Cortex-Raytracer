#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../textures/solid_color_texture.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

/// <summary>
/// Diffuse light emissive material
/// </summary>
class diffuse_light : public material
{
public:
    diffuse_light(shared_ptr<texture> a) : emit(a) {}

    diffuse_light(color _c) : emit(make_shared<solid_color_texture>(_c)), invisible(true), directional(true) {}

    diffuse_light(color _c, bool _directional, bool _invisible) : emit(make_shared<solid_color_texture>(_c)), directional(_directional), invisible(_invisible){}

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override
    {
        // Material emission, directional
        if (directional && !rec.front_face)
        {
            return invisible ? color(0, 0, 0, 0) : color(1, 1, 1, 0);
        }

        return emit->value(u, v, p);
    }

private:
    shared_ptr<texture> emit;

    double intensity = 1.0;
    bool directional = true;
    bool invisible = true;
};