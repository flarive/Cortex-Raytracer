#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "../constants.h"
#include "../vec3.h"
#include "../ray.h"
#include "../color.h"
#include "../texture.h"
#include "../hittable.h"
#include "../materials/material.h"

/// <summary>
/// Emissive material
/// </summary>
class diffuse_light : public material
{
public:
    diffuse_light(shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
        return false;
    }

    color emitted(double u, double v, const point3& p) const override
    {
        return emit->value(u, v, p);
    }

private:
    shared_ptr<texture> emit;
};

#endif