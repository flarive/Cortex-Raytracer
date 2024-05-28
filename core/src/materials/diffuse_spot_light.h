#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../materials/material.h"
#include "../misc/hit_record.h"

/// <summary>
/// Diffuse spot light emissive material
/// https://github.com/tylermorganwall/rayrender/blob/1feb2f1328c9eab5d67c09a53ffea6d8d403c064/src/material.h#L167
/// </summary>
class diffuse_spot_light : public material
{
public:
    diffuse_spot_light(std::shared_ptr<texture> emitTex, point3 pos, vector3 dir, double cosTotalWidth, double cosFalloffStart, double intensity, bool invisible);

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override;
    double falloff(const vector3& w) const;

    color get_albedo(const ray& r_in, const hit_record& rec) const;
    size_t getSize();

private:
    std::shared_ptr<texture> m_emit = nullptr;
    point3 m_position{};
    vector3 m_direction{};
    double m_intensity = 0.0;
    double m_cosTotalWidth = 0.0;
    double m_cosFalloffStart = 0.0;
    bool m_invisible = false;
};