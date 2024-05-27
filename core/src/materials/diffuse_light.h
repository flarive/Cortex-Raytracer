#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../materials/material.h"
#include "../utilities/types.h"
#include "../misc/hit_record.h"

/// <summary>
/// Diffuse light emissive material
/// </summary>
class diffuse_light : public material
{
public:
    diffuse_light(std::shared_ptr<texture> a);
    diffuse_light(color _c);
    diffuse_light(color _c, double _intensity, bool _directional, bool _invisible);

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override;


private:
    std::shared_ptr<texture> m_emit;
    double m_intensity = 1.0;
    bool m_directional = true;
    bool m_invisible = true;
};