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
    diffuse_light(color c) : emit(make_shared<solid_color_texture>(c)) {}

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override
    {
        // Material emission, directional
        if (!rec.front_face)
            return color(0, 0, 0);

        return emit->value(u, v, p);
    }

private:
    shared_ptr<texture> emit;
};

#endif