#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

/// <summary>
/// Spot light material
/// </summary>
class spot_light : public material
{
public:
    spot_light(std::shared_ptr<texture>  a, vector3 dir, double cosTotalWidth, double cosFalloffStart, double intensity, bool invisible);
    ~spot_light();

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override;
    double falloff(const vector3& w) const;

private:
    std::shared_ptr<texture> emit;
    vector3 spot_direction;
    double intensity;
    const double cosTotalWidth, cosFalloffStart;
    bool invisible;
};