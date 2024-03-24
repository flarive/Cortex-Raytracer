#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "../constants.h"
#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../textures/solid_color_texture.h"

/// <summary>
/// Emissive material
/// </summary>
class diffuse_light : public material
{
public:
    diffuse_light(shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(make_shared<solid_color_texture>(c)), invisible(true) {}
    diffuse_light(color c, bool invisible) : emit(make_shared<solid_color_texture>(c)), invisible(invisible){}

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override
    {
        // Material emission, directional
        if (!rec.front_face)
        {
            return invisible ? color(0, 0, 0, 0) : color(1, 1, 1, 1);
        }

        return emit->value(u, v, p);
    }

private:
    shared_ptr<texture> emit;

    double intensity = 1.0;
    bool invisible = true;
};

#endif