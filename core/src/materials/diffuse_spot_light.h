#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../materials/material.h"
#include "../misc/hit_record.h"

/// <summary>
/// Diffuse spot light emissive material
/// </summary>
class diffuse_spot_light : public material
{
public:
    diffuse_spot_light(std::shared_ptr<texture> emitTex, vector3 dir, double cosTotalWidth, double cosFalloffStart, double intensity, bool invisible);

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override;
    double falloff(const vector3& w) const;

private:
    std::shared_ptr<texture> m_emit;
    vector3 m_spot_direction;
    double m_intensity = 0;
    const double m_cosTotalWidth = 0;
    const double m_cosFalloffStart = 0;
    bool m_invisible;
};